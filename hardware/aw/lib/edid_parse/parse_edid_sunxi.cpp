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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/mount.h>
#include <sys/resource.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>

#include <pthread.h>
#include "edid_parse/parse_edid.h"
#include "edid_parse/parse_edid_sunxi.h"

#define MAX_BUF_LENGTH 0x100
#define EDID_FILE_PATH "/sys/class/hdmi/hdmi/attr/edid"

static int g_ext_block_num = 0;
static vendor_info_t g_vendor;
static hdmi_mode_info_t g_hdmi_mode ;
static cec_info_t g_cec;
pthread_mutex_t lock;

struct black_list_3D {
    char  manufacturer[4];
    unsigned int model;
};

static const struct black_list_3D bad3D[] = {
    {
        .manufacturer = "KOA",
        .model = 0x0030,
    },
};

static int checkBad3D(vendor_info_t vendor)
{
    unsigned int i;
    for(i = 0; i < sizeof(bad3D) / sizeof(struct black_list_3D); i++)
        if (!strncmp(vendor.manufacturer, bad3D[i].manufacturer, 4)
          && bad3D[i].model == vendor.model) {
            ALOGD("Manufacturer: %s Model: 0x%x is known to unsupport 3D\n",
              vendor.manufacturer, vendor.model);
            return 0;
        }
    return 1;
}

static int read_edid_data(char *buf, int buf_length)
{
    FILE *file = NULL;
    int ret = 0;
    if(NULL == (file = fopen(EDID_FILE_PATH, "r"))) {
        ALOGD("cannot open this file edid.bin\n");
        return -1;
    }
    if(fread(buf, sizeof(char), buf_length, file) != buf_length) {
        ALOGD("fread error\n");
        ret = -1;
    }
    fclose(file);
    return ret;
}

static int isSameVendor(vendor_info_t *vd0, vendor_info_t *vd1)
{
    unsigned int i = 0;
    for (; i < sizeof(vd0->datas) / sizeof(vd0->datas[0]); ++i) {
        if (vd0->datas[i] != vd1->datas[i])
            return 0;
    }
    return 1;
}

int resetParseEdidForDisp(int hwDisp)
{
    int ret = 0;
    char edid_buf[MAX_BUF_LENGTH] = {0};
    edid_info_t edid_info[EDID_GET_CMD_NUM];
    vendor_info_t vendor;

    if(0 > read_edid_data(edid_buf, MAX_BUF_LENGTH)) {
        ALOGD("read_edid_data failed!\n");
        return -2;
    }

    memset((void *)edid_info, 0, sizeof(edid_info));
    edid_info[0].cmd = EDID_GET_VERDOR_INFO;
    edid_info[0].length = sizeof(vendor_info_t);
    edid_info[0].data = (void *)&vendor;
    edid_info[EDID_GET_CMD_NUM - 1].cmd = EDID_GET_FINISH;
    ret = get_edid_info(edid_buf, MAX_BUF_LENGTH, edid_info);

    pthread_mutex_lock(&lock);
    if(0 <= ret) {
        if(isSameVendor(&vendor, &g_vendor)) {
            ALOGD("the same vendor[%s,%d,%d,%d,%d]", vendor.manufacturer,
                vendor.model, vendor.SN, vendor.week, vendor.year);
        } else {
            ALOGD("Different vendor:");
            memcpy((void *)&g_vendor, (void *)&vendor, sizeof(vendor_info_t));
            g_ext_block_num = ret;
            get_hdmi_mode(edid_buf, g_ext_block_num, &g_hdmi_mode);
            get_cec_info(edid_buf, g_ext_block_num, &g_cec);
            ALOGD("manufacturer:%s, model:%d, SN:0x%x, week:%d, year: %d\n",
                vendor.manufacturer, vendor.model, vendor.SN,
                vendor.week, vendor.year);
            ALOGD("g_hdmi_mode: common_mode = 0x%llx, 0x%llx\n",
                g_hdmi_mode.common_mode, g_hdmi_mode.common_mode >> 32);
            ALOGD("      standard_mode = 0x%llx, 0x%llx\n",
                g_hdmi_mode.standard_mode, g_hdmi_mode.standard_mode >> 32);
            ALOGD("      4k = 0x%x, 3D = 0x%x\n",
                g_hdmi_mode.hd_2160P_mode, g_hdmi_mode.stereo_present);
            ALOGD("      native_mode = 0x%x\n", g_hdmi_mode.native_mode);
            ALOGD("g_cec: physical address: 0x%x\n", g_cec.phy_addr);
        }
        ret = 0;
    }
    pthread_mutex_unlock(&lock);
    ALOGD("resetParseEdid4Disp finish: %d!\n", ret);
    return ret;
}

int initParseEdid(int hwDisp)
{
    ALOGD("initParseEdid\n");
    pthread_mutex_init(&lock, NULL);
    memset((void *)&g_vendor, 0, sizeof(vendor_info_t));
    memset((void *)&g_hdmi_mode, 0, sizeof(hdmi_mode_info_t));
    memset((void *)&g_cec, 0, sizeof(cec_info_t));
    return resetParseEdidForDisp(hwDisp);
}

static int trans2StdMode(int mode)
{
    int ret = -1;
    switch((sunxi_tv_mode)mode) {
    case SUNXI_TV_MOD_720P_50HZ:
        ret = HDMI720P_50;
        break;
    case SUNXI_TV_MOD_720P_60HZ:
        ret = HDMI720P_60;
        break;
    case SUNXI_TV_MOD_1080P_50HZ:
        ret = HDMI1080P_50;
        break;
    case SUNXI_TV_MOD_1080P_60HZ:
        ret = HDMI1080P_60;
        break;
    case SUNXI_TV_MOD_1080P_24HZ:
        ret = HDMI1080P_24;
        break;
    case SUNXI_TV_MOD_1080I_50HZ:
        ret = HDMI1080I_50;
        break;
    case SUNXI_TV_MOD_1080I_60HZ:
        ret = HDMI1080I_60;
        break;
    case SUNXI_TV_MOD_480P:
        ret = HDMI480P;
        break;
    case SUNXI_TV_MOD_576P:
        ret = HDMI576P;
        break;
    case SUNXI_TV_MOD_480I:
        ret = HDMI1440_480I;
        break;
    case SUNXI_TV_MOD_576I:
        ret = HDMI1440_576I;
        break;
    case SUNXI_TV_MOD_1080P_25HZ:
        ret = HDMI1080P_25;
        break;
    case SUNXI_TV_MOD_1080P_30HZ:
        ret = HDMI1080P_30;
        break;
    default:
        ret = -1;
    }
    return ret;
}

static int trans2HdmiMode(int std_mode)
{
    int ret = -1;
    switch(std_mode) {
    case HDMI720P_50:
        ret = SUNXI_TV_MOD_720P_50HZ;
        break;
    case HDMI720P_60 :
        ret = SUNXI_TV_MOD_720P_60HZ;
        break;
    case HDMI1080P_50:
        ret = SUNXI_TV_MOD_1080P_50HZ;
        break;
    case HDMI1080P_60:
        ret = SUNXI_TV_MOD_1080P_60HZ;
        break;
    case HDMI1080P_24:
        ret = SUNXI_TV_MOD_1080P_24HZ;
        break;
    case HDMI1080I_50:
        ret = SUNXI_TV_MOD_1080I_50HZ;
        break;
    case HDMI1080I_60:
        ret = SUNXI_TV_MOD_1080I_60HZ;
        break;
    case HDMI480P:
        ret = SUNXI_TV_MOD_480P;
        break;
    case HDMI576P:
        ret = SUNXI_TV_MOD_576P;
        break;
    case HDMI1440_480I:
        ret = SUNXI_TV_MOD_480I;
        break;
    case HDMI1440_576I:
        ret = SUNXI_TV_MOD_576I;
        break;
    case HDMI1080P_25:
        ret = SUNXI_TV_MOD_1080P_25HZ;
        break;
    case HDMI1080P_30:
        ret = SUNXI_TV_MOD_1080P_30HZ;
        break;
    default:
        ret = -1;
    }
    return ret;
}

static int trans2HdMode(int mode)
{
    int ret = -1;
    switch((sunxi_tv_mode)mode) {
    case SUNXI_TV_MOD_3840_2160P_30HZ:
        ret = HDMI3840_2160P_30_BIT_MASK;
        break;
    case SUNXI_TV_MOD_3840_2160P_25HZ:
        ret = HDMI3840_2160P_30_BIT_MASK;
        break;
    case SUNXI_TV_MOD_3840_2160P_24HZ:
        ret = HDMI3840_2160P_30_BIT_MASK;
        break;
    default:
        ret = -1;
    }
    return ret;
}

int isHdmiModeSupport(int hwDisp, int mode)
{
    int ret = 0;
    int stdMode = 0;

    pthread_mutex_lock(&lock);
    if(SUNXI_TV_MOD_3840_2160P_30HZ > mode) {
        mode = trans2StdMode(mode);
        ret = (g_hdmi_mode.common_mode >> mode) & 0x1;
    } else {
        //4k
        mode = trans2HdMode(mode);
        ret = (g_hdmi_mode.hd_2160P_mode >> mode) & 0x1;
    }
    pthread_mutex_unlock(&lock);
    return ret;
}

//get the best hdmi mode, as flowing:
// 1.the native mode in vddb
// 2.the std mode in standard timing
// 3.todo: the commonly used the mode that is supported ?
// 4.return -1 if failed.
int getBestHdmiMode(int hwDisp)
{
    int hdmi_mode = -1;
    int native_mode = 0;
    unsigned long long std_mode = 0;
    unsigned int i = 0;

    pthread_mutex_lock(&lock);
    native_mode = g_hdmi_mode.native_mode;
    std_mode = g_hdmi_mode.standard_mode;
    pthread_mutex_unlock(&lock);
    if(native_mode) {
        hdmi_mode = trans2HdmiMode(native_mode);
    } else if (std_mode) {
        for(i = 0; i < sizeof(unsigned long long); i++) {
            if(!(std_mode & 0x1)) {
                std_mode >>= 1;
            } else {
                native_mode = i;
            }
        }
        hdmi_mode = trans2HdmiMode(native_mode);
    }
    return hdmi_mode;
}

int isHdmi3DSupport(int hwDisp)
{
    int ret = 0;
    pthread_mutex_lock(&lock);
    ret = g_hdmi_mode.stereo_present;
    if(ret)
        ret = checkBad3D(g_vendor);
    pthread_mutex_unlock(&lock);
    return ret;
}

int getVendorInfo(int hwDisp, vendor_info_t * vendor)
{
    pthread_mutex_lock(&lock);
    memcpy((void *)vendor, (void *)&g_vendor, sizeof(vendor_info_t));
    pthread_mutex_unlock(&lock);
    return 0;
}

/*
static char g_edid_buf[MAX_BUF_LENGTH] = {0};
// main: call get_edid_info for debug.
int main(int argc, char *argv[])
{
    int ret;
    edid_info_t edid_info[EDID_GET_CMD_NUM] = {0};
    hdmi_mode_info_t hdmi_mode = {0};
    cec_info_t cec_info = {0};
    vendor_info_t vendor = {0};

    // 1. get the edid raw data.
    if(0 > read_edid_data(g_edid_buf, MAX_BUF_LENGTH)) {
        ALOGD("read_edid_data failed!\n");
        return -1;
    }

    // 2.fill the edid_info[i], i = 0,1,...,EDID_GET_CMD_NUM - 2.
    memset((void *)edid_info, 0, sizeof(edid_info));
    edid_info[0].cmd = EDID_GET_VERDOR_INFO;
    edid_info[0].length = sizeof(vendor_info_t);
    edid_info[0].data = (void *)&vendor;
    edid_info[1].cmd = EDID_GET_HDMI_MODE;
    edid_info[1].length = sizeof(hdmi_mode_info_t);
    edid_info[1].data = (void *)&hdmi_mode;
    edid_info[2].cmd = EDID_GET_CEC_INFO;
    edid_info[2].length = sizeof(cec_info_t);
    edid_info[2].data = (void *)&cec_info;

    // 3. get edid info by parsing.
    edid_info[EDID_GET_CMD_NUM - 1].cmd = EDID_GET_FINISH;
    ret = get_edid_info(g_edid_buf, MAX_BUF_LENGTH, edid_info);

    // 4. ALOGD the results...
    ALOGD("the result: %d\n", ret);
    if(ret != -1) {
        ALOGD("manufacturer:%s, model:%d, SN:0x%x, week:%d, year: %d\n",
            vendor.manufacturer, vendor.model, vendor.SN,
            vendor.week, vendor.year);
        ALOGD("hdmi_mode: common_mode = 0x%lx, 0x%lx\n",
            hdmi_mode.common_mode, hdmi_mode.common_mode >> 32);
        ALOGD("      standard_mode = 0x%lx, 0x%lx\n",
            hdmi_mode.standard_mode, hdmi_mode.standard_mode >> 32);
        ALOGD("      4k = 0x%x, 3D = 0x%x\n",
            hdmi_mode.hd_2160P_mode, hdmi_mode.stereo_present);
        ALOGD("      native_mode = 0x%x\n", hdmi_mode.native_mode);
        ALOGD("cec physical address: 0x%x\n", cec_info.phy_addr);
        //memcpy((void *)&g_vendor, (void *)&vendor, sizeof(vendor_info_t));

        if(!strcmp(vendor.manufacturer, g_vendor.manufacturer) &&
            vendor.model == g_vendor.model) {
            ALOGD("the same vendor\n");
            return 0;
        } else {
            ALOGD("different vendor\n");
            memcpy((void *)&g_vendor, (void *)&vendor, sizeof(vendor_info_t));
            g_ext_block_num = ret;
            get_hdmi_mode(g_edid_buf, g_ext_block_num, &g_hdmi_mode);
            get_cec_info(g_edid_buf, g_ext_block_num, &g_cec);
        }

    }
    ALOGD("edid parse debug finish!\n");

    return 0;
}
*/

const char *eo_name[] = {
    "SDR_LUMINANCE_RANGE",
    "HDR_LUMINANCE_RANGE",
    "SMPTE_ST_2048",
    "FUTURE_EOTF"
};

const char *ck_name[] = {
    "XVYCC601",
    "XVYCC709",
    "SYCC601",
    "ADOBE_YCC601",
    "ADOBE_RGB",
    "BT2020_CYCC",
    "BT2020_YCC",
    "BT2020_RGB"
};

const char *vf_name[] = {
    "RGB",
    "YCBCR444",
    "YCBCR422",
    "YCBCR420"
};

const char *en_name[] = {
    "YUV420_8BIT",
    "YUV420_10BIT",
    "YUV420_12BIT",
    "YUV420_16BIT",
};

static int
dump_bitfields(char *p, const char *prefix, const char** name, int size, int bitfields) {
    char *start = p;
    p += sprintf(p, "%s", prefix);
    for (int i = 0; i < size; i++)
        if (bitfields & (1 << i))
            p += sprintf(p, " %s", name[i]);
    p += sprintf(p, "\n");
    return (p - start);
}


static void dump_ycbcr420_vic(sunxi_hdmi_capability_t *cap)
{
    char buf[512] = {0};
    char *p = buf;

	p += sprintf(p, "\n");
	p += sprintf(p, "Supported YCbCr420 VICs (%d):\n", cap->total_ycbcr420_vic);
	for (int i = 0; i < cap->total_ycbcr420_vic; i++)
		p += sprintf(p, "    VIC %3d %3d %s\n",
			cap->ycbcr420_vic[i].vic,
			cap->ycbcr420_vic[i].sunxi_disp_mode,
			cap->ycbcr420_vic[i].support_regular_sampling_mode ? "(support Y444/Y422/RGB sampling)" : "");
    ALOGE("%s", buf);
}

static void dump_video_format(sunxi_hdmi_capability_t *cap)
{
    char buf[512] = {0};
    char *p = buf;
	const char *video_format_name[] = {
		"RGB   8:8:8",
		"YCbCr 4:4:4",
		"YCbCr 4:2:2",
		"YCbCr 4:2:0",
	};
	p += sprintf(p, "Supported video format:\n");
	for (size_t i = 0;
			i < sizeof(video_format_name) / sizeof(video_format_name[0]);
			i++) {
		if (!cap->supported_format[i].supported)
			continue;
		p += sprintf(p, "    %s: %s %s %s %s\n", video_format_name[i],
			cap->supported_format[i].bits & COLOR_DEPTH_16BIT ? "48bit" : "     ",
			cap->supported_format[i].bits & COLOR_DEPTH_12BIT ? "36bit" : "     ",
			cap->supported_format[i].bits & COLOR_DEPTH_10BIT ? "30bit" : "     ",
			cap->supported_format[i].bits & COLOR_DEPTH_8BIT  ? "24bit" : "     ");
	}
    ALOGE("%s", buf);
}


void dump_hdmi_capability(sunxi_hdmi_capability_t *cap)
{
    char buffer[2048] = {0};
    char *p = buffer;

    p += sprintf(p, "RGB only: %s\n", cap->rgb_only ? "true" : "false");
    p += sprintf(p, "DVI(1) OR HDMI(2): %d\n", cap->dvihdmi);

    p += sprintf(p, "metadata_descriptor        : %04x\n", cap->capability.metadata_descriptor);
    p += sprintf(p, "max_luminance              : %04x\n", cap->capability.max_luminance);
    p += sprintf(p, "max_frame_average_luminance: %04x\n", cap->capability.max_frame_average_luminance);
    p += sprintf(p, "min_luminance              : %04x\n", cap->capability.max_luminance);

    p += sprintf(p, "max tmds rate: %d MHz\n", cap->max_tmds_rate);
    p += dump_bitfields(p, "eotf               :", eo_name, sizeof(eo_name) / sizeof(eo_name[0]), cap->capability.eotf);
    p += dump_bitfields(p, "colorimetry        :", ck_name, sizeof(ck_name) / sizeof(ck_name[0]), cap->colorimetry);
    p += dump_bitfields(p, "video_format       :", vf_name, sizeof(vf_name) / sizeof(vf_name[0]), cap->video_format);
    p += dump_bitfields(p, "pixel_encoding     :", en_name, sizeof(en_name) / sizeof(en_name[0]), cap->pixel_encoding);
    p += sprintf(p, "ycbcr420_resolution:");

    for (int i = 0; i < 8; i++) {
        p += sprintf(p, " %d", cap->ycbcr420_resolution[i]);
    }
    ALOGE("HDMI CAPABILITY:\n%s\n", buffer);
    dump_ycbcr420_vic(cap);
    dump_video_format(cap);
}


int getHdmiCapability(sunxi_hdmi_capability_t *cap)
{
    int ret = 0;
    char edid[MAX_BUF_LENGTH] = {0};

    if (!cap) return -1;
    memset(cap, 0, sizeof(*cap));

    if(0 > read_edid_data(edid, MAX_BUF_LENGTH)) {
        ALOGD("read_edid_data failed!\n");
        return -2;
    }
    ret = get_hdmi_capability_from_edid(edid, MAX_BUF_LENGTH, cap);
    if (cap->capability.eotf & SMPTE_ST_2048) {
        if (!(cap->colorimetry & STANDARD_BT2020_YCC)) {
            ALOGW("colorimetry is reset forcely to STANDARD_BT2020_YCC");
            cap->colorimetry |= STANDARD_BT2020_YCC;
        }
    }
    dump_hdmi_capability(cap);
    return ret;
}

