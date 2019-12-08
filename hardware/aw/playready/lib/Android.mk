LOCAL_PATH:= $(call my-dir)

#####################################################################
# libplayreadypk.so
#####################################################################
include $(CLEAR_VARS)
LOCAL_MODULE := libplayreadypk

ifeq ($(BOARD_PLAYREADY_USE_SECUREOS), 1)
LOCAL_SRC_FILES := secure/lib32/libplayreadypk.so
else
LOCAL_SRC_FILES := nonsecure/lib32/libplayreadypk.so
endif

LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_SUFFIX := .so
LOCAL_MULTILIB := 32
$(info $(LOCAL_SRC_FILES))
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libplayreadypk

ifeq ($(BOARD_PLAYREADY_USE_SECUREOS), 1)
LOCAL_SRC_FILES := secure/lib64/libplayreadypk.so
else
LOCAL_SRC_FILES := nonsecure/lib64/libplayreadypk.so
endif

LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_SUFFIX := .so
LOCAL_MULTILIB := 64
include $(BUILD_PREBUILT)
