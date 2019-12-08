# Copyright 2006 The Android Open Source Project
LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)
ifdef WIFI_VENDOR_NAME
LOCAL_CFLAGS += -DWIFI_VENDOR_NAME=\"$(WIFI_VENDOR_NAME)\"
endif
ifdef WIFI_MODULE_NAME
LOCAL_CFLAGS += -DWIFI_MODULE_NAME=\"$(WIFI_MODULE_NAME)\"
endif
ifdef WIFI_DRIVER_NAME
LOCAL_CFLAGS += -DWIFI_DRIVER_NAME=\"$(WIFI_DRIVER_NAME)\"
endif
ifdef WIFI_DRIVER_FW_PATH_STA
LOCAL_CFLAGS += -DWIFI_DRIVER_FW_PATH_STA=\"$(WIFI_DRIVER_FW_PATH_STA)\"
endif
ifdef WIFI_DRIVER_FW_PATH_AP
LOCAL_CFLAGS += -DWIFI_DRIVER_FW_PATH_AP=\"$(WIFI_DRIVER_FW_PATH_AP)\"
endif
ifdef WIFI_DRIVER_FW_PATH_P2P
LOCAL_CFLAGS += -DWIFI_DRIVER_FW_PATH_P2P=\"$(WIFI_DRIVER_FW_PATH_P2P)\"
endif

LOCAL_MODULE:= libwifi_hardware_info
LOCAL_SRC_FILES += wifi_hardware_info.c
include $(BUILD_STATIC_LIBRARY)