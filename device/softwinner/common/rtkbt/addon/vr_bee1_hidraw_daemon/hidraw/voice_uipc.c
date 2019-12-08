/*****************************************************************************
 *
 *  Filename:      uipc.c
 *
 *  Description:   UIPC implementation for bluedroid
 *
 *****************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#include <sys/socket.h>
#include <sys/un.h>
#include <signal.h>
#include <errno.h>
#include <pthread.h>
#include <sys/select.h>
#include <sys/poll.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/prctl.h>


#include "voice_uipc.h"
#include "voice_hidraw.h"

#include <cutils/sockets.h>

volatile int uipc_inited = 0;


/*****************************************************************************
**  Constants & Macros
******************************************************************************/

#define MAX(a,b) ((a)>(b)?(a):(b))

#define CASE_RETURN_STR(const) case const: return #const;

#define UIPC_DISCONNECTED (-1)

#define UIPC_LOCK() /*BTIF_TRACE_EVENT(" %s lock", __FUNCTION__);*/ pthread_mutex_lock(&uipc_main.mutex);
#define UIPC_UNLOCK() /*BTIF_TRACE_EVENT("%s unlock", __FUNCTION__);*/ pthread_mutex_unlock(&uipc_main.mutex);

#define SAFE_FD_ISSET(fd, set) (((fd) == -1) ? 0 : FD_ISSET((fd), (set)))

#define UIPC_FLUSH_BUFFER_SIZE 1024

/*****************************************************************************
**  Local type definitions
******************************************************************************/

typedef enum {
    UIPC_TASK_FLAG_DISCONNECT_CHAN = 0x1,
} tUIPC_TASK_FLAGS;

typedef struct {
    int srvfd;
    int fd;
    int read_poll_tmo_ms;
    int task_evt_flags;   /* event flags pending to be processed in read task */
    tUIPC_EVENT cond_flags;
    pthread_mutex_t cond_mutex;
    pthread_cond_t  cond;
    tUIPC_RCV_CBACK *cback;
} tUIPC_CHAN;

typedef struct {
    pthread_t tid; /* main thread id */
    int running;
    pthread_mutex_t mutex;

    fd_set active_set;
    fd_set read_set;
    int max_fd;
    int signal_fds[2];

    tUIPC_CHAN ch[UIPC_CH_NUM];
} tUIPC_MAIN;


/*****************************************************************************
**  Static variables
******************************************************************************/

static tUIPC_MAIN uipc_main;


/*****************************************************************************
**  Static functions
******************************************************************************/

static int uipc_close_ch_locked(tUIPC_CH_ID ch_id);

/*****************************************************************************
**  Externs
******************************************************************************/


/*****************************************************************************
**   Helper functions
******************************************************************************/


const char* dump_uipc_event(tUIPC_EVENT event)
{
    switch(event)
    {
        CASE_RETURN_STR(UIPC_OPEN_EVT)
        CASE_RETURN_STR(UIPC_CLOSE_EVT)
        CASE_RETURN_STR(UIPC_RX_DATA_EVT)
        CASE_RETURN_STR(UIPC_RX_DATA_READY_EVT)
        CASE_RETURN_STR(UIPC_TX_DATA_READY_EVT)
        default:
            return "UNKNOWN MSG ID";
    }
}

/*****************************************************************************
**   socket helper functions
*****************************************************************************/

static inline int create_server_socket(const char* name)
{
    int s = socket(AF_LOCAL, SOCK_STREAM, 0);
    if (s < 0)
        return -1;

    info("create_server_socket %s", name);

    if(socket_local_server_bind(s, name, ANDROID_SOCKET_NAMESPACE_ABSTRACT) < 0)
    {
        error("socket failed to create (%s)", strerror(errno));
        close(s);
        return -1;
    }

    if(listen(s, 5) < 0)
    {
        error("listen failed", strerror(errno));
        close(s);
        return -1;
    }

    info("created socket fd %d", s);
    return s;
}

static int accept_server_socket(int sfd)
{
    struct sockaddr_un remote;
    struct pollfd pfd;
    int fd;
    socklen_t len = sizeof(struct sockaddr_un);

    info("accept fd %d", sfd);

    /* make sure there is data to process */
    pfd.fd = sfd;
    pfd.events = POLLIN;

    if (poll(&pfd, 1, 0) == 0)
    {
        error("accept poll timeout");
        return -1;
    }

    //BTIF_TRACE_EVENT("poll revents 0x%x", pfd.revents);

    if ((fd = accept(sfd, (struct sockaddr *)&remote, &len)) == -1)
    {
         error("sock accept failed (%s)", strerror(errno));
         return -1;
    }

    //BTIF_TRACE_EVENT("new fd %d", fd);

    return fd;
}

/*****************************************************************************
**
**   uipc helper functions
**
*****************************************************************************/

static int uipc_main_init(void)
{
    int i;
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init(&uipc_main.mutex, &attr);

    debug("### uipc_main_init ###");

    /* setup interrupt socket pair */
    if (socketpair(AF_UNIX, SOCK_STREAM, 0, uipc_main.signal_fds) < 0)
    {
        return -1;
    }

    FD_SET(uipc_main.signal_fds[0], &uipc_main.active_set);
    uipc_main.max_fd = MAX(uipc_main.max_fd, uipc_main.signal_fds[0]);

    for (i=0; i< UIPC_CH_NUM; i++)
    {
        tUIPC_CHAN *p = &uipc_main.ch[i];
        p->srvfd = UIPC_DISCONNECTED;
        p->fd = UIPC_DISCONNECTED;
        p->task_evt_flags = 0;
        pthread_cond_init(&p->cond, NULL);
        pthread_mutex_init(&p->cond_mutex, NULL);
        p->cback = NULL;
    }

    return 0;
}

void uipc_main_cleanup(void)
{
    int i;

    info("uipc_main_cleanup");

    close(uipc_main.signal_fds[0]);
    close(uipc_main.signal_fds[1]);

    /* close any open channels */
    for (i=0; i<UIPC_CH_NUM; i++)
        uipc_close_ch_locked(i);
}



/* check pending events in read task */
static void uipc_check_task_flags_locked(void)
{
    int i;

    for (i=0; i<UIPC_CH_NUM; i++)
    {
        //BTIF_TRACE_EVENT("CHECK TASK FLAGS %x %x",  uipc_main.ch[i].task_evt_flags, UIPC_TASK_FLAG_DISCONNECT_CHAN);
        if (uipc_main.ch[i].task_evt_flags & UIPC_TASK_FLAG_DISCONNECT_CHAN)
        {
            uipc_main.ch[i].task_evt_flags &= ~UIPC_TASK_FLAG_DISCONNECT_CHAN;
            uipc_close_ch_locked(i);
        }

        /* add here */

    }
}


static int uipc_check_fd_locked(tUIPC_CH_ID ch_id)
{
    if (ch_id >= UIPC_CH_NUM)
        return -1;

    //BTIF_TRACE_EVENT("CHECK SRVFD %d (ch %d)", uipc_main.ch[ch_id].srvfd, ch_id);

    if (SAFE_FD_ISSET(uipc_main.ch[ch_id].srvfd, &uipc_main.read_set))
    {
        debug("INCOMING CONNECTION ON CH %d", ch_id);

        uipc_main.ch[ch_id].fd = accept_server_socket(uipc_main.ch[ch_id].srvfd);

        debug("NEW FD %d", uipc_main.ch[ch_id].fd);

        if ((uipc_main.ch[ch_id].fd > 0) && uipc_main.ch[ch_id].cback)
        {
            /*  if we have a callback we should add this fd to the active set
                and notify user with callback event */
            debug("ADD FD %d TO ACTIVE SET", uipc_main.ch[ch_id].fd);
            FD_SET(uipc_main.ch[ch_id].fd, &uipc_main.active_set);
            uipc_main.max_fd = MAX(uipc_main.max_fd, uipc_main.ch[ch_id].fd);
        }

        if (uipc_main.ch[ch_id].fd < 0)
        {
            error("FAILED TO ACCEPT CH %d (%s)", ch_id, strerror(errno));
            return -1;
        }

        if (uipc_main.ch[ch_id].cback)
            uipc_main.ch[ch_id].cback(ch_id, UIPC_OPEN_EVT);
    }

    //BTIF_TRACE_EVENT("CHECK FD %d (ch %d)", uipc_main.ch[ch_id].fd, ch_id);

    if (SAFE_FD_ISSET(uipc_main.ch[ch_id].fd, &uipc_main.read_set))
    {
        //BTIF_TRACE_EVENT("INCOMING DATA ON CH %d", ch_id);

        if (uipc_main.ch[ch_id].cback)
            uipc_main.ch[ch_id].cback(ch_id, UIPC_RX_DATA_READY_EVT);
    }
    return 0;
}

static void uipc_check_interrupt_locked(void)
{
    if (SAFE_FD_ISSET(uipc_main.signal_fds[0], &uipc_main.read_set))
    {
        char sig_recv = 0;
        //BTIF_TRACE_EVENT("UIPC INTERRUPT");
        recv(uipc_main.signal_fds[0], &sig_recv, sizeof(sig_recv), MSG_WAITALL);
    }
}

static inline void uipc_wakeup_locked(void)
{
    char sig_on = 1;
    info("UIPC SEND WAKE UP");
    send(uipc_main.signal_fds[1], &sig_on, sizeof(sig_on), 0);
}

static int uipc_setup_server_locked(tUIPC_CH_ID ch_id, char *name, tUIPC_RCV_CBACK *cback)
{
    int fd;

    debug("SETUP CHANNEL SERVER %d", ch_id);

    if (ch_id >= UIPC_CH_NUM)
        return -1;

    UIPC_LOCK();

    fd = create_server_socket(name);

    if (fd < 0)
    {
        error("failed to setup %s", name, strerror(errno));
        UIPC_UNLOCK();
         return -1;
    }

    debug("ADD SERVER FD TO ACTIVE SET %d", fd);
    FD_SET(fd, &uipc_main.active_set);
    uipc_main.max_fd = MAX(uipc_main.max_fd, fd);

    uipc_main.ch[ch_id].srvfd = fd;
    uipc_main.ch[ch_id].cback = cback;
    uipc_main.ch[ch_id].read_poll_tmo_ms = DEFAULT_READ_POLL_TMO_MS;

    /* trigger main thread to update read set */
    uipc_wakeup_locked();

    UIPC_UNLOCK();

    return 0;
}

static void uipc_flush_ch_locked(tUIPC_CH_ID ch_id)
{
    char buf[UIPC_FLUSH_BUFFER_SIZE];
    struct pollfd pfd;
    int ret;

    pfd.events = POLLIN;
    pfd.fd = uipc_main.ch[ch_id].fd;

    if (uipc_main.ch[ch_id].fd == UIPC_DISCONNECTED)
    {
        debug("%s() - fd disconnected. Exiting", __FUNCTION__);
        return;
    }

    while (1)
    {
        ret = poll(&pfd, 1, 1);
        info("%s() - polling fd %d, revents: 0x%x, ret %d",
                __FUNCTION__, pfd.fd, pfd.revents, ret);

        if (pfd.revents & (POLLERR|POLLHUP))
        {
            debug("%s() - POLLERR or POLLHUP. Exiting", __FUNCTION__);
            return;
        }

        if (ret <= 0)
        {
            error("%s() - error (%d). Exiting", __FUNCTION__, ret);
            return;
        }

        /* read sufficiently large buffer to ensure flush empties socket faster than
           it is getting refilled */
        read(pfd.fd, &buf, UIPC_FLUSH_BUFFER_SIZE);
    }
}


static void uipc_flush_locked(tUIPC_CH_ID ch_id)
{
    if (ch_id >= UIPC_CH_NUM)
        return;

    switch(ch_id)
    {
        case UIPC_CH_ID_VOICE_DATA:
            uipc_flush_ch_locked(UIPC_CH_ID_VOICE_DATA);
            break;

        case UIPC_CH_ID_VOICE_CTRL:
            uipc_flush_ch_locked(UIPC_CH_ID_VOICE_CTRL);
            break;
    }
}


static int uipc_close_ch_locked(tUIPC_CH_ID ch_id)
{
    int wakeup = 0;

    debug("CLOSE CHANNEL %d", ch_id);

    if (ch_id >= UIPC_CH_NUM)
        return -1;

    if (uipc_main.ch[ch_id].srvfd != UIPC_DISCONNECTED)
    {
        info("CLOSE SERVER (FD %d)", uipc_main.ch[ch_id].srvfd);
        close(uipc_main.ch[ch_id].srvfd);
        FD_CLR(uipc_main.ch[ch_id].srvfd, &uipc_main.active_set);
        uipc_main.ch[ch_id].srvfd = UIPC_DISCONNECTED;
        wakeup = 1;
    }

    if (uipc_main.ch[ch_id].fd != UIPC_DISCONNECTED)
    {
        info("CLOSE CONNECTION (FD %d)", uipc_main.ch[ch_id].fd);
        close(uipc_main.ch[ch_id].fd);
        FD_CLR(uipc_main.ch[ch_id].fd, &uipc_main.active_set);
        uipc_main.ch[ch_id].fd = UIPC_DISCONNECTED;
        wakeup = 1;
    }

    /* notify this connection is closed */
    if (uipc_main.ch[ch_id].cback)
        uipc_main.ch[ch_id].cback(ch_id, UIPC_CLOSE_EVT);

    /* trigger main thread update if something was updated */
    if (wakeup)
        uipc_wakeup_locked();

    return 0;
}


void uipc_close_locked(tUIPC_CH_ID ch_id)
{
    if (uipc_main.ch[ch_id].srvfd == UIPC_DISCONNECTED)
    {
        warn("CHANNEL %d ALREADY CLOSED", ch_id);
        return;
    }

    /* schedule close on this channel */
    uipc_main.ch[ch_id].task_evt_flags |= UIPC_TASK_FLAG_DISCONNECT_CHAN;
    uipc_wakeup_locked();
}


static void uipc_read_task(void *arg)
{
    int ch_id;
    int result;

    prctl(PR_SET_NAME, (unsigned long)"uipc-main", 0, 0, 0);


    while (uipc_main.running)
    {
        uipc_main.read_set = uipc_main.active_set;

        result = select(uipc_main.max_fd+1, &uipc_main.read_set, NULL, NULL, NULL);

        if (result == 0)
        {
            info("select timeout");
            continue;
        }
        else if (result < 0)
        {
            warn("select failed %s", strerror(errno));
            continue;
        }

        UIPC_LOCK();

        /* clear any wakeup interrupt */
        uipc_check_interrupt_locked();

        /* check pending task events */
        uipc_check_task_flags_locked();


        /* check for other connections */
        for (ch_id = 0; ch_id < UIPC_CH_NUM; ch_id++)
        {
            uipc_check_fd_locked(ch_id);
        }

        UIPC_UNLOCK();
    }

    debug("UIPC READ THREAD EXITING");

    uipc_main_cleanup();

    uipc_main.tid = 0;

    debug("UIPC READ THREAD DONE");
}


int uipc_start_main_server_thread(void)
{
    uipc_main.running = 1;

    if (pthread_create(&uipc_main.tid, (const pthread_attr_t *) NULL, (void*)uipc_read_task, NULL) < 0)
    {
        error("uipc_thread_create pthread_create failed:%d", errno);
        return -1;
    }

    return 0;
}

/* blocking call */
void uipc_stop_main_server_thread(void)
{
    /* request shutdown of read thread */
    UIPC_LOCK();
    uipc_main.running = 0;
    uipc_wakeup_locked();
    UIPC_UNLOCK();

    /* wait until read thread is fully terminated */
    /* tid might hold pointer value where it's value
       is negative vaule with singed bit is set, so
       corrected the logic to check zero or non zero */
    if (uipc_main.tid)
        pthread_join(uipc_main.tid, NULL);
}

/*******************************************************************************
 **
 ** Function         UIPC_Init
 **
 ** Description      Initialize UIPC module
 **
 ** Returns          void
 **
 *******************************************************************************/

void UIPC_Init(void *p_data)
{
    debug("UIPC_Init");
    if (uipc_inited == 1)
    {
        return;
    }
    uipc_inited = 1;

    memset(&uipc_main, 0, sizeof(tUIPC_MAIN));

    uipc_main_init();

    uipc_start_main_server_thread();
}

/*******************************************************************************
 **
 ** Function         UIPC_Open
 **
 ** Description      Open UIPC interface
 **
 ** Returns          TRUE in case of success, FALSE in case of failure.
 **
 *******************************************************************************/
int UIPC_Open(tUIPC_CH_ID ch_id, tUIPC_RCV_CBACK *p_cback)
{
    debug("UIPC_Open : ch_id %d, p_cback %x", ch_id, p_cback);

    UIPC_LOCK();

    if (ch_id >= UIPC_CH_NUM)
    {
        UIPC_UNLOCK();
        return -1;
    }

    if (uipc_main.ch[ch_id].srvfd != UIPC_DISCONNECTED)
    {
        debug("CHANNEL %d ALREADY OPEN", ch_id);

        if(uipc_main.ch[ch_id].fd > 0)
        {
            debug("the client socket is not disconnect");
        }

        UIPC_UNLOCK();
        return 0;
    }

    switch(ch_id)
    {
        case UIPC_CH_ID_VOICE_CTRL:
            uipc_setup_server_locked(ch_id, VOICE_CTRL_PATH, p_cback);
            break;

        case UIPC_CH_ID_VOICE_DATA:
            uipc_setup_server_locked(ch_id, VOICE_DATA_PATH, p_cback);
            break;

    }

    UIPC_UNLOCK();

    return 0;
}

/*******************************************************************************
 **
 ** Function         UIPC_Close
 **
 ** Description      Close UIPC interface
 **
 ** Returns          void
 **
 *******************************************************************************/

void UIPC_Close(tUIPC_CH_ID ch_id)
{
    debug("UIPC_Close : ch_id %d", ch_id);

    /* special case handling uipc shutdown */
    if (ch_id != UIPC_CH_ID_ALL)
    {
        UIPC_LOCK();
        uipc_close_locked(ch_id);
        UIPC_UNLOCK();
    }
    else
    {

        int i;
        for (i=0; i<UIPC_CH_NUM; i++)
            uipc_close_ch_locked(i);

        info("UIPC_Close : waiting for shutdown to complete");
        uipc_stop_main_server_thread();
        debug("UIPC_Close : shutdown complete");
    }
}

/*******************************************************************************
 **
 ** Function         UIPC_Send
 **
 ** Description      Called to transmit a message over UIPC.
 **
 ** Returns          TRUE in case of success, FALSE in case of failure.
 **
 *******************************************************************************/
int UIPC_Send(tUIPC_CH_ID ch_id, unsigned short msg_evt, unsigned char *p_buf,
        unsigned short msglen)
{
    int ret = false;

    info("UIPC_Send : ch_id:%d %d bytes", ch_id, msglen);

    UIPC_LOCK();

    if (write(uipc_main.ch[ch_id].fd, p_buf, msglen) < 0)
    {
        error("failed to write (%s)", strerror(errno));
        ret = errno;
    }

    UIPC_UNLOCK();

    return ret;

}
int UIPC_Send_noblock(tUIPC_CH_ID ch_id, unsigned short msg_evt, unsigned char *p_buf, unsigned short msglen)
{
    int res = 0;
    struct pollfd pfd;
    int fd = uipc_main.ch[ch_id].fd;

    UIPC_LOCK();

    pfd.fd = fd;
    pfd.events = POLLOUT|POLLHUP;
    if (poll(&pfd, 1, 0) == 0)
    {
        goto unlock;
    }

    if (pfd.revents & (POLLHUP|POLLNVAL) )
    {
        debug("poll : channel detached remotely");
        uipc_close_locked(ch_id);
        goto unlock;
    }

    res = send(uipc_main.ch[ch_id].fd, p_buf, msglen, MSG_DONTWAIT);
    if (res < 0)
    {
        error("failed to write (%s)", strerror(errno));
    }

unlock:
    UIPC_UNLOCK();

    return res;
}


int UIPC_Read_noblock(tUIPC_CH_ID ch_id, unsigned short *p_msg_evt, unsigned char *p_buf, unsigned int len)
{
    int n;
    int n_read = 0;
    int fd = uipc_main.ch[ch_id].fd;
    struct pollfd pfd;

    if (ch_id >= UIPC_CH_NUM)
    {
        error("UIPC_Read_noblock : invalid ch id %d", ch_id);
        return 0;
    }

    if (fd == UIPC_DISCONNECTED)
    {
        error("UIPC_Read_noblock : channel %d closed", ch_id);
        return 0;
    }

    pfd.fd = fd;
    pfd.events = POLLIN|POLLHUP;

    if (poll(&pfd, 1, 0) == 0)
    {
        return 0;
    }

    if (pfd.revents & (POLLHUP|POLLNVAL) )
    {
        debug("poll : channel detached remotely");
        UIPC_LOCK();
        uipc_close_locked(ch_id);
        UIPC_UNLOCK();
        return 0;
    }

    n_read = recv(fd, p_buf, len, MSG_DONTWAIT|MSG_NOSIGNAL);

    return n_read;
}

/*******************************************************************************
 **
 ** Function         UIPC_Read
 **
 ** Description      Called to read a message from UIPC.
 **
 ** Returns          return the number of bytes read.
 **
 *******************************************************************************/

int UIPC_Read(tUIPC_CH_ID ch_id, unsigned short *p_msg_evt, unsigned char *p_buf, unsigned int len)
{
    int n;
    int n_read = 0;
    int fd = uipc_main.ch[ch_id].fd;
    struct pollfd pfd;


    if (ch_id >= UIPC_CH_NUM)
    {
        error("UIPC_Read : invalid ch id %d", ch_id);
        return 0;
    }

    if (fd == UIPC_DISCONNECTED)
    {
        error("UIPC_Read : channel %d closed", ch_id);
        return 0;
    }

    //BTIF_TRACE_DEBUG("UIPC_Read : ch_id %d, len %d, fd %d, polltmo %d", ch_id, len,
    //        fd, uipc_main.ch[ch_id].read_poll_tmo_ms);

    while (n_read < (int)len)
    {
        pfd.fd = fd;
        pfd.events = POLLIN|POLLHUP;

        /* make sure there is data prior to attempting read to avoid blocking
           a read for more than poll timeout */
        if (poll(&pfd, 1, uipc_main.ch[ch_id].read_poll_tmo_ms) == 0)
        {
            debug("poll timeout (%d ms)", uipc_main.ch[ch_id].read_poll_tmo_ms);
            break;
        }

        //BTIF_TRACE_EVENT("poll revents %x", pfd.revents);

        if (pfd.revents & (POLLHUP|POLLNVAL) )
        {
            debug("poll : channel detached remotely");
            UIPC_LOCK();
            uipc_close_locked(ch_id);
            UIPC_UNLOCK();
            return 0;
        }

        n = recv(fd, p_buf+n_read, len-n_read, 0);

        //BTIF_TRACE_EVENT("read %d bytes", n);

        if (n == 0)
        {
            debug("UIPC_Read : channel detached remotely");
            UIPC_LOCK();
            uipc_close_locked(ch_id);
            UIPC_UNLOCK();
            return 0;
        }

        if (n < 0)
        {
            warn("UIPC_Read : read failed (%s)", strerror(errno));
            return 0;
        }

        n_read+=n;

    }

    return n_read;
}

/*******************************************************************************
**
** Function         UIPC_Ioctl
**
** Description      Called to control UIPC.
**
** Returns          void
**
*******************************************************************************/

extern int UIPC_Ioctl(tUIPC_CH_ID ch_id, unsigned int request, void *param)
{
    info("#### UIPC_Ioctl : ch_id %d, request %d ####", ch_id, request);

    UIPC_LOCK();

    switch(request)
    {
        case UIPC_REQ_RX_FLUSH:
            uipc_flush_locked(ch_id);
            break;

        case UIPC_REG_CBACK:
            //BTIF_TRACE_EVENT("register callback ch %d srvfd %d, fd %d", ch_id, uipc_main.ch[ch_id].srvfd, uipc_main.ch[ch_id].fd);
            uipc_main.ch[ch_id].cback = (tUIPC_RCV_CBACK*)param;
            break;

        case UIPC_REG_REMOVE_ACTIVE_READSET:

            /* user will read data directly and not use select loop */
            if (uipc_main.ch[ch_id].fd != UIPC_DISCONNECTED)
            {
                /* remove this channel from active set */
                FD_CLR(uipc_main.ch[ch_id].fd, &uipc_main.active_set);

                /* refresh active set */
                uipc_wakeup_locked();
            }
            break;

        case UIPC_SET_READ_POLL_TMO:
            uipc_main.ch[ch_id].read_poll_tmo_ms = (intptr_t)param;
            debug("UIPC_SET_READ_POLL_TMO : CH %d, TMO %d ms", ch_id, uipc_main.ch[ch_id].read_poll_tmo_ms );
            break;

        case UIPC_SET_SNDBUFSIZE:
            info("UIPC_SET_SNDBUFSIZE : CH %d, size %d", ch_id, (int)param);
            if (uipc_main.ch[ch_id].fd != UIPC_DISCONNECTED)
            {
                setsockopt(uipc_main.ch[ch_id].fd , SOL_SOCKET, SO_SNDBUF, (char*)&param, (int)sizeof(int));
            }
            break;


        default:
            debug("UIPC_Ioctl : request not handled (%d)", request);
            break;
    }

    UIPC_UNLOCK();

    return -1;
}

