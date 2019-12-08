#include "../hwc.h"
#include "drv_display.h"
#include "private.h"
#include "../common/utils.h"

static const int mModeNum = 19;
static const tv_para_t mTvPara[]= {
    {8, DISP_TV_MOD_NTSC,             720,    480, 60},
    {8, DISP_TV_MOD_PAL,              720,    576, 60},

    {5, DISP_TV_MOD_480I,             720,    480, 60},
    {5, DISP_TV_MOD_576I,             720,    576, 60},
    {5, DISP_TV_MOD_480P,             720,    480, 60},
    {5, DISP_TV_MOD_576P,             720,    576, 60},
    {5, DISP_TV_MOD_720P_50HZ,        1280,   720, 50},
    {5, DISP_TV_MOD_720P_60HZ,        1280,   720, 60},
    {5, DISP_TV_MOD_1080I_50HZ,       1920,   1080, 50},
    {5, DISP_TV_MOD_1080I_60HZ,       1920,   1080, 60},
    {1, DISP_TV_MOD_1080P_24HZ,       1920,   1080, 24},
    {5, DISP_TV_MOD_1080P_50HZ,       1920,   1080, 50},
    {5, DISP_TV_MOD_1080P_60HZ,       1920,   1080, 60},

	{5, DISP_TV_MOD_3840_2160P_25HZ,  3840,   2160, 25},
	{5, DISP_TV_MOD_3840_2160P_24HZ,  3840,   2160, 24},
    {5, DISP_TV_MOD_3840_2160P_30HZ,  3840,   2160, 30},

    {1, DISP_TV_MOD_1080P_24HZ_3D_FP, 1920,   1080, 24},
    {1, DISP_TV_MOD_720P_50HZ_3D_FP,  1280,   720, 50},
    {1, DISP_TV_MOD_720P_60HZ_3D_FP,  1280,   720, 60},

};

//internal functions

float getFeClk(int mode){
    switch(mode){
    case DISP_TV_MOD_3840_2160P_30HZ:
    case DISP_TV_MOD_3840_2160P_24HZ:
    case DISP_TV_MOD_3840_2160P_25HZ:
        return 396000000;
    default:
        return 297000000;
    }
}

/*  switch the output of hwDisp to type and mode
* hwDisp:  hardware channel of disp
* type :  HDMI, CVBS, and so on.
* mode: 720p50hz, 720p60hz, ...
*/
int outputSwitch(int hwDisp, int type, int mode){
    hwc_context_t *glbCtx = &gHwcContext;
    unsigned long arg[4] = {0};
    device_info_t *psDeviceInfo = NULL;
    psDeviceInfo = hwcFindDevice(hwDisp);
    ALOGD("%s : hwDisp=%d, type=%d, mode=%d", __func__, hwDisp, type, mode);
    switch(type){
    case DISP_OUTPUT_TYPE_HDMI:
        arg[0] = hwDisp;
        ioctl(glbCtx->displayFd, DISP_CMD_HDMI_DISABLE, (unsigned long)arg);
        usleep(300*1000);
        if(glbCtx->mainDispId == HDMI_USED){
            mFeClk = getFeClk(mode);
            mDeFps = getInfoOfMode(mTvPara, mModeNum, mode, REFRESHRATE);
        }
        psDeviceInfo->disp.id = hwDisp;
        psDeviceInfo->varDisplayWidth = getInfoOfMode(mTvPara, mModeNum, mode, WIDTH);
        psDeviceInfo->varDisplayHeight = getInfoOfMode(mTvPara, mModeNum, mode, HEIGHT);
        psDeviceInfo->displayType = DISP_OUTPUT_TYPE_HDMI;
        psDeviceInfo->displayMode = mode;
        psDeviceInfo->displayDPI_X = 213000;
        psDeviceInfo->displayDPI_Y = 213000;
        psDeviceInfo->displayVsyncP = 1000000000/getInfoOfMode(mTvPara, mModeNum, mode, REFRESHRATE);

        arg[1] = mode;
        ioctl(glbCtx->displayFd, DISP_CMD_HDMI_SET_MODE, (unsigned long)arg);
        ioctl(glbCtx->displayFd, DISP_CMD_HDMI_ENABLE, (unsigned long)arg);

        invalidate(10);
        if(glbCtx->psHwcProcs){
            glbCtx->psHwcProcs->invalidate(glbCtx->psHwcProcs);
        }
        ALOGD("switch to HDMI[0x%x]", mode);
        break;
    case DISP_OUTPUT_TYPE_TV:
        arg[0] = hwDisp;
        ioctl(glbCtx->displayFd, DISP_CMD_TV_OFF, (unsigned long)arg);
        if(glbCtx->mainDispId == CVBS_USED){
            mFeClk = getFeClk(mode);
            mDeFps = getInfoOfMode(mTvPara, mModeNum, mode, REFRESHRATE);
        }
        psDeviceInfo->disp.id = hwDisp;
        psDeviceInfo->varDisplayWidth = getInfoOfMode(mTvPara, mModeNum, mode, WIDTH);
        psDeviceInfo->varDisplayHeight = getInfoOfMode(mTvPara, mModeNum, mode, HEIGHT);
        psDeviceInfo->displayType = DISP_OUTPUT_TYPE_TV;
        psDeviceInfo->displayMode = mode;
        psDeviceInfo->displayDPI_X = 213000;
        psDeviceInfo->displayDPI_Y = 213000;
        psDeviceInfo->displayVsyncP = 1000000000/getInfoOfMode(mTvPara, mModeNum, mode, REFRESHRATE);

        arg[1] = mode;
        ioctl(glbCtx->displayFd, DISP_CMD_TV_SET_MODE, (unsigned long)arg);
        ioctl(glbCtx->displayFd, DISP_CMD_TV_ON, (unsigned long)arg);
        invalidate(10);
        if(glbCtx->psHwcProcs){
            glbCtx->psHwcProcs->invalidate(glbCtx->psHwcProcs);
        }
        ALOGD("switch to CVBS[0x%x]", mode);
        break;
    case DISP_OUTPUT_TYPE_LCD:
        if(glbCtx->mainDispId == LCD_USED){
            mFeClk = getFeClk(mode);
            mDeFps = 60;
        }
        arg[0] = hwDisp;
        ioctl(glbCtx->displayFd, DISP_CMD_LCD_DISABLE, arg);
        ioctl(glbCtx->displayFd, DISP_CMD_LCD_ENABLE, arg);
        //If the hwDisp is used as the LCD, then the width and height should be never change.
        psDeviceInfo->disp.id = hwDisp;
        psDeviceInfo->varDisplayWidth = ioctl(glbCtx->displayFd, DISP_CMD_GET_SCN_WIDTH, arg);
        psDeviceInfo->varDisplayHeight = ioctl(glbCtx->displayFd, DISP_CMD_GET_SCN_HEIGHT, arg);
        ALOGD("%s : mainDisp=%d, secDisp=%d, LCD width=%d, height=%d", __func__, glbCtx->mainDispId,
            glbCtx->secDispId, psDeviceInfo->varDisplayWidth, psDeviceInfo->varDisplayHeight);
        psDeviceInfo->displayType = DISP_OUTPUT_TYPE_LCD;
        psDeviceInfo->displayMode = DISP_TV_MOD_INVALID;
        psDeviceInfo->displayDPI_X = 160000;
        psDeviceInfo->displayDPI_Y = 160000;
        psDeviceInfo->displayVsyncP = 1000000000/60;
        invalidate(10);
        break;
    case DISP_OUTPUT_TYPE_VGA:
        //to do
        break;
    case DISP_OUTPUT_TYPE_NONE:
        if(psDeviceInfo->disp.id == INVALID_VALUE){
            return 0;
        }
        switch(psDeviceInfo->displayType){
        case DISP_OUTPUT_TYPE_HDMI:
            arg[0] = HDMI_USED;
            ioctl(glbCtx->displayFd, DISP_CMD_HDMI_DISABLE, (unsigned long)arg);
            break;
        case DISP_OUTPUT_TYPE_TV:
            arg[0] = CVBS_USED;
            ioctl(glbCtx->displayFd, DISP_CMD_TV_OFF, (unsigned long)arg);
            break;
        case DISP_OUTPUT_TYPE_LCD:
            arg[0] = LCD_USED;
            ioctl(glbCtx->displayFd, DISP_CMD_LCD_DISABLE, (unsigned long)arg);
            break;
        case DISP_OUTPUT_TYPE_VGA:
            //to do
            break;
        default:
            break;
        }
        break;
    }
    ALOGD("%s : disp=%d, mode=%d switch end.", __func__, hwDisp, mode);
    return 0;
}

int set3DModePerDevice(hwc_context_t *glbCtx,int disp,int new_mode){
    device_info_t *psDeviceInfo = &glbCtx->sunxiDevice[disp];
    int old_mode = psDeviceInfo->current3DMode;
    if(old_mode == new_mode || psDeviceInfo->disp.id == INVALID_VALUE){
        return 0;
    }
    //only the hdmi can set the 3D output mode.
    if(psDeviceInfo->displayType == DISP_OUTPUT_TYPE_HDMI){
        psDeviceInfo->current3DMode = new_mode;
        if(is3DMode(old_mode) != is3DMode(new_mode)){
            int hwDisp = psDeviceInfo->disp.id;
            int tv_mode = INVALID_VALUE;
            ALOGD("%s: old_mode[%d] --> new_mode[%d]", __func__, old_mode, new_mode);
            if(psDeviceInfo->current3DMode == DISPLAY_3D_LEFT_RIGHT_HDMI
                || psDeviceInfo->current3DMode == DISPLAY_3D_TOP_BOTTOM_HDMI
                || psDeviceInfo->current3DMode == DISPLAY_3D_DUAL_STREAM){
                tv_mode = DISP_TV_MOD_1080P_24HZ_3D_FP;
            }else if(psDeviceInfo->disp.id == glbCtx->mainDispId){
                tv_mode = glbCtx->mainDispMode;
            }else if(psDeviceInfo->disp.id == glbCtx->secDispId){
                tv_mode = glbCtx->secDispMode;
            }
            outputSwitch(hwDisp, DISP_OUTPUT_TYPE_HDMI, tv_mode);
        }
    }
    return 0;
}

//
int sun9iSet3DMode(int disp, int mode){
    for(int i = 0; i < gHwcContext.displayNum; i++){
        set3DModePerDevice(&gHwcContext, i, mode);
    }
    return 0;
}
int sun9iSetBacklightMode(int disp, int enable){
    hwc_context_t *glbCtx = &gHwcContext;
    //the HDMI and Tv can not set backlight.
    for(int i = 0; i < NUMBEROFDISPLAY; i++){
        if(glbCtx->sunxiDevice[i].disp.id == INVALID_VALUE){
            continue;
        }
        if(glbCtx->sunxiDevice[i].displayType != DISP_OUTPUT_TYPE_HDMI
            && glbCtx->sunxiDevice[i].displayType != DISP_OUTPUT_TYPE_TV
            && glbCtx->sunxiDevice[i].displayType != DISP_OUTPUT_TYPE_VGA){
            unsigned long arg[4] = {0};
            arg[0] = glbCtx->sunxiDevice[i].disp.id;
            switch(enable){
            case 1:
                return ioctl(glbCtx->displayFd, DISP_CMD_DRC_ENABLE, arg);
            case 0:
                return ioctl(glbCtx->displayFd, DISP_CMD_DRC_DISABLE, arg);
            }
        }
    }
    return 0;
}
int sun9iSetEnhanceMode(int disp, int enable){
    hwc_context_t *glbCtx = &gHwcContext;
    unsigned long arg[4] = {0};
    arg[0] = getHwDispId(disp);
    if(arg[0] == INVALID_VALUE){
        return 0;
    }
    switch(enable){
    case 1:
        return ioctl(glbCtx->displayFd, DISP_CMD_ENHANCE_ENABLE, arg);
    case 0:
        return ioctl(glbCtx->displayFd, DISP_CMD_ENHANCE_DISABLE, arg);
    }
    return 0;
}
int sun9iSetOutputMode(int disp, int type, int mode){
    hwc_context_t *glbCtx = &gHwcContext;
    device_info_t *psDeviceInfo = NULL;
    int hwDisp = INVALID_VALUE;
    int tmp = INVALID_VALUE;
    if((glbCtx->mainDispMode == mode && disp == HWC_DISPLAY_PRIMARY)
        || disp >= HWC_DISPLAY_VIRTUAL){
        return 0;
    }
    switch(type){
    case DISP_OUTPUT_TYPE_HDMI:
        hwDisp = HDMI_USED;
        break;
    case DISP_OUTPUT_TYPE_TV:
        hwDisp = CVBS_USED;
        break;
    case DISP_OUTPUT_TYPE_LCD:
        hwDisp = LCD_USED;
        break;
    default:
        return 0;
    }
    psDeviceInfo = hwcFindDevice(hwDisp);
    if(psDeviceInfo != NULL){
        switch(disp){
        case HWC_DISPLAY_PRIMARY:
            //close the pre mainDisp
            tmp = glbCtx->mainDispId;
            glbCtx->mainDispId = hwDisp;
            glbCtx->mainDispMode = mode;

            if(tmp != hwDisp && tmp != INVALID_VALUE){
                ALOGD("close the pre mainDisp %d", tmp);
                outputSwitch(tmp, DISP_OUTPUT_TYPE_NONE, 0);
            }
            //if the hwDisp is also used in second disp, we must remove the secDisp.
            if(glbCtx->secDispId == hwDisp){
                glbCtx->secDispId = INVALID_VALUE;
                if(glbCtx->psHwcProcs != NULL){
                    ALOGD("we must remove the secDisp %d", hwDisp);
                    glbCtx->psHwcProcs->hotplug(glbCtx->psHwcProcs, HWC_DISPLAY_EXTERNAL, 0);
                }
            }
            //open the cur mainDisp
            ALOGD("open the cur mainDisp %d", hwDisp);
            outputSwitch(hwDisp, type, mode);
            break;
        case HWC_DISPLAY_EXTERNAL:
            //If the hwDiso is also used in main disp, we do not set it as the secDisp.
            if(glbCtx->mainDispId== hwDisp){
                return 0;
            }
            //close the pre secDisp, and unplug it
            if(glbCtx->secDispId!= hwDisp && glbCtx->secDispId!= INVALID_VALUE){
                outputSwitch(hwDisp, DISP_OUTPUT_TYPE_NONE, 0);
            }
            if(glbCtx->psHwcProcs != NULL){
                glbCtx->psHwcProcs->hotplug(glbCtx->psHwcProcs, HWC_DISPLAY_EXTERNAL, 0);
            }
            glbCtx->secDispId = hwDisp;
            glbCtx->secDispMode = mode;
            //open the cur mainDisp
            outputSwitch(hwDisp, type, mode);
            glbCtx->secDispWidth = psDeviceInfo->varDisplayWidth;
            glbCtx->secDispHeight = psDeviceInfo->varDisplayHeight;
            //broadcast plugin message for surfaceflinger
            if(glbCtx->psHwcProcs != NULL){
                glbCtx->psHwcProcs->hotplug(glbCtx->psHwcProcs, HWC_DISPLAY_EXTERNAL, 1);
            }
        default:
            return 0;
        }
    }
    return 0;
}
int sun9iSetSaturation(int disp, int saturation){
    hwc_context_t *glbCtx = &gHwcContext;
    unsigned long arg[4] = {0};
    int ret = 0;
    arg[0] = getHwDispId(disp);
    if(arg[0] == INVALID_VALUE){
        return 0;
    }
    arg[1] = saturation;
    ret = ioctl(glbCtx->displayFd, DISP_CMD_SET_SATURATION, (unsigned long)arg);
    return ret;
}
int sun9iSetHue(int disp, int hue){
    hwc_context_t *glbCtx = &gHwcContext;
    unsigned long arg[4] = {0};
    int ret = 0;
    arg[0] = getHwDispId(disp);
    if(arg[0] == INVALID_VALUE){
        return 0;
    }
    arg[1] = hue;
    ret = ioctl(glbCtx->displayFd, DISP_CMD_SET_HUE, (unsigned long)arg);
    return ret;
}
int sun9iSetBright(int disp, int bright){
    hwc_context_t *glbCtx = &gHwcContext;
    unsigned long arg[4] = {0};
    int ret = 0;
    arg[0] = getHwDispId(disp);
    if(arg[0] == INVALID_VALUE){
        return 0;
    }
    arg[1] = bright;
    ret = ioctl(glbCtx->displayFd, DISP_CMD_SET_BRIGHT, (unsigned long)arg);
    return ret;
}
int sun9iSetContrast(int disp, int contrast){
    hwc_context_t *glbCtx = &gHwcContext;
    unsigned long arg[4] = {0};
    int ret = 0;
    arg[0] = getHwDispId(disp);
    if(arg[0] == INVALID_VALUE){
        return 0;
    }
    arg[1] = contrast;
    ret = ioctl(glbCtx->displayFd, DISP_CMD_SET_CONTRAST, (unsigned long)arg);
    return ret;
}
int sun9iSetMargin(int disp, int hpercent, int vpercent){
    ALOGE("sun9iSetMargin %d, %d, %d", disp, hpercent, vpercent);
    hwc_context_t *glbCtx = &gHwcContext;
    device_info_t *psDeviceInfo = NULL;
    for(int i = 0; i < NUMBEROFDISPLAY; i++){
        psDeviceInfo = &glbCtx->sunxiDevice[i];
        psDeviceInfo->displayPercentWT = (hpercent < MARGIN_MIN_PERCENT) ? MARGIN_MIN_PERCENT :
            ((hpercent > MARGIN_MAX_PERCENT) ? MARGIN_MAX_PERCENT : hpercent);
        psDeviceInfo->displayPercentHT = (vpercent < MARGIN_MIN_PERCENT) ? MARGIN_MIN_PERCENT :
            ((vpercent > MARGIN_MAX_PERCENT) ? MARGIN_MAX_PERCENT : vpercent);
    }
    return 0;
}
int sun9iIsSupportHdmiMode(int disp, int mode){
    hwc_context_t *glbCtx = &gHwcContext;
    unsigned long arg[4] = {0};
    arg[0] = HDMI_USED;
    arg[1] = mode;
    if(ioctl(glbCtx->displayFd, DISP_CMD_HDMI_SUPPORT_MODE, (unsigned long)arg)){
        return 1;
    }
    return 0;
}
int sun9iIsSupport3DMode(int disp){
    return sun9iIsSupportHdmiMode(disp, DISP_TV_MOD_1080P_24HZ_3D_FP);
}
int sun9iGetOutputType(int disp){
    hwc_context_t *glbCtx = &gHwcContext;
    device_info_t *psDeviceInfo = NULL;
    int hwDisp = INVALID_VALUE;
    for(int i = 0; i < NUMBEROFDISPLAY; i++){
        psDeviceInfo = &glbCtx->sunxiDevice[i];
        hwDisp = psDeviceInfo->disp.id;
        if(hwDisp == INVALID_VALUE){
            continue;
        }
        if((hwDisp == glbCtx->mainDispId && disp == HWC_DISPLAY_PRIMARY)
            || (hwDisp == glbCtx->secDispId && disp == HWC_DISPLAY_EXTERNAL)){
            return psDeviceInfo->displayType;
        }
    }
    return DISP_OUTPUT_TYPE_NONE;
}
int sun9iGetOutputMode(int disp){
    hwc_context_t *glbCtx = &gHwcContext;
    switch(disp){
    case HWC_DISPLAY_PRIMARY:
        return glbCtx->mainDispMode;
    case HWC_DISPLAY_EXTERNAL:
        return glbCtx->secDispMode;
    default:
        return INVALID_VALUE;
    }
}
int sun9iGetSaturation(int disp){
    hwc_context_t *glbCtx = &gHwcContext;
    unsigned long arg[4] = {0};
    arg[0] = getHwDispId(disp);
    if(arg[0] == INVALID_VALUE){
        return 0;
    }
    arg[1] = 0;
    return ioctl(glbCtx->displayFd, DISP_CMD_GET_SATURATION, (unsigned long)arg);
}
int sun9iGetHue(int disp){
    hwc_context_t *glbCtx = &gHwcContext;
    unsigned long arg[4] = {0};
    arg[0] = getHwDispId(disp);
    if(arg[0] == INVALID_VALUE){
        return 0;
    }
    arg[1] = 0;
    return ioctl(glbCtx->displayFd, DISP_CMD_GET_HUE, (unsigned long)arg);
}
int sun9iGetBright(int disp){
    hwc_context_t *glbCtx = &gHwcContext;
    unsigned long arg[4] = {0};
    arg[0] = getHwDispId(disp);
    if(arg[0] == INVALID_VALUE){
        return 0;
    }
    arg[1] = 0;
    return ioctl(glbCtx->displayFd, DISP_CMD_GET_BRIGHT, (unsigned long)arg);
}
int sun9iGetContrast(int disp){
    hwc_context_t *glbCtx = &gHwcContext;
    unsigned long arg[4] = {0};
    arg[0] = getHwDispId(disp);
    if(arg[0] == INVALID_VALUE){
        return 0;
    }
    arg[1] = 0;
    return ioctl(glbCtx->displayFd, DISP_CMD_GET_CONTRAST, (unsigned long)arg);
}
int sun9iGetMarginW(int disp){
    hwc_context_t *glbCtx = &gHwcContext;
    //As all the margin size of displays are the same , we can just returm
    //one of them.
    device_info_t *psDeviceInfo = &glbCtx->sunxiDevice[0];
    return (int)(psDeviceInfo->displayPercentWT);
}
int sun9iGetMarginH(int disp){
    hwc_context_t *glbCtx = &gHwcContext;
    device_info_t *psDeviceInfo = &glbCtx->sunxiDevice[0];
    return (int)(psDeviceInfo->displayPercentHT);
}

int sun9iVsyncEnable(int disp, int enable){
    hwc_context_t *glbCtx = &gHwcContext;
    unsigned long arg[4] = {0};
    for(int i = 0; i < NUMBEROFDISPLAY; i++){
        device_info_t *psDeviceInfo = &glbCtx->sunxiDevice[i];
        if(psDeviceInfo->disp.id == INVALID_VALUE
            || psDeviceInfo->disp.id != glbCtx->mainDispId){
            continue;
        }
        arg[0] = psDeviceInfo->disp.id;
        arg[1] = !!enable;
        ioctl(glbCtx->displayFd, DISP_CMD_VSYNC_EVENT_EN, (unsigned long)arg);
        psDeviceInfo->vsyncEnable = (!!enable);
        ALOGV("%s : hwc vsync %d", __func__, psDeviceInfo->vsyncEnable);
        return 0;
    }
    return -EINVAL;
}

static int getTvMode4SysResolution(void){
    int tvmode = getValueFromProperty(SYSTEM_RSL_PROPERTY);
    switch(tvmode){
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
            break;
        default:
            tvmode = DISP_TV_MOD_1080P_60HZ;
            break;
    }
    return tvmode;
}

int initDisplayDeviceInfo(hwc_context_t *context){
    int refreshRate;
    int arg[4] = {0};
    int i;
    int tvmode4sysrsl;
    ALOGD("#########The MainDisplay:%d ", context->sunxiDevice[0].disp.id);
    for(i = 0; i < context->displayNum; i++){
        device_info_t *deviceInfo = &context->sunxiDevice[i];
        if(deviceInfo->disp.id != INVALID_VALUE){
            switch(deviceInfo->displayType){
            case DISP_OUTPUT_TYPE_LCD:
                arg[0] = deviceInfo->disp.id;
                refreshRate = 60;
                deviceInfo->displayDPI_X = 160000;
                deviceInfo->displayDPI_Y = 160000;
                deviceInfo->displayVsyncP = 1000000000 / refreshRate;
                deviceInfo->displayPercentHT = 100;
                deviceInfo->displayPercentWT = 100;
                deviceInfo->current3DMode = DISPLAY_2D_ORIGINAL;
                deviceInfo->varDisplayWidth = ioctl(context->displayFd, DISP_CMD_GET_SCN_WIDTH, arg);
                deviceInfo->varDisplayHeight = ioctl(context->displayFd, DISP_CMD_GET_SCN_HEIGHT, arg);
                if(LCD_USED == context->mainDispId){
                    context->mainDispWidth = deviceInfo->varDisplayWidth;
                    context->mainDispHeight = deviceInfo->varDisplayHeight;
                    ALOGD("LCD, mainDispWidth is %d, mainDispHeight is %d", context->mainDispWidth, context->mainDispHeight);
                }else if(LCD_USED == context->secDispId){
                    context->secDispWidth = deviceInfo->varDisplayWidth;
                    context->secDispHeight = deviceInfo->varDisplayHeight;
                    ALOGD("LCD, secDispWidth is %d, secDispHeight is %d", context->secDispWidth, context->secDispHeight);
                }
                deviceInfo->vsyncEnable = 1;
                break;
            case DISP_OUTPUT_TYPE_HDMI:
                arg[0] = deviceInfo->disp.id;
                deviceInfo->displayType = DISP_OUTPUT_TYPE_HDMI;
                deviceInfo->displayMode = ioctl(context->displayFd, DISP_CMD_HDMI_GET_MODE, arg);
                deviceInfo->varDisplayWidth = ioctl(context->displayFd, DISP_CMD_GET_SCN_WIDTH, arg);
                deviceInfo->varDisplayHeight = ioctl(context->displayFd, DISP_CMD_GET_SCN_HEIGHT, arg);
                mDeFps = getInfoOfMode(mTvPara, mModeNum,deviceInfo->displayMode, REFRESHRATE);
                mFeClk = getFeClk(deviceInfo->displayMode);
                if(context->mainDispId == HDMI_USED){
                    if(0 == isDisplayP2P()){
                        //not point to point
                        tvmode4sysrsl = getTvMode4SysResolution();
                        context->mainDispWidth = getInfoOfMode(mTvPara, mModeNum, tvmode4sysrsl, WIDTH);
                        context->mainDispHeight = getInfoOfMode(mTvPara, mModeNum, tvmode4sysrsl, HEIGHT);
                        ALOGD("HDMI tvMode is %x, mainDispWidth is %d, mainDispHeight is %d", tvmode4sysrsl, context->mainDispWidth, context->mainDispHeight);
                    }else{
                        context->mainDispWidth = deviceInfo->varDisplayWidth;
                        context->mainDispHeight = deviceInfo->varDisplayHeight;
                        ALOGD("HDMI tvMode is %x, secDispWidth is %d, secDispHeight is %d", tvmode4sysrsl, context->secDispWidth, context->secDispHeight);
                    }
                }else if(context->secDispId == HDMI_USED){
                    context->secDispWidth = deviceInfo->varDisplayWidth;
                    context->secDispHeight = deviceInfo->varDisplayHeight;
                }
                deviceInfo->displayDPI_X = 213000;
                deviceInfo->displayDPI_Y = 213000;
                deviceInfo->displayVsyncP = 1000000000 / getInfoOfMode(mTvPara, mModeNum, deviceInfo->displayMode, REFRESHRATE);
                deviceInfo->current3DMode = DISPLAY_2D_ORIGINAL;
                if(-1 == getDispMarginFromFile(&deviceInfo->displayPercentWT,
                            &deviceInfo->displayPercentHT)){
                    deviceInfo->displayPercentWT = MARGIN_DEFAULT_PERCENT_WIDTH;
                    deviceInfo->displayPercentHT = MARGIN_DEFAULT_PERCENT_HEIGHT;
                }
                deviceInfo->vsyncEnable = 1;
                break;
            case DISP_OUTPUT_TYPE_TV:
                arg[0] = deviceInfo->disp.id;
                deviceInfo->displayType = DISP_OUTPUT_TYPE_TV;
                deviceInfo->displayMode = ioctl(context->displayFd, DISP_CMD_TV_GET_MODE, arg);
                deviceInfo->varDisplayWidth = ioctl(context->displayFd, DISP_CMD_GET_SCN_WIDTH, arg);
                deviceInfo->varDisplayHeight = ioctl(context->displayFd, DISP_CMD_GET_SCN_HEIGHT, arg);
                //Caution: do not use point to point in CVBS mode, because it make ui see worst.
                if(context->mainDispId == CVBS_USED){
                    tvmode4sysrsl = getTvMode4SysResolution();
                    context->mainDispHeight = getInfoOfMode(mTvPara, mModeNum, tvmode4sysrsl, HEIGHT);
                    context->mainDispWidth = getInfoOfMode(mTvPara, mModeNum, tvmode4sysrsl, WIDTH);
                    ALOGD("CVBS tvMode is %x, mainDispWidth is %d, mainDispHeight is %d", tvmode4sysrsl, context->mainDispWidth, context->mainDispHeight);
                }
                else if(context->secDispId== CVBS_USED){
                    context->secDispWidth = deviceInfo->varDisplayWidth;
                    context->secDispHeight = deviceInfo->varDisplayHeight;
                    ALOGD("CVBS tvMode is %x, secDispWidth is %d, secDispHeight is %d", tvmode4sysrsl, context->secDispWidth, context->secDispHeight);
                }
                mDeFps = getInfoOfMode(mTvPara, mModeNum, deviceInfo->displayMode, REFRESHRATE);
                mFeClk = getFeClk(deviceInfo->displayMode);
                deviceInfo->displayDPI_X = 213000;
                deviceInfo->displayDPI_Y = 213000;
                deviceInfo->displayVsyncP = 1000000000/getInfoOfMode(mTvPara, mModeNum, deviceInfo->displayMode,REFRESHRATE);
                deviceInfo->current3DMode = DISPLAY_2D_ORIGINAL;
                if(-1 == getDispMarginFromFile(&deviceInfo->displayPercentWT,
                        &deviceInfo->displayPercentHT)){
                    deviceInfo->displayPercentWT = MARGIN_DEFAULT_PERCENT_WIDTH;
                    deviceInfo->displayPercentHT = MARGIN_DEFAULT_PERCENT_HEIGHT;
                }
                deviceInfo->vsyncEnable = 1;
                break;
            case DISP_OUTPUT_TYPE_VGA:
            default:
                ALOGD("not support type");
                continue;
            }
        }
    }
    return 0;
}

void *hotplugThreadWrapper(void *priv){
    bool hdmiPlug = false;
    bool state = false;
    int hdmiFd;
    char val;
    bool first = true;
    while(0){
        usleep(500000);
        //check hdmi hotplug state
        hdmiFd = open("/sys/class/switch/hdmi/state", O_RDONLY);
        if (hdmiFd){
            if (read(hdmiFd, &val, 1) == 1 && val == '1'){
                hdmiPlug = true;
            }else{
                hdmiPlug = false;
            }
            close(hdmiFd);
        }
        if(sun9iGetOutputType(HWC_DISPLAY_PRIMARY) != DISP_OUTPUT_TYPE_HDMI &&
            hdmiPlug == true){
            ALOGD("Switch to HDMI 1080p");
                sun9iSetOutputMode(HWC_DISPLAY_PRIMARY, DISP_OUTPUT_TYPE_HDMI,
                    DISP_TV_MOD_1080P_60HZ);
        }
    }
    return NULL;
}

int sun9iInit(hwc_context_t *context){
    memset(context, 0, sizeof(hwc_context_t));
    //init the global operation fd.
    context->displayFd = open("/dev/disp", O_RDWR);
    if(context->displayFd < 0){
        ALOGE("%s : Failed to open disp device, ret:%d, errno: %d", __func__, context->displayFd, errno);
    }
    context->ionFd = open("/dev/ion", O_RDWR);
    if(context->ionFd < 0){
        ALOGE("%s : Failed to open ion device, ret:%d, errno: %d", __func__, context->ionFd, errno);
    }
    mG2dFd = open("/dev/g2d", O_RDWR);
    if(mG2dFd < 0){
        ALOGE("Failed to open  g2d device, ret:%d, errno:%d", mG2dFd, errno);
    }
    //init display number
    context->displayNum = CHANNEL_NUM;
    //init sunxidevice
    context->sunxiDevice = (device_info_t*)calloc(context->displayNum, sizeof(device_info_t));
    memset(context->sunxiDevice, 0, context->displayNum*sizeof(device_info_t));
    for(int i = 0; i < context->displayNum; i++){
        context->sunxiDevice[i].disp.id = INVALID_VALUE;
        if(-1 == getDispMarginFromFile(&(context->sunxiDevice[i].displayPercentWT),
                    &(context->sunxiDevice[i].displayPercentHT))){
            context->sunxiDevice[i].displayPercentWT = MARGIN_DEFAULT_PERCENT_WIDTH;
            context->sunxiDevice[i].displayPercentHT = MARGIN_DEFAULT_PERCENT_HEIGHT;
        }
    }

    //init others
    context->screenRadio = 1;
    unsigned long arg[4] = {0};
    arg[0] = HDMI_USED;
    int disp1 = ioctl(context->displayFd, DISP_CMD_GET_OUTPUT_TYPE, arg);
    int mode1 = ioctl(context->displayFd, DISP_CMD_HDMI_GET_MODE, arg);
    arg[0] = CVBS_USED;
    int disp0 = ioctl(context->displayFd, DISP_CMD_GET_OUTPUT_TYPE, arg);
    int mode0 = ioctl(context->displayFd, DISP_CMD_TV_GET_MODE, arg);
    arg[0] = LCD_USED;
    int disp3 = ioctl(context->displayFd, DISP_CMD_GET_OUTPUT_TYPE, arg);
    //init the main display device
    device_info_t *deviceInfo0 = &context->sunxiDevice[0];
    device_info_t *deviceInfo1 = &context->sunxiDevice[1];
    context->mainDispId = INVALID_VALUE;
    if(disp1 == DISP_OUTPUT_TYPE_HDMI){
        deviceInfo0->displayType = DISP_OUTPUT_TYPE_HDMI;
        deviceInfo0->disp.id = HDMI_USED;
        context->mainDispId = HDMI_USED;
        context->mainDispMode = mode1;
    }else if(disp0 == DISP_OUTPUT_TYPE_TV){
        deviceInfo0->displayType = DISP_OUTPUT_TYPE_TV;
        deviceInfo0->disp.id = CVBS_USED;
        context->mainDispId = CVBS_USED;
        context->mainDispMode = mode0;
    }else if(disp3 == DISP_OUTPUT_TYPE_LCD){
        deviceInfo0->displayType = DISP_OUTPUT_TYPE_LCD;
        deviceInfo0->disp.id = LCD_USED;
        context->mainDispId = LCD_USED;
        context->mainDispMode = DISP_TV_MOD_INVALID;
    }else{
        deviceInfo0->displayType = DISP_OUTPUT_TYPE_NONE;
        deviceInfo0->disp.id = INVALID_VALUE;
        context->mainDispId = INVALID_VALUE;
        context->mainDispMode = INVALID_VALUE;
    }
    //init the second display device
    context->secDispId = INVALID_VALUE;
    if(context->mainDispId != CVBS_USED && disp0 == DISP_OUTPUT_TYPE_TV){
        deviceInfo1->displayType = DISP_OUTPUT_TYPE_TV;
        deviceInfo1->disp.id = CVBS_USED;
        context->secDispId = CVBS_USED;
        context->secDispMode = mode0;
    }else if(context->mainDispId != LCD_USED && disp3 == DISP_OUTPUT_TYPE_LCD){
        deviceInfo1->displayType = DISP_OUTPUT_TYPE_LCD;
        deviceInfo1->disp.id = LCD_USED;
        context->secDispId = LCD_USED;
        context->secDispMode = DISP_TV_MOD_INVALID;
    }
    initDisplayDeviceInfo(context);
    ALOGD( "#### mainDisp is %d, secDisp is %d", context->mainDispId, context->secDispId);
    ALOGD( "#### Type:%d  DisplayMode:%d PrimaryDisplay:%d  DisplayWidth:%d  DisplayHeight:%d ",
		context->sunxiDevice[0].displayType, context->sunxiDevice[0].displayMode,
		context->sunxiDevice[0].disp.id, context->sunxiDevice[0].varDisplayWidth,
		context->sunxiDevice[0].varDisplayHeight);

    //for tmp
    pthread_t hotplugThread;
    pthread_create(&hotplugThread, NULL, hotplugThreadWrapper, NULL);

    //if kernel does not init the device, the hwc force to do it.
    if(context->mainDispId == INVALID_VALUE){
        sun9iSetOutputMode(HWC_DISPLAY_PRIMARY, DISP_OUTPUT_TYPE_HDMI,
                    DISP_TV_MOD_1080P_60HZ);
    }
	return 0;
}

device_opr_t sun9iDeviceOpr={
    init : sun9iInit,
    set3DMode : sun9iSet3DMode,
    setBacklightMode : sun9iSetBacklightMode,
    setEnhanceMode : sun9iSetEnhanceMode,
    setOutputMode : sun9iSetOutputMode,
    setSaturation : sun9iSetSaturation,
    setHue : sun9iSetHue,
    setBright : sun9iSetBright,
    setContrast : sun9iSetContrast,
    setMargin : sun9iSetMargin,
    isSupportHdmiMode : sun9iIsSupportHdmiMode,
    isSupport3DMode : sun9iIsSupport3DMode,
    getOutputType : sun9iGetOutputType,
    getOutputMode : sun9iGetOutputMode,
    getSaturation : sun9iGetSaturation,
    getHue : sun9iGetHue,
    getBright : sun9iGetBright,
    getContrast : sun9iGetContrast,
    getMarginW : sun9iGetMarginW,
    getMarginH : sun9iGetMarginH,
    vsyncEnable : sun9iVsyncEnable,
};