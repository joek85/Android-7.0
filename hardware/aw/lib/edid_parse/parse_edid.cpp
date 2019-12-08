/*
* (C) Copyright 2007-2013
* Allwinner Technology Co., Ltd. <www.allwinnertech.com>
* Charles <yanjianbo@allwinnertech.com>
*
* See file CREDITS for list of people who contributed to this
* project.
*
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License as
* published by the Free Software Foundation; either version 2 of
* the License, or (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.   See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place, Suite 330, Boston,
* MA 02111-1307 USA
*/

#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include "edid_parse/parse_edid_i.h"
#include "resolution.h"

//EDID_LENGTH: the size of one edid block.
#define EDID_LENGTH 0x80

//EDID-BLOCK-0
#define ID_HEADER 0x00
#define ID_VENDOR 0x08
#define ID_VERSION 0x12
#define ID_BASIC_FEATURES 0x14
#define ID_COLOR_CHARACTER 0x19
#define ID_ETB_TIMINGS 0x23
#define ID_STD_TIMINGS 0x26
#define ID_DET_DESCRIPTOR1 0x36
#define ID_DET_DESCRIPTOR2 0x48
#define ID_MON_DESCRIPTOR1 0x5A
#define ID_MON_DESCRIPTOR2 0x6C
#define ID_EXTENSION_FLAG 0x7E

//EDID-BLOCK-1
#define ID_EXTENSION_BASE 0x80
#define ID_EXTENSION_HEADER 0x00
#define ID_DISPLYA_SUPPORTS 0x03
#define ID_DATA_BLOCK 0x04

// CEA block type
#define ADDB_TAG    (0x1 << 5)
#define VDDB_TAG    (0x2 << 5)
#define VSDB_TAG    (0x3 << 5)
#define SADB_TAG    (0x4 << 5)
#define EXTENED_TAG (0x7 << 5)
#define TAG_MASK    0xE0

// CEA::Extended tag code
#define VIDEO_CAPABILITY        (0x00)
#define VENDOR_SPECIFIC_VIDEO   (0x01)
#define VESA_DISPLAY_DEVICE     (0x02)
#define VESA_VIDEO_TIMING       (0x03)
#define RESERVED_FOR_HDMI_VIDEO (0x04)
#define COLORIMETRY             (0x05)
#define HDR_STATIC_METADATA     (0x06)
#define YCbCr420_VDB            (0x0E)
#define YCbCr420_CMDB           (0x0F)

#define FIX_HEADER  1
#define FIX_INPUT   2
#define FIX_TIMINGS 3

struct broken_edid {
    char  manufacturer[4];
    unsigned int model;
    unsigned int fix;
};

struct version_info {
    char version;
    char revision;
};

struct timing_info {
    unsigned int h_res;
    unsigned int v_res;
    unsigned int frequency;
};

//**************************** FOR CHECK *******************************//
static const struct broken_edid brokendb[] = {
    /* examples:
    //DEC FR-PCXAV-YZ
    {
        .manufacturer = "DEC",
        .model        = 0x073a,
        .fix          = FIX_HEADER,
    },
   // ViewSonic PF775a
    {
        .manufacturer = "VSC",
        .model        = 0x5a44,
        .fix          = FIX_INPUT,
    },
    //Sharp UXGA
    {
        .manufacturer = "SHP",
        .model        = 0x138e,
        .fix          = FIX_TIMINGS,
    },
    */
};

static const char edid_header0[] = { 0x00, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0x00
};

static const struct version_info edid_version[] = {
    {
        .version = 0x1,
        .revision = 0x3,
    },
};

int check_edid(char *edid)
{
    char *block = edid + ID_VENDOR, manufacturer[4];
    unsigned char *b;
    unsigned int model;
    int i, fix = 0, ret = 0;

    manufacturer[0] = ((block[0] & 0x7c) >> 2) + '@';
    manufacturer[1] = ((block[0] & 0x03) << 3) +
        ((block[1] & 0xe0) >> 5) + '@';
    manufacturer[2] = (block[1] & 0x1f) + '@';
    manufacturer[3] = 0;
    model = block[2] + (block[3] << 8);
    for (i = 0; i < sizeof(brokendb) / sizeof(struct broken_edid); i++) {
        if (!strncmp(manufacturer, brokendb[i].manufacturer, 4) &&
                brokendb[i].model == model) {
            fix = brokendb[i].fix;
            break;
        }
    }

    switch (fix) {
    case FIX_HEADER:
        for (i = 0; i < 8; i++) {
            if (edid[i] != edid_header0[i]) {
                ret = fix;
                break;
            }
        }
        break;
    case FIX_INPUT:
        /*
        b = edid + EDID_STRUCT_DISPLAY;
         Only if display is GTF capable will
           the input type be reset to analog
        if (b[4] & 0x01 && b[0] & 0x80)
            ret = fix;
        break;
         */
    case FIX_TIMINGS:
        /*
        b = edid + DETAILED_TIMING_DESCRIPTIONS_START;
        ret = fix;

        for (i = 0; i < 4; i++) {
            if (edid_is_limits_block(b)) {
                ret = 0;
                break;
            }

            b += DETAILED_TIMING_DESCRIPTION_SIZE;
        }

        break;
        */
    default:
        ret = fix;
    }

    if (ret)
        ALOGD("check_edid: The EDID Block of "
               "Manufacturer: %s Model: 0x%x is known to "
               "be broken,\n",  manufacturer, model);

    return ret;
}

int fix_edid(unsigned char *edid)
{
    return 0;
}

static int edid_checksum(char const *edid_buf)
{
    char csum = 0, all_null = 0;
    int i;

    for (i = 0; i < EDID_LENGTH; i++) {
        csum += edid_buf[i];
        all_null |= edid_buf[i];
    }

    if (csum == 0x00 && all_null) {
        /* checksum passed, everything's good */
        return 0;
    }

    ALOGD("edid_checksum failed!\n");
    return -1;
}

static int edid_check_header0(char *header)
{
    int i;

    for (i = 0; i < 8; i++) {
        if (header[i] != edid_header0[i]) {
            ALOGD("edid_check_header0 failed \n");
            return -1;
        }
    }
    return 0;
}

static int edid_check_version(char const *version)
{
    int i;

    for(i = 0; i < sizeof(edid_version) / sizeof(struct version_info); i++) {
        if(edid_version[i].version == version[0] &&
            edid_version[i].revision == version[1]) {
            return 0;
        }
    }
    ALOGD("edid_check_version failed\n");
    return -1;
}

int check_edid_block0(char *edid, int const length)
{
    if((EDID_LENGTH <= length) &&
        !edid_checksum(edid) &&
        !edid_check_header0(edid) &&
        !edid_check_version(edid + ID_VERSION)) {
        return 0;
    }
    //if(check_edid(edid)) {
    //  //fix_edid(edid_buf);
    //  return 0;
    //}
    ALOGD("check_edid_block0 failed\n");
    return -1;
}

int check_extension_block(char const *edid, int const length)
{
    if(0 == edid[ID_EXTENSION_FLAG] ||
      length < EDID_LENGTH + EDID_LENGTH ||
      2 != edid[ID_EXTENSION_BASE + ID_EXTENSION_HEADER] ||
      3 != edid[ID_EXTENSION_BASE + ID_EXTENSION_HEADER + 1] || //support version 3
      0 != edid_checksum(edid + ID_EXTENSION_BASE)) {
        ALOGD("Can't find extension block");
        return 0;
    }
    ALOGD("Extension_block exist.\n");
    return 1;
}

//********************** some usful functions begin *********************//
static int find_data_block_offset(unsigned char *db_buf, int tag)
{
    int offset = 0;
    int length, i;

    for(i = 0; i < 4; i++ ) {
        if(tag == (TAG_MASK & db_buf[0])) {
            break;
        }
        length = db_buf[0] & ~(TAG_MASK);
        db_buf += (length + 1);
        offset += (length + 1);
    }

    if(i < 4)
        return offset;
    else
        return -1;
}

static int cal_v_res(int x_res, int ratio)
{
    switch(ratio) {
    case 0: // 00=1:1 prior to v1.3
        return x_res * 10 / 16;
    case 1:
        return x_res * 3 / 4;
    case 2:
        return x_res * 4 / 5;
    case 3:
        return x_res * 9 / 16;
    default:
        return 0;
    }
}

//this function only support partly mode
static unsigned long long timing_to_mode(struct timing_info *timing, int num)
{
    int i;
    unsigned long long mode = 0;
    for(i = 0; i < num; i++, timing++) {
        switch(timing->frequency) {
        case 60:
            if(720 == timing->h_res && 240 == timing->v_res)
                mode |= ((unsigned long long)1 << HDMI1440_480I);
            else if(720 == timing->h_res && 480 == timing->v_res)
                mode |= ((unsigned long long)1 << HDMI480P);
            else if(1280 == timing->h_res && 720 == timing->v_res)
                mode |= ((unsigned long long)1 << HDMI720P_60);
            else if(1920 == timing->h_res && 540 == timing->v_res)
                mode |= ((unsigned long long)1 << HDMI1080I_60);
            else if(1920 == timing->h_res && 1080 == timing->v_res)
                mode |= ((unsigned long long)1 << HDMI1080P_60);
            break;
        case 50:
            if(720 == timing->h_res && 288 == timing->v_res)
                mode |= ((unsigned long long)1 << HDMI1440_576I);
            else if(720 == timing->h_res && 576 == timing->v_res)
                mode |= ((unsigned long long)1 << HDMI576P);
            else if(1280 == timing->h_res && 720 == timing->v_res)
                mode |= ((unsigned long long)1 << HDMI720P_50);
            else if(1920 == timing->h_res && 540 == timing->v_res)
                mode |= ((unsigned long long)1 << HDMI1080I_50);
            else if(1920 == timing->h_res && 1080 == timing->v_res)
                mode |= ((unsigned long long)1 << HDMI1080P_50);
            break;
        case 25:
            if(1920 == timing->h_res && 1080 == timing->v_res)
                mode |= ((unsigned long long)1 << HDMI1080P_25);
            break;
        case 24:
            if(1920 == timing->h_res && 1080 == timing->v_res)
                mode |= ((unsigned long long)1 << HDMI1080P_24);
            break;
        default:
            //ALOGD("frequency =%d\n", timing[i].frequency);
            break;
        }
    }

    return mode;
}
//********************** some usful functions end *********************//


//**************************** VENDOR ****************************//
int get_vendor_info(char const *vendor, vendor_info_t *vendor_info)
{
    vendor_info->manufacturer[0] = ((vendor[0] & 0x7c) >> 2) + '@';
    vendor_info->manufacturer[1] = ((vendor[0] & 0x03) << 3) +
        ((vendor[1] & 0xe0) >> 5) + '@';
    vendor_info->manufacturer[2] = (vendor[1] & 0x1f) + '@';
    vendor_info->manufacturer[3] = 0;
    vendor_info->model = vendor[2] + (vendor[3] << 8);
    vendor_info->SN = vendor[4] + (vendor[5] << 8) +
        (vendor[6] << 8) + (vendor[7] << 8);
    vendor_info->week = vendor[8];
    vendor_info->year = vendor[9] + 1990;
    memcpy((void *)(vendor_info->datas), (void *)vendor,
        sizeof(vendor_info->datas) / sizeof(vendor_info->datas[0]));

    return 0;
}


//**************************** MODE ****************************//
//get mode by parsing standard timings block.
static int get_std_mode(unsigned char *buf, struct timing_info *info,
    int const max_num)
{
    int num = 0;
    int i = 0;
    int ratio = 0;
    struct timing_info mode;

    for(i = 0; i < 8 && max_num > num; i++, buf += 2) {
        if(0x01 != buf[0]) {
            mode.h_res = buf[0] * 8 + 248;
            ratio = (buf[1] >> 6) & 0x03;
            mode.v_res = cal_v_res(mode.h_res, ratio);
            mode.frequency = (buf[1] & 0x3F) + 60;
            memcpy((void *)info, (void *)&mode, sizeof(struct timing_info));
            info++;
            num++;
        }
    }

    return num;
}

//get timing of mode by parsing DTD block.
static int get_timing(unsigned char *buf, struct timing_info *info,
    int const max_num)
{
    int pclk, h_res, h_blanking, v_res, v_blanking, pixels_total, frame_rate;

    if((0 == buf[0] && 0 == buf[1] && 0 == buf[2]) || 1 > max_num) {
        return 0;
    }

    pclk       = buf[0] + ((unsigned int)buf[1] << 8); //10KHz
    h_res      = buf[2] + (((unsigned int)buf[4] << 4) & 0x0F00);
    h_blanking = buf[3] + (((unsigned int)buf[4] << 8) & 0x0F00);
    v_res      = buf[5] + (((unsigned int)buf[7] << 4) & 0x0F00);
    v_blanking = buf[6] + (((unsigned int)buf[7] << 8) & 0x0F00);
    pixels_total = (h_res + h_blanking) * (v_res + v_blanking);

    if(0 != pixels_total) {
        frame_rate = pclk * 10000 / pixels_total;
    } else {
        //ALOGD("get build in mode failed\n");
        return 0;
    }
    switch(frame_rate) {
    case 59:
    case 60:
        info->frequency = 60;
        break;
    case 49:
    case 50:
        info->frequency = 50;
        break;
    case 23:
    case 24:
        info->frequency = 24;
        break;
    }
    info->h_res = h_res;
    info->v_res = v_res;
    //ALOGD("get_timing:[%d,%d,%d]\n", h_res, v_res, info->frequency);
    return 1;
}

//get video mode by parsing VDDB.
static unsigned long long get_video_mode(unsigned char *vddb)
{
    int i;
    int const length = vddb[0] & ~(TAG_MASK);
    unsigned long long mode = 0;

    vddb++;
    for(i = 0; i < length; i++, vddb++) {
        mode |= ((unsigned long long)1 << ((*vddb) & 0x7F));
    }

    return mode;
}

//get native video mode by parsing VDDB.
static unsigned int get_native_video_mode(unsigned char *vddb)
{
    int i;
    int const length = vddb[0] & ~(TAG_MASK);
    unsigned int mode = 0;

    vddb++;
    for(i = 0; i < length; i++, vddb++) {
        if((*vddb) & 0x80) {
            mode = ((*vddb) & 0x7F);
            break;
        }
    }
    //ALOGD("#########get_native_video_mode = %d\n", mode);
    return mode;
}

//get 4k mode by parsing VSDB.
static unsigned int get_4K_mode(unsigned char *vsdb)
{
    int const length = vsdb[0] & ~(TAG_MASK);
    int mode = 0;
    int hdmi_vic_index = 9;
    int hdmi_vic_len = 0;
    int i = 0;

    if(length < 8 ||
        (vsdb[1] != 0x03) || (vsdb[2] != 0x0c) || (vsdb[3] != 0x00) ||
        !(vsdb[8] & 0x20)) {
        ALOGD("get_4K_mode failed\n");
        return 0;
    }

    if((vsdb[8] & 0x80)) {
        hdmi_vic_index += 2;
    }
    if((vsdb[8] & 0x40)) {
        hdmi_vic_index += 2;
    }
    hdmi_vic_index++;

    if(length > hdmi_vic_index) {
        hdmi_vic_len = vsdb[hdmi_vic_index] >> 5;
        if(length >= hdmi_vic_index + hdmi_vic_len) {
            hdmi_vic_index++;
            for(i = 0; i < hdmi_vic_len; i++) {
                mode |= (1 << vsdb[hdmi_vic_index + i]);
            }
        }
    }

    return mode;
}

//get 3d mode by parsing VSDB.
static unsigned int get_stereo_mode(unsigned char *vsdb)
{
    int const length = vsdb[0] & ~(TAG_MASK);
    int mode = 0;
    int index = 9;
    int hdmi_vic_len = 0;
    int hdmi_3d_len = 0;
    int trd_multi_present = 0;
    int i = 0;

    if(length < 8 ||
        (vsdb[1] != 0x03) || (vsdb[2] != 0x0c) || (vsdb[3] != 0x00) ||
        !(vsdb[8] & 0x20)) {
        ALOGD("get_stereo_mode failed\n");
        return 0;
    }

    if((vsdb[8] & 0x80)) {
        index += 2;
    }
    if((vsdb[8] & 0x40)) {
        index += 2;
    }

/*  fixme: how to pare 3D infomations ?
    if(length > index && (vsdb[index] & 0x80) && (vsdb[index] & 0x60)) {
        trd_multi_present = (vsdb[index] & 0x60) >> 5;
        hdmi_vic_len = vsdb[index + 1] >> 5;
        hdmi_3d_len = vsdb[index + 1] & 0x1F;
        if(length >= index + hdmi_vic_len + hdmi_3d_len + 1) {
            index += (hdmi_vic_len + 2);
            for(i = 0; i < hdmi_3d_len; i++) {
            //  mode |= (1 << vsdb[hdmi_vic_index + i]);
            }
        }
    }
*/
    if(length > index && (vsdb[index] & 0x80)) {
        return 1; // hdmi_3d_present
    }

    return mode;
}

int get_hdmi_mode(char *edid, int extension_blocks,
    hdmi_mode_info_t *info)
{
#define HDMI_COMMON_MODE_NUM 64
    int offset, ret = 0;
    int mode_sum = 0;
    struct timing_info timing_info[HDMI_COMMON_MODE_NUM];
    struct timing_info *p_time_info = timing_info;

    memset((void *)info, 0, sizeof(hdmi_mode_info_t));
    // 1. get standard mode
    ret = get_std_mode((unsigned char *)edid + ID_STD_TIMINGS,
        p_time_info, HDMI_COMMON_MODE_NUM);
    info->standard_mode = timing_to_mode(timing_info, ret);

    // 2. get common mode
    ret = get_timing((unsigned char *)edid + ID_DET_DESCRIPTOR1,
        p_time_info, HDMI_COMMON_MODE_NUM - mode_sum);
    mode_sum += ret;

    p_time_info += ret;
    ret = get_timing((unsigned char *)edid + ID_DET_DESCRIPTOR2,
        p_time_info, HDMI_COMMON_MODE_NUM - mode_sum);
    mode_sum += ret;

    info->common_mode = timing_to_mode(timing_info, mode_sum);

    // 3. get common mode and native mode.
    if(0 == extension_blocks) {
        return 0;
    }

    offset = find_data_block_offset((unsigned char *)edid +
        ID_EXTENSION_BASE + ID_DATA_BLOCK, VDDB_TAG);
    if(0 <= offset) {
        info->common_mode |= get_video_mode((unsigned char *)edid +
            ID_EXTENSION_BASE + ID_DATA_BLOCK + offset);
        info->native_mode = get_native_video_mode((unsigned char *)edid +
            ID_EXTENSION_BASE + ID_DATA_BLOCK + offset);
        //ALOGD("info->native_mode = 0x%x\n", info->native_mode);
    }

    offset = (unsigned char)edid[ID_EXTENSION_BASE + 2];
    p_time_info = timing_info;
    mode_sum = 0;
    ret = get_timing((unsigned char *)edid + ID_EXTENSION_BASE + offset,
        p_time_info, HDMI_COMMON_MODE_NUM - mode_sum);
    mode_sum += ret;

    offset += 18;
    p_time_info += ret;
    ret = get_timing((unsigned char *)edid + ID_EXTENSION_BASE + offset,
        p_time_info, HDMI_COMMON_MODE_NUM - mode_sum);
    mode_sum += ret;

    offset += 18;
    p_time_info += ret;
    ret = get_timing((unsigned char *)edid + ID_EXTENSION_BASE + offset,
        p_time_info, HDMI_COMMON_MODE_NUM - mode_sum);
    mode_sum += ret;

    offset += 18;
    p_time_info += ret;
    ret = get_timing((unsigned char *)edid + ID_EXTENSION_BASE + offset,
        p_time_info, HDMI_COMMON_MODE_NUM - mode_sum);
    mode_sum += ret;

    info->common_mode |= timing_to_mode(timing_info, mode_sum);

    // 4. get 4K mode and 3d mode
    offset = find_data_block_offset((unsigned char *)edid +
        ID_EXTENSION_BASE + ID_DATA_BLOCK, VSDB_TAG);
    if(0 <= offset) {
        info->hd_2160P_mode = get_4K_mode((unsigned char *)edid +
            ID_EXTENSION_BASE + ID_DATA_BLOCK + offset);
        info->stereo_present = get_stereo_mode((unsigned char *)edid +
            ID_EXTENSION_BASE + ID_DATA_BLOCK + offset);
        //ALOGD("3d present =%d\n", info->stereo_present);
    }

    return 0;
}

//**************************** CEC ****************************//
//get cec physical address by parsing VSDB
static unsigned int get_cec_phy_addr(unsigned char *vsdb)
{
    int const length = vsdb[0] & ~(TAG_MASK);

    if(length < 5 ||
        (vsdb[1] != 0x03) || (vsdb[2] != 0x0c) || (vsdb[3] != 0x00)) {
        ALOGD("get_cec_phy_addr failed\n");
        return 0;
    }
    return (vsdb[4]) | (vsdb[5] << 8);
}

int get_cec_info(char *edid, int extension_blocks,
    cec_info_t *info)
{
    int offset = 0;

    memset((void *)info, 0, sizeof(cec_info_t));
    if(0 == extension_blocks) {
        return -1;
    }
    offset = find_data_block_offset((unsigned char *)edid +
        ID_EXTENSION_BASE + ID_DATA_BLOCK, VSDB_TAG);
    if(0 <= offset) {
        info->phy_addr = get_cec_phy_addr((unsigned char *)edid +
            ID_EXTENSION_BASE + ID_DATA_BLOCK + offset);
    }
    return 0;
}

//************************ THE MAIN INTERFACE FOR DEBUG *******************************//
int get_edid_info(char *edid_buf, int const length, edid_info_t *info)
{
    int i = 0;
    int extension_blocks = 0;

    if(check_edid_block0(edid_buf, length)) {
        return -1;
    }

    extension_blocks = check_extension_block(edid_buf, length);

    for(i = 0; info[i].cmd != EDID_GET_FINISH; i++) {
        switch(info[i].cmd) {
        case EDID_GET_VERDOR_INFO: {
            info[i].ret = get_vendor_info(edid_buf + ID_VENDOR,
                (vendor_info_t *)info[i].data);
        }
            break;
        case EDID_GET_HDMI_MODE: {
            info[i].ret = get_hdmi_mode(edid_buf, extension_blocks,
                (hdmi_mode_info_t *)info[i].data);
        }
            break;
        case EDID_GET_CEC_INFO: {
            info[i].ret = get_cec_info(edid_buf, extension_blocks,
                (cec_info_t *)info[i].data);
        }
            break;
        default:
            ALOGD("info_cmd[%d] is not supported\n", info[i].cmd);
        }
    }

    return extension_blocks;
}

// parse CEA block for HDR
static int do_fine_all_cea_data_block_offset(char *data, int length, int type, int extend_tag,
    int *offset)
{
    int find = 0;
    int i;
    for (i = 0; i < length; i += (data[i] & 0x1f) + 1)
        if ((data[i] & TAG_MASK) == type) {
            if (extend_tag == -1)
                offset[find++] = i;
            else if (data[i+1] == extend_tag)
                offset[find++] = i;
        }
    ALOGD("Find %d block match (%d %d)\n", find, type, extend_tag);
    return find;
}

static int find_cea_block_offset(char *edid, int extension_blocks, int type, int extend_tag,
    int *offset) {
    for (int i = 1; i <= extension_blocks; i++) {
        char * extension_tag = (char *)(edid + i * EDID_LENGTH);
        if (*extension_tag != 0x02)
            continue;

        int version    = extension_tag[1];
        int dtd_offset = extension_tag[2];

        if (version != 3) {
            ALOGE("CEA extension block version: %d\n", version);
            continue;
        }

        // Data Block Collection
        ALOGD("CEA Data Block Collection length: %d\n", dtd_offset - 4);
        int out[4];
        int cnt = do_fine_all_cea_data_block_offset(extension_tag + 4, dtd_offset - 4, type, extend_tag, &out[0]);
        if (cnt > 0) {
            for (int j = 0; j < cnt; j++)
                offset[j] = out[j] + i * EDID_LENGTH + 4;
            return cnt;
        }
    }
    ALOGE("Can't find tag (%d %d)\n", type, extend_tag);
    return -1;
}

static void dump_data_block(const char *prefix, char *data) {
    int length = (data[0]) & 0x1f;
    char buf[1024] = {0};
    char *p = buf;
    p += sprintf(p, "%s: TAG(%02X)", prefix, ((unsigned char)(data[0]) >> 5) & 0x07);
    for (int i = 0; i < length; i++)
        p += sprintf(p, " %02X", (unsigned char)(data[i+1]));
    ALOGD("%s\n", buf);
}

int get_hdr_capabilites(char *edid, int extension_blocks,
    hdr_capability_t *cap)
{
    int offset[4] = {0};

    memset((void *)cap, 0, sizeof(hdr_capability_t));
    if (extension_blocks == 0)
        return -1;
    int cnt = find_cea_block_offset(edid, extension_blocks, EXTENED_TAG, HDR_STATIC_METADATA, offset);
    if (cnt < 0) {
        ALOGE("Can't find HDR static metadata\n");
        return -1;
    }

    ALOGD("HDR static metadata offset: %d\n", offset[0]);
    dump_data_block("HDR static metadata", edid + offset[0]);
    char *hdr_block = (edid + offset[0]);
    int length = (hdr_block[0] & 0x1f);
    cap->eotf = (unsigned int)hdr_block[2];
    cap->metadata_descriptor = hdr_block[3];

    if (length >= 4) cap->max_luminance = hdr_block[4];
    if (length >= 5) cap->max_frame_average_luminance = hdr_block[5];
    if (length >= 6) cap->min_luminance = hdr_block[6];

    return 0;
}

int get_supported_colorimetry(char *edid, int extension_blocks,
    int *colorimetry)
{
    int offset[4] = {0};

    *colorimetry = 0;
    if (extension_blocks == 0)
        return -1;
    int cnt = find_cea_block_offset(edid, extension_blocks, EXTENED_TAG, COLORIMETRY, offset);
    if (cnt < 0) {
        ALOGE("Can't find Colorimetry Data Block\n");
        return -1;
    }

    ALOGD("Colorimetry Data Block offset: %d\n", offset[0]);
    dump_data_block("Colorimetry block", edid + offset[0]);
    char *colorimetry_block = (edid + offset[0]);
    *colorimetry = (unsigned int)colorimetry_block[2];
    return 0;
}

int get_hdmi_vendor_specific_data_block(char *edid, int extension_blocks,
    hdmi_forum_vsdb_t *vsdb)
{
    int offset[4] = {0};

    memset(vsdb, 0, sizeof(hdmi_forum_vsdb_t));
    if (extension_blocks == 0)
        return -1;
    int cnt = find_cea_block_offset(edid, extension_blocks, VSDB_TAG, -1, offset);
    if (cnt < 0) {
        ALOGE("Can't find Vendor specific Data Block\n");
        return -1;
    }

    for (int i = 0; i < cnt; i++) {
        unsigned char *vsdb_block = (unsigned char *)(edid + offset[i]);
        int ieee_oui = (vsdb_block[3] << 16) + (vsdb_block[2] << 8) + vsdb_block[1];
        if (ieee_oui == 0xc45dd8) {
            ALOGD("HDMI 2.0 Vendor specific data block offset: %d\n", offset[0]);
            dump_data_block("Vendor specific block", edid + offset[i]);
            vsdb->ieee_oui = ieee_oui;
            vsdb->version  = vsdb_block[4];
            vsdb->max_tmds_rate = vsdb_block[5] * 5;
            vsdb->reserved = vsdb_block[6];

            vsdb->deep_color_420_encoding |= PIXEL_ENCODING_YUV420_8BIT;
            if (vsdb_block[7] & 0x01) vsdb->deep_color_420_encoding |= PIXEL_ENCODING_YUV420_10BIT;
            if (vsdb_block[7] & 0x02) vsdb->deep_color_420_encoding |= PIXEL_ENCODING_YUV420_12BIT;
            if (vsdb_block[7] & 0x04) vsdb->deep_color_420_encoding |= PIXEL_ENCODING_YUV420_16BIT;
        }
    }
    return 0;
}

int get_hdmi1p4_vendor_specific_data_block(char *edid, int extension_blocks,
                                           sunxi_hdmi_capability_t *cap)
{
    int offset[4] = {0};

    if (extension_blocks == 0)
        return -1;
    int cnt = find_cea_block_offset(edid, extension_blocks, VSDB_TAG, -1, offset);
    if (cnt < 0) {
        ALOGE("Can't find Vendor specific Data Block\n");
        return -1;
    }

    for (int i = 0; i < cnt; i++) {
        unsigned char *vsdb_block = (unsigned char *)(edid + offset[i]);
        int ieee_oui = (vsdb_block[3] << 16) + (vsdb_block[2] << 8) + vsdb_block[1];
        if (ieee_oui == 0x000c03) {
            ALOGD("HDMI 1.4 Vendor specific data block offset: %d\n", offset[0]);
            dump_data_block("Vendor specific block", edid + offset[i]);

            int length = vsdb_block[0] & 0x1f;
            cap->video_format |= (VIDEO_FORMAT_YCBCR444 | VIDEO_FORMAT_YCBCR422 | VIDEO_FORMAT_RGB);
            cap->supported_format[FMT_RGB888].supported = 1;
            cap->supported_format[FMT_YUV444].supported = 1;
            cap->supported_format[FMT_YUV422].supported = 1;
            cap->supported_format[FMT_RGB888].bits |= COLOR_DEPTH_8BIT;
            cap->supported_format[FMT_YUV444].bits |= COLOR_DEPTH_8BIT;
            cap->supported_format[FMT_YUV422].bits |= COLOR_DEPTH_8BIT;
            if (length > 5) {
                if (vsdb_block[6] & 0x40) cap->supported_format[FMT_RGB888].bits |= COLOR_DEPTH_16BIT;
                if (vsdb_block[6] & 0x20) cap->supported_format[FMT_RGB888].bits |= COLOR_DEPTH_12BIT;
                if (vsdb_block[6] & 0x10) cap->supported_format[FMT_RGB888].bits |= COLOR_DEPTH_10BIT;
                if (vsdb_block[6] & 0x08) {
                    cap->supported_format[FMT_YUV444].bits |= cap->supported_format[FMT_RGB888].bits;
                }
            }

            if (length > 6) {
                int tmds_rate = vsdb_block[7] * 5;
                if (tmds_rate > cap->max_tmds_rate) cap->max_tmds_rate = tmds_rate;
            }
        }
    }
    cap->dvihdmi = 2;
    return 0;
}

static int cea_svd(unsigned char *x, int length, int *vics)
{
    int cnt = 0;
    for (int i = 0; i < length; i++) {
        unsigned char svd = x[i];
        unsigned char native;
        unsigned char vic;

        if ((svd & 0x7f) == 0)
            continue;

        if ((svd - 1) & 0x40) {
            vic = svd;
            native = 0;
        } else {
            vic = svd & 0x7f;
            native = svd & 0x80;
        }
        vics[cnt++] = vic;
    }
    return cnt;
}

int get_video_formats(char *edid, int extension_blocks,
	int **out)
{
    int *vics;
    int offset[4] = {0};

    if (extension_blocks == 0)
        return -1;
    int cnt = find_cea_block_offset(edid, extension_blocks, VDDB_TAG, -1, offset);
    ALOGD("Video Data Block offset: %d\n", offset[0]);
    if (cnt < 0) {
        ALOGE("Can't find Video Data Block\n");
        return -1;
    }
    dump_data_block("Video Data Block", edid + offset[0]);

    char *vdb = (edid + offset[0]);
    int length = (vdb[0] & 0x1f);
    vics = (int *)malloc(sizeof(int) * length);
    memset(vics, 0, sizeof(int) * length);
    cea_svd((unsigned char *)vdb + 1, length, vics);
    *out = vics;
    return 0;
}

int get_supported_y420_encoding_vic(char *edid, int extension_blocks,
    int vics[8])
{
    int offset[4] = {0};

    if (extension_blocks == 0)
        return -1;
    int cnt = find_cea_block_offset(edid, extension_blocks, EXTENED_TAG, YCbCr420_VDB, offset);
    ALOGD("YCbCr4:2:0 Video Data Block offset: %d\n", offset[0]);
    if (cnt < 0) {
        ALOGE("Can't find YCbCr4:2:0 Video Data Block\n");
        return -1;
    }
    dump_data_block("YCbCr4:2:0 Video Data Block", edid + offset[0]);
    char *y420vdb = (edid + offset[0]);

    int length = y420vdb[0] & 0x1f;
    return cea_svd((unsigned char *)y420vdb + 2, length - 1, vics);
}

int get_supported_y420_encoding_vic_map(char *edid, int extension_blocks,
    int vics[8])
{
    int offset[4] = {0};

    memset(vics, 0, sizeof(int) * 8);
    if (extension_blocks == 0)
        return -1;
    int cnt = find_cea_block_offset(edid, extension_blocks, EXTENED_TAG, YCbCr420_CMDB, offset);
    ALOGD("YCbCr4:2:0 Capability Map Data Block offset: %d\n", offset[0]);
    if (cnt < 0) {
        ALOGE("Can't find YCbCr4:2:0 Capability Map Data Block\n");
        return -1;
    }
    dump_data_block("YCbCr4:2:0 Capability Map Data Block", edid + offset[0]);
    unsigned char *y420vdb = (unsigned char *)(edid + offset[0]);
    int length = (y420vdb[0] & 0x1f) - 1;

    if (length == 0) {
        ALOGD("All svds support YCbCr420 sampling");
        return 0;
    }

    int index[16] = {0};
    for (int i = 0; i < length; i++) {
        index[i] = y420vdb[2 + i];
    }

    int total = 0;
    int *vdb = 0;
    if (get_video_formats(edid, extension_blocks, &vdb) == 0) {
        for (int i = 0; i < length; i++) {
            int *vdb_entry = vdb + i * 8;
            for (int j = 0; j < 8; j++)
                if (index[i] & (1 << j)) {
                    vics[total++] = *(vdb_entry + j);
                }
        }
        free(vdb);
    }

    return total;
}

int is_rgb_only(char *edid, int extension_blocks)
{
    int supported_yuv = 0;

    for (int i = 1; i <= extension_blocks; i++) {
        char * extension_tag = (char *)(edid + i * EDID_LENGTH);
        if (*extension_tag != 0x02)
            continue;

        int version    = extension_tag[1];
        int dtd_offset = extension_tag[2];

        if (version >= 2) {
            ALOGE("CEA extension block version: %d\n", version);
            if ((extension_tag[3] & 0x20) || (extension_tag[3] & 0x10)) {
                supported_yuv = 1;
                break;
            }
        }
    }

    ALOGE("Sink support yuv: %d", supported_yuv);
    return !supported_yuv;
}

/* return sunxi display tv mode */
static int to_sunxi_output_resolution(int vic)
{
    for (unsigned int i = 0; i < sizeof(vic2resolution) / sizeof(vic2resolution[0]); i++) {
        if (vic == vic2resolution[i].vic) {
            return vic2resolution[i].sunxi_disp_mode;
        }
    }
    return -1;
}

int get_supported_pixel_formats(char *edid, int extension_blocks, int *formats)
{
    *formats |= VIDEO_FORMAT_RGB;
    for (int i = 1; i <= extension_blocks; i++) {
        char * extension_tag = (char *)(edid + i * EDID_LENGTH);
        if (*extension_tag != 0x02)
            continue;

        int version    = extension_tag[1];
        int dtd_offset = extension_tag[2];

        if (version != 3) {
            ALOGE("CEA extension block version: %d\n", version);
            continue;
        }

        int support = extension_tag[3];
        if (support & (1 << 5))
            *formats |= VIDEO_FORMAT_YCBCR444;
        if (support & (1 << 4))
            *formats |= VIDEO_FORMAT_YCBCR422;
    }
    return 0;
}

static void update_yuv420_vic(int *vics, int count, sunxi_hdmi_capability_t *cap, int flag)
{
    for (int i = 0; i < count; i++) {
        int tv_mode = to_sunxi_output_resolution(vics[i]);
        if (tv_mode == -1)
            continue;

        struct vic_info *find = NULL;
        for (int j = 0; j < 8; j++) {
            if (cap->ycbcr420_vic[j].vic == vics[i]) {
                find = &cap->ycbcr420_vic[j];
                break;
            }
        }

        if (find) {
            find->support_regular_sampling_mode |= flag;
        } else if (cap->total_ycbcr420_vic < 8) {
            find = &cap->ycbcr420_vic[cap->total_ycbcr420_vic];

            find->vic = vics[i];
            find->sunxi_disp_mode = tv_mode;
            find->support_regular_sampling_mode |= flag;

            cap->ycbcr420_resolution[cap->total_ycbcr420_vic] = tv_mode;
            cap->total_ycbcr420_vic++;
        }
    }
}

int get_hdmi_capability_from_edid(char *edid, int length, sunxi_hdmi_capability_t *cap)
{
    int extension_blocks = 0;
    hdmi_forum_vsdb_t vsdb;

    memset(cap, 0, sizeof(sunxi_hdmi_capability_t));
    for (int i = 0; i < 8; i++)
        cap->ycbcr420_resolution[i] = -1;

    if(check_edid_block0(edid, length)) {
        ALOGE("check edid block0 failed");
        return -1;
    }

    extension_blocks = check_extension_block(edid, length);

    /*
     * Check device type: rgb/yuv
     */
    cap->rgb_only = is_rgb_only(edid, extension_blocks);

    get_hdr_capabilites(edid, extension_blocks, &cap->capability);
    get_supported_colorimetry(edid, extension_blocks, &cap->colorimetry);

    /*
     * HDMI 2.0 vendor specific data block
     */
    cap->dvihdmi = 1;  /* set as dvi default */
    if (get_hdmi_vendor_specific_data_block(edid, extension_blocks, &vsdb) == 0) {
        cap->pixel_encoding = vsdb.deep_color_420_encoding;
        cap->max_tmds_rate = vsdb.max_tmds_rate;

        cap->supported_format[FMT_YUV420].supported = 1;
        cap->supported_format[FMT_YUV420].bits = cap->pixel_encoding;
        cap->dvihdmi = 2;
    }

    /*
     * HDMI 1.4 vendor specific data block
     */
    get_hdmi1p4_vendor_specific_data_block(edid, extension_blocks, cap);

    int y420_vics[8] = {0};
    int cnt;
    /* vic only support ycbcr420 */
    cnt = get_supported_y420_encoding_vic(edid, extension_blocks, y420_vics);
    if (cnt > 0)
        update_yuv420_vic(y420_vics, cnt, cap, 0);

    /* vic support ycbcr420 and other sampling mode */
    cnt = get_supported_y420_encoding_vic_map(edid, extension_blocks, y420_vics);
    if (cnt == 0) {
        cap->all_svd_support_ycbcr420 = 1;
        y420_vics[0] = HDMI3840_2160P_60;
        y420_vics[1] = HDMI3840_2160P_50;
        update_yuv420_vic(y420_vics, 2, cap, 1);
    }
    else if (cnt > 0)
        update_yuv420_vic(y420_vics, cnt, cap, 1);

    if (cap->total_ycbcr420_vic > 0)
        cap->video_format |= VIDEO_FORMAT_YCBCR420;

    int idx = cap->total_ycbcr420_vic;
    while (idx < 8) {
        cap->ycbcr420_resolution[idx] = - 1;
        idx++;
    }

    get_supported_pixel_formats(edid, extension_blocks, &cap->video_format);

    /* correct the yuv420 pixel format */
    if (cap->pixel_encoding & PIXEL_ENCODING_YUV420_10BIT) {
        if (cap->max_tmds_rate < 372) {
            ALOGE("The max tmds rate is %d MHz, not support YCbCr 4:2:0 10bit/12bit", cap->max_tmds_rate);
            cap->pixel_encoding &= (~PIXEL_ENCODING_YUV420_10BIT);
            cap->pixel_encoding &= (~PIXEL_ENCODING_YUV420_12BIT);
            cap->pixel_encoding &= (~PIXEL_ENCODING_YUV420_16BIT);

            cap->supported_format[FMT_YUV420].bits &= (~COLOR_DEPTH_10BIT);
            cap->supported_format[FMT_YUV420].bits &= (~COLOR_DEPTH_12BIT);
            cap->supported_format[FMT_YUV420].bits &= (~COLOR_DEPTH_16BIT);
        }
    }

    return 0;
}


