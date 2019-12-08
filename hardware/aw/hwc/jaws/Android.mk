LOCAL_PATH := $(call my-dir)

# HAL module implemenation stored in
# hw/<OVERLAY_HARDWARE_MODULE_ID>.<ro.product.board>.so
include $(CLEAR_VARS)

LOCAL_MODULE_PATH := $(TARGET_OUT_SHARED_LIBRARIES)/hw
LOCAL_SHARED_LIBRARIES := liblog libEGL
LOCAL_SRC_FILES := \
	hwc.cpp \
	common/utils.cpp
LOCAL_SHARED_LIBRARIES := \
	libutils \
	libEGL \
	libGLESv1_CM \
	liblog \
	libcutils

ifneq ($(filter jaws,$(TARGET_BOARD_PLATFORM)),)
    LOCAL_SRC_FILES += \
        jaws/device_opr.cpp \
        jaws/hwc_sun9i.cpp
else
    ifneq ($(filter eagle,$(TARGET_BOARD_PLATFORM)),)
        LOCAL_SRC_FILES += \
            eagle/device_opr.cpp \
            eagle/hwc_sun8i.cpp
    else
        $(warning $(TARGET_BOARD_PLATFORM))
    endif
endif

LOCAL_C_INCLUDES += $(TARGET_HARDWARE_INCLUDE)
LOCAL_C_INCLUDES +=system/core/include/
LOCAL_MODULE := hwcomposer.$(TARGET_BOARD_PLATFORM)
LOCAL_CFLAGS:= -DLOG_TAG=\"hwcomposer\"
LOCAL_MODULE_TAGS := optional
TARGET_GLOBAL_CFLAGS += -DTARGET_BOARD_PLATFORM=$(TARGET_BOARD_PLATFORM)
include $(BUILD_SHARED_LIBRARY)