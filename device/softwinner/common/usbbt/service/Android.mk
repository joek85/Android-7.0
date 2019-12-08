
LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_SRC_FILES:= main.c

LOCAL_C_INCLUDES += external/libusb

LOCAL_CFLAGS += -std=c99

LOCAL_MODULE:= usbbt_native

LOCAL_SHARED_LIBRARIES := libutils liblog libcutils

LOCAL_STATIC_LIBRARIES += libusb

LOCAL_INIT_RC := usbbt_native.rc

include $(BUILD_EXECUTABLE)
