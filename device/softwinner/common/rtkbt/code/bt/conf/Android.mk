LOCAL_PATH := $(call my-dir)

# Bluetooth bt_stack.conf config file
# ========================================================
include $(CLEAR_VARS)
LOCAL_MODULE := bt_stack-rtk.conf
LOCAL_MODULE_CLASS := ETC
LOCAL_MODULE_PATH := $(TARGET_OUT)/etc/bluetooth
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := $(LOCAL_MODULE)
include $(BUILD_PREBUILT)

# Bluetooth bt_did.conf config file
# ========================================================
include $(CLEAR_VARS)
LOCAL_MODULE := bt_did-rtk.conf
LOCAL_MODULE_CLASS := ETC
LOCAL_MODULE_PATH := $(TARGET_OUT)/etc/bluetooth
LOCAL_MODULE_TAGS := optional
LOCAL_SRC_FILES := $(LOCAL_MODULE)
include $(BUILD_PREBUILT)

