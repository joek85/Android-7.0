
#DEVICE_PACKAGE_OVERLAYS := device/softwinner/dolphin-fvd-p1/overlay \
#                           $(DEVICE_PACKAGE_OVERLAYS)

# force copy init.zygote32.rc for 32bit project
PRODUCT_COPY_FILES += \
    system/core/rootdir/init.zygote32.rc:root/init.zygote32.rc

PRODUCT_COPY_FILES += \
    device/softwinner/dolphin-fvd-p1/kernel:kernel \
    device/softwinner/dolphin-fvd-p1/fstab.sun8iw7p1:root/fstab.sun8iw7p1 \
    device/softwinner/dolphin-fvd-p1/init.sun8iw7p1.rc:root/init.sun8iw7p1.rc \
    device/softwinner/dolphin-fvd-p1/init.recovery.sun8iw7p1.rc:root/init.recovery.sun8iw7p1.rc \
    device/softwinner/dolphin-fvd-p1/ueventd.sun8iw7p1.rc:root/ueventd.sun8iw7p1.rc \
    device/softwinner/dolphin-fvd-p1/recovery.fstab:recovery.fstab \
    device/softwinner/dolphin-fvd-p1/modules/modules/sunxi-ir-rx.ko:root/sunxi-ir-rx.ko \
    device/softwinner/dolphin-fvd-p1/modules/modules/nand.ko:root/nand.ko \
    device/softwinner/dolphin-fvd-p1/modules/modules/fivm.ko:root/fivm.ko \

PRODUCT_COPY_FILES += \
    frameworks/native/data/etc/android.hardware.ethernet.xml:system/etc/permissions/android.hardware.ethernet.xml \
    frameworks/native/data/etc/android.hardware.wifi.xml:system/etc/permissions/android.hardware.wifi.xml \
    frameworks/native/data/etc/android.hardware.wifi.direct.xml:system/etc/permissions/android.hardware.wifi.direct.xml \
    frameworks/native/data/etc/android.software.pppoe.xml:system/etc/permissions/android.software.pppoe.xml

PRODUCT_PROPERTY_OVERRIDES += \
    ro.carrier=unknown \
    ro.com.android.dateformat=MM-dd-yyyy \
    ro.config.ringtone=Ring_Synth_04.ogg \
    ro.config.notification_sound=pixiedust.ogg

#PRODUCT_COPY_FILES += \
#    hardware/realtek/bluetooth/rtl8723bs/firmware/rtl8723b_fw:system/etc/firmware/rtl8723b_fw \
#    hardware/realtek/bluetooth/rtl8723bs/firmware/rtl8723b_config:system/etc/firmware/rtl8723b_config

PRODUCT_CHARACTERISTICS := tablet
#
PRODUCT_PACKAGES += \
	rild \
    Bluetooth \
    audio.a2dp.default \
    libkaraokejni \
    libaudutils \
    micphone.dolphin \
    hifactory \
    ubootenv \
    USBBT
#    BLEAutoPair \

PRODUCT_PACKAGES += \
    audio.pt71600_remote.default.so

# Add wifi-related packages
PRODUCT_PACKAGES += libwpa_client wpa_supplicant hostapd wpa_supplicant.conf
PRODUCT_PROPERTY_OVERRIDES += wifi.interface=wlan0 \
                              wifi.supplicant_scan_interval=15

# iploader for dynamic library protection
PRODUCT_PACKAGES += \
	libip_loader \
	iploader_server

PRODUCT_COPY_FILES += \
    device/softwinner/dolphin-fvd-p1/configs/virtual-remote.kl:system/usr/keylayout/virtual-remote.kl \
    device/softwinner/dolphin-fvd-p1/configs/sunxi-keyboard.kl:system/usr/keylayout/sunxi-keyboard.kl \
    device/softwinner/dolphin-fvd-p1/configs/sunxi-ir.kl:system/usr/keylayout/sunxi-ir.kl \
    device/softwinner/dolphin-fvd-p1/configs/customer_ir_9f00.kl:system/usr/keylayout/customer_ir_9f00.kl \
    device/softwinner/dolphin-fvd-p1/configs/customer_ir_dd22.kl:system/usr/keylayout/customer_ir_dd22.kl \
    device/softwinner/dolphin-fvd-p1/configs/customer_ir_fb04.kl:system/usr/keylayout/customer_ir_fb04.kl \
    device/softwinner/dolphin-fvd-p1/configs/customer_ir_ff00.kl:system/usr/keylayout/customer_ir_ff00.kl \
    device/softwinner/dolphin-fvd-p1/configs/customer_ir_4cb3.kl:system/usr/keylayout/customer_ir_4cb3.kl \
    device/softwinner/dolphin-fvd-p1/configs/customer_ir_bc00.kl:system/usr/keylayout/customer_ir_bc00.kl \
    device/softwinner/dolphin-fvd-p1/configs/customer_ir_fc00.kl:system/usr/keylayout/customer_ir_fc00.kl \
    device/softwinner/dolphin-fvd-p1/configs/customer_ir_2992.kl:system/usr/keylayout/customer_ir_2992.kl \
    device/softwinner/dolphin-fvd-p1/configs/customer_rc5_ir_04.kl:system/usr/keylayout/customer_rc5_ir_04.kl \
    device/softwinner/dolphin-fvd-p1/configs/sunxi-ir-uinput.kl:system/usr/keylayout/sunxi-ir-uinput.kl


# bootanimation
#PRODUCT_COPY_FILES += \
#    device/softwinner/dolphin-fvd-p1/media/bootanimation.zip:system/media/bootanimation.zip

# Telephony
#$(call inherit-product, $(SRC_TARGET_DIR)/product/telephony.mk)

# Radio Packages and Configuration Flie
#$(call inherit-product, device/softwinner/common/ril_modem/huawei/mu509/huawei_mu509.mk)
#$(call inherit-product, device/softwinner/common/ril_modem/Oviphone/em55/oviphone_em55.mk)

PRODUCT_PROPERTY_OVERRIDES += \
    rw.logger=1

PRODUCT_PROPERTY_OVERRIDES += \
    dalvik.vm.heapsize=512m \
    dalvik.vm.heapstartsize=8m \
    dalvik.vm.heapgrowthlimit=192m \
    dalvik.vm.heaptargetutilization=0.75 \
    dalvik.vm.heapminfree=2m \
    dalvik.vm.heapmaxfree=8m \
    ro.zygote.disable_gl_preload=true

# Need AppWidget permission to prevent from Launcher's crash.
# TODO(pattjin): Remove this when the TV Launcher is used, which does not support AppWidget.
PRODUCT_COPY_FILES += \
    frameworks/native/data/etc/android.software.app_widgets.xml:system/etc/permissions/android.software.app_widgets.xml

PRODUCT_PROPERTY_OVERRIDES += \
	ro.crypto.sw2hwkeymaster = true

PRODUCT_PROPERTY_OVERRIDES += \
	ro.opengles.version=196608

#define virtual mouse key
PRODUCT_PROPERTY_OVERRIDES += \
        ro.softmouse.left.code=21 \
        ro.softmouse.right.code=22 \
        ro.softmouse.top.code=19 \
        ro.softmouse.bottom.code=20 \
        ro.softmouse.leftbtn.code=23 \
        ro.softmouse.midbtn.code=-1 \
        ro.softmouse.rightbtn.code=-1

# kernel io-device mem type setting
# 0xaf10 : iommu
# 0xfa01 : cma
PRODUCT_PROPERTY_OVERRIDES += \
    ro.kernel.iomem.type=0xfa01

