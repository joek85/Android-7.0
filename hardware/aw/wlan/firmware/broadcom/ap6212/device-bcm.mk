#
# Copyright (C) 2008 The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
#    hardware/aw/wlan/firmware/broadcom/ap6212/fw_bcm43438a0.bin:system/vendor/modules/fw_bcm43438a0.bin \
#    hardware/aw/wlan/firmware/broadcom/ap6212/fw_bcm43438a0_apsta.bin:system/vendor/modules/fw_bcm43438a0_apsta.bin \
#    hardware/aw/wlan/firmware/broadcom/ap6212/fw_bcm43438a0_p2p.bin:system/vendor/modules/fw_bcm43438a0_p2p.bin \

PRODUCT_COPY_FILES += \
    hardware/aw/wlan/firmware/broadcom/ap6212/nvram_ap6212.txt:system/vendor/modules/nvram_ap6212.txt \
    hardware/aw/wlan/firmware/broadcom/ap6212/bcm43438a0.hcd:system/vendor/modules/ap6212.hcd \
    hardware/aw/wlan/firmware/broadcom/ap6212/config.txt:system/vendor/modules/config_ap6212.txt \
    hardware/aw/wlan/firmware/broadcom/ap6212/fw_bcm43438a1.bin:system/vendor/modules/fw_bcm43438a1.bin \
    hardware/aw/wlan/firmware/broadcom/ap6212/fw_bcm43438a1_apsta.bin:system/vendor/modules/fw_bcm43438a1_apsta.bin \
    hardware/aw/wlan/firmware/broadcom/ap6212/fw_bcm43438a1_p2p.bin:system/vendor/modules/fw_bcm43438a1_p2p.bin \
    hardware/aw/wlan/firmware/broadcom/ap6212/bcm43438a1.hcd:system/vendor/modules/BCM43438A1.hcd
