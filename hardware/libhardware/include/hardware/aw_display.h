/*
 * Copyright (C) 2010 The Android Open Source Project
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

#ifndef __AW_DISPLAY_H__
#define __AW_DISPLAY_H__

#include <stdint.h>
#include <sys/cdefs.h>

__BEGIN_DECLS


typedef enum e_hwc_layer_cmd{
    /* rotation of the source image in degrees (0 to 359) */
    HWC_LAYER_ROTATION_DEG      = 1,
    /* enable or disable dithering */
    HWC_LAYER_DITHER            = 3,
    /* transformation applied (this is a superset of COPYBIT_ROTATION_DEG) */
    HWC_LAYER_SETINITPARA = 4,
    /* set videoplayer init overlay parameter */
    HWC_LAYER_SETVIDEOPARA = 5,
    /* set videoplayer play frame overlay parameter*/
    HWC_LAYER_SETFRAMEPARA = 6,
    /* get videoplayer play frame overlay parameter*/
    HWC_LAYER_GETCURFRAMEPARA = 7,
    /* query video blank interrupt*/
    HWC_LAYER_QUERYVBI = 8,
    /* set overlay screen id*/
    HWC_LAYER_SETMODE = 9,

    HWC_LAYER_SHOW = 0xa,
    HWC_LAYER_RELEASE = 0xb,
    HWC_LAYER_SET3DMODE = 0xc,
    HWC_LAYER_SETFORMAT = 0xd,
    HWC_LAYER_VPPON = 0xe,
    HWC_LAYER_VPPGETON = 0xf,
    HWC_LAYER_SETLUMASHARP = 0x10,
    HWC_LAYER_GETLUMASHARP = 0x11,
    HWC_LAYER_SETCHROMASHARP = 0x12,
    HWC_LAYER_GETCHROMASHARP = 0x13,
    HWC_LAYER_SETWHITEEXTEN = 0x14,
    HWC_LAYER_GETWHITEEXTEN = 0x15,
    HWC_LAYER_SETBLACKEXTEN = 0x16,
    HWC_LAYER_GETBLACKEXTEN = 0x17,
    HWC_LAYER_SET_3D_PARALLAX = 0x18,
    HWC_LAYER_SET_SCREEN_PARA = 0x19,

    HWC_LAYER_SETTOP        = 0x1a,
    HWC_LAYER_SETBOTTOM     = 0x1b,

    HWC_LAYER_HDMI_OUT      = 0x1c,
}e_hwc_layer_cmd_t;

typedef enum{
    DISPLAY_CMD_SET3DMODE = 0x01,
    DISPLAY_CMD_SETBACKLIGHTMODE = 0x02,
    DISPLAY_CMD_SETBACKLIGHTDEMOMODE = 0x03,
    DISPLAY_CMD_SETDISPLAYENHANCEMODE = 0x04,
    DISPLAY_CMD_SETDISPLAYENHANCEDEMOMODE = 0x05,
    DISPLAY_CMD_SETOUTPUTMODE = 0x06,
    DISPLAY_CMD_SETMARGIN = 0x07,
    DISPLAY_CMD_SETSATURATION = 0x08,
    DISPLAY_CMD_SETHUE = 0x09,
    DISPLAY_CMD_SETCONTRAST = 0x0a,
    DISPLAY_CMD_SETBRIGHT = 0x0b,
    DISPLAY_CMD_SET3DLAYEROFFSET = 0x0c,

    DISPLAY_CMD_ISSUPPORTHDMIMODE = 0x19,
    DISPLAY_CMD_GETSUPPORT3DMODE = 0x20,
    DISPLAY_CMD_GETOUTPUTTYPE = 0x21,
    DISPLAY_CMD_GETOUTPUTMODE = 0x22,
    DISPLAY_CMD_GETSATURATION = 0x23,
    DISPLAY_CMD_GETHUE = 0x24,
    DISPLAY_CMD_GETCONTRAST = 0x25,
    DISPLAY_CMD_GETBRIGHT = 0x26,
    DISPLAY_CMD_GETMARGIN_W = 0x27,
    DISPLAY_CMD_GETMARGIN_H = 0x28,

    DISPLAY_CMD_SETSCREENRADIO = 0x51,
    DISPLAY_CMD_SETVIDEOENABLE = 0x52,
}__display_cmd_t;

typedef enum
{
    DISPLAY_2D_ORIGINAL = 0,
    DISPLAY_2D_LEFT = 1,
    DISPLAY_2D_TOP = 2,
    DISPLAY_3D_LEFT_RIGHT_HDMI = 3,
    DISPLAY_3D_TOP_BOTTOM_HDMI = 4,
    DISPLAY_2D_DUAL_STREAM   = 5,
    DISPLAY_3D_DUAL_STREAM   = 6,
    DISPLAY_3D_LEFT_RIGHT_ALL = 7,
    DISPLAY_3D_TOP_BOTTOM_ALL = 8,
}__display_3d_mode;

typedef enum
{
    SCREEN_AUTO = 0,
    SCREEN_FULL = 1,
}__screen_radio;

typedef enum
{
    VIDEO_DISABLE = 0,
    VIDEO_ENABLE = 1,
}__video_opr;

enum {
    DISPLAY_OUTPUT_DATASPACE_MODE_AUTO = 0,
    DISPLAY_OUTPUT_DATASPACE_MODE_HDR,
    DISPLAY_OUTPUT_DATASPACE_MODE_WCG,
    DISPLAY_OUTPUT_DATASPACE_MODE_SDR,
    DISPLAY_OUTPUT_DATASPACE_MODE_OTHER,
    DISPLAY_OUTPUT_DATASPACE_MODE_NUM,
};

enum {
    DISPLAY_OUTPUT_FORMAT_UNKNOWN = 0,
    DISPLAY_OUTPUT_FORMAT_AUTO = DISPLAY_OUTPUT_FORMAT_UNKNOWN,
    DISPLAY_OUTPUT_FORMAT_YUV422_10bit,
    DISPLAY_OUTPUT_FORMAT_YUV420_10bit,
    DISPLAY_OUTPUT_FORMAT_YUV444_8bit,
    DISPLAY_OUTPUT_FORMAT_RGB888_8bit,

    DISPLAY_OUTPUT_FORMAT_NUM,
};

__END_DECLS

#endif /* __AW_DISPLAY_H__ */
