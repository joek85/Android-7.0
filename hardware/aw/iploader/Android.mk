# Copyright (C) 2012 The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

LOCAL_PATH := $(call my-dir)


# Allwinner Iploader

include $(CLEAR_VARS)
LOCAL_PREBUILT_LIBS := \
	libip_loader.so 

LOCAL_MODULE_TAGS := optional
include $(BUILD_MULTI_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := libip_loader
LOCAL_MODULE_TAGS := optional

LOCAL_MODULE_TARGET_ARCH := arm
LOCAL_REQUIRED_MODULES := \
	libcrypto_aw \
	libip_loader

include $(BUILD_PHONY_PACKAGE)

#######################
include $(CLEAR_VARS)
LOCAL_MODULE := iploader_server 

LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_PATH := $(TARGET_OUT)/bin
LOCAL_SRC_FILES := $(LOCAL_MODULE)
LOCAL_MODULE_CLASS := FAKE
include $(BUILD_PREBUILT)

#######################
include $(CLEAR_VARS)
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE := iploader_sample

LOCAL_MODULE_TARGET_ARCH := arm
LOCAL_STATIC_LIBRARIES := libip_hal
LOCAL_SHARED_LIBRARIES :=	libc \
							liblog
LOCAL_SRC_FILES := \
			sample.c

LOCAL_LDFLAGS += -ldl
include $(BUILD_EXECUTABLE)

