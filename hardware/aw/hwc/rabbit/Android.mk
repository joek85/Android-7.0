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

# HAL module implemenation stored in
# hw/<OVERLAY_HARDWARE_MODULE_ID>.<ro.product.board>.so
include $(CLEAR_VARS)
LOCAL_MULTILIB := both 
LOCAL_MODULE_RELATIVE_PATH := hw
LOCAL_SHARED_LIBRARIES := liblog libEGL
LOCAL_SRC_FILES := \
	hwc.cpp \
	hwc_sunxi.cpp \
	hwc_others.cpp \
	hal.cpp \
	hwc_perform.cpp \

LOCAL_SHARED_LIBRARIES := \
	libutils \
	libEGL \
	libGLESv1_CM \
	liblog \
	libcutils \
	libsync \
	libion \
	libedidParse.sunxi \
	libhwcproxy \
	libsysutils

LOCAL_C_INCLUDES += $(TARGET_HARDWARE_INCLUDE)
LOCAL_C_INCLUDES +=system/core/include/ \
	hardware/aw/include \
	hardware/aw/displayd/libhwcproxy

LOCAL_MODULE := hwcomposer.$(TARGET_BOARD_PLATFORM)
LOCAL_CFLAGS:= -DLOG_TAG=\"hwcomposer\"

HWC_WRITEBACK_ENABLE := enable
ifeq ($(HWC_WRITEBACK_ENABLE), enable)
   LOCAL_SRC_FILES += hwc_writeback.cpp
   LOCAL_SHARED_LIBRARIES += libcapture
   LOCAL_CFLAGS += -DHWC_WRITEBACK_ENABLE
endif

ifeq ($(SW_CHIP_PLATFORM),H6)
   LOCAL_CFLAGS += -DDE_VERSION=30
else
   LOCAL_CFLAGS += -DDE_VERSION=20
endif

ifneq ($(MARGIN_DEFAULT_PERCENT_WIDTH),)
    LOCAL_CFLAGS += -DMARGIN_DEFAULT_PERCENT_WIDTH=$(MARGIN_DEFAULT_PERCENT_WIDTH)
else
    LOCAL_CFLAGS += -DMARGIN_DEFAULT_PERCENT_WIDTH=100
endif
ifneq ($(MARGIN_DEFAULT_PERCENT_HEIGHT),)
    LOCAL_CFLAGS += -DMARGIN_DEFAULT_PERCENT_HEIGHT=$(MARGIN_DEFAULT_PERCENT_HEIGHT)
else
    LOCAL_CFLAGS += -DMARGIN_DEFAULT_PERCENT_HEIGHT=100
endif

ifneq ($(MAX_INIT_DISP_WIDTH),)
    LOCAL_CFLAGS += -DMAX_INIT_DISP_WIDTH=$(MAX_INIT_DISP_WIDTH)
else
    LOCAL_CFLAGS += -DMAX_INIT_DISP_WIDTH=0
endif
ifneq ($(MAX_INIT_DISP_HEIGHT),)
    LOCAL_CFLAGS += -DMAX_INIT_DISP_HEIGHT=$(MAX_INIT_DISP_HEIGHT)
else
    LOCAL_CFLAGS += -DMAX_INIT_DISP_HEIGHT=0
endif

ifneq ($(HDMI_CHANNEL),)
    LOCAL_CFLAGS += -DHDMI_USED=$(HDMI_CHANNEL)
else
    LOCAL_CFLAGS += -DHDMI_USED=-1
endif
ifneq ($(HDMI_DEFAULT_MODE),)
    LOCAL_CFLAGS += -DDISP_DEFAULT_HDMI_MODE=$(HDMI_DEFAULT_MODE)
else
    LOCAL_CFLAGS += -DDISP_DEFAULT_HDMI_MODE=4
endif

ifneq ($(CVBS_CHANNEL),)
    LOCAL_CFLAGS += -DCVBS_USED=$(CVBS_CHANNEL)
else
    LOCAL_CFLAGS += -DCVBS_USED=-1
endif
ifneq ($(CVBS_DEFAULT_MODE),)
    LOCAL_CFLAGS += -DDISP_DEFAULT_CVBS_MODE=$(CVBS_DEFAULT_MODE)
else
    LOCAL_CFLAGS += -DDISP_DEFAULT_CVBS_MODE=11
endif

ifneq ($(VGA_CHANNEL),)
    LOCAL_CFLAGS += -DVGA_USED=$(VGA_CHANNEL)
else
    LOCAL_CFLAGS += -DVGA_USED=-1
endif
ifneq ($(VGA_DEFAULT_MODE),)
    LOCAL_CFLAGS += -DDISP_DEFAULT_VGA_MODE=$(VGA_DEFAULT_MODE)
else
    LOCAL_CFLAGS += -DDISP_DEFAULT_VGA_MODE=-1
endif

ifneq ($(LCD_CHANNEL),)
    LOCAL_CFLAGS += -DLCD_USED=$(LCD_CHANNEL)
else
    LOCAL_CFLAGS += -DLCD_USED=-1
endif

ifeq ($(SAVE_DISP_CONFIGS_TO_PROPERTY), true)
    LOCAL_CFLAGS += -DSAVE_DISP_CONFIGS_TO_PROPERTY
endif

ifeq ($(CHECK_DRAM_FREQ_EACH_FRAME), true)
    LOCAL_CFLAGS += -DCHECK_DRAM_FREQ_EACH_FRAME
endif

LOCAL_MODULE_TAGS := optional
TARGET_GLOBAL_CFLAGS += -DTARGET_BOARD_PLATFORM=$(TARGET_BOARD_PLATFORM)
include $(BUILD_SHARED_LIBRARY)
