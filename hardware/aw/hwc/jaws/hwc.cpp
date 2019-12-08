#include "hwc.h"
#include "common/utils.h"

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
        name: "Allwinner hwcomposer module",
        author: "The Homlet Display Team",
        methods: &hwc_module_methods,
        dso:NULL,
        reserved:{0},
    }
};

//define int hwc.h , as global valiables
hwc_context_t gHwcContext;

static hwc_opr_t *hwcOpr;
static device_opr_t *deviceOpr;

static int hwc_blank(struct hwc_composer_device_1* dev, int disp, int blank){
    return 0;
}

static int hwc_setParameter(hwc_composer_device_1_t* dev, int disp, int cmd,
    int para0, int para1, int para2){
    hwc_context_t *glbCtx = &gHwcContext;
    switch(disp){
    case HWC_DISPLAY_PRIMARY:
    case HWC_DISPLAY_EXTERNAL:
        break;
    default:
        ALOGE("Do not support setParemter API for the virtual display device.");
        return -EINVAL;
    }
    if(deviceOpr == NULL){
        return -EINVAL;
    }
    ALOGD("disp=%d, cmd=%d, para[%d, %d, %d]\n", disp, cmd, para0, para1, para2);
    switch(cmd){
    case DISPLAY_CMD_SET3DMODE:
        return deviceOpr->set3DMode(disp, para0);
    case DISPLAY_CMD_SETBACKLIGHTMODE:
        return deviceOpr->setBacklightMode(disp, para0);
    case DISPLAY_CMD_SETDISPLAYENHANCEMODE:
        return deviceOpr->setEnhanceMode(disp, para0);
    case DISPLAY_CMD_SETOUTPUTMODE:
        return deviceOpr->setOutputMode(disp, para0, para1);
    case DISPLAY_CMD_SETMARGIN:
        return deviceOpr->setMargin(disp, para0, para1);
    case DISPLAY_CMD_SETSATURATION:
        return deviceOpr->setSaturation(disp, para0);
    case DISPLAY_CMD_SETHUE:
        return deviceOpr->setHue(disp, para0);
    case DISPLAY_CMD_SETBRIGHT:
        return deviceOpr->setBright(disp, para0);
    case DISPLAY_CMD_SETCONTRAST:
        return deviceOpr->setContrast(disp, para0);
    case DISPLAY_CMD_SETSCREENRADIO:
        glbCtx->screenRadio = para0;
        return 0;
    default:
        return -EINVAL;
    }
}

static int hwc_getParameter(struct hwc_composer_device_1* dev, int disp, int cmd,
            int para0, int para1){
    switch(cmd){
    case DISPLAY_CMD_GETOUTPUTTYPE:
        return deviceOpr->getOutputType(disp);
    case DISPLAY_CMD_GETOUTPUTMODE:
        return deviceOpr->getOutputMode(disp);
    case DISPLAY_CMD_ISSUPPORTHDMIMODE:
        return deviceOpr->isSupportHdmiMode(disp, para0);
    case DISPLAY_CMD_GETSUPPORT3DMODE:
        return deviceOpr->isSupport3DMode(disp);
    case DISPLAY_CMD_GETSATURATION:
        return deviceOpr->getSaturation(disp);
    case DISPLAY_CMD_GETHUE:
        return deviceOpr->getHue(disp);
    case DISPLAY_CMD_GETBRIGHT:
        return deviceOpr->getBright(disp);
    case DISPLAY_CMD_GETCONTRAST:
        return deviceOpr->getContrast(disp);
    case DISPLAY_CMD_GETMARGIN_W:
        return deviceOpr->getMarginW(disp);
    case DISPLAY_CMD_GETMARGIN_H:
        return deviceOpr->getMarginH(disp);
    default:
        return -EINVAL;
    }
}

static int hwc_prepare(hwc_composer_device_1_t *dev, size_t numDisplays,
    hwc_display_contents_1_t **displays){
    int disp, i , hwDisp;
    hwc_context_t* context = &gHwcContext;
    hwc_display_contents_1_t *display;
    hwc_layer_1_t* layer = NULL;
    int assignTime = 0;
    device_info_t *deviceInfo = NULL;
    int zOrder = 0;
    if((context->hwcDebug & SHOW_ALL) == LAYER_DUMP){
        ALOGD("hwc_prepare");
    }
    hwcOpr->beforePrepare();
    for(disp = 0; disp < numDisplays; disp++){
        display = displays[disp];
        if(!display){
    		continue;
    	}
        if(display->outbuf != NULL || display->outbufAcquireFenceFd != 0){
            if (display->retireFenceFd >= 0){
                close(display->retireFenceFd);
                display->retireFenceFd = -1;
            }
            if (display->outbuf != NULL){
                display->outbuf = NULL;
            }
            if (display->outbufAcquireFenceFd >= 0){
                close(display->outbufAcquireFenceFd);
                display->outbufAcquireFenceFd = -1;
            }
            //ALOGV("%s: Virtual displays are not supported",
    		//						__func__);
        }
        resetLayerType(display);
        switch(disp){
        case HWC_DISPLAY_PRIMARY:
            for(i = 0; i < context->displayNum; i++){
                if(context->sunxiDevice[i].disp.id != INVALID_VALUE
                    && context->sunxiDevice[i].disp.id == context->mainDispId){
                    deviceInfo = &context->sunxiDevice[i];
                    hwDisp = i;
                    break;
                }
            }
            break;
        case HWC_DISPLAY_EXTERNAL:
            for(i = 0; i < context->displayNum; i++){
                if(context->sunxiDevice[i].disp.id != INVALID_VALUE
                    && context->sunxiDevice[i].disp.id == context->secDispId){
                    deviceInfo = &context->sunxiDevice[i];
                    hwDisp = i;
                    break;
                }
            }
            break;
        case HWC_DISPLAY_VIRTUAL:
        default:
            ALOGV("%s: Virtual displays are not supported", __func__);
            return -1;
        }
        //recount scale percent.
        reCountPercent(deviceInfo);

        bool stopHwc = false;
        if(display->numHwLayers < 2){
            ALOGD("%s: display[%d] numHwLayer:%d less then 2",
    								__func__, disp, display->numHwLayers);
            continue;
        }
        for(i = 0; i < display->numHwLayers; i++){
            layer = &display->hwLayers[i];
            if(layer->handle == NULL && !(layer->flags & HWC_SKIP_LAYER)){
	    	    ALOGD("%s: handle is NULL", __func__);
	    		stopHwc = 1;
	    		break;
	    	}
            if (layer->videoFormat == HAL_PIXEL_FORMAT_AW_FORCE_GPU){
                ALOGD("%s: AW Force GPU", __func__);
                stopHwc = 1;
                break;
            }
        }
        if(stopHwc){
            hwcOpr->reset(hwDisp);
            layer = &(display->hwLayers[display->numHwLayers - 1]);
            if(!hwcOpr->tryToAssignLayer(layer, 0, hwDisp)){
                ALOGD("Use GPU COmposite FB failed in disp%d", hwDisp);
                continue;
            }
            hwcOpr->postPrepare(display);
        }else{
            assignTime = 0;
        TryAgain:
            hwcOpr->reset(hwDisp);
            resetLayerType(display);
            zOrder = 0;
            if(assignTime > 0){
                //the second time to assign,we assignt fbtarget layer first.
                layer = &(display->hwLayers[display->numHwLayers - 1]);
                if(!hwcOpr->tryToAssignLayer(layer, INVALID_VALUE, hwDisp)){
                    ALOGE("Assign FbTarget Layer fail, this should not happen.");
                    continue;
                }
            }
            for(i =0; i <= display->numHwLayers - 2; i++){
                layer = &(display->hwLayers[i]);
                if(hwcOpr->isLayerAvailable(layer, hwDisp)){
                    if(hwcOpr->tryToAssignLayer(layer, i, hwDisp)){
                        //assign success,continue
                        layer->compositionType = HWC_OVERLAY;
                        continue;
                    }else{
                        //assign fail,try to update FbTarget Layer.
                        if(!hwcOpr->updateFbTarget(layer, i, hwDisp)){
                            //if fail, it means that the FbTarget Layer is not yet assigned to de composer
                            assignTime ++;
                            goto TryAgain;
                        }
                    }
                }else{
                    if(!hwcOpr->updateFbTarget(layer, i, hwDisp)){
                        //if fail, it means that the FbTarget Layer is not yet assigned to de composer
                        assignTime ++;
                        goto TryAgain;
                    }
                }
            }
            hwcOpr->postPrepare(display);
        }
    }
    return 0;
}

static int hwc_set(hwc_composer_device_1_t *dev, size_t numDisplays,
    hwc_display_contents_1_t** displays){
    int releaseFenceFd = -1;
    int disp, i;
    hwc_display_contents_1_t* psDisplay;
    if((gHwcContext.hwcDebug & SHOW_ALL) == LAYER_DUMP){
        ALOGD("hwc_set");
    }
    hwcOpr->set(displays, numDisplays, &releaseFenceFd);
    gHwcContext.hwcFrameCount ++;
    for(disp =0; disp < numDisplays; disp++){
        psDisplay = displays[disp];
        if(!psDisplay){
            ALOGV("%s: display[%d] was unexpectedly NULL", __func__, disp);
            continue;
        }
        for(i = 0; i < psDisplay->numHwLayers; i++){
            if(psDisplay->hwLayers[i].acquireFenceFd >= 0){
                close(psDisplay->hwLayers[i].acquireFenceFd);
                psDisplay->hwLayers[i].acquireFenceFd = -1;
            }
            if((psDisplay->hwLayers[i].compositionType == HWC_OVERLAY) ||
                (psDisplay->hwLayers[i].compositionType == HWC_FRAMEBUFFER_TARGET)){
                if(psDisplay->hwLayers[i].releaseFenceFd >= 0){
                    close(psDisplay->hwLayers[i].releaseFenceFd);
                }
                if(releaseFenceFd >= 0){
                    psDisplay->hwLayers[i].releaseFenceFd = dup(releaseFenceFd);
                }else{
                    psDisplay->hwLayers[i].releaseFenceFd = -1;
                }
            }else{
                psDisplay->hwLayers[i].releaseFenceFd = -1;
            }
        }
    }
    if(releaseFenceFd >= 0){
        close(releaseFenceFd);
        releaseFenceFd = -1;
    }
    return 0;
}

static int hwc_eventControl(struct hwc_composer_device_1* dev, int disp,
    int event, int enabled){
    hwc_context_t *glbCtx = &gHwcContext;
    ALOGV("%s : para (disp=%d, event=%d, enable=%d)", __func__, disp, event, enabled);
    unsigned long arg[4]={0};
    for(int i = 0; i < glbCtx->displayNum; i++){
        device_info_t *psDeviceInfo = &glbCtx->sunxiDevice[i];
        if(psDeviceInfo->disp.id == INVALID_VALUE
            || psDeviceInfo->disp.id != glbCtx->mainDispId){
            continue;
        }
        switch(event){
        case HWC_EVENT_VSYNC:
            arg[0] = psDeviceInfo->disp.id;
            arg[1] = !!enabled;
            deviceOpr->vsyncEnable(psDeviceInfo->disp.id, !!enabled);
            psDeviceInfo->vsyncEnable = (!!enabled);
            ALOGV("%s disp=%d vsync=%d", __func__, disp, psDeviceInfo->vsyncEnable);
            return 0;
        default:
            return -EINVAL;
        }
    }
    return -EINVAL;
}

static void hwc_register_procs(struct hwc_composer_device_1* dev,
    hwc_procs_t const* procs){
    hwc_context_t *glbCtx = &gHwcContext;
    glbCtx->psHwcProcs = const_cast<hwc_procs_t *>(procs);
}

static int hwc_getDisplayConfigs(struct hwc_composer_device_1 *dev,
    int disp, uint32_t *configs, size_t *numConfigs){
    int err = -EINVAL;
    hwc_context_t *glbCtx = &gHwcContext;
    switch(disp){
    case HWC_DISPLAY_PRIMARY:
        if(numConfigs){
            *numConfigs = 1;
        }
        if(configs){
            configs[0] = 0;
        }
        break;
    case HWC_DISPLAY_EXTERNAL:
        if(glbCtx->secDispId != INVALID_VALUE){
            if(numConfigs){
                *numConfigs = 1;
            }
            if(configs){
                configs[0] = 0;
            }
        }else{
            *numConfigs = 0;
            goto err_out;
        }
        break;
    default:
        goto err_out;
    }
    err = 0;
err_out:
    return err;
}

static int32_t getHWAttribute(const uint32_t attribute,
         device_info_t *psDeviceInfo){
    hwc_context_t *glbCtx = &gHwcContext;
    switch(attribute){
    case HWC_DISPLAY_VSYNC_PERIOD:
        return psDeviceInfo->displayVsyncP;
    case HWC_DISPLAY_WIDTH:
        if(psDeviceInfo->disp.id == glbCtx->mainDispId){
            return glbCtx->mainDispWidth;
        }else if(psDeviceInfo->disp.id == glbCtx->secDispId){
            return glbCtx->secDispWidth;
        }
        break;
    case HWC_DISPLAY_HEIGHT:
        if(psDeviceInfo->disp.id == glbCtx->mainDispId){
            return glbCtx->mainDispHeight;
        }else if(psDeviceInfo->disp.id == glbCtx->secDispId){
            return glbCtx->secDispHeight;
        }
        break;
    case HWC_DISPLAY_DPI_X:
        return psDeviceInfo->displayDPI_X;
    case HWC_DISPLAY_DPI_Y:
        return psDeviceInfo->displayDPI_Y;
    default:
        ALOGE("unknown display attribute %u", attribute);
        return -EINVAL;
    }
    return -EINVAL;
}

static int hwc_getDisplayAttributes(struct hwc_composer_device_1 *dev,
        int disp, uint32_t config, const uint32_t *attributes, int32_t *values)
{
    hwc_context_t *glbCtx = &gHwcContext;
    device_info_t *psDeviceInfo = NULL;
    int i;
    for(i = 0; i < glbCtx->displayNum; i++){
        psDeviceInfo = &glbCtx->sunxiDevice[i];
        if(psDeviceInfo->disp.id == INVALID_VALUE){
            continue;
        }
        if((psDeviceInfo->disp.id == glbCtx->mainDispId && disp == HWC_DISPLAY_PRIMARY)
            || (psDeviceInfo->disp.id == glbCtx->secDispId && disp == HWC_DISPLAY_EXTERNAL)){
            psDeviceInfo = &glbCtx->sunxiDevice[i];
            break;
        }
    }
    if(i == glbCtx->displayNum){
        ALOGE("No hareware display ");
        return -EINVAL;
    }

    for (int i = 0; attributes[i] != HWC_DISPLAY_NO_ATTRIBUTE; i++)
    {
        values[i] = getHWAttribute(attributes[i], psDeviceInfo);
    }
    return 0;
}

static int hwc_device_close(struct hw_device_t *dev){
    return 0;
}

static int hwc_device_open(const struct hw_module_t* module, const char* name,
    struct hw_device_t** device){
    hwc_composer_device_1_t *psHwcDevice;
    hw_device_t *psHwDevice;
    if(strcmp(name, HWC_HARDWARE_COMPOSER)){
        return -EINVAL;
    }
    psHwcDevice = (hwc_composer_device_1_t *)malloc(sizeof(hwc_composer_device_1_t));
    if(!psHwcDevice){
        ALOGD("%s : Failed to allocate memory", __func__);
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
    psHwcDevice->setDisplayParameter  = hwc_setParameter;
    psHwcDevice->getDisplayParameter  = hwc_getParameter;
    psHwcDevice->registerProcs   = hwc_register_procs;
    psHwcDevice->eventControl	 = hwc_eventControl;
	psHwcDevice->blank			 = hwc_blank;
	psHwcDevice->getDisplayConfigs    = hwc_getDisplayConfigs;
	psHwcDevice->getDisplayAttributes = hwc_getDisplayAttributes;
    *device = psHwDevice;
#if(SW_CHIP_PLATFORM == SW_JAWS)
    hwcOpr = &sun9iHwcOpr;
    deviceOpr = &sun9iDeviceOpr;
#elif (SW_CHIP_PLATFORM == SW_EAGLE)
    hwcOpr = &sun8iHwcOpr;
    deviceOpr = &sun8iDeviceOpr;
#else
#error "please select a platform\n"
#endif
    deviceOpr->init(&gHwcContext);
	hwcOpr->init();

    //start thread to receive vsync and hotplug.
    pthread_t sVsyncThread;
    pthread_create(&sVsyncThread, NULL, vsyncThreadWrapper, &gHwcContext);
    return 0;
}
