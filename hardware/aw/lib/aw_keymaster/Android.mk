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


ifdef SECURE_PRODUCT

# Allwinner TEE keymaster

include $(CLEAR_VARS)
LOCAL_MODULE := libaw_tee_schw.so
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_PATH := $(TARGET_OUT)/lib
LOCAL_SRC_FILES := $(LOCAL_MODULE)
LOCAL_MODULE_CLASS := lib
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE := keystore.$(TARGET_BOARD_PLATFORM)
LOCAL_MODULE_PATH := $(TARGET_OUT_SHARED_LIBRARIES)/hw
LOCAL_SRC_FILES := module_aw.cpp
LOCAL_C_INCLUDES := hardware/aw/client-api \
                    hardware/aw/include

LOCAL_CFLAGS = -fvisibility=hidden -Wall -DTEE_KEYMASTER
LOCAL_SHARED_LIBRARIES := liblog libkeystore_binder libdl libc libcutils
LOCAL_MODULE_TAGS := optional
LOCAL_ADDITIONAL_DEPENDENCIES := $(LOCAL_PATH)/Android.mk
LOCAL_REQUIRED_MODULES := libaw_tee_schw.so
include $(BUILD_SHARED_LIBRARY)

else

# Allwinner keymaster

include $(CLEAR_VARS)
LOCAL_MODULE := libaw_schw.so
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_PATH := $(TARGET_OUT)/lib
LOCAL_SRC_FILES := $(LOCAL_MODULE)
LOCAL_MODULE_CLASS := lib
include $(BUILD_PREBUILT)


include $(CLEAR_VARS)
LOCAL_MODULE := keystore.$(TARGET_BOARD_PLATFORM)
LOCAL_MODULE_PATH := $(TARGET_OUT_SHARED_LIBRARIES)/hw
LOCAL_SRC_FILES := module_aw.cpp
LOCAL_C_INCLUDES := system/security/keystore \
                    hardware/aw/include
LOCAL_CFLAGS = -fvisibility=hidden -Wall -Werror
LOCAL_SHARED_LIBRARIES := liblog libkeystore_binder libdl libc libcutils
LOCAL_MODULE_TAGS := optional
LOCAL_ADDITIONAL_DEPENDENCIES := $(LOCAL_PATH)/Android.mk
LOCAL_REQUIRED_MODULES := libaw_schw.so
include $(BUILD_SHARED_LIBRARY)

endif

