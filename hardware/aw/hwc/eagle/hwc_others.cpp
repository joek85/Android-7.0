
#include "hwc.h"

#define MARGIN_MIN_PERCENT 50
#define MARGIN_MAX_PERCENT 100

int is3DMode(__display_3d_mode mode)
{
    switch(mode)
    {
    case DISPLAY_3D_LEFT_RIGHT_HDMI:
    case DISPLAY_3D_TOP_BOTTOM_HDMI:
    case DISPLAY_3D_DUAL_STREAM:
        return 1;
    default:
        return 0;
    }
}

static int _hwc_device_set_3d_mode_per_display(int disp, __display_3d_mode new_mode)
{
    SUNXI_hwcdev_context_t *Globctx = &gSunxiHwcDevice;
    DisplayInfo   *PsDisplayInfo = &Globctx->SunxiDisplay[disp];
    __display_3d_mode old_mode = PsDisplayInfo->Current3DMode;

    if(old_mode == new_mode)
    {
        return 0;
    }
    if(PsDisplayInfo->VirtualToHWDisplay != -EINVAL)
    {
        if(new_mode == DISPLAY_2D_ORIGINAL)
        {
            Globctx->CanForceGPUCom = 1;
        }else{
            Globctx->CanForceGPUCom = 0;
        }

        PsDisplayInfo->Current3DMode = new_mode;
        if((PsDisplayInfo->DisplayType == DISP_OUTPUT_TYPE_HDMI)
            && (is3DMode(old_mode) != is3DMode(new_mode)))
        {
            ALOGD("###set 3D mode: old_mode[%d] --> new_mode[%d]###", old_mode, new_mode);
            hwcOutputSwitch(PsDisplayInfo, DISP_OUTPUT_TYPE_HDMI,
                is3DMode(new_mode) ? DISP_TV_MOD_1080P_24HZ_3D_FP : PsDisplayInfo->setDispMode);
        }
        if(PsDisplayInfo->DisplayType == DISP_OUTPUT_TYPE_TV)
        {
            if(DISPLAY_3D_LEFT_RIGHT_HDMI == new_mode)
            {
                PsDisplayInfo->Current3DMode = DISPLAY_2D_LEFT;
            }
            else if(DISPLAY_3D_TOP_BOTTOM_HDMI == new_mode)
            {
                PsDisplayInfo->Current3DMode = DISPLAY_2D_TOP;
            }
            else if(DISPLAY_3D_DUAL_STREAM == new_mode)
            {
                PsDisplayInfo->Current3DMode = DISPLAY_2D_DUAL_STREAM;
            }
        }
    }
    return 0;
}

int _hwc_device_set_3d_mode(int disp, __display_3d_mode mode)
{
    SUNXI_hwcdev_context_t *Globctx = &gSunxiHwcDevice;
    int ret = 0;
    int i = 0;
    for( i = 0; i < Globctx->NumberofDisp; i++)
    {
        ret = _hwc_device_set_3d_mode_per_display(i, mode);
    }
    return 0;
}


int _hwc_device_set_backlight(int disp, int on_off,bool half)
{
    SUNXI_hwcdev_context_t *Globctx = &gSunxiHwcDevice;
    unsigned long arg[4]={0};
    disp_rect window;
    if(disp == HWC_DISPLAY_PRIMARY && Globctx->SunxiDisplay[disp].DisplayType != DISP_OUTPUT_TYPE_HDMI)
    {
        arg[0] = Globctx->SunxiDisplay[disp].VirtualToHWDisplay;
        window.x = 0;
        window.y = 0;
        window.width = ioctl(Globctx->DisplayFd,DISP_GET_SCN_WIDTH,arg);
        window.height = ioctl(Globctx->DisplayFd,DISP_GET_SCN_HEIGHT,arg);
        arg[1] = (unsigned long)&window;
        if(half)
        {
            if(on_off)
            {
                window.width > window.height ? window.width/= 2 : window.height /= 2;
            }
            ioctl(Globctx->DisplayFd, DISP_SMBL_SET_WINDOW,arg);
            return 0;
        }
        if(on_off == 1 )
        {
            ioctl(Globctx->DisplayFd, DISP_SMBL_ENABLE,arg);
        }else{
            ioctl(Globctx->DisplayFd, DISP_SMBL_DISABLE,arg);
        }

    }
    return 0;
}

int _hwc_device_set_enhancemode(int disp, bool on_off,bool half)
{
    SUNXI_hwcdev_context_t *Globctx= &gSunxiHwcDevice;
    DisplayInfo   *psDisplayInfo;
    int i;
    for(i = 0; i < Globctx->NumberofDisp; i++)
    {
        psDisplayInfo = &Globctx->SunxiDisplay[i];
        if(0 == psDisplayInfo->VirtualToHWDisplay)
        {
            psDisplayInfo->ehancemode = (0 == on_off) ? 0 : ((0 == half) ? 1 : 2);
            ALOGD("set enhancemode %d", psDisplayInfo->ehancemode);
        }
    }
    return 0;
}

int _hwc_device_set_output_mode(int disp, int out_type, int out_mode)
{
    SUNXI_hwcdev_context_t *Globctx = &gSunxiHwcDevice;
    DisplayInfo	 *psDisplayInfo0 = &Globctx->SunxiDisplay[HWC_DISPLAY_PRIMARY];
    DisplayInfo	 *psDisplayInfo1 = &Globctx->SunxiDisplay[HWC_DISPLAY_EXTERNAL];
    unsigned long arg[4] = {0};
    int ret = -1;

    if((0 != Globctx->ban4k)
      && (DISP_OUTPUT_TYPE_HDMI == out_type)
      && (DISP_TV_MOD_3840_2160P_30HZ <= out_mode))
    {
        return -1;
    }

    if(HWC_DISPLAY_PRIMARY == disp)
    {
        int dispExternalOpened = psDisplayInfo1->VirtualToHWDisplay;
        if(0 == out_type)
        {
            return -1;
        }
        if(out_type == psDisplayInfo0->DisplayType)
        {
            if(out_mode != psDisplayInfo0->DisplayMode)
            {
                ALOGD("switch on primary disp.");
                psDisplayInfo0->setDispMode = out_mode;
                if(DISP_TV_MOD_1080P_24HZ_3D_FP != out_mode && is3DMode(psDisplayInfo0->Current3DMode))
                {
                    psDisplayInfo0->Current3DMode = DISPLAY_2D_ORIGINAL;
                }
                ret = hwcOutputSwitch(psDisplayInfo0, out_type, out_mode);
            }
            return ret;
        }
        if(out_type != psDisplayInfo1->DisplayType)//fixme: if external disp opened ?
        {
            ALOGD("open new device on other hwdisp");
            memcpy((void *)psDisplayInfo1, (void *)psDisplayInfo0, sizeof(DisplayInfo));
            psDisplayInfo1->setDispMode = out_mode;
            if(hwcOutputSwitch(psDisplayInfo1, out_type, out_mode))
            {
                ALOGD("cannot open this type[%d]-mode[%d]!", out_type, out_mode);
                return -1;
            }
        }
        ALOGD("###exchange output[%d,%d]<-->output[%d,%d]",
            psDisplayInfo0->DisplayType, psDisplayInfo0->DisplayMode,
            psDisplayInfo1->DisplayType, psDisplayInfo1->DisplayMode);
        hwcOutputExchange();//exchange output device between primary and external
        if(-EINVAL == dispExternalOpened)
        {
            ALOGD("close external disp after exchange.");
            hwcOutputSwitch(psDisplayInfo1, 0, 0);
        }
    }
    else if(HWC_DISPLAY_EXTERNAL == disp)
    {
        if(out_type == psDisplayInfo0->DisplayType)
        {
            ALOGD("can not open the DisplayType[%d] which was opened on primary display.", out_type);
            return -1;
        }
        if(-EINVAL == psDisplayInfo1->VirtualToHWDisplay)
        {
            if(0 == out_type)
            {
                ALOGD("allready closed external display");
                return 0;
            }
          #ifdef WB_FOR_EXTERNAL_DISPLAY
            ALOGD("add here: opend writeback for external display");
            return -1;
          #else
            ALOGD("init and open external display: output[%d,%d]", out_type, out_mode);
            int needExchange = 0;
            disp_output dispOutput;
            arg[0] = getHwDispByType(out_type);
            arg[1] = out_type;
            arg[2] = out_mode;
            ioctl(Globctx->DisplayFd, DISP_DEVICE_SWITCH, (unsigned long)arg);
            dispOutput.type = out_type;
            dispOutput.mode = out_mode;
            initDisplayDevice(HWC_DISPLAY_EXTERNAL, arg[0], &dispOutput);
            if(arg[0] >=0 && arg[0] < psDisplayInfo0->VirtualToHWDisplay)
            {
                ALOGD("need to exchange...");
                needExchange = 1;
                psDisplayInfo1->InitDisplayWidth = get_info_mode(psDisplayInfo0->DisplayMode, WIDTH);
                psDisplayInfo1->InitDisplayHeight = get_info_mode(psDisplayInfo0->DisplayMode, HEIGHT);
            }
            else
            {
                psDisplayInfo1->setblank = 0;
                psDisplayInfo1->SetPersentWidth = psDisplayInfo0->SetPersentWidth;
                psDisplayInfo1->SetPersentHeight = psDisplayInfo0->SetPersentHeight;
            }
            Globctx->psHwcProcs->hotplug(Globctx->psHwcProcs, HWC_DISPLAY_EXTERNAL, 1);
            if(1 == needExchange)
            {
                ALOGD("need exchange output[%d,%d]<-->output[%d,%d]",
                    psDisplayInfo0->DisplayType, psDisplayInfo0->DisplayMode,
                    psDisplayInfo1->DisplayType, psDisplayInfo1->DisplayMode);
                hwcOutputExchange();// exchange output device between primary and external
                psDisplayInfo0->setblank = 0;
            }
            return 0;
          #endif
        }
        else
        {
            if(0 == out_type)
            {
              #ifdef WB_FOR_EXTERNAL_DISPLAY
                ALOGD("add here: close writeback for external display.");
              #else
                ALOGD("close external display.");
                Globctx->psHwcProcs->hotplug(Globctx->psHwcProcs, HWC_DISPLAY_EXTERNAL, 0);
                Globctx->psHwcProcs->invalidate(Globctx->psHwcProcs);
              #endif
            }
            if(psDisplayInfo1->DisplayMode != out_mode) {
                if(DISP_TV_MOD_1080P_24HZ_3D_FP != out_mode && is3DMode(psDisplayInfo1->Current3DMode))
                {
                    psDisplayInfo1->Current3DMode = DISPLAY_2D_ORIGINAL;
                }
                ALOGD("set external display mode.");
                psDisplayInfo1->setDispMode = out_mode;
                ret = hwcOutputSwitch(psDisplayInfo1, out_type, out_mode);
                if(ret)
                {
                    ALOGD("set external display mode failed.");
                    hwcOutputSwitch(psDisplayInfo1, 0, 0);
                    return ret;
                }
            }
        }
    }
    else
    {
        ALOGW("[ERR]disp = %d", disp);
        return -1;
    }
    return 0;
}

int _hwc_device_set_margin(int disp, int hpercent, int vpercent)
{
    SUNXI_hwcdev_context_t *Globctx = &gSunxiHwcDevice;
    DisplayInfo   *PsDisplayInfo = &Globctx->SunxiDisplay[disp];
    if(PsDisplayInfo->VirtualToHWDisplay != -EINVAL)
    {
        PsDisplayInfo->SetPersentWidth = (hpercent < MARGIN_MIN_PERCENT) ? MARGIN_MIN_PERCENT :
            ((hpercent > MARGIN_MAX_PERCENT) ? MARGIN_MAX_PERCENT : hpercent);
        PsDisplayInfo->SetPersentHeight = (vpercent < MARGIN_MIN_PERCENT) ? MARGIN_MIN_PERCENT :
            ((vpercent > MARGIN_MAX_PERCENT) ? MARGIN_MAX_PERCENT : vpercent);
    }
    return 0;
}

int _hwc_device_is_support_hdmi_mode(int disp,int mode)
{
    SUNXI_hwcdev_context_t *Globctx = &gSunxiHwcDevice;
    unsigned long arg[4] = {0};
    int ret;

    arg[0] = Globctx->SunxiDisplay[disp].VirtualToHWDisplay;
    arg[1] = mode;
    ret = ioctl(Globctx->DisplayFd, DISP_HDMI_SUPPORT_MODE, (unsigned long)arg);
    if(ret)
    {
        ALOGD("supported: args[%d,%d]", arg[0], mode);
        return 1;
    }
    ALOGD("not supported: args[%d,%d]", arg[0], mode);
    return 0;
}

int _hwc_device_get_output_type(int disp)
{
    DisplayInfo   *psDisplayInfo = &(gSunxiHwcDevice.SunxiDisplay[disp]);
    return psDisplayInfo->DisplayType;
}

int _hwc_device_get_output_mode(int disp)
{
    DisplayInfo   *psDisplayInfo = &(gSunxiHwcDevice.SunxiDisplay[disp]);
    return psDisplayInfo->setDispMode;
}

int _hwc_device_set_screenradio(int disp, int radioType)
{
    SUNXI_hwcdev_context_t *Globctx = &gSunxiHwcDevice;
    DisplayInfo *psDisplayInfo = NULL;
    switch(radioType)
    {
    case SCREEN_AUTO:
    case SCREEN_FULL:
        for(int i = 0; i < NUMBEROFDISPLAY; i++)
        {
            psDisplayInfo = &(Globctx->SunxiDisplay[i]);
            psDisplayInfo->screenRadio = radioType;
        }
        break;
    default:
        break;
    }
    return 0;
}

