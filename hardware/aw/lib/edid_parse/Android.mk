# Copyright (C) 2008 The Android Open Source Project
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

include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
	parse_edid_sunxi.cpp \
	parse_edid.cpp

LOCAL_C_INCLUDES += \
    hardware/aw/include

LOCAL_MODULE := libedidParse.sunxi

LOCAL_SHARED_LIBRARIES := \
	libutils \
	libcutils
LOCAL_MODULE_TAGS := optional
LOCAL_CFLAGS:= -DLOG_TAG=\"parseEdid\"
include $(BUILD_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_C_INCLUDES := $(LOCAL_PATH)/include

LOCAL_CFLAGS += -Wall -Wunused -Wunreachable-code
LOCAL_CFLAGS += -DLOG_TAG=\"tinyedid\" -DDEBUG

LOCAL_SRC_FILES :=    \
	debug/main.c   \
	debug/video_identification_code.c

LOCAL_MODULE := tinyedid
LOCAL_SHARED_LIBRARIES := libutils libcutils liblog libsysutils
LOCAL_MODULE_TAGS := optional
include $(BUILD_EXECUTABLE)

