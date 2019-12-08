-include device/softwinner/common/addons/binary/tools.mk
# inherit tools.mk
$(call inherit-product, device/softwinner/common/addons/binary/tools.mk)
$(call inherit-product, device/softwinner/common/addons/framework/frameworks.mk)

PRODUCT_COPY_FILES += \
   device/softwinner/common/init.common.rc:root/init.common.rc \
   device/softwinner/common/addons/binary/mount.exfat:root/sbin/mount.exfat \
   device/softwinner/common/addons/binary/save_configs_factory.sh:root/sbin/save_configs_factory.sh \
   device/softwinner/common/addons/binary/reset_configs_factory.sh:root/sbin/reset_configs_factory.sh \
   build/tools/verity/rsa_key/verity_key:root/verity_key \
   build/tools/verity/rsa_key/rsa.pk:root/rsa.pk


RECOVERY_PRIVATE_TOOLS += \
   device/softwinner/common/addons/binary/mount.exfat

PRODUCT_PACKAGES += \
   multi_ir

PRODUCT_PACKAGES += \
   DragonSN \
   DragonAging \
   DragonBox \

PRODUCT_PACKAGES += \
   libpppoe-jni \
   pppoe
include frameworks/av/media/libcedarc/libcdclist.mk
include frameworks/av/media/libcedarx/libcdxlist.mk

PRODUCT_PACKAGES += \
   tv_input.default \
   input.evdev.default

ifeq ($(TARGET_BUILD_VARIANT),eng)
PRODUCT_PROPERTY_OVERRIDES += \
	dalvik.vm.image-dex2oat-filter="" \
	dalvik.vm.dex2oat-filter=""

PRODUCT_COPY_FILES += \
    device/softwinner/common/init.debug.rc:root/init.debug.rc
endif

PRODUCT_COPY_FILES += \
    device/softwinner/common/addons/external/pppoe/pppd/script/ip-up-pppoe:system/etc/ppp/ip-up-pppoe \
    device/softwinner/common/addons/external/pppoe/pppd/script/ip-down-pppoe:system/etc/ppp/ip-down-pppoe \
    device/softwinner/common/addons/external/pppoe/pppd/script/pppoe-options:system/etc/ppp/peers/pppoe-options \
    device/softwinner/common/addons/external/pppoe/pppd/script/pppoe-connect:system/bin/pppoe-connect \
    device/softwinner/common/addons/external/pppoe/pppd/script/pppoe-disconnect:system/bin/pppoe-disconnect

#boot opt
PRODUCT_PROPERTY_OVERRIDES += \
    persist.sys.bootopt.pms=1 \
    persist.sys.bootopt.pmsdata=1 \
    persist.sys.bootopt.preload=1

# setting default audio output/input
# "AUDIO_CODEC","AUDIO_HDMI","AUDIO_SPDIF","AUDIO_I2S", etc.
PRODUCT_PROPERTY_OVERRIDES += \
     audio.output.active=AUDIO_CODEC,AUDIO_HDMI \
     audio.input.active=AUDIO_CODEC
