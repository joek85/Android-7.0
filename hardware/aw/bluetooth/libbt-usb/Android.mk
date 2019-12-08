LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

ifneq ($(BOARD_BLUETOOTH_BDROID_BUILDCFG_INCLUDE_DIR),)
  bdroid_C_INCLUDES := $(BOARD_BLUETOOTH_BDROID_BUILDCFG_INCLUDE_DIR)
  bdroid_CFLAGS += -DHAS_BDROID_BUILDCFG
else
  bdroid_C_INCLUDES :=
  bdroid_CFLAGS += -DHAS_NO_BDROID_BUILDCFG
endif

BDROID_DIR := $(TOP_DIR)system/bt


LOCAL_SRC_FILES := \
        src/bt_vendor_aw.c \
        src/hardware.c

LOCAL_C_INCLUDES += \
        $(LOCAL_PATH)/include \
        $(BDROID_DIR)/hci/include \
        $(BDROID_DIR)/include \
        $(BDROID_DIR)/stack/include \
        $(BDROID_DIR)/gki/ulinux

LOCAL_C_INCLUDES += $(bdroid_C_INCLUDES)
LOCAL_CFLAGS += $(bdroid_CFLAGS)

LOCAL_SHARED_LIBRARIES := \
        libcutils \
        liblog

LOCAL_MODULE := libbt-vendor-usb
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_MODULE_OWNER := aw
LOCAL_PROPRIETARY_MODULE := true

include $(LOCAL_PATH)/vnd_buildcfg.mk

include $(BUILD_SHARED_LIBRARY)
