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

#ifndef _CONFIG_H
#define _CONFIG_H

/*
 * current support output channel
 */
#define HW_NUM_DISPLAY 2

/*
 * default HDMI/CVBS channel maps
 */
#ifndef HDMI_CHANNEL
#define HDMI_CHANNEL 0
#endif

#ifndef CVBS_CHANNEL
#define CVBS_CHANNEL 1
#endif

#define DEFAULT_HDMI_MODE DISP_TV_MOD_720P_50HZ
#define DEFAULT_CVBS_MODE DISP_TV_MOD_PAL

/*
 * boot display resolution file name
 */
#define RSL_FILE_NAME             "/sys/class/disp/disp/attr/boot_para"

#define EXIT_PROP_NAME            "persist.sys.disp_init_exit"
#define SYSRSL_PROP_NAME          "persist.sys.sysrsl"
#define RT_DISP_POLICY_PROP_NAME  "persist.sys.disp_policy"
#define HDMI_HPD_PROP_NAME        "persist.sys.hdmi_hpd"
#define HDMI_RVTHPD_PROP_NAME     "persist.sys.hdmi_rvthpd"
#define CVBS_HPD_PROP_NAME        "persist.sys.cvbs_hpd"
#define CVBS_RVTHPD_PROP_NAME     "persist.sys.cvbs_rvthpd"
#define HDMI_HPD_STATE_FILENAME   "/sys/class/switch/hdmi/state"
#define CVBS_HPD_STATE_FILENAME   "/sys/class/switch/cvbs/state"

#endif
