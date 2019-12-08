/*************************************************************************/ /*!
@Copyright      Copyright (c) Imagination Technologies Ltd. All Rights Reserved
@License        Strictly Confidential.
*/ /**************************************************************************/

#include "hwc.h"

#include <sys/resource.h>
#include <sys/time.h>

#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#define DISP_P2P_PROPERTY  "persist.sys.hwc_p2p"
#define SYSTEM_RSL_PROPERTY "ro.hwc.sysrsl"
#define DISPLAY_RSL_FILENAME "/mnt/Reserve0/disp_rsl.fex"
#define DISPLAY_MARGIN_FILENAME "/mnt/Reserve0/disp_margin.fex"
#define HDMI_HPD_STATE_FILENAME "/sys/class/switch/hdmi/state"
#define RT_DISP_POLICY_PROP_NAME "persist.sys.disp_policy"
#define VENDOR_ID_FILENAME "/mnt/Reserve0/tv_vdid.fex"
#define DRAM_CUR_FREQ_FILENAME "/sys/class/devfreq/dramfreq/cur_freq"

#define DISPLAY_OUTPUT_DATASPACE_MODE "persist.sys.disp_dataspace"
#define DISPLAY_OUTPUT_PIXEL_FORMAT "persist.sys.disp_pxlformat"

#ifdef SAVE_DISP_CONFIGS_TO_PROPERTY
#define DISPLAY_RSL_PROPERTY "persist.sys.disp_rsl_fex"
#define DISPLAY_MARGIN_PROPERTY "persist.sys.disp_margin_fex"
#define VENDOR_ID_PROPERTY "persist.sys.tv_vdid_fex"
#define DISPLAY_DEVICE_CONFIG0_PROPERTY "persist.sys.disp_config"
#endif

static tv_para_t g_tv_para[]=
{
    {8, DISP_TV_MOD_NTSC,             720,    480, 60,0},
    {8, DISP_TV_MOD_PAL,              720,    576, 50,0},

    {5, DISP_TV_MOD_480I,             720,    480, 60,0},
    {5, DISP_TV_MOD_576I,             720,    576, 60,0},
    {5, DISP_TV_MOD_480P,             720,    480, 60,0},
    {5, DISP_TV_MOD_576P,             720,    576, 50,0},
    {5, DISP_TV_MOD_720P_50HZ,        1280,   720, 50,0},
    {5, DISP_TV_MOD_720P_60HZ,        1280,   720, 60,0},

    {1, DISP_TV_MOD_1080P_24HZ,       1920,   1080, 24,0},
    {5, DISP_TV_MOD_1080P_50HZ,       1920,   1080, 50,0},
    {5, DISP_TV_MOD_1080P_60HZ,       1920,   1080, 60,0},
    {5, DISP_TV_MOD_1080I_50HZ,       1920,   1080, 50,0},
    {5, DISP_TV_MOD_1080I_60HZ,       1920,   1080, 60,0},


    {5, DISP_TV_MOD_3840_2160P_25HZ,  3840,   2160, 25,0xff},
    {5, DISP_TV_MOD_3840_2160P_24HZ,  3840,   2160, 24,0xff},
    {5, DISP_TV_MOD_3840_2160P_30HZ,  3840,   2160, 30,0xff},
    {5, DISP_TV_MOD_3840_2160P_60HZ,  3840,   2160, 60,0xff},

    {1, DISP_TV_MOD_1080P_24HZ_3D_FP, 1920,   1080, 24,0},
    {1, DISP_TV_MOD_720P_50HZ_3D_FP,  1280,   720, 50,0},
    {1, DISP_TV_MOD_720P_60HZ_3D_FP,  1280,   720, 60,0},
    {1, DISP_TV_MODE_NUM,  0,   0, 0,0},
};

static tv_para_t g_vga_para[]=
{
#if 0
    {8, DISP_VGA_MOD_640x480P_60HZ,  640,  480,  60,  0},
    {8, DISP_VGA_MOD_800x600P_60HZ,  800,  600,  60,  0},
    {8, DISP_VGA_MOD_1024x768P_60HZ, 1024,  768,  60,  0},
    {8, DISP_VGA_MOD_1280x768P_60HZ, 1280,  768,  60,  0},
    {8, DISP_VGA_MOD_1280x800P_60HZ, 1280,  800,  60,  0},
    {8, DISP_VGA_MOD_1366x768P_60HZ, 1366,  768,  60,  0},
    {8, DISP_VGA_MOD_1440x900P_60HZ, 1440,  900,  60,  0},
    {8, DISP_VGA_MOD_1920x1080P_60HZ, 1920, 1080, 60,  0},
    {8, DISP_VGA_MOD_1920x1200P_60HZ, 1920, 1200, 60,  0},
#endif
};

static mem_speed_limit_t mem_speed_limit[] =
{
    {672000, 20736000}, /* 20736000 = 20736000 + 16588800 */
    {552000, 12441600}, /* 12441600 = 1920*1080*4*1.5 */
    {432000,  8294400}, /* 16588800 = 1920*1080*4*1 */
};

int get_info_mode(int mode,MODEINFO info)
{
    unsigned int i = 0;

    for(i=0; i<sizeof(g_tv_para)/sizeof(tv_para_t); i++)
    {
        if(g_tv_para[i].mode == mode)
        {
            return *(((int *)(g_tv_para+i))+info);
        }
    }
    return -1;
}

int getVgaInfo(int mode, MODEINFO info)
{
    unsigned int i = 0;
	unsigned int num = sizeof(g_vga_para) / sizeof(g_vga_para[0]);

    for(i = 0; i < num; i++)
        if(g_vga_para[i].mode == mode)
            return *(((int *)(g_vga_para + i)) + info);
    ALOGE("no find the vga mode[%d].", mode);
    return -1;
}

int getValueFromProperty(char const* propName)
{
    char property[PROPERTY_VALUE_MAX] = {0};
    int value = -1;
    if (property_get(propName, property, NULL) > 0)
    {
        value = atoi(property);
    }
    ALOGD("###%s: propName:%s,value=%d", __func__, propName, value);
    return value;
}

//check whether we config that display point to point
//-->Size of DisplayMode == Size of System Resolution
int isDisplayP2P(void)
{
    if(1 != getValueFromProperty(DISP_P2P_PROPERTY))
    {
        return 0; // not support display point2p
    }
    else
    {
        return 1;
    }
}

//get the tv mode for system resolution
//the system resolution desides the Buffer Size of the App.
int getTvMode4SysResolution(void)
{
    int tvmode = getValueFromProperty(SYSTEM_RSL_PROPERTY);
    switch(tvmode)
    {
        case DISP_TV_MOD_PAL:
            break;
        case DISP_TV_MOD_NTSC:
            break;
        case DISP_TV_MOD_720P_50HZ:
        case DISP_TV_MOD_720P_60HZ:
            break;
        case DISP_TV_MOD_1080I_50HZ:
        case DISP_TV_MOD_1080I_60HZ:
        case DISP_TV_MOD_1080P_24HZ:
        case DISP_TV_MOD_1080P_50HZ:
        case DISP_TV_MOD_1080P_60HZ:
            break;
        case DISP_TV_MOD_3840_2160P_24HZ:
        case DISP_TV_MOD_3840_2160P_25HZ:
        case DISP_TV_MOD_3840_2160P_30HZ:
        case DISP_TV_MOD_3840_2160P_60HZ:
            break;
        default:
            tvmode = DISP_TV_MOD_1080P_60HZ;
            break;
    }
    return tvmode;
}

int getDispPolicy(void)
{
    int policy = getValueFromProperty(RT_DISP_POLICY_PROP_NAME);
    if(-1 == policy)
        return 0;
    return policy;
}

int readStringFromAttrFile(char const *fileName,
	char *values, const int len)
{
    int ret = 0;
    int fd = open(fileName, O_RDONLY);
    if(0 > fd) {
        ALOGW("open file:%s  for reading failed, errno=%d\n", fileName, errno);
        return -1;
    }
    ret = read(fd, values, len);
    close(fd);
    return ret;
}

int writeStringToAttrFile(char const *fileName,
	char const *values, const int len)
{
	int ret = 0;
	int fd = open(fileName, O_WRONLY);
    if(0 > fd) {
        ALOGW("open file:%s  for writing failed, errno=%d\n", fileName, errno);
        return -1;
    }
    if (0 <= write(fd, values, len))
		ret = 0;
	else
		ret = -1;
    close(fd);
	return ret;
}

int getStringsFromFile(char const * fileName, char *values, unsigned int num)
{
    FILE *fp;
    unsigned int i = 0;

    if(NULL ==(fp = fopen(fileName, "r")))
    {
        ALOGW("cannot open this file:%s\n", fileName);
        return -1;
    }
    //ALOGD("open file %s success", fileName);
    while(!feof(fp) && (i < num - 1))
    {
        values[i] = fgetc(fp);
        i++;
    }
    values[i] = '\0';
    //ALOGD("###get value:\n%s", values);
    fclose(fp);

    return i;
}

int getStringsFromProperty(char const* propName, char *propStrings)
{
    int ret;

    ret = property_get(propName, propStrings, NULL);
    if (ret <= 0) {
        ALOGE("get property %s failed ", propName);
        return -1;
    }

    return ret;
}

#define ARRAYLENGTH 32
int getDispModeFromFile(int type)
{
    char valueString[ARRAYLENGTH] = {0};
    char datas[ARRAYLENGTH] = {0};
    int i = 0;
    int j = 0;
    int data = 0;

    memset(valueString, 0, ARRAYLENGTH);
#ifndef SAVE_DISP_CONFIGS_TO_PROPERTY
    if(getStringsFromFile(DISPLAY_RSL_FILENAME, valueString, ARRAYLENGTH) == -1)
    {
        return -1;
    }
#else
    if (getStringsFromProperty(DISPLAY_RSL_PROPERTY, valueString) == -1) {
        return -1;
    }
#endif
    for(i = 0; valueString[i] != '\0'; i++)
    {
        if('\n' == valueString[i])
        {
            datas[j] = '\0';
            //ALOGD("datas = %s\n", datas);
            data = (int)strtoul(datas, NULL, 16);
            if(type == ((data >> 8) & 0xFF))
            {
                return (data & 0xFF);
            }
            j = 0;
        }
        else
        {
            datas[j++] = valueString[i];
        }
    }
    return -1;
}

int saveDispModeToFile(int type,int mode)
{
    char valueString[ARRAYLENGTH] = {0};
    char *pValue, *pt, *ptEnd;
    int index = 0;
    int i = 0;
    int len = 0;
    int format = ((type & 0xFF) << 8) | (mode & 0xFF);
    int values[3] = {0, 0, 0};

    switch(type) {
    case DISP_OUTPUT_TYPE_HDMI:
        index = 1;
        break;
    case DISP_OUTPUT_TYPE_TV:
        index = 0;
        break;
    case DISP_OUTPUT_TYPE_VGA:
        index = 2;
        break;
    default:
        return 0;
    }
#ifndef SAVE_DISP_CONFIGS_TO_PROPERTY
    len = getStringsFromFile(DISPLAY_RSL_FILENAME, valueString, ARRAYLENGTH);
#else
    len = getStringsFromProperty(DISPLAY_RSL_PROPERTY, valueString);
#endif
    if(0 < len) {
        pValue = valueString;
        pt = valueString;
        ptEnd = valueString + len;
        for(;(i < 3) && (pt != ptEnd); pt++) {
            if('\n' == *pt) {
                *pt = '\0';
                values[i] = (int)strtoul(pValue, NULL, 16);
                ALOGD("pValue=%s, values[%d]=0x%x",
                    pValue, i, values[i]);
                pValue = pt + 1;
                i++;
            }
        }
    }

    values[index] = format;
    sprintf(valueString, "%x\n%x\n%x\n", values[0], values[1], values[2]);
#ifndef SAVE_DISP_CONFIGS_TO_PROPERTY
    FILE *fp = NULL;
    int ret = 0;
    if(NULL == (fp = fopen(DISPLAY_RSL_FILENAME, "w"))) {
        ALOGW("open this file:%s  for writing failed\n", DISPLAY_RSL_FILENAME);
        return -1;
    }
    len = strlen(valueString);
    ret = fwrite(valueString, len, 1, fp);
    //ALOGD("saveDispModeToFile:valueString=%s,len=%d,ret=%d",
    //    valueString, len, ret);
    fflush(fp);
    fsync(fileno(fp));
    fclose(fp);
#else
    if (property_set(DISPLAY_RSL_PROPERTY, valueString) != 0) {
        ALOGE("DISPLAY_RSL_PROPERTY set failed");
        return -1;
    }
    //ALOGD("saveDispModeToFile:valueString=%s,", valueString);
#endif
    return 0;
}

int getDispMarginFromFile(int type, unsigned char *percentWidth, unsigned char *percentHeight)
{
    char valueString[ARRAYLENGTH] = {0};
    int num = 0;
    int cvbs_percent[2];
    int hdmi_percent[2];

    memset(valueString, 0, ARRAYLENGTH);
#ifndef SAVE_DISP_CONFIGS_TO_PROPERTY
    if(getStringsFromFile(DISPLAY_MARGIN_FILENAME, valueString, ARRAYLENGTH) == -1)
    {
        return -1;
    }
#else
    if(getStringsFromProperty(DISPLAY_MARGIN_PROPERTY, valueString) == -1) {
        return -1;
    }
#endif
    // string format: "cvbs_percent_w cvbs_percent_h; hdmi_percent_w, hdmi_percent_h"
    valueString[ARRAYLENGTH - 1] = 0;
    num = sscanf(valueString, "%d %d; %d %d",
        &cvbs_percent[0], &cvbs_percent[1], &hdmi_percent[0], &hdmi_percent[1]);
    if (num != 4) {
        *percentWidth  = MARGIN_DEFAULT_PERCENT_WIDTH;
        *percentHeight = MARGIN_DEFAULT_PERCENT_HEIGHT;
        ALOGD("parse margin string error, use default value");
        return 0;
    }

    switch (type) {
    case DISP_OUTPUT_TYPE_HDMI:
        *percentWidth  = (unsigned char)hdmi_percent[0];
        *percentHeight = (unsigned char)hdmi_percent[1];
        break;
    case DISP_OUTPUT_TYPE_TV:
        *percentWidth  = (unsigned char)cvbs_percent[0];
        *percentHeight = (unsigned char)cvbs_percent[1];
        break;
    default:
        *percentWidth  = MARGIN_DEFAULT_PERCENT_WIDTH;
        *percentHeight = MARGIN_DEFAULT_PERCENT_HEIGHT;
        break;
    }
    return 0;
}

int saveDispMarginToFile(int type, unsigned char percentWidth, unsigned char percentHeight)
{
    char valueString[ARRAYLENGTH] = {0};
    int num = 0;
    unsigned char saved[4] = {0};


    if (getDispMarginFromFile(type, &saved[0], &saved[1])) {
        ALOGE("get display margin error!");
        return -1;
    }
    if ((saved[0] == percentWidth) && (saved[1] == percentHeight))
        return 0;

    if (getDispMarginFromFile(DISP_OUTPUT_TYPE_TV, &saved[0], &saved[1])
            || getDispMarginFromFile(DISP_OUTPUT_TYPE_HDMI, &saved[2], &saved[3])) {
        ALOGE("get display margin error!");
        return -1;
    }
    switch (type) {
    case DISP_OUTPUT_TYPE_HDMI:
        saved[2] = percentWidth;
        saved[3] = percentHeight;
        break;
    case DISP_OUTPUT_TYPE_TV:
        saved[0] = percentWidth;
        saved[1] = percentHeight;
        break;
    default:
        break;
    }
    sprintf(valueString, "%d %d; %d %d",
        saved[0], saved[1], saved[2], saved[3]);

#ifndef SAVE_DISP_CONFIGS_TO_PROPERTY
    int len = 0;
    int ret = 0;
    FILE *fp = NULL;

    if(NULL == (fp = fopen(DISPLAY_MARGIN_FILENAME, "w"))) {
        ALOGW("open this file:%s  for writing failed\n", DISPLAY_MARGIN_FILENAME);
        return -1;
    }

    len = strlen(valueString);
    ret = fwrite(valueString, len, 1, fp);
    //ALOGD("saveDispModeToFile:valueString=%s,len=%d,ret=%d",
    //        valueString, len, ret);
    fflush(fp);
    fsync(fileno(fp));
    fclose(fp);
#else
    if (property_set(DISPLAY_MARGIN_PROPERTY, valueString) != 0) {
        ALOGE("set DISPLAY_MARGIN_PROPERTY failed");
        return -1;
    }
    //ALOGD("saveDispMarginToFile:DISPLAY_MARGIN_PROPERTY = %s", valueString);
#endif

    return 0;
}

unsigned int isCvbsHpd(int disp)
{
    disp;
    char valueString[ARRAYLENGTH] = {0};
    int state = 0;
	const char *CVBS_HPD_STATE_FILENAME = "/sys/class/switch/cvbs/state";
    memset(valueString, 0, ARRAYLENGTH);

    if(getStringsFromFile(CVBS_HPD_STATE_FILENAME,
		valueString, ARRAYLENGTH) == -1) {
        return 0;
    }
    if(!strncmp(valueString, "0", 1)) {
        return 0;
    } else {
        return 1;
    }
}

unsigned int isHdmiHpd(int disp)
{
    disp;
    char valueString[ARRAYLENGTH] = {0};
    int state = 0;

    memset(valueString, 0, ARRAYLENGTH);
    if(getStringsFromFile(HDMI_HPD_STATE_FILENAME, valueString, ARRAYLENGTH) == -1)
    {
        return 0;
    }
    if(!strncmp(valueString, "0", 1))
    {
        return 0;
    }
    else
    {
        return 1;
    }
}

int getSavedHdmiVendorID(int disp, unsigned char *vendorID, int num)
{
    disp;
    char valueString[ARRAYLENGTH + ARRAYLENGTH] = {0};
    char *pVendorId, *pValueString, *pValueStringEnd;
    int i = 0;
    int ret = 0;

#ifndef SAVE_DISP_CONFIGS_TO_PROPERTY
    ret = getStringsFromFile(VENDOR_ID_FILENAME,
        valueString, sizeof(valueString));
    if(0 >= ret)
        return -1;
#else
    ret = getStringsFromProperty(VENDOR_ID_PROPERTY, valueString);
    if (ret == -1)
        return -1;
#endif

    pVendorId = valueString;
    pValueString = valueString;
    pValueStringEnd = pValueString + ret;
    i = 0;
    for(; pValueString != pValueStringEnd; pValueString++) {
        if(',' == *pValueString) {
            *pValueString = '\0';
            ret = (int)strtoul(pVendorId, NULL, 16);
            vendorID[i] = ret;
            //ALOGD("vendorID[%d]=%x", i, vendorID[i]);
            i++;
            if (i >= num)
                break;
            pVendorId = pValueString + 1;
        }
    }
    return i;
}

int savedHdmiVendorID(int disp, char *vendorId, int num)
{
    disp;
    char value[4] = {0};
    char valueString[ARRAYLENGTH + ARRAYLENGTH] = {0};
    FILE *fp = NULL;
    int len = 0;
    int ret = 0;

    for (ret = 0; ret < num; ++ret) {
        sprintf(value, "%x,", vendorId[ret]);
        strcat(valueString, value);
    }
    //ALOGD("valueString=%s", valueString);
#ifndef SAVE_DISP_CONFIGS_TO_PROPERTY
    if(NULL == (fp = fopen(VENDOR_ID_FILENAME, "w"))) {
        ALOGW("open this file:%s  for writing failed\n", VENDOR_ID_FILENAME);
        return -1;
    }
    len = strlen(valueString);
    ret = fwrite(valueString, len, 1, fp);
    //ALOGD("savedHdmiVendorID:valueString=%s,len=%d,ret=%d",
    //    valueString, len, ret);
    fflush(fp);
    fsync(fileno(fp));
    fclose(fp);
#else
    if (property_set(VENDOR_ID_PROPERTY, valueString) != 0) {
        ALOGE("VENDOR_ID_PROPERTY set failed");
        return -1;
    }
#endif
    return 0;
}

int getSavedDataSpaceMode(unsigned int hwDisp)
{
	hwDisp;
	char valueString[ARRAYLENGTH] = {0};

	memset(valueString, 0, ARRAYLENGTH);
	if(getStringsFromProperty(DISPLAY_OUTPUT_DATASPACE_MODE, valueString) == -1) {
		return -1;
	}
	return (int)strtoul(valueString, NULL, 10);
}

int saveDataSpaceMode(unsigned int hwDisp, int mode)
{
	hwDisp;
	char valueString[ARRAYLENGTH] = {0};
    sprintf(valueString, "%x", mode);
    if (property_set(DISPLAY_OUTPUT_DATASPACE_MODE, valueString) != 0) {
        ALOGE("DISPLAY_OUTPUT_DATASPACE_MODE set failed");
        return -1;
    }
	return 0;
}

int saveDispDeviceConfig(unsigned int hwDisp, struct disp_device_config *config)
{
    hwDisp;
    char buffer[256] = {0};
    sprintf(buffer, "%d,%d - %d,%d,%d,%d",
        config->type, config->mode, config->format,
        config->bits, config->cs, config->eotf);
    if (property_set(DISPLAY_DEVICE_CONFIG0_PROPERTY, buffer) != 0) {
        ALOGE("DISPLAY_DEVICE_CONFIG0_PROPERTY set failed");
        return -1;
    }
    return 0;
}

int getSavedOutputPixelFormat(unsigned int hwDisp)
{
	hwDisp;
	char valueString[ARRAYLENGTH] = {0};

	memset(valueString, 0, ARRAYLENGTH);
	if(getStringsFromProperty(DISPLAY_OUTPUT_PIXEL_FORMAT, valueString) == -1) {
		return -1;
	}
	return (int)strtoul(valueString, NULL, 10);
}

int saveOutputPixelFormat(unsigned int hwDisp, int mode)
{
    hwDisp;
    char valueString[ARRAYLENGTH] = {0};
    sprintf(valueString, "%x", mode);
    if (property_set(DISPLAY_OUTPUT_PIXEL_FORMAT, valueString) != 0) {
        ALOGE("DISPLAY_OUTPUT_PIXEL_FORMAT set failed");
        return -1;
    }
    return 0;
}

int getDramCurFreq()
{
    char valueString[ARRAYLENGTH] = {0};

    if(getStringsFromFile(DRAM_CUR_FREQ_FILENAME, valueString, ARRAYLENGTH) == -1)
    {
        //ALOGD("getDramCurFreq failed !");
        return 0;
    }
    return (int)strtoul(valueString, NULL, 10);
}
#undef ARRAYLENGTH

int getMemLimit()
{
#ifdef CHECK_DRAM_FREQ_EACH_FRAME
    unsigned int i = 0;
    int freq = getDramCurFreq();
    int x0,x1,y0,y1;

    for(i = 0; i < sizeof(mem_speed_limit) / sizeof(mem_speed_limit_t); i++)
        if(mem_speed_limit[i].speed == freq)
            return mem_speed_limit[i].limit;
    if (0 == freq)
        return mem_speed_limit[0].limit;
    //ALOGD("no find getMemLimit of speed[%d].", freq);
    x0 = mem_speed_limit[0].speed / 1000;
    y0 = mem_speed_limit[0].limit / 100;
    x1 = mem_speed_limit[1].speed / 1000;
    y1 = mem_speed_limit[1].limit / 100;
    return (freq / 1000 * (y0 - y1) + x0 * y1 - x1 * y0) / (x0 - x1) * 100;
#else

    static int slimit = -1;
    unsigned int i = 0;
    int freq = 0;
    int x0,x1,y0,y1;

    if (-1 != slimit)
        return slimit;

    freq = getDramCurFreq();
    for(i = 0; i < sizeof(mem_speed_limit) / sizeof(mem_speed_limit_t); i++)
        if(mem_speed_limit[i].speed == freq) {
            slimit = mem_speed_limit[i].limit;
            return slimit;
        }
    if (0 == freq) {
        slimit = mem_speed_limit[0].limit;
    } else {
        //ALOGD("no find getMemLimit of speed[%d].", freq);
        x0 = mem_speed_limit[0].speed / 1000;
        y0 = mem_speed_limit[0].limit / 100;
        x1 = mem_speed_limit[1].speed / 1000;
        y1 = mem_speed_limit[1].limit / 100;
        slimit = (freq / 1000 * (y0 - y1) + x0 * y1 - x1 * y0) / (x0 - x1) * 100;
    }
    return slimit;

#endif
}

int getMainDisplay()
{
    // ---------------------------------------------------------------------//
    // how to decide which display-device is the main disp:                 //
    //    a) let the disp-driver tells us.                                  //
    //    b) let the configs tell us.                                       //
    //    c) if a) and b) does not work, init in order by hwdisp = 0/1...   //
    //----------------------------------------------------------------------//

    return 0;
}

int getHwDispByType(int type)
{
    switch(type)
    {
    case DISP_OUTPUT_TYPE_HDMI:
        return HDMI_USED;
    case DISP_OUTPUT_TYPE_TV:
        return CVBS_USED;
    case DISP_OUTPUT_TYPE_VGA:
        return VGA_USED;
    case DISP_OUTPUT_TYPE_LCD:
        return LCD_USED;
    default:
        ALOGE("getHwDispByType ERR:type=%d", type);
        return EVALID_HWDISP_ID;
    }
}

int get_de_freq_and_fps(DisplayInfo *psDisplayInfo)
{
    #define _FPS_LIMIT_ (60)
    SUNXI_hwcdev_context_t *Globctx = &gSunxiHwcDevice;

    if (psDisplayInfo->VirtualToHWDisplay != EVALID_HWDISP_ID) {
        /* get de_clk and fps from driver */
        unsigned long args[4];
        args[0] = psDisplayInfo->VirtualToHWDisplay;
        args[1] = HWC_GET_CLK_RATE;
        psDisplayInfo->de_clk = ioctl(Globctx->DisplayFd, DISP_HWC_COMMIT, args);
        args[1] = HWC_GET_FPS;
        long long fps = ioctl(Globctx->DisplayFd, DISP_HWC_COMMIT, args);

        psDisplayInfo->fps = fps > _FPS_LIMIT_ ? _FPS_LIMIT_ : fps;
        ALOGD("hwDisp=%d, dispOutput[%d, %d], de_clk=%lld, fps=%lld",
                psDisplayInfo->VirtualToHWDisplay, psDisplayInfo->DisplayType,
                psDisplayInfo->DisplayMode, psDisplayInfo->de_clk, psDisplayInfo->fps);
    }
    return 0;
}

static int isZeroVendor(vendor_info_t *vendorInfo)
{
    char allzero = 0;
    const unsigned int VD_SIZE =
        sizeof(vendorInfo->datas) / sizeof(vendorInfo->datas[0]);
    unsigned int i = 0;
    for (; i < VD_SIZE; ++i) {
        allzero |= vendorInfo->datas[i];
    }
    return (0 == allzero);
}

static int isSameVendor(int disp, vendor_info_t *vendorInfo)
{
    const unsigned int VD_SIZE =
        sizeof(vendorInfo->datas) / sizeof(vendorInfo->datas[0]);
    unsigned char savedVendorID[VD_SIZE];
    int i;
    int num = getSavedHdmiVendorID(disp, savedVendorID, VD_SIZE);
    if (0 == num)
        num = VD_SIZE;
    for (i = 0; i < num; ++i) {
        if (savedVendorID[i] != vendorInfo->datas[i])
            return 0;
    }
    return !0;
}

unsigned int getTvCapabilities(sunxi_hdmi_capability_t *cap);
int resetDispMode(int disp, int type, int mode)
{
    SUNXI_hwcdev_context_t *Globctx = &gSunxiHwcDevice;

    if((DISP_OUTPUT_TYPE_HDMI == type) && (0xFF == mode)) {

        sunxi_hdmi_capability_t cap;
        if (!getHdmiCapability(&cap)) {
            unsigned int cap_mask = getTvCapabilities(&cap);
            DisplayInfo *psDisplayInfo = &(Globctx->SunxiDisplay[disp]);
            if (psDisplayInfo && psDisplayInfo->DisplayType == type) {
                psDisplayInfo->tv_capabilities = cap_mask;
            } else {
                ALOGW("%s,%d", __FUNCTION__, __LINE__);
            }
        }

        vendor_info_t vendorInfo;
        resetParseEdidForDisp(HDMI_USED);
        getVendorInfo(HDMI_USED, &vendorInfo);
        mode = getDispModeFromFile(DISP_OUTPUT_TYPE_HDMI);
        if(!isZeroVendor(&vendorInfo) && !isSameVendor(HDMI_USED, &vendorInfo)) {

            if (!_hwc_device_is_support_hdmi_mode(HDMI_USED, mode)) {
                const int modes[] = {
                    DISP_TV_MOD_3840_2160P_30HZ,
                    DISP_TV_MOD_1080P_60HZ,
                    DISP_TV_MOD_1080I_60HZ,
                    DISP_TV_MOD_1080P_50HZ,
                    DISP_TV_MOD_1080I_50HZ,
                    DISP_TV_MOD_720P_60HZ,
                    DISP_TV_MOD_720P_50HZ,
                    DISP_TV_MOD_576P,
                    DISP_TV_MOD_480P,
                };
                unsigned int i;
                for (i = 0; i < sizeof(modes) / sizeof(modes[0]); ++i) {
                    if (_hwc_device_is_support_hdmi_mode(HDMI_USED, modes[i])) {
                        mode = modes[i];
                        break;
                    }
                }
                mode = (-1 == mode) ? DISP_DEFAULT_HDMI_MODE : mode;
            }
            savedHdmiVendorID(disp, vendorInfo.datas,
                sizeof(vendorInfo.datas) / sizeof(vendorInfo.datas[0]));
        }
    }
    if((DISP_OUTPUT_TYPE_TV == type) && (0xFF == mode)) {
        mode = getDispModeFromFile(DISP_OUTPUT_TYPE_TV);
        mode = (-1 == mode) ? DISP_DEFAULT_CVBS_MODE : mode;
    }
    return mode;
}

int blank_disp(unsigned int hwDisp)
{
    SUNXI_hwcdev_context_t *Globctx = &gSunxiHwcDevice;
	unsigned long arg[4];
    disp_layer_t cfg;
    unsigned int chn = hwDisp ? 2 : 4;
    unsigned int ly = 4;
    unsigned int i,j;

	arg[0] = hwDisp;
    memset((void *)&cfg, 0, sizeof(cfg));

    arg[1] = 1;
    ioctl(Globctx->DisplayFd, DISP_SHADOW_PROTECT, (unsigned long)arg);

    arg[1] = (unsigned long)(&cfg);
    arg[2] = 1;
    for (i = chn; i > 0; --i) {
        for (j = ly; j > 0; --j) {
            cfg.channel = i - 1;
            cfg.layer_id = j - 1;
            ioctl(Globctx->DisplayFd, COMMIT_LAYER_CMD, (unsigned long)arg);
        }
    }
    arg[1] = HWC_COMMIT_FRAME_NUM_INC;
    arg[2] = HWC_SYNC_TIMELINE_DEFAULT;
    arg[3] = 1;
    int fd = ioctl(Globctx->DisplayFd, DISP_HWC_COMMIT, (unsigned long)arg);
    if (0 <= fd)
        close(fd);

    arg[1] = 0;
    ioctl(Globctx->DisplayFd, DISP_SHADOW_PROTECT, (unsigned long)arg);

	return 0;
}

int hwcOutputSwitch(DisplayInfo *psDisplayInfo, int type, int mode)
{
    SUNXI_hwcdev_context_t *Globctx = &gSunxiHwcDevice;
    unsigned long arg[4] = {0};
    int ret = -1;

    ALOGD("###%s: type=%d, mode=%d", __func__, type, mode);
    switch(type)
    {
    case DISP_OUTPUT_TYPE_HDMI: {
		unsigned int hwDisp = getHwDispByType(type);
		disp_device_config cfg;
		sunxi_hdmi_capability_t cap;

		arg[0] = hwDisp;
		arg[1] = (unsigned long)&cfg;
		ret = ioctl(Globctx->DisplayFd, DISP_DEVICE_GET_CONFIG, (unsigned long)arg);
		if (ret) {
			ALOGW("DISP_DEVICE_GET_CONFIG for hdmi failed");
			return ret;
		}
		if (!isHdmiHpd(hwDisp)) {
			ALOGW("%s,%d: fixme", __func__, __LINE__);
			return -1;
		}
		if (getHdmiCapability(&cap)) {
			ALOGW("getHdmiCapability failed");
		}

		unsigned int cap_mask = getTvCapabilities(&cap);
		int dataspace_mode = psDisplayInfo->dataspace_mode;
		int actual_dataspce_mode = psDisplayInfo->actual_dataspce_mode;

		if (DISP_CSC_TYPE_YUV420 == cfg.format) {
			bool support_yuv420 = 0;
			unsigned int i = 0;
			for (; i < sizeof(cap.ycbcr420_resolution) / sizeof(cap.ycbcr420_resolution[0]); ++i) {
				if (-1 == cap.ycbcr420_resolution[i])
					break;
				if (mode == cap.ycbcr420_resolution[i]) {
					support_yuv420 = 1;
					break;
				}
			}
			if (!support_yuv420) {
				cfg.format = DISP_CSC_TYPE_YUV444;
				cfg.bits = DISP_DATA_8BITS;
				psDisplayInfo->output_format = DISP_CSC_TYPE_YUV444;
				psDisplayInfo->output_bits = DISP_DATA_8BITS;
				if (DISP_TV_MOD_3840_2160P_60HZ == mode) {
					if (cap.video_format & VIDEO_FORMAT_YCBCR422) {
						cfg.format = DISP_CSC_TYPE_YUV422;
						cfg.bits = DISP_DATA_10BITS;
						psDisplayInfo->output_format = DISP_CSC_TYPE_YUV422;
						psDisplayInfo->output_bits = DISP_DATA_10BITS;
					} else {
						ALOGW("still use yuv420-10bit for 4k60hz");
						cfg.format = DISP_CSC_TYPE_YUV420;
						cfg.bits = DISP_DATA_10BITS;
						psDisplayInfo->output_format = DISP_CSC_TYPE_YUV420;
						psDisplayInfo->output_bits = DISP_DATA_10BITS;
					}
				}
			}
		}

		if (DISP_TV_MOD_3840_2160P_60HZ == mode) {
			if (DISP_CSC_TYPE_YUV420 != cfg.format) {
				cfg.format = DISP_CSC_TYPE_YUV420;
				cfg.bits = DISP_DATA_10BITS;
				psDisplayInfo->output_format = DISP_CSC_TYPE_YUV420;
				psDisplayInfo->output_bits = DISP_DATA_10BITS;
			}
		}

		if (DISPLAY_OUTPUT_DATASPACE_MODE_HDR == actual_dataspce_mode) {
			if (!(TV_CAPABILITY_HDR & cap_mask)) {
				ALOGD("reset hdmi to sdr mode");
				dataspace_mode = DISPLAY_OUTPUT_DATASPACE_MODE_AUTO;
				actual_dataspce_mode = DISPLAY_OUTPUT_DATASPACE_MODE_SDR;
				cfg.eotf = DISP_EOTF_GAMMA22;
				if (576 >= get_info_mode(mode, HEIGHT)) {
					cfg.cs = DISP_BT601;
				} else {
					cfg.cs = DISP_BT709;
				}
			}
		} else {
			if (576 >= get_info_mode(mode, HEIGHT)) {
				cfg.cs = DISP_BT601;
			} else {
				cfg.cs = DISP_BT709;
			}
		}

		cfg.mode = (disp_tv_mode)mode;

		psDisplayInfo->setblank = 1;
		arg[0] = hwDisp;
		arg[1] = (unsigned long)&cfg;
		ret = ioctl(Globctx->DisplayFd, DISP_DEVICE_SET_CONFIG, (unsigned long)arg);
		if (ret) {
			ALOGD("DISP_DEVICE_SET_CONFIG failed %d", ret);
			psDisplayInfo->setblank = 0;
			return ret;
		}
		psDisplayInfo->dataspace_mode = dataspace_mode;
		psDisplayInfo->actual_dataspce_mode = actual_dataspce_mode;

        psDisplayInfo->VirtualToHWDisplay = getHwDispByType(type);
        psDisplayInfo->VarDisplayWidth = get_info_mode(mode,WIDTH);
        psDisplayInfo->VarDisplayHeight = get_info_mode(mode,HEIGHT);
        psDisplayInfo->DisplayFps = get_info_mode(mode, REFRESHRAE);
        psDisplayInfo->DisplayVsyncP = 1000000000 / psDisplayInfo->DisplayFps;
        psDisplayInfo->DisplayType = type;
        psDisplayInfo->DisplayMode = mode;
        psDisplayInfo->HwChannelNum = psDisplayInfo->VirtualToHWDisplay ? 2 : NUMCHANNELOFDSP;
        psDisplayInfo->de_clk = 432000000;
        psDisplayInfo->setblank = 0;
        Globctx->psHwcProcs->invalidate(Globctx->psHwcProcs);
        if (DISP_TV_MOD_1080P_24HZ_3D_FP != mode)
            saveDispModeToFile(type, mode);
            saveDispDeviceConfig(hwDisp, &cfg);
    	}
        hwc_set_vsync_need_reset(HWC_VSYNC_PERIOD_CHANGED);
        break;
    case DISP_OUTPUT_TYPE_TV:
        psDisplayInfo->setblank = 1;
        arg[0] = getHwDispByType(type);
        arg[1] = type;
        arg[2] = mode;
        ret = ioctl(Globctx->DisplayFd, DISP_DEVICE_SWITCH, (unsigned long)arg);
        if (ret) {
            ALOGD("DISP_DEVICE_SWITCH failed %d", ret);
            psDisplayInfo->setblank = 0;
            return ret;
        }
        psDisplayInfo->VirtualToHWDisplay = getHwDispByType(type);
        psDisplayInfo->VarDisplayWidth = get_info_mode(mode,WIDTH);
        psDisplayInfo->VarDisplayHeight = get_info_mode(mode,HEIGHT);
        psDisplayInfo->DisplayFps = get_info_mode(mode, REFRESHRAE);
        psDisplayInfo->DisplayVsyncP = 1000000000 / psDisplayInfo->DisplayFps;
        psDisplayInfo->DisplayType = type;
        psDisplayInfo->DisplayMode = mode;
        psDisplayInfo->HwChannelNum = psDisplayInfo->VirtualToHWDisplay ? 2 : NUMCHANNELOFDSP;
        psDisplayInfo->de_clk = 432000000;
        psDisplayInfo->setblank = 0;
        Globctx->psHwcProcs->invalidate(Globctx->psHwcProcs);
        saveDispModeToFile(type, mode);
        hwc_set_vsync_need_reset(HWC_VSYNC_PERIOD_CHANGED);
        break;
    case DISP_OUTPUT_TYPE_LCD:
        ALOGD("%s:%d\n", __func__, __LINE__);
        break;
    case DISP_OUTPUT_TYPE_VGA:
        psDisplayInfo->setblank = 1;
        arg[0] = getHwDispByType(type);
        arg[1] = type;
        arg[2] = mode;
        ret = ioctl(Globctx->DisplayFd, DISP_DEVICE_SWITCH, (unsigned long)arg);
        if (ret) {
            ALOGD("DISP_DEVICE_SWITCH failed %d", ret);
            psDisplayInfo->setblank = 0;
            return ret;
        }
        psDisplayInfo->VirtualToHWDisplay = getHwDispByType(type);
        psDisplayInfo->VarDisplayWidth = getVgaInfo(mode,WIDTH);
        psDisplayInfo->VarDisplayHeight = getVgaInfo(mode,HEIGHT);
        psDisplayInfo->DisplayFps = getVgaInfo(mode, REFRESHRAE);
        psDisplayInfo->DisplayVsyncP = 1000000000 / psDisplayInfo->DisplayFps;
        psDisplayInfo->DisplayType = type;
        psDisplayInfo->DisplayMode = mode;
        psDisplayInfo->HwChannelNum = psDisplayInfo->VirtualToHWDisplay ? 2 : NUMCHANNELOFDSP;
        psDisplayInfo->de_clk = 432000000;
        psDisplayInfo->setblank = 0;
        Globctx->psHwcProcs->invalidate(Globctx->psHwcProcs);
        hwc_set_vsync_need_reset(HWC_VSYNC_PERIOD_CHANGED);
        break;
    case DISP_OUTPUT_TYPE_NONE:
        arg[0] = psDisplayInfo->VirtualToHWDisplay;
        psDisplayInfo->setblank = 1;
        psDisplayInfo->VirtualToHWDisplay = EVALID_HWDISP_ID;
        psDisplayInfo->DisplayType = 0;
        psDisplayInfo->DisplayMode = 0;
        psDisplayInfo->setDispMode = 0;
        hwcCleanHwDisp((unsigned int)(arg[0]));
        if (CVBS_USED == arg[0]) {
            ALOGD("close cvbs device");
            usleep(20000); // 20ms
            arg[1] = 0;
            arg[2] = 0;
            ret = ioctl(Globctx->DisplayFd, DISP_DEVICE_SWITCH, (unsigned long)arg);
        }
        ret = 0;
        break;
    default:
        ret = -1;
    }

    get_de_freq_and_fps(psDisplayInfo);
    return ret;
}

int setDispDeviceConfigs(int hwDisp, void *configs)
{
    SUNXI_hwcdev_context_t *Globctx = &gSunxiHwcDevice;
    unsigned long arg[4] = {0};

    arg[0] = hwDisp;
    arg[1] = (unsigned long)configs;
    return ioctl(Globctx->DisplayFd, DISP_DEVICE_SET_CONFIG, (unsigned long)arg);
}

int getDispDeviceConfigs(int hwDisp, void *configs)
{
    SUNXI_hwcdev_context_t *Globctx = &gSunxiHwcDevice;
	unsigned long arg[4] = {0};

	arg[0] = hwDisp;
	arg[1] = (unsigned long)configs;
	return ioctl(Globctx->DisplayFd, DISP_DEVICE_GET_CONFIG, (unsigned long)arg);
}

int hwcOutputExchange()
{
    SUNXI_hwcdev_context_t *Globctx = &gSunxiHwcDevice;
    pthread_mutex_lock(&Globctx->lock);
    Globctx->exchangeDispChannel = 1;
    pthread_mutex_unlock(&Globctx->lock);
    Globctx->psHwcProcs->invalidate(Globctx->psHwcProcs);
    unsigned int count = 0;
    do
    {
        usleep(1000);
        count++;
    }while(Globctx->exchangeDispChannel);
    ALOGD("use time = %d ms", count);
    return 0;
}

int hwcCleanHwDisp(unsigned int hwDisp)
{
    SUNXI_hwcdev_context_t *Globctx = &gSunxiHwcDevice;
    pthread_mutex_lock(&Globctx->lock);
    Globctx->blankHwdisp = 0x80000000 | hwDisp;
    pthread_mutex_unlock(&Globctx->lock);
    Globctx->psHwcProcs->invalidate(Globctx->psHwcProcs);
    unsigned int count = 0;
    do {
        usleep(1000);
        count++;
    }while(Globctx->blankHwdisp & 0x80000000);
    ALOGD("hwcCleanHwDisp%d use time = %d ms", hwDisp, count);
    return 0;
}

int check4KBan(void)
{
#if 0
    int ret = 1;
    int dev;
    unsigned char buf[16];
    char *p = (char *)buf;

    dev = open("/dev/sunxi_soc_info", O_RDONLY);
    if (dev < 0) {
        ALOGD("cannot open /dev/sunxi_soc_info\n");
        return 0;
    }
    memset(p, 0, sizeof(buf));
    if (ioctl(dev, 3, p) >= 0)
    {
        ALOGD("%s\n", p);
        if((0 == strcmp(p, "00000000"))
          || (0 == strcmp(p, "00000081")))
        {
            ret = 0;
        }
        else
        {
            ret = 1;
        }
    }
    else
    {
        ALOGD("ioctl err!\n");
    }
    close(dev);

    return ret;
#endif
    return 0;
}

unsigned int getIomenType(unsigned int defval)
{
    char property[PROPERTY_VALUE_MAX] = {0};
    if (property_get("ro.kernel.iomem.type", property, NULL) > 0) {
        return (unsigned int)strtoul(property, NULL, 16);
    } else {
        return defval;
    }
}

unsigned int getTvCapabilities(sunxi_hdmi_capability_t *cap)
{
	unsigned int cap_mask = TV_CAPABILITY_SDR;

	if (cap->colorimetry & (STANDARD_BT2020_YCC)) {
		cap_mask |= TV_CAPABILITY_WCG;
	}
	if (cap->capability.eotf & SMPTE_ST_2048) {
		cap_mask |= TV_CAPABILITY_HDR;
	}
	return cap_mask;
}

void checkinTvConfig(DisplayInfo *psDisplayInfo)
{
	SUNXI_hwcdev_context_t *Globctx = &gSunxiHwcDevice;

	psDisplayInfo->output_format = 0;
	psDisplayInfo->output_bits = 0;
	psDisplayInfo->tv_capabilities = TV_CAPABILITY_SDR; 		
	psDisplayInfo->actual_dataspce_mode = DISPLAY_OUTPUT_DATASPACE_MODE_SDR;
	psDisplayInfo->dataspace_mode = DISPLAY_OUTPUT_DATASPACE_MODE_SDR;

	if (DISP_OUTPUT_TYPE_HDMI == psDisplayInfo->DisplayType) {
		unsigned int hwDisp = psDisplayInfo->VirtualToHWDisplay;
		unsigned long arg[4];
		struct disp_device_config cfg;
		arg[0] = hwDisp;
		arg[1] = (unsigned long)&cfg;
		ioctl(Globctx->DisplayFd, DISP_DEVICE_GET_CONFIG, (unsigned long)arg);

		if (DISP_EOTF_SMPTE2084 == cfg.eotf) {
			psDisplayInfo->actual_dataspce_mode = DISPLAY_OUTPUT_DATASPACE_MODE_HDR;
		} else {
			psDisplayInfo->actual_dataspce_mode = DISPLAY_OUTPUT_DATASPACE_MODE_SDR;
		}
		psDisplayInfo->dataspace_mode = getSavedDataSpaceMode(hwDisp);
		if (-1 == psDisplayInfo->dataspace_mode) {
			psDisplayInfo->dataspace_mode = DISPLAY_OUTPUT_DATASPACE_MODE_AUTO;
		}
		psDisplayInfo->dataspace_mode %= DISPLAY_OUTPUT_DATASPACE_MODE_NUM;
		if (DISPLAY_OUTPUT_DATASPACE_MODE_AUTO != psDisplayInfo->dataspace_mode) {
			ALOGD("dataspace_mode change(%d --> %d) in checkinTvConfig",
				psDisplayInfo->dataspace_mode, psDisplayInfo->actual_dataspce_mode);
			psDisplayInfo->dataspace_mode = psDisplayInfo->actual_dataspce_mode;
		}
		ALOGD("dataspace_mode=%d, actual_dataspce_mode=%d",
			psDisplayInfo->dataspace_mode, psDisplayInfo->actual_dataspce_mode);

		psDisplayInfo->setOutputformat = getSavedOutputPixelFormat(hwDisp);
		if (-1 == psDisplayInfo->setOutputformat) {
			psDisplayInfo->setOutputformat = DISPLAY_OUTPUT_FORMAT_AUTO;
		}
		psDisplayInfo->setOutputformat %= DISPLAY_OUTPUT_FORMAT_NUM;
		psDisplayInfo->output_format = cfg.format;
		psDisplayInfo->output_bits = cfg.bits;

		unsigned int is_hpd = isHdmiHpd(hwDisp);
		if (is_hpd) {
			// check hdmi configs according to user output dataspace policy.
			sunxi_hdmi_capability_t cap;
			if (!getHdmiCapability(&cap)) {
				unsigned int cap_mask = getTvCapabilities(&cap);
				ALOGD("cap_mask=%u. cap: eotf=%d, metadata_descriptor=%d, max_luminance=%d,"
					"max_frame_average_luminance=%d, min_luminance=%d. colorimetry=%d,"
					"video_format=%d, pixel_encoding=%d, ycbcr420_resolution=[%d,%d,%d,%d,%d,%d,%d,%d]",
					cap_mask, cap.capability.eotf, cap.capability.metadata_descriptor,
					cap.capability.max_luminance, cap.capability.max_frame_average_luminance,
					cap.capability.min_luminance, cap.colorimetry, cap.video_format,
					cap.pixel_encoding, cap.ycbcr420_resolution[0], cap.ycbcr420_resolution[1],
					cap.ycbcr420_resolution[2], cap.ycbcr420_resolution[3],
					cap.ycbcr420_resolution[4], cap.ycbcr420_resolution[5],
					cap.ycbcr420_resolution[6], cap.ycbcr420_resolution[7]);
				psDisplayInfo->tv_capabilities = cap_mask;
			} else {
				ALOGW("getHdmiCapability fail");
			}
		} else {
			ALOGW("hdmi hpd out. not to getHdmiCapability");
		}
	}
}

int checkEnhanceMode(DisplayInfo *psDisplayInfo)
{
	if (HW_CHANNEL_PRIMARY != psDisplayInfo->VirtualToHWDisplay)
		return 0;


	int value = getValueFromProperty(ENHANCE_BRIGHT_PROPERTY);
	if ((0 > value) || (10 < value))
		value = 5;
	_hwc_device_set_enhance_bright(HWC_DISPLAY_PRIMARY, value);
	value = getValueFromProperty(ENHANCE_CONTRAST_PROPERTY);
	if ((0 > value) || (10 < value))
		value = 5;
	_hwc_device_set_enhance_contrast(HWC_DISPLAY_PRIMARY, value);
	value = getValueFromProperty(ENHANCE_DENOISE_PROPERTY);
	if ((0 > value) || (10 < value))
		value = 5;
	_hwc_device_set_enhance_denoise(HWC_DISPLAY_PRIMARY, value);
	value = getValueFromProperty(ENHANCE_DETAIL_PROPERTY);
	if ((0 > value)	|| (10 < value))
		value = 5;
	_hwc_device_set_enhance_detail(HWC_DISPLAY_PRIMARY, value);
	_hwc_device_set_enhance_edge(HWC_DISPLAY_PRIMARY, value);
	value = getValueFromProperty(ENHANCE_SATURATION_PROPERTY);
	if ((0 > value) || (10 < value))
		value = 5;
	_hwc_device_set_enhance_saturation(HWC_DISPLAY_PRIMARY, value);

	value = getValueFromProperty(ENHANCE_MODE_PROPERTY);
	if (-1 == value)
		value = 1; // default enable
	if (psDisplayInfo->actual_dataspce_mode != DISPLAY_OUTPUT_DATASPACE_MODE_SDR) {
		value = 0;
	}
	if (_hwc_device_get_enhance_mode(HWC_DISPLAY_PRIMARY) != value) {
		_hwc_device_set_enhance_mode(HWC_DISPLAY_PRIMARY, value);
	}

	return 0;
}

static void updateFps(SUNXI_hwcdev_context_t *psCtx)
{

    double fCurrentTime = 0.0;
    timeval tv = { 0, 0 };
    gettimeofday(&tv, NULL);
    fCurrentTime = tv.tv_sec + tv.tv_usec / 1.0e6;

    if(fCurrentTime - psCtx->fBeginTime >= 1)
    {
        char property[PROPERTY_VALUE_MAX]={0};
        int  show_fps_settings = 0;

        if (property_get("debug.hwc.showfps", property, NULL) >= 0)
        {
            if(property[0] == '1'){
                show_fps_settings = 1;
            }else if(property[0] == '2'){
                show_fps_settings = 2;
            } else if (property[0] == '4') {
                show_fps_settings = 4;
            } else {
                show_fps_settings = 0;
            }
        }else{
            ALOGD("No hwc debug attribute node.");
            return;
        }
        if((show_fps_settings & FPS_SHOW) != (psCtx->hwcdebug & FPS_SHOW))
        {
            ALOGD("###### %s hwc fps print ######",
                (show_fps_settings & FPS_SHOW) != 0 ? "Enable":"Disable");
        }
        psCtx->hwcdebug = show_fps_settings&SHOW_ALL;
        if(psCtx->hwcdebug&1)
        {
            ALOGD(">>>fps:: %d\n", (int)((psCtx->HWCFramecount - psCtx->uiBeginFrame) * 1.0f
                                      / (fCurrentTime - psCtx->fBeginTime)));
        }
        psCtx->uiBeginFrame = psCtx->HWCFramecount;
        psCtx->fBeginTime = fCurrentTime;
    }
}

hwc_vsync_modle_t g_vsync_mod;

void hwc_set_vsync_need_reset(unsigned int flag)
{
	g_vsync_mod.reset_flag |= flag;
}

static void dump_vsync_modle()
{
	SUNXI_hwcdev_context_t *Globctx = &gSunxiHwcDevice;

	hwc_vsync_modle_t *modle = &g_vsync_mod;
	unsigned int i = 0;
	unsigned int id = modle->head_id;

	if (!(Globctx->hwcdebug & VSYNC_TRIM_DEBUG)) {
		return;
	}

	ALOGD("---------------- dump_vsync_modle begin ---------------");
	ALOGD("period_base=%llu,period_averge=%llu. (%d/%d)",
		modle->period_base, modle->period_averge, modle->sample_num, modle->count);
	for (i = 0; i < modle->sample_num; ++i) {
		ALOGD("sample(%d)= %llu, period_delta=%lld", id,
			modle->ts_slot[id],
			modle->period_delta[id]);
		id = (id + 1) % HWC_MAX_VSYNC_SAMPLES;
	}
	ALOGD("---------------- dump_vsync_modle end   ---------------");
}

static int vsync_modle_init()
{
	SUNXI_hwcdev_context_t *Globctx = &gSunxiHwcDevice;
	DisplayInfo *psDisplayInfo = &(Globctx->SunxiDisplay[HWC_DISPLAY_PRIMARY]);

	memset((void *)&g_vsync_mod, 0, sizeof(g_vsync_mod));
	g_vsync_mod.period_base = psDisplayInfo->DisplayVsyncP;
	g_vsync_mod.hwDisp = psDisplayInfo->VirtualToHWDisplay;
	return 0;
}

static void vsync_adjust_period_base(hwc_vsync_modle_t *modle)
{
#ifdef ENABLE_ADJUST_VSYNC_PERIOD_BASE

	if (0 == modle->count % HWC_MIN_VSYNC_SAMPLES_REQ) {
		unsigned int cur , prev;
		unsigned int check_num, i, id0, id1;
		nsecs_t period_delta0, period_delta1;

		if (HWC_MAX_VSYNC_SAMPLES < modle->count) {
			check_num = modle->sample_num;
		} else {
			check_num = modle->sample_num - 1;
		}

		id0 = 0;
		period_delta0 = modle->period_delta[modle->tail_id];
		id1 = 1;
		cur = (modle->tail_id + HWC_MAX_VSYNC_SAMPLES - 1) % HWC_MAX_VSYNC_SAMPLES;
		period_delta1 = modle->period_delta[cur];
		if (period_delta0 > period_delta1) {
			period_delta0 ^= period_delta1;
			period_delta1 ^= period_delta0;
			period_delta0 ^= period_delta1;
			id0 ^= id1;
			id1 ^= id0;
			id0 ^= id1;
		}
		cur = (modle->tail_id + HWC_MAX_VSYNC_SAMPLES - 2) % HWC_MAX_VSYNC_SAMPLES;
		for (i = 2; i < check_num; ++i, cur = prev) {
			prev = (cur + HWC_MAX_VSYNC_SAMPLES - 1) % HWC_MAX_VSYNC_SAMPLES;
			//ALOGD("cur=%d, prev=%d, period_delta0=%lld, period_delta1=%lld, period_cur=%lld",
			//	cur, prev, period_delta0, period_delta1, modle->period_delta[cur]);
			if (period_delta0 >= modle->period_delta[cur]) {
				period_delta1 = period_delta0;
				id1 = id0;
				period_delta0 = modle->period_delta[cur];
				id0 = i;
				//ALOGD("id0=%d", id0);
				continue;
			}
			if (period_delta1 >= modle->period_delta[cur]) {
				period_delta1 = modle->period_delta[cur];
				id1 = i;
				//ALOGD("id1=%d, id0=%d", id1, id0);
			}
		}
		//ALOGD("final: id0=%d,id1=%d", id0, id1);
		if (id0 < id1) {
			int n = id1 - id0;
			id1 = (modle->tail_id + HWC_MAX_VSYNC_SAMPLES - id1) % HWC_MAX_VSYNC_SAMPLES;
			id0 = (modle->tail_id + HWC_MAX_VSYNC_SAMPLES - id0) % HWC_MAX_VSYNC_SAMPLES;
			modle->period_base = (modle->period_base >> 1)
				+ ((modle->ts_slot[id0] - modle->ts_slot[id1]) / n >> 1);
		} else if (id0 > id1) {
			int n = id0 - id1;
			id1 = (modle->tail_id + HWC_MAX_VSYNC_SAMPLES - id1) % HWC_MAX_VSYNC_SAMPLES;
			id0 = (modle->tail_id + HWC_MAX_VSYNC_SAMPLES - id0) % HWC_MAX_VSYNC_SAMPLES;
			modle->period_base = (modle->period_base >> 1)
				+ ((modle->ts_slot[id1] - modle->ts_slot[id0]) / n >> 1);
		} else {
			ALOGE("%s,%d: should never happen", __func__, __LINE__);
		}
		//ALOGD("###period_base=%llu,period_averge=%llu",
		//	modle->period_base, modle->period_averge);

		prev = (modle->tail_id + HWC_MAX_VSYNC_SAMPLES - 1) % HWC_MAX_VSYNC_SAMPLES;
		modle->period_delta[modle->tail_id] =
			(nsecs_t)(modle->ts_slot[modle->tail_id] - modle->ts_slot[prev])
			- (nsecs_t)(modle->period_base);
	}
#else
	modle;
#endif /* #ifdef ENABLE_ADJUST_VSYNC_PERIOD_BASE */
}

static uint64_t vsync_do_trim_timestamp(hwc_vsync_modle_t *modle) {
	unsigned int cur = modle->tail_id;
	unsigned int prev = (cur + HWC_MAX_VSYNC_SAMPLES - 1) % HWC_MAX_VSYNC_SAMPLES;
	if ((HWC_MIN_TS_ABS_DIF < modle->period_delta[cur])) {
		modle->ts_slot[cur] = modle->ts_slot[prev] + modle->period_base;
		modle->period_delta[cur] = 0;
		ALOGD("trim the last timestamp(%d)=%llu", cur, modle->ts_slot[cur]);
	}

	return modle->ts_slot[modle->tail_id];
}

static uint64_t vsync_timestamp_trim(uint32_t hwDisp, uint64_t ts)
{
	hwc_vsync_modle_t *modle = &g_vsync_mod;

	// 1.checkin timestamp
	if ((hwDisp == modle->hwDisp)
		&& !(modle->reset_flag & HWC_VSYNC_PERIOD_CHANGED)) {
		if (0 != modle->sample_num) {
			uint64_t prev_ts = modle->ts_slot[modle->tail_id];

			modle->tail_id = (modle->tail_id + 1) % HWC_MAX_VSYNC_SAMPLES;
			modle->ts_slot[modle->tail_id] = ts;
			modle->period_delta[modle->tail_id] =
				(nsecs_t)(ts - prev_ts) - (nsecs_t)(modle->period_base);

				if ((nsecs_t)(modle->period_base) - HWC_MIN_TS_ABS_DIF
					< modle->period_delta[modle->tail_id]) {
					//ALOGD("reset for period_delta changed");
					modle->sample_num = 1;
					modle->count = 1;
					modle->head_id = modle->tail_id;
					modle->period_delta[modle->head_id] = 0;

					if (modle->reset_flag & HWC_VSYNC_ENABLE_CHANGED) {
						//ALOGD("HWC_VSYNC_ENABLE_CHANGED");
						modle->reset_flag &= ~HWC_VSYNC_ENABLE_CHANGED;
					}

					return ts;
				}

			if (modle->head_id == modle->tail_id) {
				modle->head_id = (modle->head_id + 1) % HWC_MAX_VSYNC_SAMPLES;
			} else {
				modle->sample_num++;
			}
			modle->count++;

		} else {
			modle->tail_id = modle->head_id;
			modle->ts_slot[modle->tail_id] = ts;
			modle->period_delta[modle->tail_id] = 0;
			modle->sample_num++;
			modle->count++;
		}
	} else {
		vsync_modle_init();
		return vsync_timestamp_trim(modle->hwDisp, ts);
	}

	if (HWC_MIN_VSYNC_SAMPLES_REQ > modle->sample_num) {
		return ts;
	}
	dump_vsync_modle();

	// 2. adjust timestamp period base
	vsync_adjust_period_base(modle);

	// 3. do trim vsync timestamp.
	return vsync_do_trim_timestamp(modle);
}

static void post_vsync_timestamp(SUNXI_hwcdev_context_t *ctx,
	char *timestamps, unsigned int disp) {

	uint64_t timestamp = 0;
	uint64_t lastTimestamp = 0;
	uint32_t hwDisp = ctx->SunxiDisplay[disp].VirtualToHWDisplay;
	char *p;

	while (1) {
		p = strstr(timestamps, ",");
		if (NULL != p) {
			ALOGD("has two timestamp,%s", timestamps);
			*p = 0;
		}
		timestamp = strtoull(timestamps, NULL, 0);
		if (lastTimestamp < timestamp) {
			lastTimestamp = timestamp;
			timestamp = vsync_timestamp_trim(hwDisp, timestamp);
			ctx->psHwcProcs->vsync(ctx->psHwcProcs, 0, timestamp);
		}
		if (NULL == p)
			break;
		timestamps = p + 1;
	}
}

static int hwc_uevent(void)
{
    struct sockaddr_nl snl;
    const int buffersize = 32*1024;
    int retval;
    int hotplug_sock;
    SUNXI_hwcdev_context_t *Globctx = &gSunxiHwcDevice;

    initParseEdid(HDMI_USED);
    vsync_modle_init();

    memset(&snl, 0x0, sizeof(snl));
    snl.nl_family = AF_NETLINK;
    snl.nl_pid = 0;
    snl.nl_groups = 0xffffffff;

    hotplug_sock = socket(PF_NETLINK, SOCK_DGRAM, NETLINK_KOBJECT_UEVENT);
    if (hotplug_sock == -1) {
        ALOGE("####socket is failed in %s error:%d %s###", __FUNCTION__, errno, strerror(errno));
        return -1;
    }

    setsockopt(hotplug_sock, SOL_SOCKET, SO_RCVBUFFORCE, &buffersize, sizeof(buffersize));

    retval = bind(hotplug_sock, (struct sockaddr *)&snl, sizeof(struct sockaddr_nl));

    if (retval < 0) {
        ALOGE("####bind is failed in %s error:%d %s###", __FUNCTION__, errno, strerror(errno));
        close(hotplug_sock);
        return -1;
    }
    char *buf;
    buf=(char *)malloc(sizeof(char) * 1024);
    struct pollfd fds;
    int err;
    unsigned int cout;
    char *s = NULL;
    int IsVsync, IsHdmi;
    unsigned int new_hdmi_hpd;
    uint64_t timestamp = 0;
    int display_id = -1,cnt = 2;
    int count = 0;
    while(1)
    {
        fds.fd = hotplug_sock;
        fds.events = POLLIN;
        fds.revents = 0;
        cout = Globctx->HWCFramecount;
        err = poll(&fds, 1, 1000);
        memset(buf, '\0', sizeof(char) * 1024);
        if(err > 0 && (fds.revents & POLLIN))
        {
            count = recv(hotplug_sock, buf, sizeof(char) * 1024,0);
            if(count > 0)
            {
                IsVsync = !strcmp(buf, "change@/devices/soc/disp");
                IsHdmi = !strcmp(buf, "change@/devices/virtual/switch/hdmi");

                if(IsVsync)
                {
                    display_id = -1,cnt = 2;
                    s = buf;

                    if(!Globctx->psHwcProcs || !Globctx->psHwcProcs->vsync){
                        free(buf);
                        buf = NULL;
                        return 0;
                    }
                    s += strlen(s) + 1;
                    while(s)
                    {
                        if (!strncmp(s, "VSYNC0=", strlen("VSYNC0=")))
                        {
                            s += strlen("VSYNC0=");
                            if(0 == Globctx->SunxiDisplay[0].VirtualToHWDisplay
                                && Globctx->SunxiDisplay[0].VsyncEnable == 1) {
                                post_vsync_timestamp(Globctx, s, HWC_DISPLAY_PRIMARY);
                            }
                        }
                        else if (!strncmp(s, "VSYNC1=", strlen("VSYNC1=")))
                        {
                            s += strlen("VSYNC1=");
                            if(1 == Globctx->SunxiDisplay[0].VirtualToHWDisplay
                                && Globctx->SunxiDisplay[0].VsyncEnable == 1) {
                                post_vsync_timestamp(Globctx, s, HWC_DISPLAY_PRIMARY);
                            }
                        }
                        s += strlen(s) + 1;
                        if(s - buf >= count)
                        {
                            break;
                        }
                    }
                }

                if(IsHdmi)
                {
                    s = buf;
                    s += strlen(s) + 1;
                    while(s)
                    {
                        if (!strncmp(s, "SWITCH_STATE=", strlen("SWITCH_STATE=")))
                        {
                            new_hdmi_hpd = strtoull(s + strlen("SWITCH_STATE="), NULL, 0);
                            ALOGD( "#### disp[%d]   hotplug[%d]###",HDMI_USED ,!!new_hdmi_hpd);
                        }
                        s += strlen(s) + 1;
                        if(s - buf >= count)
                        {
                            break;
                        }
                    }
                }
            }
            if(Globctx->SunxiDisplay[0].VsyncEnable == 1)
            {
                Globctx->ForceGPUComp = 0;
            }
        }
        updateFps(Globctx);
    }
    free(buf);
    buf = NULL;
    return 0;
}

void *VsyncThreadWrapper(void *priv)
{
    priv;
    setpriority(PRIO_PROCESS, 0, HAL_PRIORITY_URGENT_DISPLAY);

    hwc_uevent();

    return NULL;
}

