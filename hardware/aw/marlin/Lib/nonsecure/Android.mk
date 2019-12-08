LOCAL_PATH:= $(call my-dir)

########################
# wasabi-drm library

include $(CLEAR_VARS)
LOCAL_MODULE := wasabi-drm
LOCAL_MODULE_SUFFIX := .jar
LOCAL_SRC_FILES := $(LOCAL_MODULE)$(LOCAL_MODULE_SUFFIX)
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := JAVA_LIBRARIES
include $(BUILD_PREBUILT)

#####################################################################
# static lib

include $(CLEAR_VARS)
LOCAL_MODULE := libstagefright_aosputil_wasabi
LOCAL_SRC_FILES := lib32/libstagefright_aosputil_wasabi.a
LOCAL_MODULE_CLASS := STATIC_LIBRARIES
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_SUFFIX := .a
LOCAL_MULTILIB := 32
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libstagefright_nuplayer_wasabi
LOCAL_SRC_FILES := lib32/libstagefright_nuplayer_wasabi.a
LOCAL_MODULE_CLASS := STATIC_LIBRARIES
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_SUFFIX := .a
LOCAL_MULTILIB := 32
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libWasabi
LOCAL_SRC_FILES := lib32/libWasabi.a
LOCAL_MODULE_CLASS := STATIC_LIBRARIES
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_SUFFIX := .a
LOCAL_MULTILIB := 32
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libstagefright_aosputil_wasabi
LOCAL_SRC_FILES := lib64/libstagefright_aosputil_wasabi.a
LOCAL_MODULE_CLASS := STATIC_LIBRARIES
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_SUFFIX := .a
LOCAL_MULTILIB := 64 
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libstagefright_nuplayer_wasabi
LOCAL_SRC_FILES := lib64/libstagefright_nuplayer_wasabi.a
LOCAL_MODULE_CLASS := STATIC_LIBRARIES
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_SUFFIX := .a
LOCAL_MULTILIB := 64
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libWasabi
LOCAL_SRC_FILES := lib64/libWasabi.a
LOCAL_MODULE_CLASS := STATIC_LIBRARIES
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_SUFFIX := .a
LOCAL_MULTILIB := 64 
include $(BUILD_PREBUILT)
