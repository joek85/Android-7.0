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

include $(CLEAR_VARS)
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE := libsst

LOCAL_SHARED_LIBRARIES :=	libc \
							libz \
							liblog

LOCAL_SRC_FILES := 	api.c \
					fetch_env.c \
					secure_storage.c \
					secure_storage_ioctl.c

LOCAL_C_INCLUDES += \
	hardware/aw/include

LOCAL_CFLAGS += -DDEBUG_CACHE
include $(BUILD_SHARED_LIBRARY)

#include $(CLEAR_VARS)
#LOCAL_MODULE_TAGS := optional
#LOCAL_MODULE := sst_test_v1

#LOCAL_SHARED_LIBRARIES :=	libc \
							libz \
							liblog

#LOCAL_SRC_FILES := api.c \
			        fetch_env.c \
			        secure_storage.c \
					secure_storage_ioctl.c \
					test.c \
			        main.c
#LOCAL_CFLAGS += -DDEBUG_CACHE
#include $(BUILD_EXECUTABLE)

include $(CLEAR_VARS)
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE := sst_test_v2

LOCAL_SHARED_LIBRARIES :=	libc \
							libz \
							liblog \
							libsst

LOCAL_SRC_FILES := 	test.c \
			        main.c
LOCAL_CFLAGS += -DDEBUG_CACHE
include $(BUILD_EXECUTABLE)
