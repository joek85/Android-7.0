app := BluetoothRTK3ddService
LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
LOCAL_MODULE := ${app}
LOCAL_MODULE_SUFFIX := .apk
LOCAL_MODULE_CLASS := APPS
LOCAL_MODULE_TAGS := optional
LOCAL_CERTIFICATE := platform
LOCAL_SRC_FILES := ${app}.apk
include $(BUILD_PREBUILT)

