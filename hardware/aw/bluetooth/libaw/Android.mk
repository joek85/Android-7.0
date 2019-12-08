LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
    src/module_info.c

LOCAL_C_INCLUDES += \
    $(LOCAL_PATH)/include


LOCAL_MODULE := libbt-aw
LOCAL_MODULE_TAGS := optional
LOCAL_STATIC_LIBRARIES := libwifi_hardware_info
LOCAL_SHARED_LIBRARIES := libcutils libc
LOCAL_MODULE_CLASS := STATIC_LIBRARIES

include $(BUILD_STATIC_LIBRARY)