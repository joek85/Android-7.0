LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_SRC_FILES:= \
 core.c \
 descriptor.c \
 io.c \
 sync.c \
 os/linux_usbfs.c

LOCAL_C_INCLUDES += \
 external/libusb/ \
 external/libusb/libusb/ \
 external/libusb/libusb/os

LOCAL_CFLAGS := -D_SHARED_LIBRARY_
LOCAL_SHARED_LIBRARIES := liblog
LOCAL_MODULE_TAGS:= optional
LOCAL_MODULE:= libusb
include $(BUILD_SHARED_LIBRARY)


include $(CLEAR_VARS)

LOCAL_SRC_FILES:= \
 core.c \
 descriptor.c \
 io.c \
 sync.c \
 os/linux_usbfs.c

LOCAL_C_INCLUDES += \
 external/libusb/ \
 external/libusb/libusb/ \
 external/libusb/libusb/os

LOCAL_STATIC_LIBRARIES := liblog
LOCAL_MODULE:= libusb
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := STATIC_LIBRARIES
include $(BUILD_STATIC_LIBRARY)
