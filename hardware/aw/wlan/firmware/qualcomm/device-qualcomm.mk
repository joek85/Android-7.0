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

########################

#add for Qualcomm wifi
PRODUCT_COPY_FILES += \
   hardware/aw/wlan/firmware/qualcomm/bdwlan30.bin:system/etc/firmware/bdwlan30.bin \
   hardware/aw/wlan/firmware/qualcomm/qwlan30.bin:system/etc/firmware/qwlan30.bin \
   hardware/aw/wlan/firmware/qualcomm/otp30.bin:system/etc/firmware/otp30.bin \
   hardware/aw/wlan/firmware/qualcomm/wlan/cfg.dat:system/etc/firmware/wlan/cfg.dat \
   hardware/aw/wlan/firmware/qualcomm/wlan/qcom_cfg.ini:system/etc/firmware/wlan/qcom_cfg.ini
########################
