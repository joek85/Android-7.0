WIDEVINE_SUPPORTED_ARCH := arm x86
LOCAL_PATH:= $(call my-dir)

##################################################
include $(CLEAR_VARS)

LOCAL_MODULE := com.google.widevine.software.drm.xml
LOCAL_SRC_FILES := $(LOCAL_MODULE)
LOCAL_MODULE_TAGS := optional

LOCAL_MODULE_CLASS := ETC

# This will install the file in /system/etc/permissions
#
LOCAL_MODULE_PATH := $(TARGET_OUT_ETC)/permissions

include $(BUILD_PREBUILT)

########################
# Dummy library used to indicate availability of widevine drm

include $(CLEAR_VARS)
LOCAL_MODULE := com.google.widevine.software.drm
LOCAL_MODULE_SUFFIX := .jar
LOCAL_SRC_FILES := $(LOCAL_MODULE)$(LOCAL_MODULE_SUFFIX)
LOCAL_MODULE_TAGS := optional

LOCAL_MODULE_CLASS := JAVA_LIBRARIES

include $(BUILD_PREBUILT)
#####################################################################
#libWVStreamControlAPI_LX.so
include $(CLEAR_VARS)

LOCAL_MODULE := libWVStreamControlAPI_L$(BOARD_WIDEVINE_OEMCRYPTO_LEVEL)
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_MODULE_SUFFIX := .so
LOCAL_SRC_FILES := $(LOCAL_MODULE)$(LOCAL_MODULE_SUFFIX)
LOCAL_PROPRIETARY_MODULE := true
#LOCAL_STRIP_MODULE := true
LOCAL_MULTILIB := 32
LOCAL_MODULE_TAGS := optional
include $(BUILD_PREBUILT)

#####################################################################
#libwvdrm_LX.so
include $(CLEAR_VARS)
LOCAL_MODULE := libwvdrm_L$(BOARD_WIDEVINE_OEMCRYPTO_LEVEL)
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_MODULE_SUFFIX := .so
LOCAL_SRC_FILES := $(LOCAL_MODULE)$(LOCAL_MODULE_SUFFIX)
LOCAL_PROPRIETARY_MODULE := true
#LOCAL_STRIP_MODULE := true
LOCAL_MULTILIB := 32
LOCAL_MODULE_TAGS := optional
include $(BUILD_PREBUILT)

#####################################################################
#libdrmdecrypt.so
include $(CLEAR_VARS)
LOCAL_MODULE := libdrmdecrypt
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_MODULE_SUFFIX := .so
LOCAL_SRC_FILES := $(LOCAL_MODULE)$(LOCAL_MODULE_SUFFIX)
LOCAL_PROPRIETARY_MODULE := true
#LOCAL_STRIP_MODULE := true
LOCAL_MODULE_TAGS := optional
LOCAL_MULTILIB := 32
include $(BUILD_PREBUILT)

#####################################################################
#libwvm.so
include $(CLEAR_VARS)
LOCAL_MODULE := libwvm
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_MODULE_SUFFIX := .so
LOCAL_SRC_FILES := $(LOCAL_MODULE)$(LOCAL_MODULE_SUFFIX)
LOCAL_PROPRIETARY_MODULE := true
#LOCAL_STRIP_MODULE := true
LOCAL_MODULE_TAGS := optional
LOCAL_MULTILIB := 32
include $(BUILD_PREBUILT)

#####################################################################
#libwvdrmengine.so
include $(CLEAR_VARS)
LOCAL_MODULE := libwvdrmengine
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_MODULE_SUFFIX := .so
LOCAL_SRC_FILES := $(LOCAL_MODULE)$(LOCAL_MODULE_SUFFIX)
LOCAL_PROPRIETARY_MODULE := true
#LOCAL_STRIP_MODULE := true
#LOCAL_MODULE_PATH := $(TARGET_OUT_VENDOR_SHARED_LIBRARIES)/mediadrm
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_RELATIVE_PATH := mediadrm
LOCAL_MODULE_TAGS := optional
LOCAL_MULTILIB := 32
include $(BUILD_PREBUILT)

#####################################################################
#libdrmwvmplugin.so
include $(CLEAR_VARS)
LOCAL_MODULE := libdrmwvmplugin
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_MODULE_SUFFIX := .so
LOCAL_SRC_FILES := $(LOCAL_MODULE)$(LOCAL_MODULE_SUFFIX)
LOCAL_PROPRIETARY_MODULE := true
#LOCAL_STRIP_MODULE := true
#LOCAL_MODULE_PATH := $(TARGET_OUT_VENDOR_SHARED_LIBRARIES)/drm
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_RELATIVE_PATH := drm
LOCAL_MODULE_TAGS := optional
LOCAL_MULTILIB := 32
include $(BUILD_PREBUILT)



#####################################################################
#liboemcrypto.so
ifeq ($(BOARD_WIDEVINE_OEMCRYPTO_LEVEL),1)
include $(CLEAR_VARS)
LOCAL_MODULE := liboemcrypto
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_MODULE_SUFFIX := .so
LOCAL_SRC_FILES := $(LOCAL_MODULE)$(LOCAL_MODULE_SUFFIX)
#LOCAL_PROPRIETARY_MODULE copies library to vendor/lib
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_TAGS := optional
LOCAL_MULTILIB := 32
include $(BUILD_PREBUILT)
endif# liboemcrypto

