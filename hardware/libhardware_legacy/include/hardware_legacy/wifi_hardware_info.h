/*
 * Copyright (C) 2008 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef _WIFI_HARDWARE_INFO_H
#define _WIFI_HARDWARE_INFO_H

#if __cplusplus
extern "C" {
#endif

const char *get_wifi_vendor_name();
const char *get_wifi_module_name();
const char *get_wifi_driver_name();
const char *get_fw_path_sta();
const char *get_fw_path_ap();
const char *get_fw_path_p2p();
void get_driver_module_arg(char* arg);
const char *get_supplicant_para(int set_p2p_supported);

#if __cplusplus
};  // extern "C"
#endif

#endif  // _WIFI_HARDWARE_INFO_H
