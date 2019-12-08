# RELEASE NAME: 20161220_TV_ANDROID_7.x
# RTKBT_API_VERSION=3.1.1.0

BOARD_HAVE_BLUETOOTH := true
BOARD_HAVE_BLUETOOTH_RTK := true
BOARD_HAVE_BLUETOOTH_RTK_COEX := true

#BOARD_HAVE_BLUETOOTH_RTK_ADDON := bee1 rtkbtAutoPairService rtkbtAutoPairUIDemo 
BOARD_HAVE_BLUETOOTH_RTK_ADDON := bee1 vr_bee1_hidraw_daemon
#BOARD_HAVE_BLUETOOTH_RTK_ADDON := bee2 rtkbtAutoPairService rtkbtAutoPairUIDemo 
#BOARD_HAVE_BLUETOOTH_RTK_ADDON := bee2 rtkbtAutoPairService rtkbtAutoPairUIDemo vr_bee2_hidraw_daemon
#BOARD_HAVE_BLUETOOTH_RTK_ADDON := basic

ifneq ($(BOARD_HAVE_BLUETOOTH_RTK_ADDON),)
$(foreach item,$(BOARD_HAVE_BLUETOOTH_RTK_ADDON),$(call inherit-product,$(LOCAL_PATH)/addon/$(item)/addon.mk))
endif

PRODUCT_COPY_FILES += \
	$(LOCAL_PATH)/system/etc/bluetooth/rtkbt.conf:system/etc/bluetooth/rtkbt.conf \
	$(LOCAL_PATH)/system/etc/firmware/rtl8723b_config:system/etc/firmware/rtl8723b_config \
	$(LOCAL_PATH)/system/etc/firmware/rtl8723b_fw:system/etc/firmware/rtl8723b_fw \
	$(LOCAL_PATH)/system/etc/firmware/rtl8723bs_config:system/etc/firmware/rtl8723bs_config \
	$(LOCAL_PATH)/system/etc/firmware/rtl8723bs_fw:system/etc/firmware/rtl8723bs_fw \
	$(LOCAL_PATH)/system/etc/firmware/rtl8723bs_VQ0_config:system/etc/firmware/rtl8723bs_VQ0_config \
	$(LOCAL_PATH)/system/etc/firmware/rtl8723bs_VQ0_fw:system/etc/firmware/rtl8723bs_VQ0_fw \
	$(LOCAL_PATH)/system/etc/firmware/rtl8723bu_config:system/etc/firmware/rtl8723bu_config \
	$(LOCAL_PATH)/system/etc/firmware/rtl8723d_config:system/etc/firmware/rtl8723d_config \
	$(LOCAL_PATH)/system/etc/firmware/rtl8723d_fw:system/etc/firmware/rtl8723d_fw \
	$(LOCAL_PATH)/system/etc/firmware/rtl8723ds_config:system/etc/firmware/rtl8723ds_config \
	$(LOCAL_PATH)/system/etc/firmware/rtl8723ds_fw:system/etc/firmware/rtl8723ds_fw \
	$(LOCAL_PATH)/system/etc/firmware/rtl8761a_config:system/etc/firmware/rtl8761a_config \
	$(LOCAL_PATH)/system/etc/firmware/rtl8761a_fw:system/etc/firmware/rtl8761a_fw \
	$(LOCAL_PATH)/system/etc/firmware/rtl8761at_config:system/etc/firmware/rtl8761at_config \
	$(LOCAL_PATH)/system/etc/firmware/rtl8761at_fw:system/etc/firmware/rtl8761at_fw \
	$(LOCAL_PATH)/system/etc/firmware/rtl8761au8192ee_fw:system/etc/firmware/rtl8761au8192ee_fw \
	$(LOCAL_PATH)/system/etc/firmware/rtl8761au8812ae_fw:system/etc/firmware/rtl8761au8812ae_fw \
	$(LOCAL_PATH)/system/etc/firmware/rtl8761au_fw:system/etc/firmware/rtl8761au_fw \
	$(LOCAL_PATH)/system/etc/firmware/rtl8761aw8192eu_config:system/etc/firmware/rtl8761aw8192eu_config \
	$(LOCAL_PATH)/system/etc/firmware/rtl8761aw8192eu_fw:system/etc/firmware/rtl8761aw8192eu_fw \
	$(LOCAL_PATH)/system/etc/firmware/rtl8821a_config:system/etc/firmware/rtl8821a_config \
	$(LOCAL_PATH)/system/etc/firmware/rtl8821a_fw:system/etc/firmware/rtl8821a_fw \
	$(LOCAL_PATH)/system/etc/firmware/rtl8821as_config:system/etc/firmware/rtl8821as_config \
	$(LOCAL_PATH)/system/etc/firmware/rtl8821as_fw:system/etc/firmware/rtl8821as_fw \
	$(LOCAL_PATH)/system/etc/firmware/rtl8821c_config:system/etc/firmware/rtl8821c_config \
	$(LOCAL_PATH)/system/etc/firmware/rtl8821c_fw:system/etc/firmware/rtl8821c_fw \
	$(LOCAL_PATH)/system/etc/firmware/rtl8821cs_config:system/etc/firmware/rtl8821cs_config \
	$(LOCAL_PATH)/system/etc/firmware/rtl8821cs_fw:system/etc/firmware/rtl8821cs_fw \
	$(LOCAL_PATH)/system/etc/firmware/rtl8822b_config:system/etc/firmware/rtl8822b_config \
	$(LOCAL_PATH)/system/etc/firmware/rtl8822b_fw:system/etc/firmware/rtl8822b_fw \
	$(LOCAL_PATH)/system/etc/firmware/rtl8822bs_config:system/etc/firmware/rtl8822bs_config \
	$(LOCAL_PATH)/system/etc/firmware/rtl8822bs_fw:system/etc/firmware/rtl8822bs_fw \
	$(LOCAL_PATH)/system/lib/hw/audio.vr_bee_hidraw.default.so:system/lib/hw/audio.vr_bee_hidraw.default.so \
	$(LOCAL_PATH)/system/lib/rtkbt/3dd_service.so:system/lib/rtkbt/3dd_service.so \
	$(LOCAL_PATH)/system/lib/rtkbt/autopair_huawei.so:system/lib/rtkbt/autopair_huawei.so \
	$(LOCAL_PATH)/system/lib/rtkbt/autopair.so:system/lib/rtkbt/autopair.so \
	$(LOCAL_PATH)/system/lib/rtkbt/autopair_stack.so:system/lib/rtkbt/autopair_stack.so \
	$(LOCAL_PATH)/system/lib/rtkbt/heartbeat.so:system/lib/rtkbt/heartbeat.so \
	$(LOCAL_PATH)/system/lib/rtkbt/vr_bee1_hidraw.so:system/lib/rtkbt/vr_bee1_hidraw.so \
	$(LOCAL_PATH)/system/lib/rtkbt/vr_bee2_hidraw.so:system/lib/rtkbt/vr_bee2_hidraw.so \
	$(LOCAL_PATH)/system/usr/keylayout/rtkbt_virtual_hid.kl:system/usr/keylayout/rtkbt_virtual_hid.kl \


PRODUCT_PACKAGES += \
	Bluetooth \
	audio.a2dp.default
