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


static int Framecount = 0;
/*****************************************************************************/

static int hwc_device_open(const struct hw_module_t* module, const char* name,
        struct hw_device_t** device);

static struct hw_module_methods_t hwc_module_methods = {
    open: hwc_device_open
};

hwc_module_t HAL_MODULE_INFO_SYM = {
    common: {
        tag: HARDWARE_MODULE_TAG,
        version_major: 1,
        version_minor: 0,
        id: HWC_HARDWARE_MODULE_ID,
        name: "Sample hwcomposer module",
        author: "The Android Open Source Project",
        methods: &hwc_module_methods,
    }
};

/*****************************************************************************/



static int hwc_blank(struct hwc_composer_device_1* dev, int disp, int blank)
{
    return 0;
}

static int hwc_setParameter(struct hwc_composer_device_1* dev, int disp, int cmd,
            int para0, int para1, int para2)
{
	int ret = 0;
	ALOGD("hh:disp=%d,cmd=%d,paras[%d,%d,%d]", disp, cmd, para0, para1, para2);
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
		ret = _hwc_device_set_3d_mode(disp, (__display_3d_mode)para0);
		break;
	case DISPLAY_CMD_SETBACKLIGHTMODE:
		//ret = _hwc_device_set_backlight(disp, para0,0);
		break;
	case DISPLAY_CMD_SETBACKLIGHTDEMOMODE:
		//ret = _hwc_device_set_backlight(disp, para0,1);
		break;
	case DISPLAY_CMD_SETDISPLAYENHANCEMODE:
		ret = _hwc_device_set_enhancemode(disp, para0,0);
		break;
	case DISPLAY_CMD_SETDISPLAYENHANCEDEMOMODE:
		ret = _hwc_device_set_enhancemode(disp, para0,1);
		break;
	case DISPLAY_CMD_SETOUTPUTMODE:
		ret = _hwc_device_set_output_mode(disp, para0, para1);
		break;
	case DISPLAY_CMD_SETMARGIN:
		ret = _hwc_device_set_margin(HWC_DISPLAY_PRIMARY, para0, para1);
		_hwc_device_set_margin(HWC_DISPLAY_EXTERNAL, para0, para1); //fixme: dual-display use the same margin para
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
	int ret = 0;
	//ALOGD("[hwc_getParameter]disp=%d,cmd=%d,paras[%d,%d]", disp, cmd, para0, para1);
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
		ret = gSunxiHwcDevice.SunxiDisplay[disp].PersentWidth;
		break;
	case DISPLAY_CMD_GETMARGIN_H:
		ret = gSunxiHwcDevice.SunxiDisplay[disp].PersentHeight;
		break;
#if 0
	case DISPLAY_CMD_GETDISPFPS:
		ret = 255;
		for(int i =0; i < gSunxiHwcDevice.NumberofDisp; i++){
			if(gSunxiHwcDevice.SunxiDisplay[i].DisplayFps > 0 && ret > gSunxiHwcDevice.SunxiDisplay[i].DisplayFps){
				ret = gSunxiHwcDevice.SunxiDisplay[i].DisplayFps;
			}
		}
		break;
#endif
	default:
		break;
	}
	return ret;
}

static
int hwc_prepare(hwc_composer_device_1_t *dev, size_t numDisplays,
					   hwc_display_contents_1_t **displays)
{

    sunxi_prepare(displays,numDisplays);

	return 0;
}

static int hwc_set(hwc_composer_device_1_t *dev,
        size_t numDisplays, hwc_display_contents_1_t** displays)
{
    int ret = 0;
	int releaseFenceFd = -1;
    int retirefencefd = -1;
	size_t disp, i;
    hwc_display_contents_1_t* psDisplay;

    sunxi_set( displays, numDisplays, &releaseFenceFd, &retirefencefd);

	for(disp = 0; disp < numDisplays; disp++)
	{
		psDisplay = displays[disp];
		if(!psDisplay)
		{
			ALOGV("%s: display[%d] was unexpectedly NULL",
    			    					__func__, disp);
    		continue;
		}

		for(i=0 ; i<psDisplay->numHwLayers ; i++)
		{
            if(psDisplay->hwLayers[i].acquireFenceFd>=0)
            {
               close(psDisplay->hwLayers[i].acquireFenceFd);
               psDisplay->hwLayers[i].acquireFenceFd=-1;
             }
		    if((psDisplay->hwLayers[i].compositionType == HWC_OVERLAY) || ((psDisplay->hwLayers[i].compositionType == HWC_FRAMEBUFFER_TARGET)))
			{
                if(psDisplay->hwLayers[i].releaseFenceFd >=0)
                {
                    close(psDisplay->hwLayers[i].releaseFenceFd);
                }
				if(releaseFenceFd >= 0)
				{
					psDisplay->hwLayers[i].releaseFenceFd = dup(releaseFenceFd);
			    }else{
					    psDisplay->hwLayers[i].releaseFenceFd = -1;
				}
	        }else{
				psDisplay->hwLayers[i].releaseFenceFd = -1;
		    }
		}
    }
    if(releaseFenceFd >= 0)
    {
        close(releaseFenceFd);
	    releaseFenceFd = -1;
    }

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
    int ret = -EINVAL;
    SUNXI_hwcdev_context_t *Globctx = &gSunxiHwcDevice;

    switch(event)
    {
    case HWC_EVENT_VSYNC:
      {
        DisplayInfo *PsDisplayInfo = &Globctx->SunxiDisplay[HWC_DISPLAY_PRIMARY];
        if(HWC_DISPLAY_PRIMARY == disp)
        {
            ret = hwcVsyncEnable(Globctx->DisplayFd, PsDisplayInfo, enabled);
        }
      }
        break;
    default:
        break;
    }
    return ret;
}

static void hwc_register_procs(struct hwc_composer_device_1* dev,
            hwc_procs_t const* procs)
{
    SUNXI_hwcdev_context_t *Globctx = &gSunxiHwcDevice;
    Globctx->psHwcProcs = const_cast<hwc_procs_t *>(procs);
   // Globctx->psHwcProcs->hotplug(Globctx->psHwcProcs, HWC_DISPLAY_EXTERNAL, 0);
    //hwc_hotplug_switch(1, true);
    //Globctx->psHwcProcs->hotplug(Globctx->psHwcProcs, HWC_DISPLAY_EXTERNAL, 1);
}

static int hwc_getDisplayConfigs(struct hwc_composer_device_1 *dev,
        int disp, uint32_t *configs, size_t *numConfigs)
{

	SUNXI_hwcdev_context_t *Globctx = &gSunxiHwcDevice;
    DisplayInfo   *PsDisplayInfo = &Globctx->SunxiDisplay[disp];

    *numConfigs = 1;
    *configs = 0;
    return PsDisplayInfo->VirtualToHWDisplay == -EINVAL;

}

static int32_t hwc_get_attribute(const uint32_t attribute,
         uint32_t disp)
{
	SUNXI_hwcdev_context_t *Globctx = &gSunxiHwcDevice;
    DisplayInfo   *PsDisplayInfo = &Globctx->SunxiDisplay[disp];

    if(PsDisplayInfo->VirtualToHWDisplay != -EINVAL)
    {
	    switch(attribute) {
        case HWC_DISPLAY_VSYNC_PERIOD:
            return PsDisplayInfo->DisplayVsyncP;

        case HWC_DISPLAY_WIDTH:
            return PsDisplayInfo->InitDisplayWidth;

        case HWC_DISPLAY_HEIGHT:
            return PsDisplayInfo->InitDisplayHeight;

        case HWC_DISPLAY_DPI_X:
            return PsDisplayInfo->DiplayDPI_X;

        case HWC_DISPLAY_DPI_Y:
            return PsDisplayInfo->DiplayDPI_Y;

	    //case HWC_DISPLAY_IS_SECURE:
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

static int hwc_device_close(struct hw_device_t *dev)
{
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
#ifdef HWC_1_3
    psHwcDevice->common.version  = HWC_DEVICE_API_VERSION_1_3;
#else
    psHwcDevice->common.version  = HWC_DEVICE_API_VERSION_1_1;
#endif
    psHwcDevice->common.module   = const_cast<hw_module_t*>(module);
    psHwcDevice->common.close    = hwc_device_close;

    psHwcDevice->prepare         = hwc_prepare;
    psHwcDevice->set             = hwc_set;
    psHwcDevice->setDisplayParameter = hwc_setParameter;
    psHwcDevice->getDisplayParameter = hwc_getParameter;
    psHwcDevice->registerProcs   = hwc_register_procs;
    psHwcDevice->eventControl	 = hwc_eventControl;
	psHwcDevice->blank			 = hwc_blank;
	psHwcDevice->getDisplayConfigs = hwc_getDisplayConfigs;
	psHwcDevice->getDisplayAttributes = hwc_getDisplayAttributes;

    *device = psHwDevice;

	hwc_create_device();

    return err;
}


