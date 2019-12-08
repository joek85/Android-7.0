LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_SRC_FILES += \
              voice_hidraw.c \
              voice_uipc.c \
              get_voice_app.c \
              ./msbc/msbc.c \
              ./sbc/sbcdec.c \
              config.c

LOCAL_C_INCLUDES += \
                    $(LOCAL_PATH)/sbc \
                    $(LOCAL_PATH)/msbc

LOCAL_CFLAGS += $(rtk_local_CFLAGS) -Wno-error=unused-parameter

LOCAL_MODULE_TAGS := optional
LOCAL_SHARED_LIBRARIES := libcutils libc liblog libtinyalsa libandroid_runtime libutils
LOCAL_MODULE := vr_bee1_hidraw_daemon
LOCAL_MULTILIB := 32

include $(BUILD_EXECUTABLE)
