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

LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

ifeq ($(SW_CHIP_PLATFORM),H6)
   LOCAL_CFLAGS += -DPLATFORM_SUN50IW6P1
endif
ifeq ($(SW_CHIP_PLATFORM),H3)
   LOCAL_CFLAGS += -DPLATFORM_SUN8IW7P1
endif

LOCAL_C_INCLUDES := $(LOCAL_PATH)/include \
	$(LOCAL_PATH)/adapter \
	$(LOCAL_PATH)/policy \
	$(LOCAL_PATH)/platform

LOCAL_CFLAGS   += -Wall -Wunused -Wunreachable-code
LOCAL_CFLAGS   += -DLOG_TAG=\"displayd\"
LOCAL_CPPFLAGS += -std=c++11
LOCAL_LDFLAGS  += -shared

LOCAL_SRC_FILES :=              \
	adapter/socketConnector.cpp \
	adapter/deviceControler.cpp \
	adapter/HWC1Proxy.cpp       \
	CommandListener.cpp         \
	DisplayManager.cpp          \
	helper.cpp                  \
	main.cpp

LOCAL_SRC_FILES +=             \
	hotplugListener.cpp        \
	policy/policySelector.cpp  \
	policy/DispOutputState.cpp \
	policy/DisplayPolicy.cpp

LOCAL_MODULE := displayd
LOCAL_SHARED_LIBRARIES := libutils libcutils liblog libsysutils
LOCAL_MODULE_TAGS := optional
LOCAL_INIT_RC := displayd.rc
include $(BUILD_EXECUTABLE)

include $(CLEAR_VARS)
LOCAL_C_INCLUDES := $(LOCAL_PATH)/adapter $(LOCAL_PATH)/libhwcproxy $(LOCAL_PATH)/include

LOCAL_CFLAGS += -Wall -Wunused -Wunreachable-code
LOCAL_CFLAGS += -DLOG_TAG=\"hwcomposer\" -DDEBUG

LOCAL_SRC_FILES :=                  \
	libhwcproxy/displaydRequest.cpp \
	libhwcproxy/interface.cpp

LOCAL_MODULE := libhwcproxy
LOCAL_SHARED_LIBRARIES := libutils libcutils liblog libsysutils
LOCAL_MODULE_TAGS := optional
include $(BUILD_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_C_INCLUDES := $(LOCAL_PATH)/include

LOCAL_CFLAGS += -Wall -Wunused -Wunreachable-code
LOCAL_CFLAGS += -DLOG_TAG=\"dispclient\" -DDEBUG

LOCAL_SRC_FILES :=              \
	libdispclient/connector.cpp \
	libdispclient/libdispclient.cpp

LOCAL_MODULE := libdispclient
LOCAL_SHARED_LIBRARIES := libutils libcutils liblog libsysutils
LOCAL_MODULE_TAGS := optional
include $(BUILD_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_C_INCLUDES := $(LOCAL_PATH)/include

LOCAL_CFLAGS += -Wall -Wunused -Wunreachable-code
LOCAL_CFLAGS += -DLOG_TAG=\"dispctrl\" -DDEBUG

LOCAL_SRC_FILES :=      \
	tools/main.cpp      \
	tools/connector.cpp \
	tools/libdispclient.cpp

LOCAL_MODULE := dispctrl
LOCAL_SHARED_LIBRARIES := libutils libcutils liblog libsysutils
LOCAL_MODULE_TAGS := optional
include $(BUILD_EXECUTABLE)


