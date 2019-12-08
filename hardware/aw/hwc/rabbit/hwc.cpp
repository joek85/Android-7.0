/*-------------------------------------------------------------------------

-------------------------------------------------------------------------*/
/*-------------------------------------------------------------------------

-------------------------------------------------------------------------*/
/*-------------------------------------------------------------------------

-------------------------------------------------------------------------*/
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

//#define LOG_NDEBUG 0

#include "hwc.h"
#include "cutils/properties.h"
#include "hwcomposerInterface.h"

extern int hwc_perform(int display, int cmd, ...);

static int Framecount = 0;
/*****************************************************************************/

static int hwc_device_open(const struct hw_module_t* module, const char* name,
        struct hw_device_t** device);

static struct hw_module_methods_t hwc_module_methods = {
    .open = hwc_device_open
};

hwc_module_t HAL_MODULE_INFO_SYM = {
    .common = {
        .tag = HARDWARE_MODULE_TAG,
        .version_major = 1,
        .version_minor = 0,
        .id = HWC_HARDWARE_MODULE_ID,
        .name = "Sample hwcomposer module",
        .author = "The Android Open Source Project",
        .methods = &hwc_module_methods,
        .dso = 0,
        .reserved = {0},
    }
};

/*****************************************************************************/

static int hwc_blank(struct hwc_composer_device_1* dev, int disp, int blank)
{
    dev;
    unsigned long arg[4]={0};
    SUNXI_hwcdev_context_t *Globctx = &gSunxiHwcDevice;
    DisplayInfo *PsDisplayInfo = &Globctx->SunxiDisplay[disp];

    if (HWC_NUM_PHYSICAL_DISPLAY_TYPES <= disp) {
        ALOGD("invalid disp for hwc blank");
        return -1;
    }
    ALOGD("disp(%d) %s\n", disp, blank?"off":"on");

    if (blank) {
        PsDisplayInfo->power_mode = 0;
    } else {
        PsDisplayInfo->power_mode = 2;
    }

    if (HWC_DISPLAY_PRIMARY == disp) {
        arg[0] = 0;
        arg[1] = blank;
        if (ioctl(Globctx->DisplayFd, DISP_BLANK, (unsigned long)arg) != 0)
            ALOGD("###hwc_blank: DISP_BLANK failed");
    }

	if (blank) {
		blank_disp(PsDisplayInfo->VirtualToHWDisplay);
	} else {
        Globctx->psHwcProcs->invalidate(Globctx->psHwcProcs);
    }

    return 0;
}

int fixDisp(int disp)
{
#ifdef SYSRSL_SWITCH_ON_EX
    if((DISP_DUAL_POLICY != gSunxiHwcDevice.dispPolicy)
        && (isDisplayP2P()))
    {
        ALOGD("can SYSRSL_SWITCH_ON_EX");
        if(HWC_DISPLAY_PRIMARY == disp)
            disp = HWC_DISPLAY_EXTERNAL;
        else
            disp = HWC_NUM_DISPLAY_TYPES + 1; // invalue
    }
#endif
    return disp;
}

static int hwc_setParameter(struct hwc_composer_device_1* dev, int disp, int cmd,
            int para0, int para1, int para2)
{
    dev;
    int ret = 0;

    ALOGD("hh:disp=%d,cmd=%d,paras[%d,%d,%d]", disp, cmd, para0, para1, para2);
    disp = fixDisp(disp);
    if(disp > HWC_DISPLAY_EXTERNAL)
    {
        ALOGD("[hwc_setParameter]disp=%d,cmd=%d", disp, cmd);
        return -1;
    }
    if(disp > HWC_DISPLAY_EXTERNAL)
    {
        ALOGD("[hwc_setParameter]disp=%d,cmd=%d", disp, cmd);
        return -1;
    }
    switch(cmd)
    {
    case DISPLAY_CMD_SET3DMODE:
        ret = _hwc_device_set_3d_mode(disp, (__display_3d_mode)para0, para1);
        break;
    case DISPLAY_CMD_SETBACKLIGHTMODE:
        //ret = _hwc_device_set_backlight(disp, para0,0);
        break;
    case DISPLAY_CMD_SETBACKLIGHTDEMOMODE:
        //ret = _hwc_device_set_backlight(disp, para0,1);
        break;
    case DISPLAY_CMD_SETDISPLAYENHANCEMODE:
        ret = _hwc_device_set_enhance_mode(disp, para0);
        break;
    case DISPLAY_CMD_SETDISPLAYENHANCEDEMOMODE:
        break;
    case DISPLAY_CMD_SETOUTPUTMODE:
        ret = _hwc_device_set_output_mode(disp, para0, para1);
        break;
    case DISPLAY_CMD_SETMARGIN:
        ret = _hwc_device_set_margin(disp, para0, para1);
        break;
    case DISPLAY_CMD_SETSCREENRADIO:
        ret = _hwc_device_set_screenradio(disp, para0);
        break;
    default:
        break;
    }

    return ret;
}

static int hwc_getParameter(struct hwc_composer_device_1* dev, int disp, int cmd,
            int para0, int para1)
{
    enum{
        DISPLAY_CMD_GETDISPFPS = 0x29,
    };

    dev;
    int ret = 0;
    //ALOGD("[hwc_getParameter]disp=%d,cmd=%d,paras[%d,%d]", disp, cmd, para0, para1);
    disp = fixDisp(disp);
    if(disp > HWC_DISPLAY_EXTERNAL)
    {
        ALOGD("[hwc_getParameter]disp=%d,cmd=%d", disp, cmd);
        return -1;
    }
    if(disp > HWC_DISPLAY_EXTERNAL)
    {
        ALOGD("[hwc_getParameter]disp=%d,cmd=%d", disp, cmd);
        return -1;
    }
    switch(cmd)
    {
    case DISPLAY_CMD_GETOUTPUTTYPE:
        ret = _hwc_device_get_output_type(disp);
        break;
    case DISPLAY_CMD_GETOUTPUTMODE:
        ret = _hwc_device_get_output_mode(disp);
        break;
    case DISPLAY_CMD_ISSUPPORTHDMIMODE:
        ret = _hwc_device_is_support_hdmi_mode(disp,para0);
        break;
    case DISPLAY_CMD_GETSUPPORT3DMODE:
        ALOGD("[hwc_getParameter]disp=%d,cmd=%d,paras[%d,%d]", disp, cmd, para0, para1);
        ret = _hwc_device_is_support_hdmi_mode(disp, DISP_TV_MOD_1080P_24HZ_3D_FP);
        break;
    case DISPLAY_CMD_GETMARGIN_W:
        ret = gSunxiHwcDevice.SunxiDisplay[disp].SetPersentWidth;
        break;
    case DISPLAY_CMD_GETMARGIN_H:
        ret = gSunxiHwcDevice.SunxiDisplay[disp].SetPersentHeight;
        break;
    case DISPLAY_CMD_GETDISPFPS:
        ret = gSunxiHwcDevice.SunxiDisplay[disp].DisplayFps;
        break;
    default:
        break;
    }
    return ret;
}

static
int hwc_prepare(hwc_composer_device_1_t *dev, size_t numDisplays,
                       hwc_display_contents_1_t **displays)
{
    dev;
    sunxi_prepare(displays,numDisplays);

    return 0;
}

static int hwc_set(hwc_composer_device_1_t *dev,
        size_t numDisplays, hwc_display_contents_1_t** displays)
{
    dev;
    int ret = 0;
    int releaseFenceFd = -1;
    int retirefencefd = -1;
    size_t disp, i;
    hwc_display_contents_1_t* psDisplay;

    sunxi_set( displays, numDisplays, &releaseFenceFd, &retirefencefd);

    return ret;
}

static int hwcVsyncEnable(int dispFd, DisplayInfo *PsDisplayInfo, int enabled)
{
    unsigned long arg[4]={0};

    arg[0] = PsDisplayInfo->VirtualToHWDisplay;
    arg[1] = !!enabled;
    ioctl(dispFd, DISP_VSYNC_EVENT_EN,(unsigned long)arg);
    PsDisplayInfo->VsyncEnable = !!enabled;
    return 0;
}

static int hwc_eventControl(struct hwc_composer_device_1* dev, int disp,
            int event, int enabled)
{
    dev;
    int ret = -EINVAL;
    SUNXI_hwcdev_context_t *Globctx = &gSunxiHwcDevice;

    switch(event)
    {
    case HWC_EVENT_VSYNC:
      {
        DisplayInfo *PsDisplayInfo = &Globctx->SunxiDisplay[HWC_DISPLAY_PRIMARY];
        if(PsDisplayInfo->VirtualToHWDisplay == F_HWDISP_ID)
        {
            PsDisplayInfo = &Globctx->SunxiDisplay[HWC_DISPLAY_EXTERNAL];
        }
        if(HWC_DISPLAY_PRIMARY == disp)
        {
            //ALOGD("eventcontrol: hwdisp=%d", PsDisplayInfo->VirtualToHWDisplay);
            hwc_set_vsync_need_reset(HWC_VSYNC_ENABLE_CHANGED);
            ret = hwcVsyncEnable(Globctx->DisplayFd, PsDisplayInfo, enabled);
        }
      }
        break;
    default:
        break;
    }
    return ret;
}

static void prepare_external_display(void) {
    SUNXI_hwcdev_context_t *Globctx = &gSunxiHwcDevice;
    DisplayInfo *dispInfo = &Globctx->SunxiDisplay[HWC_DISPLAY_EXTERNAL];

    if (dispInfo->DisplayType == DISP_OUTPUT_TYPE_TV) {
        if (isCvbsHpd(dispInfo->VirtualToHWDisplay)) {
            ALOGD("display type %d mode %d",
                dispInfo->DisplayType, dispInfo->DisplayMode);
            Globctx->psHwcProcs->hotplug(
                Globctx->psHwcProcs, HWC_DISPLAY_EXTERNAL, 1);
        }
    } else {
        ALOGD("fixme: display type %d mode %d",
            dispInfo->DisplayType, dispInfo->DisplayMode);
    }
}

static void hwc_register_procs(struct hwc_composer_device_1* dev,
            hwc_procs_t const* procs)
{
    dev;
    SUNXI_hwcdev_context_t *Globctx = &gSunxiHwcDevice;
    Globctx->psHwcProcs = const_cast<hwc_procs_t *>(procs);

    prepare_external_display();
    displaydRequest_init(reinterpret_cast<void *>(hwc_perform));
}

static int hwc_getDisplayConfigs(struct hwc_composer_device_1 *dev,
        int disp, uint32_t *configs, size_t *numConfigs)
{
    dev;
    SUNXI_hwcdev_context_t *Globctx = &gSunxiHwcDevice;
    DisplayInfo   *PsDisplayInfo = &Globctx->SunxiDisplay[disp];

    *numConfigs = 1;
    *configs = 0;
    return PsDisplayInfo->VirtualToHWDisplay == EVALID_HWDISP_ID;

}

static int32_t hwc_get_attribute(const uint32_t attribute,
         uint32_t disp)
{
    SUNXI_hwcdev_context_t *Globctx = &gSunxiHwcDevice;
    DisplayInfo   *PsDisplayInfo = &Globctx->SunxiDisplay[disp];

    if(PsDisplayInfo->VirtualToHWDisplay != EVALID_HWDISP_ID)
    {
        switch(attribute) {
        case HWC_DISPLAY_VSYNC_PERIOD:
            return PsDisplayInfo->DisplayVsyncP;

        case HWC_DISPLAY_WIDTH:
            Globctx->InitDisplaySize[disp][0] = PsDisplayInfo->InitDisplayWidth;
            return PsDisplayInfo->InitDisplayWidth;

        case HWC_DISPLAY_HEIGHT:
            Globctx->InitDisplaySize[disp][1] = PsDisplayInfo->InitDisplayHeight;
            return PsDisplayInfo->InitDisplayHeight;

        case HWC_DISPLAY_DPI_X:
            return PsDisplayInfo->DiplayDPI_X;

        case HWC_DISPLAY_DPI_Y:
            return PsDisplayInfo->DiplayDPI_Y;

        case 6: //HWC_DISPLAY_IS_SECURE:
        //    return PsDisplayInfo->issecure;

        default:
            ALOGE("unknown display attribute %u", attribute);
            return -EINVAL;
        }
    }
    ALOGE("No hareware display ");
    return -EINVAL;
}

static int hwc_getDisplayAttributes(struct hwc_composer_device_1 *dev,
        int disp, uint32_t config, const uint32_t *attributes, int32_t *values)
{
    dev, config;
    for (int i = 0; attributes[i] != HWC_DISPLAY_NO_ATTRIBUTE; i++)
    {
        if (disp <=2)
        {
            values[i]=hwc_get_attribute(attributes[i],disp);

        }else
        {
            ALOGE("unknown display type %u", disp);
            return -EINVAL;
        }
    }
    return 0;
}

static int hwc_query(struct hwc_composer_device_1* dev,
    int what, int* value)
{
	dev;
	switch (what) {
	case HWC_IS_SKIP_GPU_BUFFER:
		return is_skip_gpu_buf((buffer_handle_t)value);
	default:
		ALOGD("hwc_query not support %d", what);
		return -1;
	}
}

static int hwc_set_cursor_position_async(struct hwc_composer_device_1 *dev,
    int disp, int x_pos, int y_pos)
{
    dev, disp, x_pos, y_pos;
    //ALOGD("set_cursor_position_async: disp=%d, x=%d, y=%d",
    //    disp, x_pos, y_pos);
    return 0;
}

static int hwc_device_close(struct hw_device_t *dev)
{
    dev;
    hwc_destroy_device();
    return 0;
}

/*****************************************************************************/

static int hwc_device_open(const struct hw_module_t* module, const char* name,
        struct hw_device_t** device)
{
    hwc_composer_device_1_t *psHwcDevice;
    hw_device_t *psHwDevice;
    int err = 0;

    if (strcmp(name, HWC_HARDWARE_COMPOSER))
    {
        return -EINVAL;
    }

    psHwcDevice = (hwc_composer_device_1_t *)malloc(sizeof(hwc_composer_device_1_t));
    if(!psHwcDevice)
    {
        ALOGD("%s: Failed to allocate memory", __func__);
        return -ENOMEM;
    }

    memset(psHwcDevice, 0, sizeof(hwc_composer_device_1_t));
    psHwDevice = (hw_device_t *)psHwcDevice;

    psHwcDevice->common.tag      = HARDWARE_DEVICE_TAG;
    psHwcDevice->common.version  = HWC_DEVICE_API_VERSION_1_1;
    psHwcDevice->common.module   = const_cast<hw_module_t*>(module);
    psHwcDevice->common.close    = hwc_device_close;
    psHwcDevice->prepare         = hwc_prepare;
    psHwcDevice->set             = hwc_set;
    psHwcDevice->registerProcs   = hwc_register_procs;
    psHwcDevice->eventControl    = hwc_eventControl;
    psHwcDevice->blank           = hwc_blank;
    psHwcDevice->getDisplayConfigs = hwc_getDisplayConfigs;
    psHwcDevice->getDisplayAttributes = hwc_getDisplayAttributes;
    psHwcDevice->query           = hwc_query;
    psHwcDevice->setCursorPositionAsync = hwc_set_cursor_position_async;

    *device = psHwDevice;

    hwc_create_device();

    return err;
}


