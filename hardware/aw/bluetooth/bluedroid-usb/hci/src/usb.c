/******************************************************************************
 *
 *  Copyright (C) 2009-2012 Broadcom Corporation
 *  Portions of file: Copyright (C) 2013, Intel Corporation
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
 *  Filename:      usb.c
 *
 *  Description:   Contains open/read/write/close functions on usb
 *
 ******************************************************************************/

#define LOG_TAG "bt_usb"

#include <utils/Log.h>
#include <pthread.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/resource.h>
#include <string.h>
#include "bt_hci_bdroid.h"
#include "usb.h"
#include "bt_vendor_lib.h"
#include <sys/prctl.h>
#include <libusb/libusb.h>

//#include "buffer_allocator.h"
#include "osi/include/fixed_queue.h"
#include "osi/include/thread.h"
#include "osi/include/log.h"
#include "osi/include/compat.h"
#include "osi/include/osi.h"
#include "osi/include/alarm.h"

/******************************************************************************
**  Constants & Macros
******************************************************************************/

#ifndef USB_DBG
#define USB_DBG FALSE
#endif

#if (USB_DBG == TRUE)
#define USBDBG ALOGD
#define USBERR ALOGE
#else
#define USBDBG
#define USBERR
#endif

/*
 * Bit masks : To check the transfer status
 */
#define XMITTED                 1
#define RX_DEAD                 2
#define RX_FAILED               4
#define XMIT_FAILED             8

/*
 * Field index values
 */
#define EV_LEN_FIELD        1
#define BLK_LEN_LO          2
#define BLK_LEN_HI          3
#define SCO_LEN_FIELD       2

#define BT_CTRL_EP      0x0
#define BT_INT_EP       0x81
#define BT_BULK_IN      0x82
#define BT_BULK_OUT     0x02
#define BT_ISO_IN       0x83
#define BT_ISO_OUT      0x03


#define BT_HCI_MAX_FRAME_SIZE      1028

#define BT_MAX_ISO_FRAMES       5

#define H4_TYPE_COMMAND         1
#define H4_TYPE_ACL_DATA        2
#define H4_TYPE_SCO_DATA        3
#define H4_TYPE_EVENT           4

/*
 * USB types, the second of three bRequestType fields
 */
#define USB_TYPE_REQ                 32

/* Preamble length for HCI Commands:
**      2-bytes for opcode and 1 byte for length
*/
#define HCI_CMD_PREAMBLE_SIZE   3

/* Preamble length for HCI Events:
**      1-byte for opcode and 1 byte for length
*/
#define HCI_EVT_PREAMBLE_SIZE   2

/* Preamble length for SCO Data:
**      2-byte for Handle and 1 byte for length
*/
#define HCI_SCO_PREAMBLE_SIZE   3

/* Preamble length for ACL Data:
**      2-byte for Handle and 2 byte for length
*/
#define HCI_ACL_PREAMBLE_SIZE   4
#define RX_NEW_PKT              1
#define RECEIVING_PKT           2

#define CONTAINER_RX_HDR(ptr) \
      (RX_HDR *)((char *)(ptr) - offsetof(RX_HDR, data))

#define CONTAINER_ISO_HDR(ptr) \
      (ISO_HDR *)((char *)(ptr) - offsetof(ISO_HDR, data))

#define CONTAINER_CMD_HDR(ptr) \
      (CMD_HDR *)((char *)(ptr) - offsetof(CMD_HDR, data))

/******************************************************************************
**  Local type definitions
******************************************************************************/
/*
The mutex is protecting send_rx_event and rxed_xfer.

rxed_xfer     : Accounting the packet received at recv_xfer_cb() and processed
                at usb_read().
send_rx_event : usb_read() signals recv_xfer_cb() to signal  the
                Host/Controller lib thread about new packet arrival.

usb_read() belongs to Host/Controller lib thread.
recv_xfer_cb() belongs to USB read thread
*/

typedef struct
{
    libusb_device_handle      *handle;
    pthread_t                 read_thread;
    pthread_mutex_t           mutex;
    pthread_cond_t            cond;
    int                       rxed_xfer;
    uint8_t                   send_rx_event;
    int16_t                   rx_pkt_len;
    uint8_t                   rx_status;
    int                       iso_frame_ndx;
    struct libusb_transfer    *failed_tx_xfer;
} tUSB_CB;

static fixed_queue_t             *rx_event_queue = NULL;
static fixed_queue_t             *rx_bulk_queue = NULL;
static fixed_queue_t             *rx_iso_queue = NULL;

static pthread_cond_t            xfer_cond;
static pthread_mutex_t           xfer_mutex;
/******************************************************************************
**  Static variables
******************************************************************************/
/* The list will grow and will be updated from btusb.c in kernel */
static struct bt_usb_device btusb_table[] = {
    /* Generic Bluetooth USB device */
    { BT_USB_DEVICE_INFO(0xe0, 0x01, 0x01) },
    { BT_USB_DEVICE_INFO(0xff, 0x01, 0x01) },
    { }     /* Terminating entry */
};

typedef struct
{
    uint16_t          event;
    uint16_t          len;
    uint16_t          offset;
    unsigned char     data[0];
} RX_HDR;

struct iso_frames
{
    int               actual_length;
    int               length;
};

typedef struct
{
    uint16_t           event;
    uint16_t           len;
    uint16_t           offset;
    struct iso_frames  frames[BT_MAX_ISO_FRAMES];
    unsigned char      data[0];
} ISO_HDR;

typedef struct
{
    uint8_t                     event;
    struct libusb_control_setup setup;
    unsigned char               data[0];
} CMD_HDR;

static tUSB_CB usb;
static int usb_xfer_status, usb_running;
static int intr_pkt_size, iso_pkt_size, bulk_pkt_size;
static int intr_pkt_size_wh, iso_pkt_size_wh, bulk_pkt_size_wh;
static struct libusb_transfer *data_rx_xfer, *event_rx_xfer, *iso_rx_xfer,
                              *xmit_transfer;
static int xmited_len;
RX_HDR *p_rx_hdr = NULL;

static thread_t *thread; // Not owned by us
static thread_t *usb_thread; // Not owned by us
extern void event_usb_has_bytes(void *context);
static my_allocator_t *bt_hc_cbacks;
typedef void* TRANSAC;
static int acl_packet_ready_count = 0;
static int event_packet_ready_count = 0;

static int use_altsetting = 0;

static void usb_handle_rx_failed(UNUSED_ATTR void *context);
static void usb_handle_xmit_failed(UNUSED_ATTR void *context);
/******************************************************************************
**  Static functions
******************************************************************************/
static int is_usb_match_idtable(struct bt_usb_device *id, struct libusb_device_descriptor *desc)
{
    int ret = TRUE;

    ret = ((id->bDevClass != libusb_le16_to_cpu(desc->bDeviceClass)) ? FALSE :
           (id->bDevSubClass != libusb_le16_to_cpu(desc->bDeviceSubClass)) ? FALSE :
           (id->bDevProtocol != libusb_le16_to_cpu(desc->bDeviceProtocol)) ? FALSE : TRUE);

    return ret;
}

static int check_bt_usb_endpoints(struct bt_usb_device *id, struct libusb_config_descriptor *cfg_desc)
{
    const struct libusb_endpoint_descriptor *endpoint;
    int i, num_altsetting;

    // 检查接口0的端点
    endpoint = cfg_desc->interface[0].altsetting[0].endpoint;
    for (i = 0; i < cfg_desc->interface[0].altsetting[0].bNumEndpoints; i++)
    {
        if(!(endpoint[i].bEndpointAddress == BT_CTRL_EP || \
              endpoint[i].bEndpointAddress == BT_INT_EP || \
              endpoint[i].bEndpointAddress == BT_BULK_IN || \
              endpoint[i].bEndpointAddress == BT_BULK_OUT))
            return FALSE;
    }

    // 获取接口1含有的可选设置的个数
    num_altsetting = cfg_desc->interface[1].num_altsetting;
    // 使用第个可选设置
    use_altsetting = num_altsetting - 1;
    endpoint = cfg_desc->interface[1].altsetting[use_altsetting].endpoint;
    for (i = 0; i < cfg_desc->interface[1].altsetting[use_altsetting].bNumEndpoints; i++)
    {
        if(!(endpoint[i].bEndpointAddress == BT_ISO_IN || \
              endpoint[i].bEndpointAddress == BT_ISO_OUT))
            return FALSE;

        if (endpoint[i].bEndpointAddress == BT_ISO_IN)
        {
            iso_pkt_size =  libusb_le16_to_cpu(endpoint[i].wMaxPacketSize);
            ALOGI("%s(), iso pkt size is %d", __FUNCTION__, iso_pkt_size);
            iso_pkt_size_wh = iso_pkt_size * BT_MAX_ISO_FRAMES + sizeof(ISO_HDR);
            //ALOGI("iso pkt size wh %d", iso_pkt_size_wh);
        }
    }

/*
    // 获取第num_altsetting-1个可选设置的端点大小
    for (i = 0; i < cfg_desc->interface[1].altsetting[num_altsetting-1].bNumEndpoints; i++)
    {
        if (endpoint[i].bEndpointAddress == BT_ISO_IN)
        {
            iso_pkt_size =  libusb_le16_to_cpu(endpoint[i].wMaxPacketSize);
            ALOGI("%s(), iso pkt size is %d", __FUNCTION__, iso_pkt_size);
            iso_pkt_size_wh = iso_pkt_size * BT_MAX_ISO_FRAMES + sizeof(ISO_HDR);
            //ALOGI("iso pkt size wh %d", iso_pkt_size_wh);
        }
    }
*/

    return TRUE;
}

static int is_btusb_device(struct libusb_device *dev)
{
    struct bt_usb_device *id;
    struct libusb_device_descriptor desc;
    struct libusb_config_descriptor *cfg_desc;
    int    r, match;

    r = libusb_get_device_descriptor(dev, &desc);
    if (r < 0)
        return FALSE;

    match = 0;

    for (id = btusb_table; id->bDevClass; id++)
    {
        if (is_usb_match_idtable (id, &desc) == TRUE)
        {
                match = 1;
                break;
        }
    }

    if (!match)
    {
        return FALSE;
    }

    r = libusb_get_config_descriptor(dev, 0, &cfg_desc);
    if (r < 0)
    {
        ALOGE("libusb_get_config_descriptor  %x:%x failed ....%d\n", \
               desc.idVendor, desc.idProduct, r);
        return FALSE;
    }

    r = check_bt_usb_endpoints(id, cfg_desc);
    libusb_free_config_descriptor(cfg_desc);

    return r;
}

/*******************************************************************************
**
** Function        libusb_open_bt_device
**
** Description     Scan the system USB devices. If match is found on
**                 btusb_table ensure that  it is a bluetooth device by
**                 checking Interface endpoint addresses.
**
** Returns         NULL: termination
**                 !NULL : pointer to the libusb_device_handle
**
*******************************************************************************/
static libusb_device_handle *libusb_open_bt_device()
{
    struct libusb_device **devs;
    struct libusb_device *dev;
    struct libusb_device_handle *handle = NULL;
    int    r, i;

    if (libusb_get_device_list(NULL, &devs) < 0)
    {
        return NULL;
    }
    for (i = 0; (dev = devs[i]) != NULL; i++)
    {
        if (is_btusb_device (dev) == TRUE)
            break;
    }
    if (dev)
    {
        r = libusb_open(dev, &handle);
        if (r < 0)
        {
            ALOGE("found USB BT device failed to open .....\n");
            return NULL;
        }
    }
    else
    {
        ALOGE("No matching USB BT device found .....\n");
        return NULL;
    }

    libusb_free_device_list(devs, 1);
    r = libusb_claim_interface(handle, 0);
    if (r < 0)
    {
        ALOGE("usb_claim_interface 0 error %d\n", r);
        return NULL;
    }

    intr_pkt_size = libusb_get_max_packet_size(dev, BT_INT_EP);
    //ALOGI("Interrupt pkt size is %d", intr_pkt_size);
    intr_pkt_size_wh =  intr_pkt_size + sizeof(RX_HDR);

    r = libusb_claim_interface(handle, 1);
    if (r < 0)
    {
        ALOGE("usb_claim_interface 1 error %d\n", r);
    }

    if (libusb_set_interface_alt_setting(handle, 1, use_altsetting) == 0) {
        //iso_pkt_size = libusb_get_max_iso_packet_size(dev, BT_ISO_IN);
        //ALOGI("%s(), iso_pkt_size = %d", __FUNCTION__, iso_pkt_size);
        //iso_pkt_size_wh = iso_pkt_size * BT_MAX_ISO_FRAMES + sizeof(ISO_HDR);
    } else {
        ALOGE("%s(), libusb_set_interface_alt_setting failed...", __FUNCTION__);
    }

    return handle;
}




/////////////////////////////////////////////////////////////////////




void usb_hal_packet_finished()
{
    ALOGI("%s(), usb.rxed_xfer = %d.", __FUNCTION__, usb.rxed_xfer);

    if (usb.rxed_xfer > 0)
    {
        thread_post(thread, event_usb_has_bytes, NULL);
    }
}


// Notify upper layer to read data.
static void usb_rx_signal_event()
{
/*
    //ALOGI("%s().", __FUNCTION__);
    pthread_mutex_lock(&usb.mutex);
    usb.rxed_xfer++;
    pthread_cond_signal(&usb.cond);
    if (usb.send_rx_event == TRUE)
    {
        //ALOGI("thread_post: event_usb_has_bytes");
        thread_post(thread, event_usb_has_bytes, NULL);
        usb.send_rx_event = FALSE;
    }
    pthread_mutex_unlock(&usb.mutex);
*/

    pthread_mutex_lock(&usb.mutex);
    usb.rxed_xfer++;
    pthread_cond_signal(&usb.cond);
    thread_post(thread, event_usb_has_bytes, NULL);
    pthread_mutex_unlock(&usb.mutex);
}



static void recv_event_data_cb(struct libusb_transfer *transfer)
{
    RX_HDR *p_rx = NULL;
    int r, skip = 0;
    enum libusb_transfer_status status;

    //ALOGI("%s().", __FUNCTION__);
    status = transfer->status;
    if (status == LIBUSB_TRANSFER_COMPLETED) {
        if (transfer->actual_length == 0) {
           skip = 1;
        } else {
            p_rx = CONTAINER_RX_HDR(transfer->buffer);
            p_rx->event = H4_TYPE_EVENT;
            p_rx->len = (uint16_t)transfer->actual_length;
            // 将接收到的EVENT放入队列
            fixed_queue_enqueue(rx_event_queue, p_rx);
            // 重新分配内存
            p_rx =  (RX_HDR *) bt_hc_cbacks->alloc(intr_pkt_size_wh);
            transfer->buffer = p_rx->data;
            transfer->length = intr_pkt_size;

            event_packet_ready_count++;
        }
    } else {
        ALOGE("%s(), ******Transfer to BT Device failed %d *****", __FUNCTION__, status);
        usb_xfer_status |= RX_DEAD;
        return;
    }

    if (!skip)
        usb_rx_signal_event();

    //重新提交，以便下一次接收数据
    r = libusb_submit_transfer(transfer);
    if (r < 0) {
        p_rx = CONTAINER_RX_HDR(transfer->buffer);
        bt_hc_cbacks->dealloc(p_rx);
        transfer->buffer = NULL;
        ALOGE("%s(), libusb_submit_transfer : %d : %d : failed", __FUNCTION__, transfer->endpoint, transfer->status);
        usb_xfer_status |= RX_FAILED;
        thread_post(usb_thread, usb_handle_rx_failed, NULL);
    }
}

static void recv_acl_data_cb(struct libusb_transfer *transfer)
{
    RX_HDR *p_rx = NULL;
    int r, skip = 0;
    enum libusb_transfer_status status;

    //ALOGI("%s().", __FUNCTION__);
    status = transfer->status;
    if (status == LIBUSB_TRANSFER_COMPLETED) {
        if (transfer->actual_length == 0) {
           skip = 1;
        } else {
            p_rx = CONTAINER_RX_HDR(transfer->buffer);
            p_rx->event = H4_TYPE_ACL_DATA;
            p_rx->len = (uint16_t)transfer->actual_length;
            // 将接收到的数据放入队列
            fixed_queue_enqueue(rx_bulk_queue, p_rx);
            // 重新分配内存
            p_rx =  (RX_HDR *) bt_hc_cbacks->alloc(bulk_pkt_size_wh);
            transfer->buffer = p_rx->data;
            transfer->length = bulk_pkt_size;

            acl_packet_ready_count++;
            //ALOGI("%s(), acl_packet_ready_count: %d", __FUNCTION__, acl_packet_ready_count);
        }
    } else {
        ALOGE("%s(), ******Transfer to BT Device failed %d *****", __FUNCTION__, status);
        usb_xfer_status |= RX_DEAD;
        return;
    }

    if (!skip)
        usb_rx_signal_event();

    //重新提交，以便下一次接收数据
    r = libusb_submit_transfer(transfer);
    if (r < 0) {
        p_rx = CONTAINER_RX_HDR(transfer->buffer);
        bt_hc_cbacks->dealloc(p_rx);
        transfer->buffer = NULL;
        ALOGE("%s(), libusb_submit_transfer : %d : %d : failed", __FUNCTION__, transfer->endpoint, transfer->status);
        usb_xfer_status |= RX_FAILED;
        thread_post(usb_thread, usb_handle_rx_failed, NULL);
    }
}



static void recv_iso_data_cb(struct libusb_transfer *transfer)
{
    ISO_HDR *p_iso = NULL;
    RX_HDR *p_rx = NULL;
    struct iso_frames *frames;
    int r, i, skip = 0, len = 0;
    enum libusb_transfer_status status;

    ALOGI("%s().", __FUNCTION__);
    status = transfer->status;
    if (status == LIBUSB_TRANSFER_COMPLETED) {
        if (transfer->actual_length == 0) {
            skip = 1;
            ALOGE("%s(), transfer->actual_length = 0", __FUNCTION__);
        } else {
            ALOGE("%s(), transfer->actual_length = %d", __FUNCTION__, transfer->actual_length);
            p_iso = CONTAINER_ISO_HDR(transfer->buffer);
            frames = p_iso->frames;
            memset(frames, 0, sizeof(struct iso_frames) * BT_MAX_ISO_FRAMES);
            p_iso->event = H4_TYPE_SCO_DATA;
            for(i = 0; i < transfer->num_iso_packets; i++, frames++) {
                frames->length = transfer->iso_packet_desc[i].length;
                frames->actual_length = transfer->iso_packet_desc[i].actual_length;
                len += frames->actual_length;
            }
            p_iso->len = (uint16_t)len;
            // 将接收到的数据放入队列
            fixed_queue_enqueue(rx_iso_queue, p_iso);
            // 重新分配内存
            p_iso = (ISO_HDR *) bt_hc_cbacks->alloc(iso_pkt_size_wh);
            transfer->buffer = p_iso->data;
            for(i = 0; i < BT_MAX_ISO_FRAMES; i++) {
                transfer->iso_packet_desc[i].length = iso_pkt_size;
            }
            //libusb_set_iso_packet_lengths(transfer, iso_pkt_size);
        }
    } else {
        ALOGE("%s(), ******Transfer to BT Device failed %d *****", __FUNCTION__, status);
        usb_xfer_status |= RX_DEAD;
        return;
    }

    //if (!skip)
    //    usb_rx_signal_event();

    //重新提交，以便下一次接收数据
    r = libusb_submit_transfer(transfer);
    if (r < 0) {
        p_rx = (RX_HDR *)CONTAINER_ISO_HDR(transfer->buffer);
        bt_hc_cbacks->dealloc(p_rx);
        transfer->buffer = NULL;
        ALOGE("%s(), libusb_submit_transfer : %d : %d : failed", __FUNCTION__, transfer->endpoint, transfer->status);
        usb_xfer_status |= RX_FAILED;
        thread_post(usb_thread, usb_handle_rx_failed, NULL);
    }
}




static void usb_handle_rx_failed(UNUSED_ATTR void *context)
{
    RX_HDR  *rx_buf;
    ISO_HDR *iso_buf;
    int  r, i, iso_xfer = 0;
    struct libusb_transfer *transfer = NULL;

    ALOGE("%s().", __FUNCTION__);
    if (data_rx_xfer->buffer == NULL) {
        transfer = data_rx_xfer;
        rx_buf = (RX_HDR *) bt_hc_cbacks->alloc(bulk_pkt_size_wh);
        if (rx_buf == NULL) {
            ALOGE("%s : Allocation failed", __FUNCTION__);
            transfer = NULL;
        } else {
            transfer->buffer = rx_buf->data;
            transfer->length = bulk_pkt_size;
        }
    } else if (event_rx_xfer->buffer == NULL) {
        transfer = event_rx_xfer;
        rx_buf = (RX_HDR *) bt_hc_cbacks->alloc(intr_pkt_size_wh);
        if (rx_buf == NULL) {
            ALOGE("%s : Allocation failed", __FUNCTION__);
            transfer = NULL;
        } else {
            transfer->buffer = rx_buf->data;
            transfer->length = intr_pkt_size;
        }
    } else if (iso_rx_xfer->buffer == NULL) {
        transfer = iso_rx_xfer;
        iso_buf = (ISO_HDR *) bt_hc_cbacks->alloc(iso_pkt_size_wh);
        if (iso_buf == NULL) {
            ALOGE("%s : Allocation failed", __FUNCTION__);
            transfer = NULL;
        } else {
            transfer->buffer = iso_buf->data;
            transfer->length = BT_MAX_ISO_FRAMES * iso_pkt_size;
            for(i = 0; i < transfer->num_iso_packets; i++) {
                transfer->iso_packet_desc[i].length = iso_pkt_size;
            }
            iso_xfer = 1;
        }
    }

    if (transfer != NULL) {
        usb_xfer_status &= ~(RX_FAILED);
        r = libusb_submit_transfer(transfer);
        if (r < 0) {
            ALOGE("libusb_submit_transfer : data_rx_xfer failed");
            if (iso_xfer)
                bt_hc_cbacks->dealloc(iso_buf);
            else
                bt_hc_cbacks->dealloc(rx_buf);
            transfer->buffer = NULL;
        }
    }
}

static void usb_handle_xmit_failed(UNUSED_ATTR void *context)
{
    struct libusb_transfer *transfer = NULL;

    ALOGE("%s().", __FUNCTION__);
    transfer = usb.failed_tx_xfer;
    xmited_len = transfer->length;
    usb_xfer_status &= ~(XMIT_FAILED);
    if (libusb_submit_transfer(transfer) < 0) {
        ALOGE("libusb_submit_transfer : %d : failed", *(transfer->buffer - 1));
    }
}



// loop for receive event, acl, etc.
static void usb_loop(UNUSED_ATTR void *context)
{
    struct timeval timeout = { 1, 0 };
    usb_xfer_status &= ~RX_DEAD;
    while (!(usb_xfer_status & RX_DEAD)) {
        libusb_handle_events_timeout(NULL, &timeout);
    }
}



static void usb_init_later(UNUSED_ATTR void *context)
{
    RX_HDR  *rx_buf = NULL;
    ISO_HDR *iso_buf = NULL;
    int r;
    unsigned char *buf;

    rx_buf = (RX_HDR *) bt_hc_cbacks->alloc(bulk_pkt_size_wh);
    buf =  rx_buf->data;
    libusb_fill_bulk_transfer(data_rx_xfer, usb.handle, BT_BULK_IN, \
        buf, bulk_pkt_size, recv_acl_data_cb, NULL, 0);
    r = libusb_submit_transfer(data_rx_xfer);
    if (r < 0) {
        ALOGE("%s(), libusb_submit_transfer : data_rx_xfer : failed", __FUNCTION__);
        goto out;
    }

    rx_buf = (RX_HDR *) bt_hc_cbacks->alloc(intr_pkt_size_wh);
    buf = rx_buf->data;
    libusb_fill_interrupt_transfer(event_rx_xfer, usb.handle, BT_INT_EP, \
          buf, intr_pkt_size, recv_event_data_cb, NULL, 0);
    r = libusb_submit_transfer(event_rx_xfer);
    if (r < 0) {
        ALOGE("%s(), libusb_submit_transfer : event_rx_xfer : failed", __FUNCTION__);
        goto out;
    }

    iso_buf = (ISO_HDR *) bt_hc_cbacks->alloc(iso_pkt_size_wh);
    buf = iso_buf->data;
    libusb_fill_iso_transfer(iso_rx_xfer, usb.handle, BT_ISO_IN, buf, \
          iso_pkt_size * BT_MAX_ISO_FRAMES, BT_MAX_ISO_FRAMES, recv_iso_data_cb, \
          NULL, 0);
    libusb_set_iso_packet_lengths(iso_rx_xfer, iso_pkt_size);
/*
    r = libusb_submit_transfer(iso_rx_xfer);
    if (r < 0) {
        ALOGE("%s(), libusb_submit_transfer : iso_rx_xfer : failed", __FUNCTION__);
        goto out;
    }
*/

    usb_running = 1;
    return;
out:
    if (data_rx_xfer != NULL) {
        rx_buf = CONTAINER_RX_HDR(data_rx_xfer->buffer);
        if (rx_buf != NULL)
            bt_hc_cbacks->dealloc(rx_buf);
        libusb_free_transfer(data_rx_xfer);
    }
    if (event_rx_xfer != NULL) {
        rx_buf = CONTAINER_RX_HDR(event_rx_xfer->buffer);
        if (rx_buf != NULL)
            bt_hc_cbacks->dealloc(rx_buf);
        libusb_free_transfer(event_rx_xfer);
    }
    if(iso_rx_xfer != NULL) {
        iso_buf = CONTAINER_ISO_HDR(iso_rx_xfer->buffer);
        if (iso_buf != NULL)
            bt_hc_cbacks->dealloc(iso_buf);
        libusb_free_transfer(iso_rx_xfer);
    }
}



static void xmit_xfer_cb(struct libusb_transfer *transfer)
{
    //ALOGI("%s().", __FUNCTION__);

    if(transfer->status != LIBUSB_TRANSFER_COMPLETED) // xmit fail.
    {
        ALOGE("xfer did not succeeded .....%d", transfer->status);
        usb_xfer_status |= XMIT_FAILED;
        usb.failed_tx_xfer = transfer;
        xmited_len = 0;
        thread_post(usb_thread, usb_handle_xmit_failed, NULL);
    }
    else // xmit succ.
    {
        //ALOGI("%s(), succ...", __FUNCTION__);
        xmited_len = transfer->actual_length+1;
        libusb_free_transfer(transfer);
        usb_xfer_status  |= XMITTED;

        pthread_mutex_lock(&xfer_mutex);
        pthread_cond_signal(&xfer_cond);
        pthread_mutex_unlock(&xfer_mutex);
    }
}


/*****************************************************************************
**   USB API Functions
*****************************************************************************/

/*******************************************************************************
**
** Function        usb_init
**
** Description     Initializes the serial driver for usb
**
** Returns         TRUE/FALSE
**
*******************************************************************************/
uint8_t usb_init(thread_t *upper_thread, my_allocator_t *allocator)
{
    ALOGI("usb_init().");
    memset(&usb, 0, sizeof(tUSB_CB));
    usb.handle = NULL;
    rx_event_queue = NULL;
    rx_bulk_queue = NULL;
    rx_iso_queue = NULL;

    rx_event_queue = fixed_queue_new(SIZE_MAX);
    if (!rx_event_queue) {
        ALOGE("%s unable to create pending rx_event_queue.", __func__);
        goto error;
    }
    rx_bulk_queue = fixed_queue_new(SIZE_MAX);
    if (!rx_bulk_queue) {
        ALOGE("%s unable to create pending rx_bulk_queue.", __func__);
        goto error;
    }
    rx_iso_queue = fixed_queue_new(SIZE_MAX);
    if (!rx_iso_queue) {
        ALOGE("%s unable to create pending rx_iso_queue.", __func__);
        goto error;
    }

    pthread_mutex_init(&usb.mutex, NULL);
    pthread_cond_init(&usb.cond, NULL);
    pthread_mutex_init(&xfer_mutex, NULL);
    pthread_cond_init(&xfer_cond, NULL);
    data_rx_xfer = event_rx_xfer = iso_rx_xfer = NULL;
    usb.send_rx_event = TRUE;
    usb.rx_status = RX_NEW_PKT;
    p_rx_hdr = NULL;

    acl_packet_ready_count = 0;
    event_packet_ready_count = 0;

    thread = upper_thread;
    bt_hc_cbacks = allocator;

    usb_thread = thread_new("usb_thread");
    if (!usb_thread) {
        ALOGE("%s unable to create thread.", __func__);
        //goto error;
    } else {
        thread_set_priority(usb_thread, -19);
    }
    
    return TRUE;
error:
    return FALSE;
}


/*******************************************************************************
**
** Function        usb_open
**
** Description     Open Bluetooth device with the port ID
**
** Returns         TRUE/FALSE
**
*******************************************************************************/
uint8_t usb_open(uint8_t port)
{

    ALOGI("usb_open(port:%d)", port);

    if (usb_running)
    {
        /* Userial is open; close it first */
        usb_close();
        //utils_delay(50);
    }
    if (libusb_init(NULL) < 0)
    {
        ALOGE("libusb_init : failed");
        return FALSE;
    }

    usb.handle = libusb_open_bt_device();
    bulk_pkt_size_wh = BT_HCI_MAX_FRAME_SIZE + sizeof(RX_HDR);
    bulk_pkt_size = BT_HCI_MAX_FRAME_SIZE;
    if (usb.handle == NULL)
    {
        ALOGE("usb_open: HCI USB failed to open");
        goto out;
    }

    data_rx_xfer = libusb_alloc_transfer(0);
    if (!data_rx_xfer)
    {
        ALOGE("Failed alloc data_rx_xfer");
        goto out;
    }

    event_rx_xfer  = libusb_alloc_transfer(0);
    if (!event_rx_xfer)
    {
        ALOGE("Failed alloc event_rx_xfer");
        goto out;
    }

    iso_rx_xfer = libusb_alloc_transfer(BT_MAX_ISO_FRAMES);
    if (!iso_rx_xfer)
    {
        ALOGE("Failed alloc iso_rx_xfer");
        goto out;
    }

    thread_post(usb_thread, usb_init_later, NULL);
    thread_post(usb_thread, usb_loop, NULL);

    return TRUE;
out:
    if (usb.handle != NULL)
    {
        if (data_rx_xfer != NULL)
            libusb_free_transfer(data_rx_xfer);
        if (event_rx_xfer != NULL)
            libusb_free_transfer(event_rx_xfer);
        if (iso_rx_xfer != NULL)
            libusb_free_transfer(iso_rx_xfer);
        libusb_release_interface(usb.handle, 1);
        libusb_release_interface(usb.handle, 0);
        libusb_close(usb.handle);
        libusb_exit(NULL);
    }
    return FALSE;
}


/*******************************************************************************
**
** Function        usb_read
**
** Description     Read data from the usb port
**
** Returns         Number of bytes actually read from the usb port and
**                 copied into p_data.  This may be less than len.
**
*******************************************************************************/
uint16_t usb_read(uint8_t *p_buffer, uint16_t len)
{
    uint16_t total_len = 0;
    uint16_t copy_len = 0;
    uint8_t *p_data = NULL;
    int different_xfer = 0;
    int pkt_rxing = 0;
    int rem_len = 0;

    //ALOGI("%s(), need to read %d byte.", __FUNCTION__, len);
    if (!usb_running) {
        ALOGE("%s(), usb is not running...", __FUNCTION__);
        return 0;
    }

    while (total_len < len)
    {
        if (p_rx_hdr == NULL)
        {
            pthread_mutex_lock(&usb.mutex);
            if (usb.rxed_xfer < 0)
            {
                ALOGE("Rx thread and usb_read out of sync %d", usb.rxed_xfer);
                usb.rxed_xfer = 0;
            }
            if (usb.rxed_xfer == 0 && usb.rx_status == RX_NEW_PKT)
            {
                usb.send_rx_event = TRUE;
                pthread_mutex_unlock(&usb.mutex);
                ALOGE("%s(), has nothing to read....", __FUNCTION__);
                return 0;
            }
            // 如果USB此时没有数据可读，则等待
            while (usb.rxed_xfer == 0)
            {
               pthread_cond_wait(&usb.cond, &usb.mutex);
            }
            
            usb.rxed_xfer--;
            pthread_mutex_unlock(&usb.mutex);

            if (usb.rx_status == RX_NEW_PKT)
            {
                if (!fixed_queue_is_empty(rx_bulk_queue)) {
                    p_rx_hdr = (RX_HDR *)fixed_queue_dequeue(rx_bulk_queue);
                    acl_packet_ready_count--;
                    goto read_packet;
                }
                if (!fixed_queue_is_empty(rx_event_queue)) {
                    p_rx_hdr = (RX_HDR *)fixed_queue_dequeue(rx_event_queue);
                    event_packet_ready_count--;
                    goto read_packet;
                }
                ALOGE("rxed_xfer is %d but no packet found", usb.rxed_xfer);
                return 0;
read_packet:
                switch (p_rx_hdr->event)
                {
                    case H4_TYPE_EVENT:
                        // 指向不包含HCI_H4头部一个字节的全部数据
                        p_data = p_rx_hdr->data;
                        p_rx_hdr->offset = 0;
                        //数据总长度，EVENT头部2个字节(event code + para len) + 参数长度
                        usb.rx_pkt_len = p_data[EV_LEN_FIELD] + HCI_EVT_PREAMBLE_SIZE;
                        usb.rx_status = RECEIVING_PKT;
                        // 填充HCI_H4头部
                        *p_buffer = p_rx_hdr->event;
                        total_len += 1;
                        p_buffer++;
                        break;

                    case H4_TYPE_ACL_DATA:
                        p_data = p_rx_hdr->data;
                        p_rx_hdr->offset = 0;
                        usb.rx_pkt_len = ((uint16_t)p_data[BLK_LEN_LO] | \
                              (uint16_t)p_data[BLK_LEN_HI] << 8) + \
                               HCI_ACL_PREAMBLE_SIZE;
                        usb.rx_status = RECEIVING_PKT;
                        *p_buffer = p_rx_hdr->event;
                        total_len += 1;
                        p_buffer++;
                        break;
                }
            }
            else   // rx_status == RECIVING_PKT
            {
                switch (pkt_rxing)
                {
                    case H4_TYPE_EVENT:
                        if (!fixed_queue_is_empty(rx_event_queue)) {
                            p_rx_hdr = (RX_HDR *)fixed_queue_dequeue(rx_event_queue);
                            if (p_rx_hdr != NULL)
                                event_packet_ready_count--;
                        }
                        break;

                    case H4_TYPE_ACL_DATA:
                        if (!fixed_queue_is_empty(rx_bulk_queue)) {
                            p_rx_hdr = (RX_HDR *)fixed_queue_dequeue(rx_bulk_queue);
                            if (p_rx_hdr != NULL)
                                acl_packet_ready_count--;
                        }
                        break;
                 }
                 if (p_rx_hdr == NULL)
                 {
                     ALOGI("%s(), #######Rxed packet from different end_point#######", __FUNCTION__);
                     different_xfer++;
                 }
                 else
                 {
                     p_rx_hdr->offset = 0;
                 }
            }
        }
        else //if (p_rx_hdr != NULL)
        {
            
            p_data = p_rx_hdr->data + p_rx_hdr->offset;
            pkt_rxing = p_rx_hdr->event;

            if((p_rx_hdr->len) <= (len - total_len))
                copy_len = p_rx_hdr->len;
            else
                copy_len = (len - total_len);

            p_rx_hdr->offset += copy_len;
            p_rx_hdr->len -= copy_len;
            rem_len = p_rx_hdr->len;
            
            memcpy((p_buffer + total_len), p_data, copy_len);
            total_len += copy_len;

            // 是否已经读完一个数据包
            if (rem_len == 0)
            {
               bt_hc_cbacks->dealloc(p_rx_hdr);
               p_rx_hdr = NULL;
            }
            usb.rx_pkt_len -= copy_len;
            // 是否已经读完整个完整的数据包
            if (usb.rx_pkt_len == 0)
            {
                usb.rx_status = RX_NEW_PKT;
                usb.send_rx_event = TRUE;
                break;
            }
            if (usb.rx_pkt_len < 0)
            {
                ALOGE("pkt len expected %d rxed len of %d", len, total_len);
                usb.rx_status = RX_NEW_PKT;
                break;
            }
        }
    } // end while

    if (different_xfer)
    {
        pthread_mutex_lock(&usb.mutex);
        usb.rxed_xfer += different_xfer;
        pthread_mutex_unlock(&usb.mutex);
    }

    return total_len;
}

/*******************************************************************************
**
** Function        usb_write
**
** Description     Write data to the usb port
**
** Returns         Number of bytes actually written to the usb port. This
**                 may be less than len.
**
*******************************************************************************/
uint16_t usb_write(uint8_t *p_data, uint16_t len)
{
    char pkt_type;
    CMD_HDR *cmd_hdr;

    pkt_type = *p_data;

    if ((pkt_type == H4_TYPE_COMMAND) || (pkt_type == H4_TYPE_ACL_DATA)) {
        if (!(xmit_transfer = libusb_alloc_transfer(0)))
        {
            ALOGE( "%s(), libusb_alloc_tranfer() 1 failed", __FUNCTION__);
            return 0;
        }
    }
    
    if (pkt_type == H4_TYPE_SCO_DATA) {
        if (!(xmit_transfer = libusb_alloc_transfer(BT_MAX_ISO_FRAMES)))
        {
            ALOGE( "%s(), libusb_alloc_tranfer() 2 failed", __FUNCTION__);
            return 0;
        }
    }

    //ALOGI("%s(), pkt type: %d", __FUNCTION__, pkt_type);
    switch (pkt_type)
    {
        case H4_TYPE_COMMAND:
            /* Make use of BT_HDR space to populate setup */
            cmd_hdr = CONTAINER_CMD_HDR(p_data + 1); // skip pkt type.
            cmd_hdr->setup.bmRequestType = USB_TYPE_REQ;
            cmd_hdr->setup.wLength = len - 1;
            cmd_hdr->setup.wIndex = 0;
            cmd_hdr->setup.wValue = 0;
            cmd_hdr->setup.bRequest = 0;
            cmd_hdr->event = H4_TYPE_COMMAND;
            libusb_fill_control_transfer(xmit_transfer, usb.handle,
                (uint8_t *)&cmd_hdr->setup, xmit_xfer_cb, NULL, 0);
            break;

        case H4_TYPE_ACL_DATA:
            libusb_fill_bulk_transfer(xmit_transfer, usb.handle,
                BT_BULK_OUT, (p_data+1), (len-1), xmit_xfer_cb, NULL, 0);
            break;

        case H4_TYPE_SCO_DATA:
            libusb_fill_iso_transfer(xmit_transfer, usb.handle, \
                  BT_ISO_OUT, (p_data+1), (len-1), BT_MAX_ISO_FRAMES, \
                  xmit_xfer_cb, NULL, 0);

            libusb_set_iso_packet_lengths(xmit_transfer, iso_pkt_size);
            break;

         default:
            ALOGE("%s(), Unknown packet type to transmit 0x%x", __FUNCTION__, *p_data);
            return 0;
    }

    if (libusb_submit_transfer(xmit_transfer) < 0)
    {
        ALOGE("%s(), libusb_submit_transfer : %d : failed", __FUNCTION__, *p_data);
        return 0;
    }
    xmited_len = len;
    usb_xfer_status &= ~(XMITTED);

    // wait for transfer complete.
    pthread_mutex_lock(&xfer_mutex);
    pthread_cond_wait(&xfer_cond, &xfer_mutex);
    pthread_mutex_unlock(&xfer_mutex);

    //ALOGI("%s() end...", __FUNCTION__);
    return (xmited_len);
}

/*******************************************************************************
**
** Function        usb_close
**
** Description     Close the serial port
**
** Returns         None
**
*******************************************************************************/
void usb_close(void)
{
    ALOGI("%s().", __FUNCTION__);

    usb_xfer_status |= RX_DEAD;

    thread_free(usb_thread);
    usb_thread = NULL;

    if (usb.handle) {
        if (data_rx_xfer != NULL)
            libusb_free_transfer(data_rx_xfer);
        if (event_rx_xfer != NULL)
            libusb_free_transfer(event_rx_xfer);
        if (iso_rx_xfer != NULL)
            libusb_free_transfer(iso_rx_xfer);
        libusb_release_interface(usb.handle, 1);
        libusb_release_interface(usb.handle, 0);
        libusb_close(usb.handle);
    }

    usb.handle = NULL;
    if (usb_running)
        libusb_exit(NULL);
    usb_running = 0;

    if (rx_event_queue != NULL) {
        fixed_queue_free(rx_event_queue, bt_hc_cbacks->dealloc);
        rx_event_queue = NULL;
    }
    if (rx_bulk_queue != NULL) {
        fixed_queue_free(rx_bulk_queue, bt_hc_cbacks->dealloc);
        rx_bulk_queue = NULL;
    }
    if (rx_iso_queue != NULL) {
        fixed_queue_free(rx_iso_queue, bt_hc_cbacks->dealloc);
        rx_iso_queue = NULL;
    }

    //ALOGI("%s(), exit.", __FUNCTION__);
}

/*******************************************************************************
**
** Function        usb_ioctl
**
** Description     ioctl inteface
**
** Returns         None
**
*******************************************************************************/
void usb_ioctl(usb_ioctl_op_t op, void *p_data)
{
    return;
}


///////////////////////////////////////////////////////////////////



#define ID_RIFF 0x46464952
#define ID_WAVE 0x45564157
#define ID_FMT  0x20746d66
#define ID_DATA 0x61746164

struct riff_wave_header {
    uint32_t riff_id;
    uint32_t riff_sz;
    uint32_t wave_id;
};

struct chunk_header {
    uint32_t id;
    uint32_t sz;
};

struct chunk_fmt {
    uint16_t audio_format;
    uint16_t num_channels;
    uint32_t sample_rate;
    uint32_t byte_rate;
    uint16_t block_align;
    uint16_t bits_per_sample;
};


static void read_and_play_wav(UNUSED_ATTR void *context)
{
    FILE *file;
    int more_chunks = 1;
    struct riff_wave_header riff_wave_header;
    struct chunk_header chunk_header;
    struct chunk_fmt chunk_fmt;

    uint8_t *buffer;
    int size;
    int num_read;

    ALOGI("%s().", __FUNCTION__);
    file = fopen("/sdcard/loveyou_8k_1ch.wav", "rb");
    if (!file) {
        ALOGE("Unable to open file: /sdcard/loveyou_8k_1ch.wav");
        return;
    }

    fread(&riff_wave_header, sizeof(riff_wave_header), 1, file);
    if ((riff_wave_header.riff_id != ID_RIFF) ||
        (riff_wave_header.wave_id != ID_WAVE)) {
        ALOGE("Error: is not a riff/wave file\n");
        fclose(file);
        return;
    }

    do {
        fread(&chunk_header, sizeof(chunk_header), 1, file);

        switch (chunk_header.id) {
        case ID_FMT:
            fread(&chunk_fmt, sizeof(chunk_fmt), 1, file);
            /* If the format header is larger, skip the rest */
            if (chunk_header.sz > sizeof(chunk_fmt))
                fseek(file, chunk_header.sz - sizeof(chunk_fmt), SEEK_CUR);
            break;
        case ID_DATA:
            /* Stop looking for chunks */
            more_chunks = 0;
            break;
        default:
            /* Unknown chunk, skip bytes */
            fseek(file, chunk_header.sz, SEEK_CUR);
        }
    } while (more_chunks);

    size = iso_pkt_size * BT_MAX_ISO_FRAMES + 1;
    buffer = (uint8_t *)malloc(size);
    if (!buffer) {
        ALOGE("%s(), Unable to allocate %d bytes\n", __FUNCTION__, size);
        goto err;
    }

    ALOGI("%s(), start to play sco...", __FUNCTION__);

    buffer[0] = H4_TYPE_SCO_DATA;
    do {
        num_read = fread(&buffer[1], 1, size - 1, file);
        if (num_read > 0) {
            if (usb_write(buffer, num_read + 1) <= 0) {
                ALOGE("%s(), usb_write failed...", __FUNCTION__);
                break;
            }
        }
    } while (num_read > 0);

    free(buffer);
err:
    fclose(file);
    ALOGI("%s() end...", __FUNCTION__);
}


void tell_usb_to_play_sco()
{
    ALOGI("%s().", __FUNCTION__);
    //thread_post(usb_thread, read_and_play_wav, NULL);
    read_and_play_wav(NULL);
}





static void submit_to_read_sco(UNUSED_ATTR void *context)
{
    int r;
    r = libusb_submit_transfer(iso_rx_xfer);
    if (r < 0) {
        ALOGE("%s(), libusb_submit_transfer : iso_rx_xfer : failed", __FUNCTION__);
    }
}

void tell_usb_to_read_sco()
{
    ALOGI("%s().", __FUNCTION__);

    alarm_t *timer;

    timer = alarm_new("read_sco_timer");
    if (!timer) {
        ALOGE("%s unable to create timer.", __func__);
        return;
    }

    alarm_set(timer, 2000, submit_to_read_sco, NULL);
}

