/******************************************************************************
 *
 *  Copyright (C) 2009-2012 Broadcom Corporation
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at:
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 ******************************************************************************/

/******************************************************************************
 *
 *  Filename:      hardware.c
 *
 *  Description:   Contains controller-specific functions, like
 *                      firmware patch download
 *                      low power mode operations
 *
 ******************************************************************************/

#define LOG_TAG "bt_hwcfg"

#include <utils/Log.h>
#include <sys/types.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <signal.h>
#include <time.h>
#include <errno.h>
#include <fcntl.h>
#include <dirent.h>
#include <ctype.h>
#include <cutils/properties.h>
#include <stdlib.h>
#include <string.h>
#include "bt_hci_bdroid.h"
#include "bt_vendor_aw.h"

/******************************************************************************
**  Constants & Macros
******************************************************************************/

#ifndef BTHW_DBG
#define BTHW_DBG FALSE
#endif

#if (BTHW_DBG == TRUE)
#define BTHWDBG(param, ...) {ALOGD(param, ## __VA_ARGS__);}
#else
#define BTHWDBG(param, ...) {}
#endif

#define FW_PATCHFILE_EXTENSION      ".hcd"
#define FW_PATCHFILE_EXTENSION_LEN  4
#define FW_PATCHFILE_PATH_MAXLEN    248 /* Local_Name length of return of
                                           HCI_Read_Local_Name */

#define HCI_CMD_MAX_LEN             258

#define HCI_RESET                               0x0C03
#define HCI_VSC_WRITE_UART_CLOCK_SETTING        0xFC45
#define HCI_VSC_UPDATE_BAUDRATE                 0xFC18
#define HCI_READ_LOCAL_NAME                     0x0C14
#define HCI_VSC_DOWNLOAD_MINIDRV                0xFC2E
#define HCI_VSC_WRITE_BD_ADDR                   0xFC01
#define HCI_VSC_WRITE_SLEEP_MODE                0xFC27
#define HCI_VSC_WRITE_SCO_PCM_INT_PARAM         0xFC1C
#define HCI_VSC_WRITE_PCM_DATA_FORMAT_PARAM     0xFC1E
#define HCI_VSC_WRITE_I2SPCM_INTERFACE_PARAM    0xFC6D
#define HCI_VSC_ENABLE_WBS                      0xFC7E
#define HCI_VSC_LAUNCH_RAM                      0xFC4E
#define HCI_READ_LOCAL_BDADDR                   0x1009

#define HCI_EVT_CMD_CMPL_STATUS_RET_BYTE        5
#define HCI_EVT_CMD_CMPL_LOCAL_NAME_STRING      6
#define HCI_EVT_CMD_CMPL_LOCAL_BDADDR_ARRAY     6
#define HCI_EVT_CMD_CMPL_OPCODE                 3
#define LPM_CMD_PARAM_SIZE                      12
#define UPDATE_BAUDRATE_CMD_PARAM_SIZE          6
#define HCI_CMD_PREAMBLE_SIZE                   3
#define HCD_REC_PAYLOAD_LEN_BYTE                2
#define BD_ADDR_LEN                             6
#define LOCAL_NAME_BUFFER_LEN                   32
#define LOCAL_BDADDR_PATH_BUFFER_LEN            256

#define STREAM_TO_UINT16(u16, p) {u16 = ((uint16_t)(*(p)) + (((uint16_t)(*((p) + 1))) << 8)); (p) += 2;}
#define UINT8_TO_STREAM(p, u8)   {*(p)++ = (uint8_t)(u8);}
#define UINT16_TO_STREAM(p, u16) {*(p)++ = (uint8_t)(u16); *(p)++ = (uint8_t)((u16) >> 8);}
#define UINT32_TO_STREAM(p, u32) {*(p)++ = (uint8_t)(u32); *(p)++ = (uint8_t)((u32) >> 8); *(p)++ = (uint8_t)((u32) >> 16); *(p)++ = (uint8_t)((u32) >> 24);}


/******************************************************************************
**  Local type definitions
******************************************************************************/

/* Hardware Configuration State */
enum {
    HW_CFG_START = 1,
    HW_CFG_SET_UART_CLOCK,
    HW_CFG_SET_UART_BAUD_1,
    HW_CFG_READ_LOCAL_NAME,
    HW_CFG_DL_MINIDRIVER,
    HW_CFG_DL_FW_PATCH,
    HW_CFG_SET_UART_BAUD_2,
    HW_CFG_SET_BD_ADDR
#if (USE_CONTROLLER_BDADDR == TRUE)
    , HW_CFG_READ_BD_ADDR
#endif
};

/* h/w config control block */
typedef struct
{
    uint8_t state;                          /* Hardware configuration state */
    int     fw_fd;                          /* FW patch file fd */
    uint8_t f_set_baud_2;                   /* Baud rate switch state */
    char    local_chip_name[LOCAL_NAME_BUFFER_LEN];
} bt_hw_cfg_cb_t;

/******************************************************************************
**  Externs
******************************************************************************/

void hw_config_cback(void *p_evt_buf);
extern uint8_t vnd_local_bd_addr[BD_ADDR_LEN];

static bt_hw_cfg_cb_t hw_cfg_cb;

/******************************************************************************
**  Controller Initialization Static Functions
******************************************************************************/

/*******************************************************************************
**
** Function        ms_delay
**
** Description     sleep unconditionally for timeout milliseconds
**
** Returns         None
**
*******************************************************************************/
void ms_delay (uint32_t timeout)
{
    struct timespec delay;
    int err;

    if (timeout == 0)
        return;

    delay.tv_sec = timeout / 1000;
    delay.tv_nsec = 1000 * 1000 * (timeout%1000);

    /* [u]sleep can't be used because it uses SIGALRM */
    do {
        err = nanosleep(&delay, &delay);
    } while (err < 0 && errno ==EINTR);
}

/*******************************************************************************
**
** Function         hw_config_set_bdaddr
**
** Description      Program controller's Bluetooth Device Address
**
** Returns          TRUE, if valid address is sent
**                  FALSE, otherwise
**
*******************************************************************************/
static uint8_t hw_config_set_bdaddr(HC_BT_HDR *p_buf)
{
    uint8_t retval = FALSE;
    uint8_t *p = (uint8_t *) (p_buf + 1);

    ALOGI("Setting local bd addr to %02X:%02X:%02X:%02X:%02X:%02X",
        vnd_local_bd_addr[0], vnd_local_bd_addr[1], vnd_local_bd_addr[2],
        vnd_local_bd_addr[3], vnd_local_bd_addr[4], vnd_local_bd_addr[5]);

    UINT16_TO_STREAM(p, HCI_VSC_WRITE_BD_ADDR);
    *p++ = BD_ADDR_LEN; /* parameter length */
    *p++ = vnd_local_bd_addr[5];
    *p++ = vnd_local_bd_addr[4];
    *p++ = vnd_local_bd_addr[3];
    *p++ = vnd_local_bd_addr[2];
    *p++ = vnd_local_bd_addr[1];
    *p = vnd_local_bd_addr[0];

    p_buf->len = HCI_CMD_PREAMBLE_SIZE + BD_ADDR_LEN;
    hw_cfg_cb.state = HW_CFG_SET_BD_ADDR;

    retval = bt_vendor_cbacks->xmit_cb(HCI_VSC_WRITE_BD_ADDR, p_buf, \
                                 hw_config_cback);

    return (retval);
}

#if (USE_CONTROLLER_BDADDR == TRUE)
/*******************************************************************************
**
** Function         hw_config_read_bdaddr
**
** Description      Read controller's Bluetooth Device Address
**
** Returns          TRUE, if valid address is sent
**                  FALSE, otherwise
**
*******************************************************************************/
static uint8_t hw_config_read_bdaddr(HC_BT_HDR *p_buf)
{
    uint8_t retval = FALSE;
    uint8_t *p = (uint8_t *) (p_buf + 1);

    UINT16_TO_STREAM(p, HCI_READ_LOCAL_BDADDR);
    *p = 0; /* parameter length */

    p_buf->len = HCI_CMD_PREAMBLE_SIZE;
    hw_cfg_cb.state = HW_CFG_READ_BD_ADDR;

    retval = bt_vendor_cbacks->xmit_cb(HCI_READ_LOCAL_BDADDR, p_buf, \
                                 hw_config_cback);

    return (retval);
}
#endif // (USE_CONTROLLER_BDADDR == TRUE)

/*******************************************************************************
**
** Function         hw_config_cback
**
** Description      Callback function for controller configuration
**
** Returns          None
**
*******************************************************************************/
void hw_config_cback(void *p_mem)
{
    HC_BT_HDR *p_evt_buf = (HC_BT_HDR *) p_mem;
    char        *p_name, *p_tmp;
    uint8_t     *p, status;
    uint16_t    opcode;
    HC_BT_HDR  *p_buf=NULL;
    uint8_t     is_proceeding = FALSE;
    int         i;
    int         delay=100;
#if (USE_CONTROLLER_BDADDR == TRUE)
    const uint8_t null_bdaddr[BD_ADDR_LEN] = {0,0,0,0,0,0};
#endif

    status = *((uint8_t *)(p_evt_buf + 1) + HCI_EVT_CMD_CMPL_STATUS_RET_BYTE);
    p = (uint8_t *)(p_evt_buf + 1) + HCI_EVT_CMD_CMPL_OPCODE;
    STREAM_TO_UINT16(opcode,p);

    /* Ask a new buffer big enough to hold any HCI commands sent in here */
    if ((status == 0) && bt_vendor_cbacks)
        p_buf = (HC_BT_HDR *) bt_vendor_cbacks->alloc(BT_HC_HDR_SIZE + \
                                                       HCI_CMD_MAX_LEN);

    if (p_buf != NULL)
    {
        p_buf->event = MSG_STACK_TO_HC_HCI_CMD;
        p_buf->offset = 0;
        p_buf->len = 0;
        p_buf->layer_specific = 0;

        p = (uint8_t *) (p_buf + 1);

        switch (hw_cfg_cb.state)
        {
            case HW_CFG_START:
                is_proceeding = hw_config_read_bdaddr(p_buf);
                break;

            case HW_CFG_SET_BD_ADDR:
                ALOGI("vendor lib fwcfg completed");
                bt_vendor_cbacks->dealloc(p_buf);
                bt_vendor_cbacks->fwcfg_cb(BT_VND_OP_RESULT_SUCCESS);

                hw_cfg_cb.state = 0;

                is_proceeding = TRUE;
                break;

#if (USE_CONTROLLER_BDADDR == TRUE)
            case HW_CFG_READ_BD_ADDR:
                p_tmp = (char *) (p_evt_buf + 1) + \
                         HCI_EVT_CMD_CMPL_LOCAL_BDADDR_ARRAY;

                if (memcmp(p_tmp, null_bdaddr, BD_ADDR_LEN) == 0)
                {
                    // Controller does not have a valid OTP BDADDR!
                    // Set the BTIF initial BDADDR instead.
                    if ((is_proceeding = hw_config_set_bdaddr(p_buf)) == TRUE)
                        break;
                }
                else
                {
                    ALOGI("Controller OTP bdaddr %02X:%02X:%02X:%02X:%02X:%02X",
                        *(p_tmp+5), *(p_tmp+4), *(p_tmp+3),
                        *(p_tmp+2), *(p_tmp+1), *p_tmp);
                }

                ALOGI("vendor lib fwcfg completed");
                bt_vendor_cbacks->dealloc(p_buf);
                bt_vendor_cbacks->fwcfg_cb(BT_VND_OP_RESULT_SUCCESS);

                hw_cfg_cb.state = 0;

                is_proceeding = TRUE;
                break;
#endif // (USE_CONTROLLER_BDADDR == TRUE)
        } // switch(hw_cfg_cb.state)
    } // if (p_buf != NULL)

    /* Free the RX event buffer */
    if (bt_vendor_cbacks)
        bt_vendor_cbacks->dealloc(p_evt_buf);

    if (is_proceeding == FALSE)
    {
        ALOGE("vendor lib fwcfg aborted!!!");
        if (bt_vendor_cbacks)
        {
            if (p_buf != NULL)
                bt_vendor_cbacks->dealloc(p_buf);

            bt_vendor_cbacks->fwcfg_cb(BT_VND_OP_RESULT_FAIL);
        }

        hw_cfg_cb.state = 0;
    }
}

/*******************************************************************************
**
** Function        hw_config_start
**
** Description     Kick off controller initialization process
**
** Returns         None
**
*******************************************************************************/
void hw_config_start(void)
{
    HC_BT_HDR  *p_buf = NULL;
    uint8_t     *p;

    hw_cfg_cb.state = 0;
    hw_cfg_cb.fw_fd = -1;
    hw_cfg_cb.f_set_baud_2 = FALSE;

    /* Start from sending HCI_RESET */

    if (bt_vendor_cbacks)
    {
        p_buf = (HC_BT_HDR *) bt_vendor_cbacks->alloc(BT_HC_HDR_SIZE + \
                                                       HCI_CMD_PREAMBLE_SIZE);
    }

    if (p_buf)
    {
        p_buf->event = MSG_STACK_TO_HC_HCI_CMD;
        p_buf->offset = 0;
        p_buf->layer_specific = 0;
        p_buf->len = HCI_CMD_PREAMBLE_SIZE;

        p = (uint8_t *) (p_buf + 1);
        UINT16_TO_STREAM(p, HCI_RESET);
        *p = 0; /* parameter length */

        hw_cfg_cb.state = HW_CFG_START;

        bt_vendor_cbacks->xmit_cb(HCI_RESET, p_buf, hw_config_cback);
    }
    else
    {
        if (bt_vendor_cbacks)
        {
            ALOGE("vendor lib fw conf aborted [no buffer]");
            bt_vendor_cbacks->fwcfg_cb(BT_VND_OP_RESULT_FAIL);
        }
    }
}

#if (SCO_CFG_INCLUDED == TRUE)
/*******************************************************************************
**
** Function         hw_sco_config
**
** Description      Configure SCO related hardware settings
**
** Returns          None
**
*******************************************************************************/
void hw_sco_config(void)
{
    if (bt_vendor_cbacks)
    {
        bt_vendor_cbacks->scocfg_cb(BT_VND_OP_RESULT_SUCCESS);
    }
}
#endif

/*****************************************************************************
**   Sample Codes Section
*****************************************************************************/

#if (HW_END_WITH_HCI_RESET == TRUE)
/*******************************************************************************
**
** Function         hw_epilog_cback
**
** Description      Callback function for Command Complete Events from HCI
**                  commands sent in epilog process.
**
** Returns          None
**
*******************************************************************************/
void hw_epilog_cback(void *p_mem)
{
    HC_BT_HDR   *p_evt_buf = (HC_BT_HDR *) p_mem;
    uint8_t     *p, status;
    uint16_t    opcode;

    status = *((uint8_t *)(p_evt_buf + 1) + HCI_EVT_CMD_CMPL_STATUS_RET_BYTE);
    p = (uint8_t *)(p_evt_buf + 1) + HCI_EVT_CMD_CMPL_OPCODE;
    STREAM_TO_UINT16(opcode,p);

    BTHWDBG("%s Opcode:0x%04X Status: %d", __FUNCTION__, opcode, status);

    if (bt_vendor_cbacks)
    {
        /* Must free the RX event buffer */
        bt_vendor_cbacks->dealloc(p_evt_buf);

        /* Once epilog process is done, must call epilog_cb callback
           to notify caller */
        bt_vendor_cbacks->epilog_cb(BT_VND_OP_RESULT_SUCCESS);
    }
}

/*******************************************************************************
**
** Function         hw_epilog_process
**
** Description      Sample implementation of epilog process
**
** Returns          None
**
*******************************************************************************/
void hw_epilog_process(void)
{
    HC_BT_HDR  *p_buf = NULL;
    uint8_t     *p;

    BTHWDBG("hw_epilog_process");

    /* Sending a HCI_RESET */
    if (bt_vendor_cbacks)
    {
        /* Must allocate command buffer via HC's alloc API */
        p_buf = (HC_BT_HDR *) bt_vendor_cbacks->alloc(BT_HC_HDR_SIZE + \
                                                       HCI_CMD_PREAMBLE_SIZE);
    }

    if (p_buf)
    {
        p_buf->event = MSG_STACK_TO_HC_HCI_CMD;
        p_buf->offset = 0;
        p_buf->layer_specific = 0;
        p_buf->len = HCI_CMD_PREAMBLE_SIZE;

        p = (uint8_t *) (p_buf + 1);
        UINT16_TO_STREAM(p, HCI_RESET);
        *p = 0; /* parameter length */

        /* Send command via HC's xmit_cb API */
        bt_vendor_cbacks->xmit_cb(HCI_RESET, p_buf, hw_epilog_cback);
    }
    else
    {
        if (bt_vendor_cbacks)
        {
            ALOGE("vendor lib epilog process aborted [no buffer]");
            bt_vendor_cbacks->epilog_cb(BT_VND_OP_RESULT_FAIL);
        }
    }
}
#endif // (HW_END_WITH_HCI_RESET == TRUE)
