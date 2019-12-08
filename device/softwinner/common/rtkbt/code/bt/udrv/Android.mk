LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_SRC_FILES+= \
	./ulinux/uipc.c

LOCAL_C_INCLUDES += $(LOCAL_PATH)/include

LOCAL_C_INCLUDES+= . \
	$(LOCAL_PATH)/../ \
	$(LOCAL_PATH)/../bta/include \
	$(LOCAL_PATH)/../bta/sys \
	$(LOCAL_PATH)/../bta/dm \
	$(LOCAL_PATH)/../btcore/include \
	$(LOCAL_PATH)/../include \
	$(LOCAL_PATH)/../stack/include \
	$(LOCAL_PATH)/../stack/l2cap \
	$(LOCAL_PATH)/../stack/a2dp \
	$(LOCAL_PATH)/../stack/btm \
	$(LOCAL_PATH)/../stack/avdt \
	$(LOCAL_PATH)/../hcis \
	$(LOCAL_PATH)/../hcis/include \
	$(LOCAL_PATH)/../hcis/patchram \
	$(LOCAL_PATH)/../udrv/include \
	$(LOCAL_PATH)/../btif/include \
	$(LOCAL_PATH)/../btif/co \
	$(LOCAL_PATH)/../hci/include\
	$(LOCAL_PATH)/../vnd/include \
	$(LOCAL_PATH)/../brcm/include \
	$(LOCAL_PATH)/../embdrv/sbc/encoder/include \
	$(LOCAL_PATH)/../embdrv/sbc/decoder/include \
	$(LOCAL_PATH)/../audio_a2dp_hw \
	$(LOCAL_PATH)/../utils/include \
	$(bluetooth_C_INCLUDES) \
	external/tinyxml2 \
	external/zlib

LOCAL_MODULE:= libudrv-rtk
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := STATIC_LIBRARIES

LOCAL_CFLAGS += $(bluetooth_CFLAGS)
LOCAL_CONLYFLAGS += $(bluetooth_CONLYFLAGS)
LOCAL_CPPFLAGS += $(bluetooth_CPPFLAGS)

include $(BUILD_STATIC_LIBRARY)
