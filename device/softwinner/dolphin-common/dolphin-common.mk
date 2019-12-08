# inherit common.mk
$(call inherit-product, device/softwinner/common/common.mk)


PRODUCT_PACKAGES += \
    libedidParse.sunxi \
    libhwcproxy \
    camera.dolphin \
    displayd \
    hwcomposer.dolphin \

PRODUCT_PACKAGES += \
    libion

PRODUCT_PACKAGES += \
    audio.primary.dolphin

PRODUCT_PACKAGES += \
    setup_fs


PRODUCT_COPY_FILES += \
	hardware/aw/audio/audio_policy.conf:system/etc/audio_policy.conf \
	device/softwinner/dolphin-common/media_codecs.xml:system/etc/media_codecs.xml \
	device/softwinner/dolphin-common/media_codecs_performance.xml:system/etc/media_codecs_performance.xml \
	hardware/aw/audio/ac100_paths.xml:system/etc/ac100_paths.xml \
	frameworks/av/media/libstagefright/data/media_codecs_google_audio.xml:system/etc/media_codecs_google_audio.xml \
	frameworks/av/media/libstagefright/data/media_codecs_google_telephony.xml:system/etc/media_codecs_google_telephony.xml \
	frameworks/av/media/libstagefright/data/media_codecs_google_video.xml:system/etc/media_codecs_google_video.xml

PRODUCT_COPY_FILES += \
	frameworks/native/data/etc/android.hardware.usb.host.xml:system/etc/permissions/android.hardware.usb.host.xml \
        frameworks/native/data/etc/android.hardware.usb.accessory.xml:system/etc/permissions/android.hardware.usb.accessory.xml \
	frameworks/native/data/etc/android.hardware.screen.landscape.xml:system/etc/permissions/android.hardware.screen.landscape.xml \
        frameworks/native/data/etc/android.software.backup.xml:system/etc/permissions/android.software.backup.xml \
	frameworks/native/data/etc/android.hardware.screen.portrait.xml:system/etc/permissions/android.hardware.screen.portrait.xml

PRODUCT_COPY_FILES += \
	device/softwinner/dolphin-common/init.sun8iw7p1.usb.rc:root/init.sun8iw7p1.usb.rc

# egl
PRODUCT_COPY_FILES += \
	device/softwinner/dolphin-common/egl/egl.cfg:system/lib/egl/egl.cfg \
	device/softwinner/dolphin-common/egl/lib/gralloc.sunxi.so:system/lib/hw/gralloc.sun8iw7p1.so \
	device/softwinner/dolphin-common/egl/lib/libGLES_mali.so:system/lib/egl/libGLES_mali.so

PRODUCT_PROPERTY_OVERRIDES += \
	wifi.interface=wlan0 \
	wifi.supplicant_scan_interval=15 \
	keyguard.no_require_sim=true

PRODUCT_PROPERTY_OVERRIDES += \
	ro.kernel.android.checkjni=0
# 131072=0x20000 196608=0x30000
PRODUCT_PROPERTY_OVERRIDES += \
	ro.opengles.version=131072

#ExoPlayer youtube will use it
PRODUCT_PROPERTY_OVERRIDES += \
        sys.display-size=1280x720 \
	
# For mali GPU only
PRODUCT_PROPERTY_OVERRIDES += \
	debug.hwui.render_dirty_regions=false

PRODUCT_PROPERTY_OVERRIDES += \
        persist.sys.strictmode.visual=0 \
        persist.sys.strictmode.disable=1

PRODUCT_PROPERTY_OVERRIDES += \
	ro.sys.cputype=QuadCore-H64

# Enabling type-precise GC results in larger optimized DEX files.  The
# additional storage requirements for ".odex" files can cause /system
# to overflow on some devices, so this is configured separately for
# each product.
PRODUCT_TAGS += dalvik.gc.type-precise

PRODUCT_PROPERTY_OVERRIDES += \
    ro.product.firmware=v1.0rc5


# if DISPLAY_BUILD_NUMBER := true then
# BUILD_DISPLAY_ID := $(BUILD_ID).$(BUILD_NUMBER)
# required by gms.
DISPLAY_BUILD_NUMBER := true
BUILD_NUMBER := $(shell date +%Y%m%d)

# widevine
BOARD_WIDEVINE_OEMCRYPTO_LEVEL := 3

PRODUCT_PROPERTY_OVERRIDES += \
    drm.service.enabled=true

PRODUCT_PACKAGES += \
    com.google.widevine.software.drm.xml \
    com.google.widevine.software.drm \
    libdrmwvmplugin \
    libwvm \
    libWVStreamControlAPI_L${BOARD_WIDEVINE_OEMCRYPTO_LEVEL} \
    libwvdrm_L${BOARD_WIDEVINE_OEMCRYPTO_LEVEL} \
    libdrmdecrypt \
    libwvdrmengine

ifeq ($(BOARD_WIDEVINE_OEMCRYPTO_LEVEL), 1)
PRODUCT_PACKAGES += \
    liboemcrypto \
    libteec \
    tee_supplicant
PRODUCT_COPY_FILES += \
    device/softwinner/common/optee_ta/a98befed-d679-ce4a-a3c827dcd51d21ed.ta:system/bin/a98befed-d679-ce4a-a3c827dcd51d21ed.ta \
    device/softwinner/common/optee_ta/4d78d2ea-a631-70fb-aaa787c2b5773052.ta:system/bin/4d78d2ea-a631-70fb-aaa787c2b5773052.ta \
    device/softwinner/common/optee_ta/e41f7029-c73c-344a-8c5bae90c7439a47.ta:system/bin/e41f7029-c73c-344a-8c5bae90c7439a47.ta
PRODUCT_PROPERTY_OVERRIDES += \
    ro.sys.widevine_oemcrypto_level=1
SECURE_OS_OPTEE := yes
else
PRODUCT_PROPERTY_OVERRIDES += \
    ro.sys.widevine_oemcrypto_level=3
SECURE_OS_OPTEE := no
endif


