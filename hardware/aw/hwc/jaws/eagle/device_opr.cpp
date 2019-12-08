#include "../hwc.h"
#include "sunxi_display2.h"
#include "private.h"
#include "../common/utils.h"

//internal functions
int outputSwitch(int hwDisp, int type, int mode){
    hwc_context_t *glbCtx = &gHwcContext;
    unsigned long arg[4] = {0};
    device_info_t *psDeviceInfo = NULL;
    psDeviceInfo = hwcFindDevice(hwDisp);
    int number = sizeof(sun8iTvPara)/sizeof(tv_para_t);
    sun8i_disp_private_t *privateData = (sun8i_disp_private_t*)(psDeviceInfo->disp.privateData);
    ALOGD("%s : hwDisp=%d, type=%d, mode=%d", __func__, hwDisp, type, mode);
    switch(type){
    case DISP_OUTPUT_TYPE_HDMI:
        psDeviceInfo->disp.id = hwDisp;
        psDeviceInfo->varDisplayWidth = getInfoOfMode(sun8iTvPara, number, mode, WIDTH);
        psDeviceInfo->varDisplayHeight = getInfoOfMode(sun8iTvPara, number, mode, HEIGHT);
        psDeviceInfo->displayType = DISP_OUTPUT_TYPE_HDMI;
        psDeviceInfo->displayMode = mode;
        psDeviceInfo->displayDPI_X = 213000;
        psDeviceInfo->displayDPI_Y = 213000;
        psDeviceInfo->displayVsyncP = 1000000000/getInfoOfMode(sun8iTvPara, number,mode, REFRESHRATE);
        privateData->hwChannelNum = hwDisp?2:4;
        privateData->layerNumOfCh = NUMLAYEROFCHANNEL;
        privateData->videoChNum = 1;

        arg[0] = hwDisp;
        ioctl(glbCtx->displayFd, DISP_BLANK, (unsigned long)arg);
        arg[0] = hwDisp;
        arg[1] = DISP_OUTPUT_TYPE_HDMI;
        arg[2] = mode;
        ioctl(glbCtx->displayFd, DISP_DEVICE_SWITCH, (unsigned long)arg);
        glbCtx->psHwcProcs->invalidate(glbCtx->psHwcProcs);
        ALOGD("%s : switch to HDMI[0x%x] finish!", __func__, mode);
        break;
    case DISP_OUTPUT_TYPE_TV:
        psDeviceInfo->disp.id = hwDisp;
        psDeviceInfo->varDisplayWidth = getInfoOfMode(sun8iTvPara, number, mode, WIDTH);
        psDeviceInfo->varDisplayHeight = getInfoOfMode(sun8iTvPara, number, mode, HEIGHT);
        psDeviceInfo->displayType = DISP_OUTPUT_TYPE_TV;
        psDeviceInfo->displayMode = mode;
        psDeviceInfo->displayDPI_X = 213000;
        psDeviceInfo->displayDPI_Y = 213000;
        psDeviceInfo->displayVsyncP = 1000000000/getInfoOfMode(sun8iTvPara, number, mode, REFRESHRATE);
        privateData->hwChannelNum = hwDisp?2:4;
        privateData->layerNumOfCh = NUMLAYEROFCHANNEL;
        privateData->videoChNum = 1;

        arg[0] = hwDisp;
        ioctl(glbCtx->displayFd, DISP_BLANK, (unsigned long)arg);
        arg[0] = hwDisp;
        arg[1] = DISP_OUTPUT_TYPE_TV;
        arg[2] = mode;
        ioctl(glbCtx->displayFd, DISP_DEVICE_SWITCH, (unsigned long)arg);
        glbCtx->psHwcProcs->invalidate(glbCtx->psHwcProcs);
        ALOGD("%s : switch to CVBS[0x%x] finish!", __func__, mode);
        break;
    case DISP_OUTPUT_TYPE_LCD:
        arg[0] = hwDisp;
        ioctl(glbCtx->displayFd, DISP_LCD_DISABLE, arg);
        ioctl(glbCtx->displayFd, DISP_LCD_ENABLE, arg);
        //If the hwDisp is used as the LCD, then the width and height should be never change.
        psDeviceInfo->disp.id = hwDisp;
        psDeviceInfo->varDisplayWidth = ioctl(glbCtx->displayFd, DISP_GET_SCN_WIDTH, arg);
        psDeviceInfo->varDisplayHeight = ioctl(glbCtx->displayFd, DISP_GET_SCN_HEIGHT, arg);
        psDeviceInfo->displayType = DISP_OUTPUT_TYPE_LCD;
        psDeviceInfo->displayMode = INVALID_VALUE;
        psDeviceInfo->displayDPI_X = 160000;
        psDeviceInfo->displayDPI_Y = 160000;
        psDeviceInfo->displayVsyncP = 1000000000/60;
        privateData->hwChannelNum = hwDisp?2:4;
        privateData->layerNumOfCh = NUMLAYEROFCHANNEL;
        privateData->videoChNum = 1;
        glbCtx->psHwcProcs->invalidate(glbCtx->psHwcProcs);
        ALOGD("%s : switch to disp[%d] LCD[%d, %d] finish!", __func__, hwDisp, psDeviceInfo->varDisplayWidth,
            psDeviceInfo->varDisplayHeight);
        break;
    case DISP_OUTPUT_TYPE_VGA:
        break;
    case DISP_OUTPUT_TYPE_NONE:
        if(psDeviceInfo->disp.id == INVALID_VALUE){
            return 0;
        }
        arg[0] = hwDisp;
        ioctl(glbCtx->displayFd, DISP_BLANK, (unsigned long)arg);
        break;
    }
    glbCtx->detectError = 0;
    return 0;
}

int set3DModePerDevice(hwc_context_t *glbCtx,int disp,int new_mode){
    device_info_t *psDeviceInfo = &glbCtx->sunxiDevice[disp];
    int old_mode = psDeviceInfo->current3DMode;
    if(old_mode == new_mode || psDeviceInfo->disp.id == INVALID_VALUE){
        return 0;
    }
    if(new_mode == DISPLAY_2D_ORIGINAL){
        glbCtx->canForceGpuCom = 1;
    }else{
        //we must use display engine to show 3D video
        glbCtx->canForceGpuCom = 0;
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
int sun8iSet3DMode(int disp, int mode){
    hwc_context_t *glbCtx = &gHwcContext;
    for(int i = 0; i < glbCtx->displayNum; i++){
        set3DModePerDevice(glbCtx, i, mode);
    }
    return 0;
}
int sun8iSetBacklightMode(int disp, int enable){
    hwc_context_t *glbCtx = &gHwcContext;
    for(int i = 0; i < glbCtx->displayNum; i++){
        if(glbCtx->sunxiDevice[i].disp.id == INVALID_VALUE){
            continue;
        }
        if(glbCtx->sunxiDevice[i].displayType == DISP_OUTPUT_TYPE_LCD){
            unsigned long arg[4] = {0};
            arg[0] = glbCtx->sunxiDevice[i].disp.id;
            switch(enable){
            case 1:
                return ioctl(glbCtx->displayFd, DISP_SMBL_ENABLE, arg);
            case 0:
                return ioctl(glbCtx->displayFd, DISP_SMBL_DISABLE, arg);
            }
        }
    }
    return 0;
}
int sun8iSetEnhanceMode(int disp, int enable){
    hwc_context_t *glbCtx = &gHwcContext;
    device_info_t *psDeviceInfo = NULL;
    for(int i = 0; i < glbCtx->displayNum; i++){
        psDeviceInfo = &glbCtx->sunxiDevice[i];
        sun8i_disp_private_t *privateData = (sun8i_disp_private_t *)(psDeviceInfo->disp.privateData);
        privateData->enhanceMode = enable?true:false;
    }
    return 0;
}
/** disp : primary display device or second display device.
*/
int sun8iSetOutputMode(int disp, int type, int mode){
    hwc_context_t *glbCtx = &gHwcContext;
    device_info_t *psDeviceInfo = NULL;
    int hwDisp = INVALID_VALUE;
    int tmp = INVALID_VALUE;
    if((glbCtx->mainDispMode == mode && disp == HWC_DISPLAY_PRIMARY)
        || disp >= HWC_DISPLAY_VIRTUAL){
        return 0;
    }
    //choose the display channel, as for H8, the main display device only use the
    //display channel 0, and the second display device use the display channel 1
    switch(disp){
    case HWC_DISPLAY_PRIMARY:
        hwDisp = DISP_CHN_0;
        psDeviceInfo = hwcFindDevice(hwDisp);
        tmp = glbCtx->mainDispId;
        glbCtx->mainDispId = hwDisp;
        glbCtx->mainDispMode = mode;
        //close the pre mainDisp
        if(tmp != hwDisp && tmp != INVALID_VALUE){
            outputSwitch(tmp, DISP_OUTPUT_TYPE_NONE, 0);
        }
        //if the hwDisp is also used in second disp, we must remove the secDisp.
        if(glbCtx->secDispId == hwDisp){
            glbCtx->secDispId = INVALID_VALUE;
            if(glbCtx->psHwcProcs != NULL){
                glbCtx->psHwcProcs->hotplug(glbCtx->psHwcProcs, HWC_DISPLAY_EXTERNAL, 0);
            }
        }
        //open the cur mainDisp
        outputSwitch(hwDisp, type, mode);
        break;
    case HWC_DISPLAY_EXTERNAL:
        hwDisp = DISP_CHN_1;
        //if the hwDisp is also used in main disp ,we do not set it as the secDisp.
        if(glbCtx->mainDispId == hwDisp){
            return 0;
        }
        //close the pre secDisp , and unplug it
        if(glbCtx->secDispId != hwDisp && glbCtx->secDispId != INVALID_VALUE){
            outputSwitch(glbCtx->secDispId, DISP_OUTPUT_TYPE_NONE, 0);
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
        break;
    }
    return 0;
}
int sun8iSetSaturation(int disp, int saturation){
    return 0;
}
int sun8iSetHue(int disp, int hue){
    return 0;
}
int sun8iSetBright(int disp, int bright){
    return 0;
}
int sun8iSetContrast(int disp, int contrast){
    return 0;
}
int sun8iSetMargin(int disp, int hpercent, int vpercent){
    setAllDeviceMargin(hpercent, vpercent);
    return 0;
}
int sun8iIsSupportHdmiMode(int disp, int mode){
    hwc_context_t *glbCtx = &gHwcContext;
    unsigned long arg[4] = {0};
    for(int i = 0; i < glbCtx->displayNum; i++){
        arg[0] = i;
        arg[1] = mode;
        if(ioctl(glbCtx->displayFd, DISP_HDMI_SUPPORT_MODE, (unsigned long)arg)){
            return 1;
        }
    }
    return 0;
}
int sun8iIsSupport3DMode(int disp){
    return sun8iIsSupportHdmiMode(0, DISP_TV_MOD_1080P_24HZ_3D_FP);
}
int sun8iGetOutputType(int disp){
    return deviceGetOutputType(disp);
}
int sun8iGetOutputMode(int disp){
    return deviceGetOutputMode(disp);
}
int sun8iGetSaturation(int disp){
    return 0;
}
int sun8iGetHue(int disp){
    return 0;
}
int sun8iGetBright(int disp){
    return 0;
}
int sun8iGetContrast(int disp){
    return 0;
}
int sun8iGetMarginW(int disp){
    return deviceGetMarginW();
}
int sun8iGetMarginH(int disp){
    return deviceGetMarginH();
}

int sun8iVsyncEnable(int dispChn, int enable){
    hwc_context_t *glbCtx = &gHwcContext;
    unsigned long arg[4] = {0};
    arg[0] = dispChn;
    arg[1] = enable;
    return ioctl(glbCtx->displayFd, DISP_VSYNC_EVENT_EN, (unsigned long)arg);
}

device_opr_t sun8iDeviceOpr={
    set3DMode : sun8iSet3DMode,
    setBacklightMode : sun8iSetBacklightMode,
    setEnhanceMode : sun8iSetEnhanceMode,
    setOutputMode : sun8iSetOutputMode,
    setSaturation : sun8iSetSaturation,
    setHue : sun8iSetHue,
    setBright : sun8iSetBright,
    setContrast : sun8iSetContrast,
    setMargin : sun8iSetMargin,
    isSupportHdmiMode : sun8iIsSupportHdmiMode,
    isSupport3DMode : sun8iIsSupport3DMode,
    getOutputType : sun8iGetOutputType,
    getOutputMode : sun8iGetOutputMode,
    getSaturation : sun8iGetSaturation,
    getHue : sun8iGetHue,
    getBright : sun8iGetBright,
    getContrast : sun8iGetContrast,
    getMarginW : sun8iGetMarginW,
    getMarginH : sun8iGetMarginH,
    vsyncEnable : sun8iVsyncEnable,
};
