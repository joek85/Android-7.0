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


-include hardware/broadcom/wlan/bcmdhd/config/config-bcm.mk

PRODUCT_COPY_FILES += \
    hardware/aw/wlan/firmware/broadcom/ap6356s/fw_bcm4356a2_ag.bin:system/vendor/modules/fw_bcm4356a2_ag.bin \
    hardware/aw/wlan/firmware/broadcom/ap6356s/fw_bcm4356a2_ag_apsta.bin:system/vendor/modules/fw_bcm4356a2_ag_apsta.bin \
    hardware/aw/wlan/firmware/broadcom/ap6356s/nvram_ap6356s.txt:system/vendor/modules/nvram_ap6356s.txt \
    hardware/aw/wlan/firmware/broadcom/ap6356s/bcm4356a2.hcd:system/vendor/modules/ap6356s.hcd \
    hardware/aw/wlan/firmware/broadcom/ap6356s/config.txt:system/vendor/modules/config_ap6356s.txt
