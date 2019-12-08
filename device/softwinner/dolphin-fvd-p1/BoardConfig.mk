# BoardConfig.mk
#
# Product-specific compile-time definitions.
#

include device/softwinner/dolphin-common/BoardConfigCommon.mk

TARGET_CPU_ABI := armeabi-v7a
TARGET_CPU_ABI2 := armeabi
TARGET_CPU_SMP := true
TARGET_ARCH := arm
TARGET_ARCH_VARIANT := armv7-a-neon
TARGET_CPU_VARIANT := cortex-a7

# Enable dex-preoptimization to speed up first boot sequence
ifeq ($(HOST_OS),linux)
  ifneq ($(TARGET_BUILD_VARIANT),eng)
    ifeq ($(WITH_DEXPREOPT),)
      WITH_DEXPREOPT := true
    endif
  endif
endif

# image related
TARGET_NO_BOOTLOADER := true
TARGET_NO_RECOVERY := false
TARGET_NO_KERNEL := false
#WITH_DEXPREOPT := true
#BOARD_IS_AUTOMOTIVE := true
INSTALLED_KERNEL_TARGET := kernel
BOARD_KERNEL_CMDLINE := selinux=1 androidboot.selinux=permissive
#selinux=1 androidboot.selinux=enforcing cma=64M
TARGET_USERIMAGES_USE_EXT4 := true
BOARD_FLASH_BLOCK_SIZE := 4096
BOARD_SYSTEMIMAGE_PARTITION_SIZE := 1610612736
BOARD_USERDATAIMAGE_PARTITION_SIZE := 1610612736
# recovery stuff
TARGET_RECOVERY_PIXEL_FORMAT := "BGRA_8888"
#TARGET_RECOVERY_UI_LIB := librecovery_ui_dolphin
SW_BOARD_TOUCH_RECOVERY := true
SW_BOARD_IR_RECOVERY :=true

TARGET_USES_AOSP := true
ENABLE_CPUSETS := true
# wifi and bt configuration
# 1. Wifi Configuration
BOARD_WIFI_VENDOR := common
WPA_SUPPLICANT_VERSION := VER_0_8_X
BOARD_WPA_SUPPLICANT_DRIVER := NL80211
BOARD_WPA_SUPPLICANT_PRIVATE_LIB := lib_driver_cmd_common
BOARD_HOSTAPD_DRIVER        := NL80211
BOARD_HOSTAPD_PRIVATE_LIB   := lib_driver_cmd_common

include hardware/aw/wlan/firmware/firmware.mk

# 2. Bluetooth Configuration
# make sure BOARD_HAVE_BLUETOOTH is true for every bt vendor
BOARD_HAVE_BLUETOOTH := true
BOARD_BLUETOOTH_BDROID_BUILDCFG_INCLUDE_DIR := device/softwinner/dolphin-fvd-p1/bluetooth
TARGET_USE_BOOSTUP_OPZ := true
