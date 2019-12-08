#include "../hwc.h"
#include "private.h"
#include "../common/utils.h"
#include "sunxi_display2.h"
#include "component.h"

static layer_info    *psAllLayer;
static channel_info_t channelInfo[4];
static hwc_disp_context_t *dispContext;
#define ALOGV ALOGD

//get the tv mode for system resolution
//the system resolution desides the Buffer Size of the App.
static int getTvMode4SysResolution(void)
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
            break;
        default:
            tvmode = DISP_TV_MOD_1080P_60HZ;
            break;
    }
    return tvmode;
}

static int initDeviceInfo(hwc_context_t *context){
    int refreshRate;
    int arg[4] = {0};
    disp_output dispOutput;
    int tvModeForSysRsl;
    sun8i_disp_private_t *privateData;
    int number = sizeof(sun8iTvPara)/sizeof(tv_para_t);
    for(int i = 0; i < context->displayNum; i++){
        device_info_t *psDeviceInfo = &context->sunxiDevice[i];
        if(psDeviceInfo->disp.id == INVALID_VALUE){
            continue;
        }
        switch(psDeviceInfo->displayType){
        case DISP_OUTPUT_TYPE_LCD:
            arg[0] = psDeviceInfo->disp.id;
            refreshRate = 60;
            psDeviceInfo->displayDPI_X = 160000;
            psDeviceInfo->displayDPI_Y = 160000;
            psDeviceInfo->displayVsyncP = 1000000000 / refreshRate;


            psDeviceInfo->varDisplayWidth = ioctl(context->displayFd, DISP_GET_SCN_WIDTH, arg);
            psDeviceInfo->varDisplayHeight = ioctl(context->displayFd, DISP_GET_SCN_HEIGHT, arg);
            if(context->mainDispType == DISP_OUTPUT_TYPE_LCD){
                context->mainDispWidth = psDeviceInfo->varDisplayWidth;
                context->mainDispHeight = psDeviceInfo->varDisplayHeight;
                ALOGD("LCD, mainDispWidth is %d, mainDispHeight is %d",
                    context->mainDispWidth, context->mainDispHeight);
            }else if(context->secDispType == DISP_OUTPUT_TYPE_LCD){
                context->secDispWidth = psDeviceInfo->varDisplayWidth;
                context->secDispHeight = psDeviceInfo->varDisplayHeight;
                ALOGD("LCD, secDispWidth is %d, secDispHeight is %d",
                    context->secDispWidth, context->secDispHeight);
            }
            break;
        case DISP_OUTPUT_TYPE_HDMI:
            arg[0] = psDeviceInfo->disp.id;
            arg[1] = (int)&dispOutput;
            psDeviceInfo->displayType = DISP_OUTPUT_TYPE_HDMI;
            ioctl(context->displayFd, DISP_GET_OUTPUT, arg);
            psDeviceInfo->displayMode = dispOutput.mode;
            arg[1] = 0;
            psDeviceInfo->varDisplayWidth = ioctl(context->displayFd, DISP_GET_SCN_WIDTH, arg);
            psDeviceInfo->varDisplayHeight = ioctl(context->displayFd, DISP_GET_SCN_HEIGHT, arg);
            if(context->mainDispType == DISP_OUTPUT_TYPE_HDMI){
                if(0 == isDisplayP2P()){
                    //not point to point
                    tvModeForSysRsl = getTvMode4SysResolution();
                    context->mainDispWidth = getInfoOfMode(sun8iTvPara, number, tvModeForSysRsl, WIDTH);
                    context->mainDispHeight = getInfoOfMode(sun8iTvPara, number, tvModeForSysRsl, HEIGHT);
                    ALOGD("%s : HDMI tvMode is %x, mainDispWidth is %d, mainDispHeight is %d",
                        __func__, tvModeForSysRsl, context->mainDispWidth, context->mainDispHeight);
                }else{
                    context->mainDispWidth = psDeviceInfo->varDisplayWidth;
                    context->mainDispHeight = psDeviceInfo->varDisplayHeight;
                }
            }else if(context->secDispType == DISP_OUTPUT_TYPE_HDMI){
                context->secDispWidth = psDeviceInfo->varDisplayWidth;
                context->secDispHeight = psDeviceInfo->varDisplayHeight;
            }
            psDeviceInfo->displayDPI_X = 213000;
            psDeviceInfo->displayDPI_Y = 213000;
            psDeviceInfo->displayVsyncP = 1000000000/getInfoOfMode(sun8iTvPara, number, psDeviceInfo->displayMode, REFRESHRATE);
            break;
        case DISP_OUTPUT_TYPE_TV:
            arg[0] = psDeviceInfo->disp.id;
            psDeviceInfo->displayType = DISP_OUTPUT_TYPE_TV;
            arg[1] = (int)&dispOutput;
            ioctl(context->displayFd, DISP_GET_OUTPUT, arg);
            psDeviceInfo->displayMode = dispOutput.mode;
            psDeviceInfo->varDisplayWidth = ioctl(context->displayFd, DISP_GET_SCN_WIDTH, arg);
            psDeviceInfo->varDisplayHeight = ioctl(context->displayFd, DISP_GET_SCN_HEIGHT, arg);
            //Caution: do not use point to point in CVBS mode, because it make ui see worst.
            if(context->mainDispType == DISP_OUTPUT_TYPE_TV){
                tvModeForSysRsl = getTvMode4SysResolution();
                context->mainDispHeight = getInfoOfMode(sun8iTvPara, number, tvModeForSysRsl, HEIGHT);
                context->mainDispWidth = getInfoOfMode(sun8iTvPara, number, tvModeForSysRsl, WIDTH);
                ALOGD("CVBS tvMode is %x, mainDispWidth is %d, mainDispHeight is %d", psDeviceInfo->displayMode,
                    context->mainDispWidth, context->mainDispHeight);
            }else if(context->secDispType == DISP_OUTPUT_TYPE_TV){
                context->secDispWidth = psDeviceInfo->varDisplayWidth;
                context->secDispHeight = psDeviceInfo->varDisplayHeight;
                ALOGD("CVBS tvMode is %x, secDispWidth is %d, secDispHeight is %d", psDeviceInfo->displayMode,
                    context->secDispWidth, context->secDispHeight);
            }
            psDeviceInfo->displayDPI_X = 213000;
            psDeviceInfo->displayDPI_Y = 213000;
            psDeviceInfo->displayVsyncP = 1000000000/getInfoOfMode(sun8iTvPara, number, psDeviceInfo->displayMode,REFRESHRATE);
            break;
        case DISP_OUTPUT_TYPE_VGA:
            break;
        default:
            ALOGD("%s : not support type %d", __func__, psDeviceInfo->displayType);
        }
        psDeviceInfo->displayPercentHT = 100;
        psDeviceInfo->displayPercentWT = 100;
        psDeviceInfo->current3DMode = DISPLAY_2D_ORIGINAL;
        psDeviceInfo->vsyncEnable = 1;
        privateData = (sun8i_disp_private_t *)(psDeviceInfo->disp.privateData);
        privateData->isSecure = 1;
        privateData->hwChannelNum = (psDeviceInfo->disp.id) ? 2:NUMCHANNELOFDSP;
        privateData->layerNumOfCh = NUMLAYEROFCHANNEL;
        privateData->videoChNum = NUMCHANNELOFVIDEO;
        privateData->enhanceMode = 0;
    }
    return 0;
}

int ionHandleAddRef(hwc_layer_1_t *psLayer){
    hwc_context_t *glbCtx = &gHwcContext;
    sun8i_hwc_private_t *privateData = (sun8i_hwc_private_t *)glbCtx->hwcPrivate;
    IMG_native_handle_t* handle = (IMG_native_handle_t*)psLayer->handle;
    ion_list * item;
    ion_list * head = privateData->ionHandleHead;
    struct ion_fd_data data ;
    int ret = -1;

    if(handle == NULL){
        return -1;
    }

    data.fd = handle->fd[0];
    ret = ioctl(glbCtx->ionFd, ION_IOC_IMPORT, &data);
    if (ret < 0){
        ALOGE("#######ion_import  error#######");
        return 0;
    }
    ALOGV("########add %d %p",glbCtx->hwcFrameCount, data.handle);

    item = (ion_list *)malloc(sizeof(ion_list));
    item->frameIndex = glbCtx->hwcFrameCount;
    item->fd = data.fd;
    item->handle = data.handle;
    if(head == NULL){
        head = item;
        item->next = item;
        item->prev= item;
    }else{
        item->next = head;
        item->prev = head->prev;
        head->prev->next = item;
        head->prev = item;
        head = item;
    }
    privateData->ionHandleHead = head;
    return 0;
}

int ionHandleDecRef(void){
    hwc_context_t *glbCtx = &gHwcContext;
    sun8i_hwc_private_t *privateData = (sun8i_hwc_private_t *)glbCtx->hwcPrivate;
    ion_list * head = privateData->ionHandleHead;
    ion_list * item;
    ion_handle_data freedata;

    if(head == NULL)
    {
        return 0;
    }

    item = head->prev;
    while(item && item->frameIndex < glbCtx->hwcFrameCount - 1){
        int ret = -1;
	    freedata.handle = item->handle;
        ret = ioctl(glbCtx->ionFd, ION_IOC_FREE, &freedata);
        if(ret < 0){
            ALOGE("ION_IOC_FREE(err=%d)",ret);
            return 0;
        }
        ALOGV("########remove %d %p",item->frameIndex, item->handle);

        if(item == head){
            free(item);
            item = NULL;
            head = NULL;
        }else{
            ion_list * itemTmp;
            itemTmp = item->prev;
            item->next->prev = item->prev;
            item->prev->next = item->next;
            free(item);
            item = itemTmp;
        }
        privateData->ionHandleHead = head;
    }

    return 0;
}


void sun8iHwcCreateContext(hwc_context_t *context){
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

    //init the hardware private data.
    context->hwcPrivate = (sun8i_hwc_private_t*)calloc(1, sizeof(sun8i_hwc_private_t));
    memset(context->hwcPrivate, 0, sizeof(sun8i_hwc_private_t));
    //init display number
    context->displayNum = 2;
    //init sunxidevice
    context->sunxiDevice = (device_info_t* )calloc(context->displayNum, sizeof(device_info_t));
    memset(context->sunxiDevice, 0, context->displayNum * sizeof(device_info_t));
    for(int i = 0; i < context->displayNum; i++){
        context->sunxiDevice[i].disp.id = INVALID_VALUE;
        context->sunxiDevice[i].disp.privateData = (sun8i_disp_private_t *)calloc(1, sizeof(sun8i_disp_private_t));
        memset(context->sunxiDevice[i].disp.privateData, 0, sizeof(sun8i_disp_private_t));
    }
    //init display context;
    dispContext = (hwc_disp_context_t*)calloc(context->displayNum, sizeof(hwc_disp_context_t));
    memset(dispContext, 0, context->displayNum * sizeof(hwc_disp_context_t));
    //init pvPrivateData;
    context->pvPrivateData = (setup_dispc_data_t *)calloc(1, sizeof(setup_dispc_data_t));
    memset(context->pvPrivateData, 0, sizeof(setup_dispc_data_t));
    //init hwcPrivate

    //init the main and sec display device
    unsigned long arg[4] = {0};
    disp_output dispOutput;
    arg[0] = DISP_CHN_0;
    arg[1] = (int)&dispOutput;
    ioctl(context->displayFd, DISP_GET_OUTPUT, arg);
    int type0 = dispOutput.type;
    int mode0 = dispOutput.mode;
    arg[0] = DISP_CHN_1;
    ioctl(context->displayFd, DISP_GET_OUTPUT, arg);
    int type1 = dispOutput.type;
    int mode1 = dispOutput.mode;
    context->mainDispId = INVALID_VALUE;
    device_info_t *psDeviceInfo0 = &context->sunxiDevice[0];
    device_info_t *psDeviceInfo1 = &context->sunxiDevice[1];
    //init the main disp
    if(type0 == DISP_OUTPUT_TYPE_HDMI || type1 == DISP_OUTPUT_TYPE_HDMI){
        psDeviceInfo0->displayType = DISP_OUTPUT_TYPE_HDMI;
        context->mainDispType = DISP_OUTPUT_TYPE_HDMI;
        if(type0 == DISP_OUTPUT_TYPE_HDMI){
            psDeviceInfo0->disp.id = DISP_CHN_0;
            context->mainDispId = DISP_CHN_0;
            context->mainDispMode = mode0;
        }else{
            psDeviceInfo0->disp.id = DISP_CHN_1;
            context->mainDispId = DISP_CHN_1;
            context->mainDispMode = mode1;
        }
    }else if(type0 == DISP_OUTPUT_TYPE_TV || type1 == DISP_OUTPUT_TYPE_TV){
        psDeviceInfo0->displayType = DISP_OUTPUT_TYPE_TV;
        context->mainDispType = DISP_OUTPUT_TYPE_TV;
        if(type0 == DISP_OUTPUT_TYPE_TV){
            psDeviceInfo0->disp.id = DISP_CHN_0;
            context->mainDispId = DISP_CHN_0;
            context->mainDispMode = mode0;
        }else{
            psDeviceInfo0->disp.id = DISP_CHN_1;
            context->mainDispId = DISP_CHN_1;
            context->mainDispMode = mode1;
        }
    }else if(type0 == DISP_OUTPUT_TYPE_LCD || type1 == DISP_OUTPUT_TYPE_LCD){
        psDeviceInfo0->displayType = DISP_OUTPUT_TYPE_LCD;
        context->mainDispType = DISP_OUTPUT_TYPE_LCD;
        if(type0 == DISP_OUTPUT_TYPE_LCD){
            psDeviceInfo0->disp.id = DISP_CHN_0;
            context->mainDispId = DISP_CHN_0;
            context->mainDispMode = mode0;
        }else{
            psDeviceInfo0->disp.id = DISP_CHN_1;
            context->mainDispId = DISP_CHN_1;
            context->mainDispMode = mode1;
        }
    }else{
        psDeviceInfo0->displayType = DISP_OUTPUT_TYPE_NONE;
        context->mainDispType = DISP_OUTPUT_TYPE_NONE;
        context->mainDispId = INVALID_VALUE;
        context->mainDispMode = INVALID_VALUE;
    }
    //int the second display device
    context->secDispId = INVALID_VALUE;
    if(context->mainDispType != DISP_OUTPUT_TYPE_TV
        &&(type0 == DISP_OUTPUT_TYPE_TV || type1 == DISP_OUTPUT_TYPE_TV)){
        psDeviceInfo1->displayType = DISP_OUTPUT_TYPE_TV;
        context->secDispType = DISP_OUTPUT_TYPE_TV;
        if(type0 == DISP_OUTPUT_TYPE_TV){
            psDeviceInfo1->disp.id = DISP_CHN_0;
            context->secDispId = DISP_CHN_0;
            context->secDispMode = mode0;
        }else{
            psDeviceInfo1->disp.id = DISP_CHN_1;
            context->secDispId = DISP_CHN_1;
            context->secDispMode = mode1;
        }
    }else if(context->mainDispType != DISP_OUTPUT_TYPE_LCD
        &&(type0 == DISP_OUTPUT_TYPE_LCD || type1 == DISP_OUTPUT_TYPE_LCD)){
        psDeviceInfo1->displayType = DISP_OUTPUT_TYPE_LCD;
        context->secDispType = DISP_OUTPUT_TYPE_LCD;
        if(type0 == DISP_OUTPUT_TYPE_LCD){
            psDeviceInfo1->disp.id = DISP_CHN_0;
            context->secDispId = DISP_CHN_0;
            context->secDispMode = mode0;
        }else{
            psDeviceInfo1->disp.id = DISP_CHN_1;
            context->secDispId = DISP_CHN_1;
            context->secDispMode = mode1;
        }
    }
    initDeviceInfo(context);

    //init others
    context->screenRadio = 1;
    context->canForceGpuCom = true;
    context->forceGPUComp = false;
    context->fBeginTime = 0.0;
    context->uiBeginFrame = 0;
    context->hwcDebug = 0;

    pthread_t tid;
    pthread_create(&tid, NULL, vsyncThreadWrapper, context);
}

static void reCountPercent(device_info_t *psDeviceInfo){
    if(psDeviceInfo->displayType == DISP_OUTPUT_TYPE_HDMI){
        if(psDeviceInfo->displayMode != DISP_TV_MOD_3840_2160P_25HZ &&
            psDeviceInfo->displayMode != DISP_TV_MOD_3840_2160P_24HZ &&
            psDeviceInfo->displayMode != DISP_TV_MOD_3840_2160P_30HZ &&
            psDeviceInfo->current3DMode != DISPLAY_3D_LEFT_RIGHT_HDMI &&
            psDeviceInfo->current3DMode != DISPLAY_3D_TOP_BOTTOM_HDMI &&
            psDeviceInfo->current3DMode != DISPLAY_3D_DUAL_STREAM){

            psDeviceInfo->displayPercentW = psDeviceInfo->displayPercentWT;
            psDeviceInfo->displayPercentH = psDeviceInfo->displayPercentHT;
        }else{
            psDeviceInfo->displayPercentW = 100;
            psDeviceInfo->displayPercentH = 100;

        }
    }else if(psDeviceInfo->displayType == DISP_OUTPUT_TYPE_TV
        || psDeviceInfo->displayType == DISP_OUTPUT_TYPE_VGA){
        psDeviceInfo->displayPercentW = psDeviceInfo->displayPercentWT;
        psDeviceInfo->displayPercentH = psDeviceInfo->displayPercentHT;
    }else{
        psDeviceInfo->displayPercentW = 100;
        psDeviceInfo->displayPercentH = 100;
    }
}

static void resetGlobDevice(hwc_context_t *glbCtx){
    int i = 0;
    setup_dispc_data_t* displayData = (setup_dispc_data_t*)(glbCtx->pvPrivateData);
    memset(displayData, 0, sizeof(setup_dispc_data_t));
    for(i = 0; i < glbCtx->displayNum; i++){
        if(glbCtx->sunxiDevice[i].disp.id != INVALID_VALUE){
            reCountPercent(&glbCtx->sunxiDevice[i]);
        }
    }
}

static void resetLayerType(hwc_display_contents_1_t* list,layer_info *pslayers, int hwctype){
    unsigned int j = 0;
    if (list && list->numHwLayers >= 1)
    {
        for(j = 0; j < list->numHwLayers; j++)
        {
            if(list->hwLayers[j].compositionType != HWC_FRAMEBUFFER_TARGET)
            {
               list->hwLayers[j].compositionType = hwctype;
            }
            pslayers[j].psLayer = &list->hwLayers[j];
            pslayers[j].assigned = ASSIGN_INIT;
            pslayers[j].hwChannel = -2;
            pslayers[j].virChannel = -2;
            pslayers[j].hwzOrder  = -1;
            pslayers[j].origOrder = -1;
            pslayers[j].is3D = 0;
            pslayers[j].assignInfo = DEFAULT;
        }
    }
}

static void resetLocalInfo(device_info_t *psDeviceInfo, hwc_disp_context_t *localCtx,
    hwc_display_contents_1_t *psDisplay, layer_info_t *allLayers){
    int i = 4, j = 0;
    localCtx->hwChnUsedCnt = -1;
    localCtx->videoChnCnt = -1;
    localCtx->haveVideo = 0;
    localCtx->psAllLayer = allLayers;
    localCtx->numberOfLayer = psDisplay->numHwLayers;
    localCtx->psDeviceInfo = psDeviceInfo;
    localCtx->fbHasVideo = 0;
    calculateFactor(psDeviceInfo, &localCtx->widthScaleFactor,&localCtx->hightScaleFactor);
    resetLayerType(psDisplay, allLayers, HWC_FRAMEBUFFER);
    while(i--)
    {
        j = 4;
        localCtx->channelInfo[i].hasBlend = 0;
        localCtx->channelInfo[i].hasVideo= 0;
        localCtx->channelInfo[i].wtScaleFactor = 1.0;
        localCtx->channelInfo[i].htScaleFactor = 1.0;
        localCtx->channelInfo[i].iChnFormat = 0;
        localCtx->channelInfo[i].planeAlpha = 0xff;
        localCtx->channelInfo[i].hwLayerCnt = 0;
        while(j--)
        {
            localCtx->channelInfo[i].hwLayer[j] = NULL;
        }
    }
}

static unsigned int cntOfLayerMem(hwc_layer_1_t *psLayer){
    unsigned int hight, width;
    IMG_native_handle_t* handle = (IMG_native_handle_t*)psLayer->handle;
    if(handle->iFormat == HAL_PIXEL_FORMAT_YV12 || handle->iFormat == HAL_PIXEL_FORMAT_YCrCb_420_SP)
    {
       width = ALIGN(handle->iWidth, YV12_ALIGN);
    }else{
       width = ALIGN(handle->iWidth, HW_ALIGN);
    }
    return handle->uiBpp * width * handle->iHeight;
}

static int hwcUsageSW(IMG_native_handle_t *psHandle)
{
	return psHandle->usage & (GRALLOC_USAGE_SW_READ_OFTEN |
							  GRALLOC_USAGE_SW_WRITE_OFTEN);
}

static int hwcUsageSWwrite(IMG_native_handle_t *psHandle)
{
	return psHandle->usage & GRALLOC_USAGE_SW_WRITE_OFTEN;
}

static int hwcUsageProtected(IMG_native_handle_t *psHandle)
{
	return psHandle->usage & GRALLOC_USAGE_PROTECTED;
}

static inline int hwcValidFormat(int format){
    switch(format){
    case HAL_PIXEL_FORMAT_RGBA_8888:
    case HAL_PIXEL_FORMAT_RGBX_8888:
    case HAL_PIXEL_FORMAT_RGB_888:
    case HAL_PIXEL_FORMAT_RGB_565:
    case HAL_PIXEL_FORMAT_BGRA_8888:
    case HAL_PIXEL_FORMAT_sRGB_A_8888:
    case HAL_PIXEL_FORMAT_sRGB_X_8888:
    case HAL_PIXEL_FORMAT_YV12:
	case HAL_PIXEL_FORMAT_YCrCb_420_SP:
    case HAL_PIXEL_FORMAT_BGRX_8888:
        return 1;
    default:
        return 0;
    }
}

static int hwcIsValidLayer(hwc_layer_1_t *layer){
    IMG_native_handle_t *handle = (IMG_native_handle_t *)layer->handle;
    if ((layer->flags & HWC_SKIP_LAYER)){
        return D_SKIP_LAYER;
    }
    if (!hwcValidFormat(handle->iFormat)){
        return D_NO_FORMAT;
    }
    if (layer->compositionType == HWC_BACKGROUND){
        return D_BACKGROUND;
    }
    if(layer->transform){
        return D_TR_N_0;
    }
    return I_OVERLAY;
}

static bool hwcSupportAlpha(int format){
    switch(format){
    case HAL_PIXEL_FORMAT_RGBA_8888:
    case HAL_PIXEL_FORMAT_RGBX_8888:
    case HAL_PIXEL_FORMAT_RGB_888:
    case HAL_PIXEL_FORMAT_RGB_565:
    case HAL_PIXEL_FORMAT_BGRA_8888:
    case HAL_PIXEL_FORMAT_sRGB_A_8888:
    case HAL_PIXEL_FORMAT_sRGB_X_8888:
    case HAL_PIXEL_FORMAT_BGRX_8888:
        return 1;
    default:
        return 0;
    }
}

static bool hwcIsScaled(hwc_layer_1_t *layer,float *wScaleFactor, float * hScaleFactor)
{
    bool ret = 0;

#ifdef HWC_1_3
    float w = layer->sourceCropf.right - layer->sourceCropf.left;
    float h = layer->sourceCropf.bottom - layer->sourceCropf.top;
    ret = ((layer->displayFrame.right - layer->displayFrame.left) != int(ceilf(w)))
            || ((layer->displayFrame.bottom - layer->displayFrame.top) != int(ceilf(h)));
#else
    int w = layer->sourceCrop.right - layer->sourceCrop.left;
    int h = layer->sourceCrop.bottom - layer->sourceCrop.top;
    ret = ((layer->displayFrame.right - layer->displayFrame.left) != w)
            || ((layer->displayFrame.bottom - layer->displayFrame.top) != h);
#endif
    if(ret)
    {
        *wScaleFactor = float(layer->displayFrame.right - layer->displayFrame.left) / float(w);
        *hScaleFactor = float(layer->displayFrame.bottom - layer->displayFrame.top) / float(h);
    }
    return ret;
}

static bool checkSameScale(float srWScaleFac, float srHScaleFac, float dtWScalFac, float dtHScaleFac )
{
    return (((srWScaleFac - dtWScalFac) > -0.001) && ((srWScaleFac - dtWScalFac) < 0.001))
           &&(((srHScaleFac - dtHScaleFac) > -0.001)&&((srHScaleFac - dtHScaleFac) < 0.001));
}

static bool checkScaleFormat(int format){
    switch(format){
    case HAL_PIXEL_FORMAT_YV12:
	case HAL_PIXEL_FORMAT_YCrCb_420_SP:
    case HAL_PIXEL_FORMAT_AW_NV12:
    case HAL_PIXEL_FORMAT_RGBA_8888:
        return 1;
    default:
        return 0;
    }
}

static int hwcCanScale(hwc_disp_context_t *localCtx,hwc_layer_1_t * psLayer)
{
   int src_w,frame_rate,lcd_h,lcd_w,dst_w;
   long long layer_line_peroid,lcd_line_peroid,de_freq;
#ifdef HWC_1_3
    src_w = int (ceilf(psLayer->sourceCropf.right - psLayer->sourceCropf.left));
#else
    src_w = psLayer->sourceCrop.right - psLayer->sourceCrop.left;
#endif
    const device_info_t *psDeviceInfo = localCtx->psDeviceInfo;
    lcd_w = psDeviceInfo->varDisplayWidth;
    lcd_h = psDeviceInfo->varDisplayHeight;
	dst_w = (int)(psLayer->displayFrame.right * localCtx->widthScaleFactor) - (int)(psLayer->displayFrame.left * localCtx->widthScaleFactor);
    frame_rate = 60;
    if(psDeviceInfo->current3DMode == DISPLAY_3D_LEFT_RIGHT_HDMI)
    {
        dst_w /=2;
        frame_rate = 30;
    }
    de_freq = 224000000;
	lcd_line_peroid = 1000000000/frame_rate/lcd_h;
    layer_line_peroid = (src_w > dst_w)?(1000000*((long long)(lcd_w - dst_w + src_w))/(de_freq/1000)):(1000000*((long long)lcd_w)/(de_freq/1000));

    if((lcd_line_peroid *4/5) < layer_line_peroid)
        return 0; //can't
    else
        return 1;//can
}

int checkXList(hwc_layer_1_t *psLayer, hwc_disp_context_t *localCtx, int from, int to, int channel, int type){
    const device_info_t *psDeviceInfo = localCtx->psDeviceInfo;
    sun8i_disp_private_t *privateData = (sun8i_disp_private_t *)(psDeviceInfo->disp.privateData);
    layer_info_t *dispInfo = localCtx->psAllLayer;
    dispInfo += from;
    while(from >= 0 && from <= to && to <= localCtx->numberOfLayer ){
        if((dispInfo->psLayer != NULL)
            && ((channel >= -1 && channel < privateData->hwChannelNum ) ? dispInfo->virChannel == channel : 1 )
            && ((type == ASSIGN_GPU || type == ASSIGN_OVERLAY) ? dispInfo->assigned == type : 1)
            && hwcRegionIntersect(&dispInfo->psLayer->displayFrame, &psLayer->displayFrame)){

            return 1;
        }
        dispInfo++;
        from++;
    }
    return 0;
}

static int matchNoFullChannel(hwc_disp_context_t *localCtx, int channel, bool isVideo,int emptyNum,int format,float srWScaleFac, float srHScaleFac, unsigned char planeAlpha){
    channel_info_t *chn = NULL;
    const device_info_t *psDeviceInfo = localCtx->psDeviceInfo;
    sun8i_disp_private_t *privateData = (sun8i_disp_private_t *)psDeviceInfo->disp.privateData;
    int whileCnt,tmpCh;
    (channel >= 0 && channel < privateData->hwChannelNum) ? whileCnt = 1, tmpCh = channel : whileCnt = localCtx->hwChnUsedCnt +1 , tmpCh = 0;

    while(whileCnt --){
        chn = &localCtx->channelInfo[tmpCh];
        if(chn->hwLayerCnt != 0
            ?
             ((chn->hwLayerCnt + emptyNum) <= privateData->layerNumOfCh
             && isVideo ? ((chn->hasVideo) && (chn->iChnFormat == format)) : (chn->hasVideo ? 0 : 1)
             && chn->planeAlpha == planeAlpha
             && checkSameScale(chn->wtScaleFactor,chn->htScaleFactor,srWScaleFac,srHScaleFac))
            :
              1 ){
            return tmpCh;
        }
        tmpCh++;
    }
    return -1;
}

static inline int findChannelLayer(channel_info_t *channelInfo , bool last ){
   layer_info_t *dispInfo = NULL;
   if(channelInfo->hwLayerCnt == 0){
        return -1;
   }
   if(!last){
        dispInfo = channelInfo->hwLayer[0];
   }else{
        dispInfo = channelInfo->hwLayer[channelInfo->hwLayerCnt-1];
   }
   ALOGV("####findChannelLayer###");
   int ret = dispInfo == NULL ? -1 : dispInfo->origOrder;
   ALOGV("####end###");
   return ret;
}

int hwcTryAssignLayer(hwc_disp_context_t *localCtx, size_t singCout,int zOrder)
{

    bool needChannel = 1, isVideo = 0,isAlpha = 0, isFB = 0;
    float wScaleFac = 1.0, hScaleFac = 1.0;
    int chn = -1, tmCnt1 = 0, tmCnt2 = 0, addLayerCnt = 1, chDiff = 0,tmpChn = 0 ;
    int dueto = I_OVERLAY;
    unsigned char planeAlpha = 0xff;
    hwc_layer_1_t *psLayer;
    psLayer = localCtx->psAllLayer[singCout].psLayer;
    const device_info_t *psDeviceInfo = localCtx->psDeviceInfo;
    sun8i_disp_private_t *privateData = (sun8i_disp_private_t *)(psDeviceInfo->disp.privateData);
    IMG_native_handle_t* handle = (IMG_native_handle_t*)psLayer->handle;
    channel_info_t *psChn = localCtx->channelInfo;

    if(psLayer->compositionType == HWC_FRAMEBUFFER_TARGET ){
        if(handle != NULL){
            isFB = 1;
            isAlpha = 1;
            goto needchannel;
        }else{
            return ASSIGN_FAILED;
        }
    }
    if(hwcIsBlended(psLayer) && singCout == 0){
        psLayer->blending = HWC_BLENDING_NONE;
    }
    if(handle == NULL){
        ALOGV("%s:Buffer handle is NULL", __func__);
        localCtx->usedFB = ASSIGN_GPU;
        dueto = D_NULL_BUF;
	    goto assign_gpu;
    }
    if(!(handle->uiFlags & PVRSRV_MEM_CONTIGUOUS)){
        ALOGV("%s:not continuous Memory", __func__);
        localCtx->usedFB = ASSIGN_GPU;
        dueto = D_CONTIG_MEM;
	    goto assign_gpu;
    }

	if(hwcUsageProtected(handle) && privateData->isSecure){
        ALOGV("%s:Video Protected", __func__);
        dueto = D_VIDEO_PD;
	    goto assign_gpu;
	}

    dueto = hwcIsValidLayer(psLayer);
    if(dueto != I_OVERLAY){
        ALOGV("hwcIsValidLayer:0x%08x", handle->iFormat);
        goto assign_gpu;
    }

    if(hwcUsageSW(handle) && !checkVideoFormat(handle->iFormat)){
        ALOGV("not vedo  and   GRALLOC_USAGE_SW_WRITE_OFTEN");
        dueto = D_SW_OFTEN;
        goto assign_gpu;
    }

    if(hwcIsBlended(psLayer)){
        planeAlpha = psLayer->planeAlpha;
        isAlpha = 1;//cann't distinguish only pixel alpha
        if(!hwcSupportAlpha(handle->iFormat)){
            ALOGV("not surpport alpha layer");
            dueto = D_CANNT_SCALE;
			goto assign_gpu;
        }

        if( hwcIsScaled(psLayer, &wScaleFac, &hScaleFac)
            || !checkSameScale(localCtx->widthScaleFactor, localCtx->hightScaleFactor,1.0,1.0)){
            if(checkScaleFormat(handle->iFormat))
            {
                if(!hwcCanScale(localCtx, psLayer))
			    {
                    ALOGV("Scale can not handle");
                    dueto = D_SCALE_OUT;
			        goto assign_gpu;
                }
            }else{
                ALOGV("not support  scale layer");
                dueto = D_CANNT_SCALE;
			    goto assign_gpu;
            }
        }

        if(localCtx->usedFB)
        {
            if(checkXList(psLayer, localCtx, 0, singCout-1, -1, ASSIGN_GPU))
            {
                dueto = D_X_FB;
                goto assign_gpu;
            }
        }

    }else if(hwcIsScaled(psLayer, &wScaleFac, &hScaleFac)
               || !checkSameScale(localCtx->widthScaleFactor,localCtx->hightScaleFactor,1.0,1.0))
    {
        if(checkScaleFormat(handle->iFormat))
        {
			if(!hwcCanScale(localCtx, psLayer))
			{
			    ALOGV("Scale can not used");
                dueto = D_SCALE_OUT;
			    goto assign_gpu;
			}
		}else{
            ALOGV("not support scale layer");
            dueto = D_CANNT_SCALE;
			goto assign_gpu;
        }
    }

    isVideo = checkVideoFormat(handle->iFormat);
    if(isVideo && is3DMode(psDeviceInfo->current3DMode)){
        //reserver two layers for 3D.
        addLayerCnt = 2;
    }
needchannel:
    chn = localCtx->hwChnUsedCnt;
    tmpChn = matchNoFullChannel(localCtx, -1, isVideo, addLayerCnt, handle->iFormat, wScaleFac, hScaleFac, planeAlpha);

    while(tmpChn!= -1 && chn !=-1 && chn >= tmpChn){// can assigned to the lowest Channel?
        tmCnt1 =  findChannelLayer(&localCtx->channelInfo[chn], 0);
        tmCnt2 =  findChannelLayer(&localCtx->channelInfo[chn], 1);
        if((chn == tmpChn) || (matchNoFullChannel(localCtx, chn, isVideo, addLayerCnt, handle->iFormat, wScaleFac, hScaleFac, planeAlpha) != -1) ){
            if(checkXList(psLayer, localCtx, tmCnt1, tmCnt2, chn, ASSIGN_OVERLAY)){
                if(!isAlpha){
                    chDiff = localCtx->hwChnUsedCnt - chn;
                    needChannel = 0;
                }
                chn = -1;
            }else{
                chDiff = localCtx->hwChnUsedCnt - chn;
                needChannel = 0;
                chn--;
            }

        }else if(checkXList(psLayer, localCtx, tmCnt1, tmCnt2, chn, ASSIGN_OVERLAY)){
            chn = -1;
        }else{
            chn--;
        }
    }

    if(needChannel){
        if((isVideo ? (localCtx->haveVideo ? localCtx->videoChnCnt < privateData->videoChNum -1 : 1) : 1)
           && localCtx->hwChnUsedCnt < (privateData->hwChannelNum - (isFB ? 0: localCtx->usedFB) -1)){
            //here is a fix  :UI has used video channel ,wether reasigend?
            //could add check video before hwc_try_assign_layer() but must Check_X_List() between the videos
            localCtx->hwChnUsedCnt++;
            isVideo ? localCtx->videoChnCnt++ :0;
        }else{
            goto assigned_need_resigne;
        }
    }
    if(localCtx->usedFB && !isAlpha && !isFB && checkXList(psLayer, localCtx, 0, singCout-1, -1, ASSIGN_GPU ))
    {
        psLayer->hints |= HWC_HINT_CLEAR_FB;
    }

assign_overlay:
    localCtx->haveVideo += isVideo;
    psChn[localCtx->hwChnUsedCnt - chDiff].hasVideo = isVideo;
    psChn[localCtx->hwChnUsedCnt - chDiff].iChnFormat = handle->iFormat;
    psChn[localCtx->hwChnUsedCnt - chDiff].wtScaleFactor = wScaleFac * localCtx->widthScaleFactor;
    psChn[localCtx->hwChnUsedCnt - chDiff].htScaleFactor = hScaleFac * localCtx->hightScaleFactor;

    psChn[localCtx->hwChnUsedCnt - chDiff].planeAlpha = planeAlpha;
    psChn[localCtx->hwChnUsedCnt - chDiff].hwLayer[psChn[localCtx->hwChnUsedCnt - chDiff].hwLayerCnt] = &localCtx->psAllLayer[singCout];
    psChn[localCtx->hwChnUsedCnt - chDiff].hwLayerCnt += addLayerCnt;
    psChn[localCtx->hwChnUsedCnt - chDiff].isFB = isFB;
    psChn[localCtx->hwChnUsedCnt - chDiff].hasBlend ? 0: psChn[localCtx->hwChnUsedCnt - chDiff].hasBlend = isAlpha;

    localCtx->psAllLayer[singCout].assigned = ASSIGN_OVERLAY;
    localCtx->psAllLayer[singCout].virChannel = localCtx->hwChnUsedCnt- chDiff;
    localCtx->psAllLayer[singCout].hwzOrder = zOrder;
    localCtx->psAllLayer[singCout].origOrder = singCout;
    localCtx->psAllLayer[singCout].is3D = (addLayerCnt == 2 ? 1 : 0);
    localCtx->psAllLayer[singCout].assignInfo = dueto;
    return ASSIGN_OVERLAY;

assign_gpu:

    localCtx->fbHasVideo += (handle != NULL ? checkVideoFormat(handle->iFormat):0);
    localCtx->psAllLayer[singCout].assigned = ASSIGN_GPU;
    localCtx->psAllLayer[singCout].virChannel = -1;
    localCtx->psAllLayer[singCout].hwzOrder = -1;
    localCtx->psAllLayer[singCout].origOrder = singCout;
    localCtx->usedFB = ASSIGN_GPU;
    localCtx->psAllLayer[singCout].assignInfo = dueto;
    return ASSIGN_GPU;

assigned_need_resigne:
    if((!localCtx->usedFB) || isFB)
    {
        localCtx->usedFB = ASSIGN_GPU ;
        return ASSIGN_NEEDREASSIGNED;
    }else{
        dueto = D_NO_PIPE;
        goto assign_gpu;
    }
}

int sun8iHwcPrepare(hwc_display_contents_1_t **displays, size_t numDisplays){
    bool forceSoftwareRendering = 0;
    hwc_display_contents_1_t *psDisplay;
    size_t disp , i, hwDisp=0;
    unsigned int sizeOfMem = 0;
    int assignStatus;
    int needReAssignedLayer = 0;
    hwc_layer_1_t *psLayer;
    int setOrder = 0;
    hwc_context_t *glbCtx = &gHwcContext;
    setup_dispc_data_t *displayData = (setup_dispc_data_t *)glbCtx->pvPrivateData;
    hwc_disp_context_t *localCtx = NULL;
    device_info_t *psDeviceInfo;
    int err = 0;
    resetGlobDevice(glbCtx);
    for(disp = 0; disp < glbCtx->displayNum; disp++){
        needReAssignedLayer = 0;
        setOrder = 0;
        psDisplay = displays[disp];
        if(!psDisplay){
            continue;
        }
        if(psDisplay->outbuf != NULL || psDisplay->outbufAcquireFenceFd != 0){
            if(psDisplay->retireFenceFd >= 0){
                close(psDisplay->retireFenceFd);
                psDisplay->retireFenceFd = -1;
            }
            if(psDisplay->outbuf != NULL){
                psDisplay->outbuf = NULL;
            }
            if(psDisplay->outbufAcquireFenceFd >= 0){
                close(psDisplay->outbufAcquireFenceFd);
                psDisplay->outbufAcquireFenceFd = -1;
            }
        }
        switch(disp){
        case HWC_DISPLAY_PRIMARY:
            for(i = 0; i < glbCtx->displayNum; i++){
                if(glbCtx->sunxiDevice[i].disp.id != INVALID_VALUE
                    && glbCtx->sunxiDevice[i].disp.id == glbCtx->mainDispId){
                    psDeviceInfo = &glbCtx->sunxiDevice[i];
                    hwDisp = i;
                    break;
                }
            }
            if(i == glbCtx->displayNum){
                continue;
            }
            break;
        case HWC_DISPLAY_EXTERNAL:
            for(i = 0; i < glbCtx->displayNum; i++){
                if(glbCtx->sunxiDevice[i].disp.id != INVALID_VALUE
                    && glbCtx->sunxiDevice[i].disp.id == glbCtx->secDispId){
                    psDeviceInfo = &glbCtx->sunxiDevice[i];
                    hwDisp = i;
                    break;
                }
            }
            if(i == glbCtx->displayNum){
                continue;
            }
            break;
        case HWC_DISPLAY_VIRTUAL:
        default:
            ALOGV("%s : Virtual displays are not supported", __func__);
            return -1;
        }

        layer_info_t *allLayers = (layer_info_t*)calloc(psDisplay->numHwLayers, sizeof(layer_info_t));
        memset(allLayers, 0, (sizeof(layer_info_t))*psDisplay->numHwLayers);
        localCtx = &dispContext[hwDisp];
        memset(localCtx, 0, sizeof(hwc_disp_context_t));
ReAssignedLayer:
        resetLocalInfo(psDeviceInfo, localCtx, psDisplay, allLayers);
        for(i = (forceSoftwareRendering ? psDisplay->numHwLayers-1:0); i < psDisplay->numHwLayers; i++){
            psLayer = &psDisplay->hwLayers[i];
            if(i >= psDisplay->numHwLayers - 1){
                if(psDisplay->numHwLayers == 1 || localCtx->usedFB
                    || forceSoftwareRendering || sizeOfMem > cntOfLayerMem(&psDisplay->hwLayers[psDisplay->numHwLayers - 1])){
                    if((glbCtx->forceGPUComp && !localCtx->haveVideo)
                        || forceSoftwareRendering || localCtx->usedFB){
                        ALOGV("Force GPU Composer");
                        setOrder = 0;
                        resetLocalInfo(psDeviceInfo, localCtx, psDisplay, allLayers);
                    }
                    if(glbCtx->forceGPUComp && localCtx->haveVideo &&
                        !localCtx->usedFB && psDisplay->numHwLayers != 1){
                        break;
                    }
                }else{
                    break;
                }
            }
            assignStatus = hwcTryAssignLayer(localCtx, i, setOrder);
            switch(assignStatus){
            case ASSIGN_OVERLAY:
                if(psLayer->compositionType == HWC_FRAMEBUFFER){
                    psLayer->compositionType = HWC_OVERLAY;
                    if(glbCtx->forceGPUComp && !localCtx->usedFB){
                        sizeOfMem += cntOfLayerMem(psLayer);
                    }
                }
                setOrder++;
                break;
			case ASSIGN_GPU :

                break;
            case ASSIGN_FAILED:
                ALOGD("Use GPU composite FB failed ");
                resetLocalInfo(psDeviceInfo, localCtx, psDisplay, allLayers);
                setOrder=0;
                break;
            case ASSIGN_NEEDREASSIGNED :
			    if(needReAssignedLayer == 0)
			    {
                    needReAssignedLayer++;
                    sizeOfMem = 0;
                    goto ReAssignedLayer;
			    }else{
			        forceSoftwareRendering = 1;
                    goto ReAssignedLayer;
			    }
                break;
            default:
                ALOGE("No choice in assign layers");
    		}
        }
        displayData->layerNum[psDeviceInfo->disp.id] = setOrder;
    }
    return 0;
}

bool matchFormat(layer_info_t *psHwlayerInfo, disp_layer_info *layer_info, const device_info_t *psDeviceInfo){
    IMG_native_handle_t *handle;
    hwc_context_t *glbCtx = &gHwcContext;
    handle = (IMG_native_handle_t *)(psHwlayerInfo->psLayer->handle);
    bool err = 0;
    layer_info->fb.addr[0] = getIonAddress(handle->fd[0]);
    if(layer_info->fb.addr[0] == 0){
        err = 1;
        goto returnerr;
    }
    layer_info->fb.size[0].width = ALIGN(handle->iWidth, HW_ALIGN);
    layer_info->fb.size[0].height = handle->iHeight;
    layer_info->fb.align[0] = HW_ALIGN;

    switch(handle->iFormat){
        case HAL_PIXEL_FORMAT_RGBA_8888:
            layer_info->fb.format = DISP_FORMAT_ABGR_8888;
            break;
        case HAL_PIXEL_FORMAT_RGBX_8888:
            layer_info->fb.format = DISP_FORMAT_XBGR_8888;
            break;
        case HAL_PIXEL_FORMAT_RGB_888:
            layer_info->fb.format = DISP_FORMAT_BGR_888;
            break;
        case HAL_PIXEL_FORMAT_RGB_565:
            layer_info->fb.format = DISP_FORMAT_RGB_565;
            break;
        case HAL_PIXEL_FORMAT_BGRA_8888:
            layer_info->fb.format = DISP_FORMAT_ARGB_8888;
            break;
	    case HAL_PIXEL_FORMAT_BGRX_8888:
	        layer_info->fb.format = DISP_FORMAT_XRGB_8888;
	        break;
        case HAL_PIXEL_FORMAT_YV12:
            layer_info->fb.format = DISP_FORMAT_YUV420_P;
            layer_info->fb.size[0].width = ALIGN(handle->iWidth, YV12_ALIGN);
            layer_info->fb.size[1].width = ALIGN(layer_info->fb.size[0].width / 2,YV12_ALIGN /2);
            layer_info->fb.size[2].width = ALIGN(layer_info->fb.size[0].width / 2,YV12_ALIGN /2);
            layer_info->fb.size[1].height = handle->iHeight / 2;
            layer_info->fb.size[2].height = handle->iHeight / 2;

            layer_info->fb.addr[2] = layer_info->fb.addr[0] +
                                      (layer_info->fb.size[0].width * layer_info->fb.size[0].height);
            layer_info->fb.addr[1] = layer_info->fb.addr[2] +
                                     (layer_info->fb.size[2].width* layer_info->fb.size[2].height);

            layer_info->fb.align[0] = YV12_ALIGN;
            layer_info->fb.align[1] = YV12_ALIGN /2;
            layer_info->fb.align[2] = YV12_ALIGN /2;
            break;
        case HAL_PIXEL_FORMAT_YCrCb_420_SP:
            layer_info->fb.format = DISP_FORMAT_YUV420_SP_VUVU;
            layer_info->fb.size[0].width = ALIGN(handle->iWidth, YV12_ALIGN);
            layer_info->fb.size[1].width = layer_info->fb.size[0].width / 2;
            layer_info->fb.size[1].height = handle->iHeight / 2;
            layer_info->fb.addr[1] = layer_info->fb.addr[0] +
                                    layer_info->fb.size[0].height * layer_info->fb.size[0].width;
            layer_info->fb.align[0] = YV12_ALIGN;
            layer_info->fb.align[1] = YV12_ALIGN /2;

            break;
        case HAL_PIXEL_FORMAT_AW_NV12:
            layer_info->fb.format = DISP_FORMAT_YUV420_SP_UVUV;
            layer_info->fb.size[0].width = ALIGN(handle->iWidth, YV12_ALIGN);
            layer_info->fb.size[1].width = layer_info->fb.size[0].width / 2;
            layer_info->fb.size[1].height = handle->iHeight / 2;
            layer_info->fb.addr[1] = layer_info->fb.addr[0] +
                                    layer_info->fb.size[0].height * layer_info->fb.size[0].width;
            layer_info->fb.align[0] = YV12_ALIGN;
            layer_info->fb.align[1] = layer_info->fb.align[0] / 2;

            break;
        default:
            ALOGE("Not support format 0x%x in %s", handle->iFormat, __FUNCTION__);
            err = 1;
        }
        if(psHwlayerInfo->is3D){
            //Fix Me
            int trMode = psDeviceInfo->current3DMode;

            switch(trMode)
            {
            case DISPLAY_2D_LEFT:
                layer_info->b_trd_out = 0;
                layer_info->fb.flags = DISP_BF_STEREO_SSH;
                break;
            case DISPLAY_2D_TOP:
                layer_info->b_trd_out = 0;
                layer_info->fb.flags = DISP_BF_STEREO_TB;
                break;
            case DISPLAY_3D_LEFT_RIGHT_HDMI:
                layer_info->b_trd_out = 1;
                layer_info->out_trd_mode = DISP_3D_OUT_MODE_FP;
                layer_info->fb.flags = DISP_BF_STEREO_SSH;
                break;
            case DISPLAY_3D_TOP_BOTTOM_HDMI:
                layer_info->b_trd_out = 1;
                layer_info->out_trd_mode = DISP_3D_OUT_MODE_FP;
                layer_info->fb.flags = DISP_BF_STEREO_TB;
                break;
            default :
                layer_info->b_trd_out = 0;
                layer_info->fb.flags = DISP_BF_NORMAL;
                break;
            }
        }
returnerr:
    return err;

}

static inline long long recalcCoordinate(const unsigned char percent, const long long middle,  const long long coordinate){
    long long diff = 0;
    diff = middle - coordinate;
    diff *= percent/100;
    return  middle - diff;
}

static bool resizeLayer(const device_info_t *psDeviceInfo, disp_layer_info *layerInfo , hwc_layer_1_t *psLayer)
{
    bool isOutlayer = 0;
    long long srcCut = 0, dstCut = 0 , coordinate = 0;

    hwc_context_t *glbCtx = &gHwcContext;

    float wScaleFactor = 1.0 , hScaleFactor = 1.0;
    IMG_native_handle_t* handle = (IMG_native_handle_t*)psLayer->handle;
    hwc_rect_t sourceCrop;
    bool cutLeft = 0 ,cutRight = 0 , cutTop = 0, cutBottom = 0;
    typedef struct{
        long long left;
        long long right;
	    long long top;
	    long long bottom;
    }rect64;
    rect64  layerCrop[3];
    rect64 *fbCrop = &layerCrop[0];
    rect64 *screenWin = &layerCrop[1];
    rect64 *scnBound = &layerCrop[2];
#ifdef HWC_1_3

    sourceCrop.left =  int(ceilf(psLayer->sourceCropf.left)) < 0  ? 0 : int(ceilf(psLayer->sourceCropf.left));
    sourceCrop.right = int(floorf(psLayer->sourceCropf.right)) < 0 ? 0 : int(floorf(psLayer->sourceCropf.right));
    sourceCrop.top =  int(ceilf(psLayer->sourceCropf.top)) < 0 ? 0 : int(ceilf(psLayer->sourceCropf.top));
    sourceCrop.bottom = int(floorf(psLayer->sourceCropf.bottom)) < 0 ? 0 : int(floorf(psLayer->sourceCropf.bottom));
#else
    sourceCrop.left =   psLayer->sourceCrop.left < 0 ? 0 : psLayer->sourceCrop.left;
    sourceCrop.right =  psLayer->sourceCrop.right < 0 ? 0 : psLayer->sourceCrop.right;
    sourceCrop.top =    psLayer->sourceCrop.top < 0 ? 0 : psLayer->sourceCrop.top;
    sourceCrop.bottom = psLayer->sourceCrop.bottom < 0 ? 0 : psLayer->sourceCrop.bottom;
#endif

    fbCrop->left = (long long)(((long long)(sourceCrop.left)) << 32);
    fbCrop->top = (long long)(((long long)(sourceCrop.top)) << 32);
    fbCrop->right = (long long)(((long long)(sourceCrop.right)) << 32);
    fbCrop->bottom = (long long)(((long long)(sourceCrop.bottom)) << 32);


    screenWin->left = (long long)(((long long)(psLayer->displayFrame.left)) << 32);
    screenWin->top = (long long)(((long long)(psLayer->displayFrame.top)) << 32);
    screenWin->right = (long long)(((long long)(psLayer->displayFrame.right)) << 32 );
    screenWin->bottom = (long long)(((long long)(psLayer->displayFrame.bottom)) << 32 );

    scnBound->left = 0;
    scnBound->top = 0;
    scnBound->right = (long long)(((long long)(psDeviceInfo->varDisplayWidth)) << 32 );
    scnBound->bottom = (long long)(((long long)(psDeviceInfo->varDisplayHeight)) << 32 );
    if(hwcIsScaled(psLayer, &wScaleFactor, &hScaleFactor)){
        if(screenWin->left < 0 ){
            dstCut = 0 - screenWin->left;
            screenWin->left = 0;
            fbCrop->left += (long long)(dstCut / wScaleFactor);
            cutLeft = 1;
        }
        if(screenWin->right > scnBound->right){
            dstCut = screenWin->right - scnBound->right;
            screenWin->right = scnBound->right;
            fbCrop->right -=  (long long)(dstCut / wScaleFactor);
            cutRight = 1;
        }
        if(screenWin->top < 0 )
        {
            dstCut = 0 - screenWin->top;
            screenWin->top = 0;
            fbCrop->top += (long long)(dstCut / wScaleFactor);
            cutTop = 1;
        }
        if( screenWin->bottom > scnBound->bottom )
        {
            dstCut = screenWin->bottom - scnBound->bottom;
            screenWin->bottom = scnBound->bottom ;
            fbCrop->right -= (long long)(dstCut / hScaleFactor);
            cutBottom = 1;
        }
        if(( fbCrop->left + fbCrop->right) >((long long) ((long long)(handle->iWidth)) << 32) )
        {
            srcCut = fbCrop->left + fbCrop->right - ((long long) ((long long)(handle->iWidth)) << 32);
            fbCrop->right -= srcCut;
            cutLeft = 1;
        }
        if(( fbCrop->top + fbCrop->bottom) > ((long long) ((long long)(handle->iHeight)) << 32))
        {
            srcCut = fbCrop->top + fbCrop->bottom - ((long long) ((long long)(handle->iHeight)) << 32);
            fbCrop->bottom -= srcCut;
            cutBottom = 1;
        }
    }
    if((psDeviceInfo->displayPercentH != 100 || psDeviceInfo->displayPercentW != 100) && !isOutlayer ) //can >100
    {
        coordinate =  recalcCoordinate(psDeviceInfo->displayPercentW, scnBound->right>>1, screenWin->left);
        if(coordinate >= scnBound->right)
        {
           isOutlayer = 1;
        }else if( coordinate < 0)
        {
            dstCut = 0 - coordinate;
            screenWin->left = 0;
            srcCut = (long long)(dstCut / psDeviceInfo->displayPercentW / wScaleFactor);
            fbCrop->left += srcCut;
            cutLeft = 1;
        }else{
            screenWin->left = coordinate;
        }

        coordinate =  recalcCoordinate(psDeviceInfo->displayPercentW, scnBound->right>>1, screenWin->right );
        if(coordinate >= scnBound->right )
        {
            dstCut = coordinate - scnBound->right;
            screenWin->right = scnBound->right;
            srcCut = (long long)(dstCut / psDeviceInfo->displayPercentW / wScaleFactor);
            fbCrop->right -= srcCut;
            cutRight = 1;
        }else if(coordinate <= 0)
        {
            isOutlayer = 1;
        }else{
            screenWin->right = coordinate;
        }

        coordinate =  recalcCoordinate(psDeviceInfo->displayPercentH, scnBound->bottom>>1, screenWin->top);
        if(coordinate >= scnBound->bottom)
        {
            isOutlayer = 1;
        }else if(coordinate <= 0)
        {
            dstCut = 0 - coordinate;
            screenWin->top = 0;
            srcCut = (long long)(dstCut / psDeviceInfo->displayPercentH / hScaleFactor);
            fbCrop->top += srcCut;
            cutTop = 1;
        }else{
            screenWin->top = coordinate;
        }

        coordinate = recalcCoordinate(psDeviceInfo->displayPercentH, scnBound->bottom>>1, screenWin->bottom);
        if(coordinate <= 0)
        {
            isOutlayer = 1;
        }else if(coordinate >= scnBound->bottom)
        {
            dstCut =  coordinate - scnBound->bottom;
            screenWin->bottom -= scnBound->bottom;
            srcCut = (long long)(dstCut / psDeviceInfo->displayPercentH / hScaleFactor);
            fbCrop->bottom -= srcCut;
            cutRight = 1;
        }else{
            screenWin->bottom= coordinate;
        }
    }
    if( screenWin->top >= screenWin->bottom || screenWin->right <= screenWin->left
            || fbCrop->top >= fbCrop->bottom || fbCrop->left >= fbCrop->right)
    {
        isOutlayer = 1;
    }
    if(!isOutlayer )
    {
        long long srcDiff,destDiff,cutMod;
        srcDiff = fbCrop->right - fbCrop->left;
        destDiff = screenWin->right - screenWin->left;
        float step = (srcDiff << 18) / destDiff;
        layerInfo->screen_win.x  = (!cutRight) ? (int)(screenWin->left >> 32) :
            ((screenWin->right >> 32) - (screenWin->left >> 32) - (destDiff >> 32)) > 0 ? (int)(screenWin->left>>32)+1 : (int)(screenWin->left >> 32);

        layerInfo->screen_win.width = (int)(destDiff >> 32);
        cutMod = (long long)(step*((float)(destDiff - (((long long)layerInfo->screen_win.width) << 32)) / (((long long)1) << 32))*(((long long)1) << 14));

        layerInfo->fb.crop.x = fbCrop->left + ((cutLeft == 1) ? cutMod:0);
        layerInfo->fb.crop.width = srcDiff - cutMod;


        srcDiff = fbCrop->bottom - fbCrop->top;
        destDiff = screenWin->bottom - screenWin->top;
        step = (srcDiff << 18) /destDiff;

        layerInfo->screen_win.y = ( !cutBottom )? (int)(screenWin->top >> 32) :
				((screenWin->bottom >> 32) - (screenWin->top >> 32) - (destDiff >> 32)) > 0 ? ((int)(screenWin->top >> 32) + 1) :((int)(screenWin->top >> 32));

        layerInfo->screen_win.height = (int)(destDiff >> 32);

        cutMod = (long long)(step*((float)(destDiff - (((long long)layerInfo->screen_win.height) << 32)) / (((long long)1) << 32))*(((long long)1) << 14));
	    layerInfo->fb.crop.y = fbCrop->top + ((cutTop == 1) ? cutMod:0);
		layerInfo->fb.crop.height = srcDiff - cutMod;
        if(psDeviceInfo->disp.id == glbCtx->mainDispId){
            layerInfo->screen_win.width = layerInfo->screen_win.width * psDeviceInfo->varDisplayWidth / glbCtx->mainDispWidth;
            layerInfo->screen_win.height = layerInfo->screen_win.height * psDeviceInfo->varDisplayHeight / glbCtx->mainDispHeight;
        }else if(psDeviceInfo->disp.id == glbCtx->secDispId){
            layerInfo->screen_win.width = layerInfo->screen_win.width * psDeviceInfo->varDisplayWidth / glbCtx->secDispWidth;
            layerInfo->screen_win.height = layerInfo->screen_win.height * psDeviceInfo->varDisplayHeight / glbCtx->secDispHeight;
        }

    }
    return isOutlayer;
}

int hwcSetupLayer(setup_dispc_data_t* displayData, hwc_disp_context_t *localCtx){
    int chCnt = 0, lCnt = 0, zOrder = 0 , videoCnt=0, uiCnt = 0, numberLayer = 0;
    disp_layer_info *layerInfo;
    const device_info_t *psDeviceInfo = localCtx->psDeviceInfo;
    sun8i_disp_private_t *privateData = (sun8i_disp_private_t*)(psDeviceInfo->disp.privateData);
    hwc_layer_1_t *psLayer;
    layer_info_t *psHwlayerInfo;
    disp_layer_config *psDispConfig;
    channel_info_t  *psChannelInfo = localCtx->channelInfo;

    localCtx->haveVideo ? uiCnt = localCtx->videoChnCnt+1 :uiCnt = 0;
    displayData->enhanceMode[psDeviceInfo->disp.id] = privateData->enhanceMode;

    while(chCnt < ((psDeviceInfo->disp.id)?2:4))
    {
        lCnt = 0;
        while(lCnt < privateData->layerNumOfCh)
        {
            psHwlayerInfo = psChannelInfo[chCnt].hwLayer[lCnt];
            psDispConfig = &(displayData->layerInfo[psDeviceInfo->disp.id][zOrder]);
            if(psHwlayerInfo != NULL)
            {
                psLayer = psHwlayerInfo->psLayer;
            }else{
                memset(psDispConfig, 0, sizeof(disp_layer_config));
                goto nulllayer;
            }
            layerInfo = &psDispConfig->info;
            if(psLayer == NULL)
            {
                memset(psDispConfig, 0, sizeof(disp_layer_config));
                goto nulllayer;
            }
            if(matchFormat(psHwlayerInfo, layerInfo, psDeviceInfo) || resizeLayer(psDeviceInfo, layerInfo, psLayer))
            {
                memset(psDispConfig, 0, sizeof(disp_layer_config));
                goto nulllayer;
            }
            layerInfo->alpha_value = psChannelInfo[chCnt].planeAlpha;
            if( hwcIsBlended(psLayer) )
            {
                layerInfo->alpha_mode  = 2;

            }else{
                layerInfo->alpha_mode  = 1;
            }

            if(psLayer->blending == HWC_BLENDING_PREMULT)
            {
                layerInfo->fb.pre_multiply = 1;
            }
#if defined(CMA)
            ionHandleAddRef(psHwlayerInfo->psLayer);
#endif
            layerInfo->zorder = numberLayer;
            psDispConfig->enable = 1;
            numberLayer++;
nulllayer:
            psDispConfig->layer_id = lCnt;
            psDispConfig->channel = psChannelInfo[chCnt].hasVideo ? videoCnt : uiCnt;
            psHwlayerInfo!= NULL ? psHwlayerInfo->hwChannel = psDispConfig->channel : 0;
            lCnt++;
            zOrder++;
        }
        psChannelInfo[chCnt].hasVideo ? videoCnt++ :uiCnt++;
        chCnt++;
    }
    return 1;
}

static void dump_displays(hwc_disp_context_t *localCtx){
	int  i;
    bool feUsed;
    hwc_context_t *glbCtx = &gHwcContext;
    layer_info_t  *allLayers = localCtx->psAllLayer;
    layer_info_t  *sunxiLayers = NULL;
    const device_info_t *psDeviceInfo = localCtx->psDeviceInfo;
    sun8i_disp_private_t *privateData = (sun8i_disp_private_t*)(psDeviceInfo->disp.privateData);
    static char const* compositionTypeName[] = {
                            "GLES",
                            "HWC",
                            "BKGD",
                            "FB",
                            "UNKN"};
	if(glbCtx->hwcDebug & LAYER_DUMP)
	{
        ALOGD("the framecount:%d  display:%d  vsync:%s  mytimestamp:%llu"
           "\n type|CH|VCH|  Scalht |  Scalwt | PL|  handle | phyaddr |  Usage  |  flags  | tr| blend| format  |        source crop      |         frame            |reason\n"
             "-----+--+---+---------+---------+---+---------+---------+---------+---------+---+------+---------+-------------------------+--------------------------|-------------\n",
             glbCtx->hwcFrameCount+1, psDeviceInfo->disp.id, psDeviceInfo->vsyncEnable?"enable":"disable", privateData->myTimeStamp);
		for(i = 0; i < localCtx->numberOfLayer ; i++)
		{
            sunxiLayers = allLayers+i;
            hwc_layer_1_t *l = sunxiLayers->psLayer;
            IMG_native_handle_t* handle = (IMG_native_handle_t*)l->handle;
            ALOGD("%5s|%2d|%3s| %f| %f| %02x| %08x| %08x| %08x| %08x| %02x| %05x| %08x|[%5d,%5d,%5d,%5d]|[%5d,%5d,%5d,%5d]|%d\n",
                    compositionTypeName[l->compositionType],
                    sunxiLayers->hwChannel,
                    sunxiLayers->virChannel >= 0 ? (localCtx->channelInfo[sunxiLayers->virChannel].hasVideo ? "Yes" : "No"):"No",
                    sunxiLayers->virChannel >= 0 ? localCtx->channelInfo[sunxiLayers->virChannel].htScaleFactor :0,
                    sunxiLayers->virChannel >= 0 ? localCtx->channelInfo[sunxiLayers->virChannel].wtScaleFactor :0,
                    sunxiLayers->virChannel >= 0 ? localCtx->channelInfo[sunxiLayers->virChannel].planeAlpha : 0xff,
                    (unsigned int)l->handle,
                    handle == 0 ? 0 : ((handle->uiFlags & PVRSRV_MEM_CONTIGUOUS) ? getIonAddress(handle->fd[0]) : 0),
                    handle == 0 ? 0 : handle->usage,
                    l->flags,
                    l->transform,
                    l->blending,
                    handle==0?0:handle->iFormat,
                    l->sourceCrop.left,
                    l->sourceCrop.top,
                    l->sourceCrop.right,
                    l->sourceCrop.bottom,
                    l->displayFrame.left,
                    l->displayFrame.top,
                    l->displayFrame.right,
                    l->displayFrame.bottom,
                    sunxiLayers->assignInfo);

        }
        ALOGD("-----+--+---+---------+---------+---+---------+---------+---------+---------+---+------+---------+-------------------------+--------------------------|------------\n");
    }
}


int sun8iHwcSet(hwc_display_contents_1_t** displays,size_t numDisplays,
		int *releaseFencefd){
    hwc_context_t *glbCtx = &gHwcContext;
    setup_dispc_data_t* displayData = (setup_dispc_data_t*)glbCtx->pvPrivateData;
    hwc_disp_context_t *localCtx = NULL;
    device_info_t *psDeviceInfo;
    int fdCnt = 0,ret = -1 ;
    size_t disp, i, hwDisp=0;
    hwc_display_contents_1_t* psDisplay;
    hwc_layer_1_t *psLayer;
    unsigned long arg[4] = {0};
    int returnFenceFd[2] = {-1, -1};

    int hwLayerCnt = displayData->layerNum[0] + displayData->layerNum[1];
    int fds[hwLayerCnt];
    memset(fds, -1, hwLayerCnt);
    for(disp = 0; disp < numDisplays; disp++){
        switch(disp){
        case HWC_DISPLAY_PRIMARY:
            for(i = 0; i < glbCtx->displayNum; i++){
                if(glbCtx->sunxiDevice[i].disp.id != INVALID_VALUE
                    && glbCtx->sunxiDevice[i].disp.id == glbCtx->mainDispId){
                    localCtx = &dispContext[i];
                    psDeviceInfo = &glbCtx->sunxiDevice[i];
                    hwDisp = i;
                    break;
                }
            }
            break;
        case HWC_DISPLAY_EXTERNAL:
            for(i = 0; i < glbCtx->displayNum; i++){
                if(glbCtx->sunxiDevice[i].disp.id != INVALID_VALUE
                    && glbCtx->sunxiDevice[i].disp.id == glbCtx->secDispId){
                    localCtx = &dispContext[i];
                    psDeviceInfo = &glbCtx->sunxiDevice[i];
                    hwDisp = i;
                    break;
                }
            }
            break;
        case HWC_DISPLAY_VIRTUAL:
        default:
            return -1;
        }
        psDisplay = displays[disp];
    	if(!psDisplay || psDisplay->numHwLayers <= 0 || disp > 2 || psDeviceInfo->disp.id == INVALID_VALUE)
    	{
    		ALOGV("%s: display[%d] was unexpectedly NULL",
    								__func__, disp);
    		continue;
    	}
        for(i = 0; i < psDisplay->numHwLayers; i++)
	    {
	    	psLayer = &psDisplay->hwLayers[i];
	    	if(psLayer->compositionType == HWC_OVERLAY || psLayer->compositionType == HWC_FRAMEBUFFER_TARGET)
	    	{
                if(psLayer->acquireFenceFd >= 0)
                {
                    *(fds + fdCnt) = psLayer->acquireFenceFd;
                    fdCnt++;
                }
	    	}
        }
        hwcSetupLayer(displayData, localCtx);
        dump_displays(localCtx);
        free(localCtx->psAllLayer);
    }

    displayData->aquireFenceCnt = fdCnt;
    displayData->aquireFenceFd = fds;
    displayData->returnFenceFd = returnFenceFd;
    displayData->androidFrmNum = glbCtx->hwcFrameCount;
    if(glbCtx->detectError == 0)
    {
        arg[0] = 0;
        arg[1] = (unsigned int)(glbCtx->pvPrivateData);
        ret = ioctl(glbCtx->displayFd, DISP_HWC_COMMIT, (unsigned long)arg);
    }
    if(!ret)
    {
        *releaseFencefd = returnFenceFd[0];
    }
    glbCtx->hwcFrameCount++;
    return 1;
}

void sun8iHwcDestroyContext(hwc_context_t *context){
    close(context->displayFd);
    close(context->ionFd);
    free(context->pvPrivateData);
    free(context->sunxiDevice);
}

hwc_opr_t sun8iHwcOpr = {
    hwcCreateContext : sun8iHwcCreateContext,
	hwcPrepare : sun8iHwcPrepare,
 	hwcSet : sun8iHwcSet,
 	hwcDestroyContext : sun8iHwcDestroyContext,
};
