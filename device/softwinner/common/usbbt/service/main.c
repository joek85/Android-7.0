#define LOG_TAG "usbbt_native"

#include <utils/Log.h>

#include <stdio.h>
#include <sys/stat.h>
#include <errno.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

#include <fcntl.h>
#include <dirent.h>
#include <unistd.h>
#include <string.h>

#include <cutils/sockets.h>
#include <private/android_filesystem_config.h>
#include <cutils/properties.h>

#include <libusb/libusb.h>

static char usb_dev_path[PATH_MAX];

#define USBBT_INSERT_PROPERTY   "sys.usbbt.inserted"

struct bt_usb_device {
    uint8_t    bDevClass;
    uint8_t    bDevSubClass;
    uint8_t    bDevProtocol;
};

#define BT_USB_DEVICE_INFO(cl, sc, pr) \
        .bDevClass = (cl), \
        .bDevSubClass = (sc), \
        .bDevProtocol = (pr)

static struct bt_usb_device btusb_table[] = {
    /* Generic Bluetooth USB device */
    { BT_USB_DEVICE_INFO(0xe0, 0x01, 0x01) },
    { BT_USB_DEVICE_INFO(0xff, 0x01, 0x01) },
    { }     /* Terminating entry */
};

static int is_usb_match_idtable (struct bt_usb_device *id, struct libusb_device_descriptor *desc)
{
    int ret = 1;

    ret = ((id->bDevClass != libusb_le16_to_cpu(desc->bDeviceClass)) ? 0 :
           (id->bDevSubClass != libusb_le16_to_cpu(desc->bDeviceSubClass)) ? 0 :
           (id->bDevProtocol != libusb_le16_to_cpu(desc->bDeviceProtocol)) ? 0 : 1);

    return ret;
}


static int is_btusb_device (struct libusb_device *dev)
{
    struct bt_usb_device *id;
    struct libusb_device_descriptor desc;
    int r, match;

    r = libusb_get_device_descriptor(dev, &desc);
    if (r < 0)
        return 0;

    match = 0;

    for (id = btusb_table; id->bDevClass; id++) {
        if (is_usb_match_idtable(id, &desc) == 1) {
            match = 1;
            break;
        }
    }

    if (!match) {
        return 0;
    }

    return 1;
}


static void get_usb_dev_path(struct libusb_device *dev, char *path)
{
	snprintf(path, PATH_MAX, "%s/%03d/%03d", "/dev/bus/usb",
            libusb_get_bus_number(dev), libusb_get_device_address(dev));
}


static int had_btusb_device()
{
    struct libusb_device **devs;
    struct libusb_device *dev;
    int ret = 0, i = 0;

    if (libusb_init(NULL) < 0) {
        ALOGE("%s(), libusb_init() failed", __FUNCTION__);
        return 0;
    }

    if (libusb_get_device_list(NULL, &devs) < 0) {
        return 0;
    }

    for (i = 0; (dev = devs[i]) != NULL; i++) {
        if (is_btusb_device(dev) == 1)
            break;
    }

    if (dev) {
        // get dev path.
        memset(usb_dev_path, 0, sizeof(usb_dev_path));
        get_usb_dev_path(dev, usb_dev_path);
        ret = 1;
    } else {
        ALOGE("%s(), No matching USB BT device found ...", __FUNCTION__);
        ret = 0;
    }

    libusb_free_device_list(devs, 1);
    libusb_exit(NULL);

    return ret;
}




/* send message to Android APK through local socket */
/*
static void send_to_usb_bt_app(char msg)
{
    int socket_fd;
    int ret;
#define SERVER_NAME    "usb_bt_server"

    socket_fd = socket_local_client(SERVER_NAME, ANDROID_SOCKET_NAMESPACE_ABSTRACT, SOCK_STREAM);
    if (socket_fd < 0) {
        ALOGE("creat client socket error!!!\n");
        return;
    }

    ret = write(socket_fd, &msg, 1);
    if (ret < 0) {
        ALOGE("write failed!!!\n");
        close(socket_fd);
        return;
    }

    close(socket_fd);
}
*/


static void set_dev_node(char path[])
{
    unsigned uid;
    unsigned gid;
    mode_t mode;

    ALOGI("%s(), Set path: %s", __FUNCTION__, path);

    uid = AID_BLUETOOTH;
    gid = AID_BLUETOOTH;
    mode = 0660;
    setegid(gid);
    chown(path, uid, -1);
    setegid(AID_ROOT);
}


int main(int argc, char *argv[])
{
    char val[PROPERTY_VALUE_MAX];

    memset(val, 0, sizeof(val));

    if (property_get("sys.boot_completed", val, "0") && (val[0] == '1')) {
        // system boot already completely.
        if (property_get("sys.usbbt.devpath", val, NULL)) {
            set_dev_node(val);
            //send_to_usb_bt_app(2);
        } else {
            ALOGE("%s(), get property: sys.usbbt.devpath failed...", __FUNCTION__);
        }
    } else {
        // system is booting.
        if (had_btusb_device()) {
            ALOGI("in Booting, found usb bt device...");
            if (property_set(USBBT_INSERT_PROPERTY, "1") < 0) {
                ALOGE("%s(), Can not set property: %s", __FUNCTION__, USBBT_INSERT_PROPERTY);
            }
            set_dev_node(usb_dev_path);
        }
    }

    return 0;
}
