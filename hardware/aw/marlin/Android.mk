LOCAL_PATH:= $(call my-dir)
ifeq ($(BOARD_MARLIN_USE_SECUREOS), 1)
include $(LOCAL_PATH)/Lib/secure/Android.mk
else
include $(LOCAL_PATH)/Lib/nonsecure/Android.mk
endif
