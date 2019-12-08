#include "../hwc.h"
#include "private.h"
#include "../common/utils.h"
#include "g2d_driver.h"
#define LOG_NDEBUG 0
static de_module_t* mDeModule = NULL;   //DE module
static setup_dispc_data_t* mFrameData = NULL;
static hwc_rect_t mFbTargetRegion[CHANNEL_NUM];
static int mUiFeUsed = 0;
static int mVeFeUsed = 0;
static int mAllFeUsed = 0;

/* For DEBUG*/
void printDEInfo(){
    hwc_context_t *context = &gHwcContext;
    if((context->hwcDebug & SHOW_ALL) != LAYER_DUMP){
        return;
    }
    char buf[2048] = {0};
    memset(buf, 0, 2048);
    int count = 0;
    int screenId = 0;
    int layerId = 0;
    int enable = 0;
    disp_layer_info *layerPara;
    for(screenId=0; screenId < CHANNEL_NUM - 1; screenId ++){
        count += sprintf(buf + count, "screen %d, layer num %d\n", screenId, mFrameData->layer_num[screenId]);
        count += sprintf(buf + count, "    type  |  id  | pipe | z | pre_mult |    alpha   | colorkey | fmt | framebuffer |  	   source crop      |       frame       |   trd   |         address\n");
        count += sprintf(buf + count, "----------+------+------+---+----------+------------+----------+-----+-------------+-----------------------+-------------------+---------+-----------------------------\n");
        for(layerId=0; layerId< mFrameData->layer_num[screenId]; layerId++){
            layerPara = &(mFrameData->layer_info[screenId][layerId]);
            count += sprintf(buf + count, " %8s |", (layerPara->mode == DISP_LAYER_WORK_MODE_SCALER)? "SCALER":"NORAML");
			count += sprintf(buf + count, " %4d |", layerId);
			count += sprintf(buf + count, " %4d |", layerPara->pipe);
			count += sprintf(buf + count, " %1d |", layerPara->zorder);
			count += sprintf(buf + count, " %8s |", (layerPara->fb.pre_multiply)? "Y":"N");
			count += sprintf(buf + count, " %5s(%3d) |", (layerPara->alpha_mode)? "globl":"pixel", layerPara->alpha_value);
			count += sprintf(buf + count, " %8s |", (layerPara->ck_enable)? "enable":"disable");
			count += sprintf(buf + count, " %3d |", layerPara->fb.format);
			count += sprintf(buf + count, " [%4d,%4d] |", layerPara->fb.size.width, layerPara->fb.size.height);
			count += sprintf(buf + count, " [%4d,%4d,%4d,%4d] |", layerPara->fb.src_win.x, layerPara->fb.src_win.y, layerPara->fb.src_win.width, layerPara->fb.src_win.height);
			count += sprintf(buf + count, " [%4d,%4d,%4d,%4d] |", layerPara->screen_win.x, layerPara->screen_win.y, layerPara->screen_win.width, layerPara->screen_win.height);
			count += sprintf(buf + count, " [%1d%1d,%1d%1d] |", layerPara->fb.b_trd_src, layerPara->fb.trd_mode, layerPara->b_trd_out, layerPara->out_trd_mode);
			count += sprintf(buf + count, " [%8x,%8x,%8x] |", layerPara->fb.addr[0], layerPara->fb.addr[1], layerPara->fb.addr[2]);
			count += sprintf(buf + count, "\n");
        }
    }
    ALOGD("DEInfo:\n%s",buf);
}

void addDebugLayer(hwc_layer_1_t* layer, int flag, int pipe){
    IMG_native_handle_t* handle = (IMG_native_handle_t*)layer->handle;
    hwc_context_t *context = &gHwcContext;
    if(context->hwcDebug == LAYER_DUMP){
        ALOGD("Layer handle=%x, format=%d, Flag=%d, pipe=%d, composition type is %s, addr is %x",
            (handle!=NULL?handle:0), (handle!=NULL?handle->iFormat:0),flag, pipe,
            (layer->compositionType == HWC_FRAMEBUFFER)? "HWC_FRAMEBUFFER":
                ((layer->compositionType == HWC_FRAMEBUFFER_TARGET)?
                    "HWC_FRAMEBUFFER_TARGET":"HWC_OVERLAY"),
            ((handle != NULL)&&!(handle->usage & GRALLOC_USAGE_PRIVATE_3))?ionGetAddr(handle->fd[0]):0);
    }
}

void sun9iInit(){
    //init composer;
    mDeModule = (de_module_t*)calloc(1, sizeof(de_module_t));
    memset(mDeModule, 0, sizeof(de_module_t));
    mFrameData = (setup_dispc_data_t*)calloc(1, sizeof(setup_dispc_data_t));
    memset(mFrameData, 0, sizeof(setup_dispc_data_t));
    mFrameData->hConfigData = calloc(LAYER_NUM*CHANNEL_NUM, sizeof(int));
}

bool isFormatSupported(int format){
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
            return true;
        default:
            return false;
    }
}
//check whether the scale desire is out of the ability of FE
bool isFeCanScale(device_info_t *deviceInfo, hwc_layer_1_t *layer){
    hwc_context_t *glbCtx = &gHwcContext;
    IMG_native_handle_t *handle = (IMG_native_handle_t*)(layer->handle);
    float widthFactor = 1;
    float hightFactor = 1;
    calculateFactor(deviceInfo, &widthFactor, &hightFactor);
    int srcW = layer->sourceCrop.right - layer->sourceCrop.left;
	int srcH = layer->sourceCrop.bottom - layer->sourceCrop.top;
	int dstW = (int)(layer->displayFrame.right * widthFactor + 0.5) - (int)(layer->displayFrame.left * widthFactor + 0.5);
	int dstH = (int)(layer->displayFrame.bottom * hightFactor +0.5) - (int)(layer->displayFrame.top *hightFactor +0.5);
	float efficience = 0.8;
	float feClk = mFeClk;
	int deFps = mDeFps;
    switch(deviceInfo->current3DMode){
    case DISPLAY_2D_LEFT:
    case DISPLAY_3D_LEFT_RIGHT_HDMI:
        if(checkVideoFormat(handle->iFormat)){
            ALOGD("asdasd");
            srcW = srcW >> 1;
        }
		break;
	case DISPLAY_2D_TOP:
    case DISPLAY_3D_TOP_BOTTOM_HDMI:
    case DISPLAY_3D_DUAL_STREAM:
        if(checkVideoFormat(handle->iFormat)){
            ALOGD("###########");
            srcH = srcH >> 1;
        }
		break;
	default:
		break;
    }
    float scaleFactorW = srcW/dstW;
    float scaleFactorH = srcH/dstH;
    float feProW = (scaleFactorW >= 1)? srcW : dstW;
	float feProH = (scaleFactorH >= 1)? srcH : dstH;

	float requiredFeClk = (feProW * feProH)/(dstW * dstH)*(deviceInfo->varDisplayWidth * deviceInfo->varDisplayHeight * deFps)/efficience;
    // must THK thether the small initdisplay  and  the biggest display  can use fe?(1280*720 ---> 3840 * 2160  ---622080000   3840 * 2160 --->1280 *720  cann't....  error,so just can surpport the 1080p screen )
    if(requiredFeClk > feClk) {
		return false;//cann't
	} else {
		return true;//can
	}
}

bool sun9iIsLayerAvailable(hwc_layer_1_t *layer, int disp){
    IMG_native_handle_t* handle = (IMG_native_handle_t*)layer->handle;
    hwc_context_t *glbCtx = &gHwcContext;
    device_info_t *deviceInfo = &glbCtx->sunxiDevice[disp];
    int flag = INVALID_VALUE;
    if(handle == NULL){
        flag = NULL_BUF;
        goto no_available;
    }
    if(handle->usage & GRALLOC_USAGE_PRIVATE_3){
        flag = CONTIG_MEM;
        goto no_available;
    }
    if(isLayerProtected(handle->usage) && (deviceInfo->displayType == DISP_OUTPUT_TYPE_HDMI)){
        flag = VIDEO_PD;
        goto no_available;
    }

    if(layer->flags & HWC_SKIP_LAYER){
        flag = SKIP_LAYER;
        goto no_available;
    }
    if(!isFormatSupported(handle->iFormat)){
        flag = NO_FORMAT;
        goto no_available;
    }
    if(layer->compositionType == HWC_BACKGROUND){
        flag = BACKGROUND;
        goto no_available;
    }
    if(layer->transform){
        flag = TR_N_0;
        goto no_available;
    }

    if(!isFeCanScale(deviceInfo, layer)){
        flag = CANNT_SCALE;
        goto no_available;
    }
    return true;
no_available:
    addDebugLayer(layer, flag, -1);
    return false;
}

int getDispChannel(int dispType){
    int chn = 0;
    switch(dispType){
    case DISP_OUTPUT_TYPE_HDMI:
        chn = HDMI_USED;
        break;
    case DISP_OUTPUT_TYPE_TV:
        chn = CVBS_USED;
        break;
    case DISP_OUTPUT_TYPE_LCD:
        chn = LCD_USED;
        break;
    case DISP_OUTPUT_TYPE_VGA:
    case DISP_OUTPUT_TYPE_NONE:
    default:
        chn = INVALID_VALUE;
        break;
    }
    if(chn == INVALID_VALUE){
        ALOGE("%s : dispType %d is invalid, this should not happend!", __func__,dispType);
    }
    return chn;
}

//try to update the FB target Layer
bool sun9iUpdateFbTarget(hwc_layer_1_t *layer, int zOrder, int disp){
    hwc_context_t *glbCtx = &gHwcContext;
    device_info_t *deviceInfo = &glbCtx->sunxiDevice[disp];
    //find the FbTarget Layer which had been assigned in De composer module.
    int chn = getDispChannel(deviceInfo->displayType);
    disp_channel_t *dispChn = &mDeModule->dispChn[chn];
    layer_t *ly = NULL;
    for(int i = 0; i < LAYER_DUMP; i++){
        ly = &dispChn->layers[i];

        //find the FbTarget Layer which is assigned to de composer
        if(ly->assigned == ASSIGNED_SUCCESS && ly->hwc_layer->compositionType == HWC_FRAMEBUFFER_TARGET){
            //update the zorder of fbTarget
            if(ly->zOrder < 0 || ly->zOrder > zOrder){
                ly->zOrder = zOrder;
            }
            //update the region of fbTarget Layer
            regionMerge(&layer->displayFrame, &mFbTargetRegion[chn], deviceInfo->varDisplayWidth, deviceInfo->varDisplayHeight);
            //update success, return true;
            return true;
        }
    }
    //update fail, because the FbTargetLayer is not assigned yet.
    return false;
}

int successAssign(hwc_layer_1_t *layer, int zOrder, int pipeId, disp_channel_t* dispChn){
    addDebugLayer(layer, OVERLAY, pipeId);
    for(int i = 0; i < LAYER_NUM; i++){
        if(dispChn->layers[i].assigned == NO_ASSIGNED){
            dispChn->layers[i].assigned = ASSIGNED_SUCCESS;
            dispChn->layers[i].hwc_layer = layer;
            dispChn->layers[i].zOrder = zOrder;
            dispChn->layers[i].pipeId = pipeId;
            return 0;
        }
    }
    return INVALID_VALUE;
}

bool sun9iTryToAssignLayer(hwc_layer_1_t *ly, int zOrder, int disp){
    IMG_native_handle_t* handle = (IMG_native_handle_t*)ly->handle;
    hwc_context_t *glbCtx = &gHwcContext;
    device_info_t *deviceInfo = &glbCtx->sunxiDevice[disp];
    de_module_t* deModule = mDeModule;
    int chn = getDispChannel(deviceInfo->displayType);
    disp_channel_t *dispChn = &deModule->dispChn[chn];
    ALOGV("pipeUsed %d, chn=%d", dispChn->pipeUsed, chn);
    if(handle == NULL){
        ALOGD("The Layer Buffer is NULL.");
        return false;
    }
    while(dispChn->pipeUsed < 2){
        //check whether the layer is intersected with FbTarget layer .
        if(twoRegionIntersect(&ly->displayFrame, &mFbTargetRegion[chn])){
            return false;
        }

        //check whether the layer need to scale
        if(isLayerScaled(deviceInfo, ly)){
            if(isValidVideoFormat(handle->iFormat)){
                if(mAllFeUsed < FE_NUM && mVeFeUsed < VE_FE_NUM){
                    dispChn->allFeUsed++;
                    dispChn->veFeUsed++;
                    mAllFeUsed ++;
                    mVeFeUsed ++;
                    //assign success
                    successAssign(ly, zOrder, dispChn->pipeUsed, dispChn);
                    dispChn->pipeUsed++;
                    return true;
                }else{
                    //ui fe is not enought
                    addDebugLayer(ly, NO_FE, -1);
                    return false;
                }
            }else{
                ALOGV("mAllFeUsed=%d, mUiFeUsed=%d", mAllFeUsed, mUiFeUsed);
                if(mAllFeUsed < FE_NUM && mUiFeUsed < UI_FE_NUM){
                    dispChn->allFeUsed ++;
                    dispChn->uiFeUsed ++;
                    mAllFeUsed ++;
                    mUiFeUsed ++;
                    //assign success
                    successAssign(ly, zOrder, dispChn->pipeUsed, dispChn);
                    dispChn->pipeUsed++;
                    return true;
                }else{
                    //ve fe is not enought
                    addDebugLayer(ly, NO_FE, -1);
                    return false;
                }
            }
        }else{
            //assign the layer to this pipe
            successAssign(ly, zOrder,dispChn->pipeUsed, dispChn);
            dispChn->pipeUsed++;
            return true;
        }
    }
    addDebugLayer(ly, NO_PIPE, -1);
    return false;
}

void sun9iReset(int disp){
    hwc_context_t *glbCtx = &gHwcContext;
    device_info_t *deviceInfo = &glbCtx->sunxiDevice[disp];
    int chn = getDispChannel(deviceInfo->displayType);
    disp_channel_t *dispChn = &mDeModule->dispChn[chn];
    ALOGV("before reset, allFeUsed=%d, uiFeUsed=%d, veFeUsed=%d",
        dispChn->allFeUsed, dispChn->uiFeUsed, dispChn->veFeUsed);
    ALOGV("mAllFeUsed=%d, mUiFeUsed=%d, mVeFeUsed=%d",
        mAllFeUsed, mUiFeUsed, mVeFeUsed);
    mAllFeUsed -= dispChn->allFeUsed;
    mUiFeUsed -= dispChn->uiFeUsed;
    mVeFeUsed -= dispChn->veFeUsed;
    memset(&mDeModule->dispChn[chn], 0, sizeof(disp_channel_t));
    for(int i = 0; i < LAYER_NUM; i++){
        mDeModule->dispChn[chn].layers[i].zOrder = INVALID_VALUE;
    }
    memset(&mFbTargetRegion[chn], -1, sizeof(hwc_rect_t));
}

int convertMbToNv21(hwc_layer_1_t *layer, unsigned int dstPhyAddr){
    g2d_blt blit;
    hwc_context_t *glbCtx = &gHwcContext;
    IMG_native_handle_t *handle = (IMG_native_handle_t *)layer->handle;
    unsigned int srcStride = (handle->iWidth+31)/32*32;
    unsigned int dstStride = (handle->iWidth+15)/16*16;
    unsigned int dstPhyAddrC = dstPhyAddr + ALIGN(dstStride * handle->iHeight, 1);
    int ret = 0;

    ALOGD( "#### %s src:[%x,%x] dst:%x w:%d h:%d src_stride:%d dst_stride:%d",
        __func__, layer->videoAddr0, layer->videoAddr1, dstPhyAddr,
        handle->iWidth, handle->iHeight, srcStride, dstStride);

    blit.flag = G2D_BLT_NONE;
    blit.src_image.addr[0] = layer->videoAddr0 + PHY_OFFSET;
    blit.src_image.addr[1] = layer->videoAddr1  + PHY_OFFSET;
    blit.src_image.w = srcStride;
    //blit.src_image.h = handle->iHeight;
    blit.src_image.h = (handle->iHeight+31)&~31;
    if(layer->videoFormat == HAL_PIXEL_FORMAT_AW_MB422)
    {
        blit.src_image.format = G2D_FMT_PYUV422UVC_MB32;
    }
    else if(layer->videoFormat == HAL_PIXEL_FORMAT_AW_MB420)
    {
        blit.src_image.format = G2D_FMT_PYUV420UVC_MB32;
    }
    else
    {
        ALOGE("%s,%d: error format: %d", __func__, __LINE__, layer->videoFormat);
        return -1;
    }

    blit.src_image.pixel_seq = G2D_SEQ_NORMAL;
    blit.src_rect.x = 0;
    blit.src_rect.y = 0;
    //blit.src_rect.w = src_stride;
    blit.src_rect.w = dstStride;
    blit.src_rect.h = handle->iHeight;
    blit.dst_image.addr[0] = dstPhyAddr;
    blit.dst_image.addr[1] = dstPhyAddrC;
    blit.dst_image.w = dstStride;
    blit.dst_image.h = handle->iHeight;
    blit.dst_image.format = G2D_FMT_PYUV420UVC;
    blit.dst_image.pixel_seq = G2D_SEQ_VUVU;
    blit.dst_x = 0;
    blit.dst_y = 0;
    blit.color = 0xFF;
    blit.alpha = 0x89;
    ret = ioctl(mG2dFd, G2D_CMD_BITBLT, (unsigned long)(&blit));
    if(ret != 0){
        ALOGD("#### %s g2d[%d] op failed", __func__, mG2dFd);
    }
    else{
        ALOGV("#### %s g2d[%d] op ok!\n", __func__, mG2dFd);
    }

    long arr[2];
    arr[0] = (long)dstPhyAddr;
    arr[1] = arr[0] + (dstStride * handle->iHeight) - 1;
    ioctl(mG2dFd, G2D_CMD_MEM_FLUSH_CACHE, arr);

    arr[0] = (long)dstPhyAddrC;
    arr[1] = arr[0] + (dstStride * handle->iHeight)/2 - 1;
    ioctl(mG2dFd, G2D_CMD_MEM_FLUSH_CACHE, arr);

    return 0;
}

void recomputeForPlatform(int screenRadio, disp_layer_info *layer_info, IMG_native_handle_t *handle){
    if(!checkScaleFormat(handle->iFormat)){
        return;
    }
    switch(screenRadio)
    {
    case SCREEN_AUTO:{
        float radio = ((float)(layer_info->fb.src_win.width)) / layer_info->fb.src_win.height;
        float scn_radio = ((float)layer_info->screen_win.width) / layer_info->screen_win.height;
        int tmp = 0, div;
        if(radio >= scn_radio)
        {
            /* change screen window, keep x and width as the same, modified the y and height.
            *                                 *************************
            * ************************        *                       *       *************************
            * *                      *        *                       *       *                       *
            * *        layer         *  --->  *      screen window    * ----> *     screen window     *
            * *                      *        *                       *       *                       *
            * ************************        *                       *       *************************
            *                                 *************************
            */
            tmp = layer_info->screen_win.width * layer_info->fb.src_win.height / layer_info->fb.src_win.width;
            div = layer_info->screen_win.height - tmp;
            if(div >= 0)
            {//we must check the div to be positive.
                layer_info->screen_win.y += div/2;
                layer_info->screen_win.height = tmp;
            }
        }
        else
        {
            /* or keep y and height as the same, modified the x and width.
            *      *******        *************************       ********
            *      *     *        *                       *       *      *
            *      *     *        *                       *       *      *
            *      *layer*  --->  *      screen window    * ----> *screen*
            *      *     *        *                       *       *window*
            *      *     *        *                       *       *      *
            *      *******        *************************       ********
            */
            tmp = layer_info->screen_win.height * layer_info->fb.src_win.width / layer_info->fb.src_win.height;
            div = layer_info->screen_win.width - tmp;
            if(div >= 0)
            {
                layer_info->screen_win.x += div/2;
                layer_info->screen_win.width = tmp;
            }
        }
        }
        break;
    case SCREEN_FULL:
        break;
    default:
        break;
    }
}

int layerConfig3D(device_info_t* deviceInfo, disp_layer_info* layerInfo, bool isVideoFormat){
    int cur3DMode = deviceInfo->current3DMode;
    if(layerInfo->mode == DISP_LAYER_WORK_MODE_SCALER && isVideoFormat){
        switch(cur3DMode){
            case DISPLAY_2D_ORIGINAL:
                layerInfo->fb.b_trd_src = 0;
                layerInfo->b_trd_out = 0;
                break;
            case DISPLAY_2D_LEFT:
                layerInfo->fb.b_trd_src = 1;
                layerInfo->fb.trd_mode = DISP_3D_SRC_MODE_SSF;
                layerInfo->b_trd_out = 0;
                break;
            case DISPLAY_2D_TOP:
                layerInfo->fb.b_trd_src = 1;
                layerInfo->fb.trd_mode = DISP_3D_SRC_MODE_TB;
                layerInfo->b_trd_out = 0;
                break;
            case DISPLAY_3D_LEFT_RIGHT_HDMI:
                layerInfo->fb.b_trd_src = 1;
                layerInfo->fb.trd_mode = DISP_3D_SRC_MODE_SSF;
                layerInfo->b_trd_out = 1;
                layerInfo->out_trd_mode = DISP_3D_OUT_MODE_FP;
                break;
            case DISPLAY_3D_TOP_BOTTOM_HDMI:
                layerInfo->fb.b_trd_src = 1;
                layerInfo->fb.trd_mode = DISP_3D_SRC_MODE_TB;
                layerInfo->b_trd_out = 1;
                layerInfo->out_trd_mode = DISP_3D_OUT_MODE_FP;
                break;
            case DISPLAY_2D_DUAL_STREAM:
                layerInfo->fb.b_trd_src = 0;
                layerInfo->b_trd_out = 0;
                break;
            case DISPLAY_3D_DUAL_STREAM:
                layerInfo->fb.b_trd_src = 1;
                layerInfo->fb.trd_mode = DISP_3D_SRC_MODE_FP;
                layerInfo->b_trd_out = 1;
                layerInfo->out_trd_mode = DISP_3D_OUT_MODE_FP;
                break;
            default:
                break;
        }
        if(is3DMode(cur3DMode)){
            layerInfo->screen_win.x = 0;
            layerInfo->screen_win.y = 0;
            layerInfo->screen_win.width = 1920;
            layerInfo->screen_win.height = 1080 * 2;
            //TMP:do not do de-interlace when play 3D video, we will fix it after the de-interlace driver
            //is in work.
            layerInfo->fb.interlace = 0;
            layerInfo->fb.top_field_first = 0;
        }
    }else{
        if(is3DMode(cur3DMode)){
            layerInfo->mode = DISP_LAYER_WORK_MODE_SCALER;
            layerInfo->screen_win.x = layerInfo->screen_win.x * 1920 / deviceInfo->varDisplayWidth;
            layerInfo->screen_win.y = layerInfo->screen_win.y * 1080 / deviceInfo->varDisplayHeight;
            layerInfo->screen_win.width = layerInfo->screen_win.width * 1920 / deviceInfo->varDisplayWidth;
            layerInfo->screen_win.height = layerInfo->screen_win.height * 1080 / deviceInfo->varDisplayHeight;
        }
    }
    return 0;
}

void sortLayer(disp_channel_t *dispChn, int* ret){
    layer_t* ly = NULL;
    int i, x, y, tmp;
    //For design, we already now that the ly1.zOrder < ly2.zOrder < ly3.zOder,
    //but do not know the where we should put ly0.
    for(i = 0; i < LAYER_NUM; i++){
        ly = &dispChn->layers[i];
        ret[i] = ly->zOrder;
    }
    for(i = 0; i < LAYER_NUM-1; i++){
        if((ret[i] > ret[i+1]) && (ret[i+1] != INVALID_VALUE)){
            tmp = ret[i];
            ret[i] = ret[i+1];
            ret[i+1] = tmp;
        }
    }
}

layer_t* findLayerByZorder(disp_channel_t *dispChn, int zOrder){
    layer_t* ly = NULL;
    for(int i = 0; i < LAYER_NUM; i++){
        ly = &dispChn->layers[i];
        if(ly != NULL && zOrder != INVALID_VALUE && ly->zOrder == zOrder){
            return ly;
        }
    }
    return NULL;
}

void sun9iPostPrepare(hwc_display_contents_1_t *display){
    //1.check the video layer which was setted to HWC_FRAMEBUFFER,this code only active
    //in display framework 1.0
    int i;
    for(i = 0; i < display->numHwLayers; i++){
        //convert mb to nv21.
        hwc_layer_1_t *layer = &display->hwLayers[i];
        IMG_native_handle_t *handle = (IMG_native_handle_t *)layer->handle;
        if(NULL == handle){
            continue;
        }
        if((HWC_FRAMEBUFFER == layer->compositionType)
          && ((HAL_PIXEL_FORMAT_AW_MB420 == layer->videoFormat)
              || (HAL_PIXEL_FORMAT_AW_MB422 == layer->videoFormat))){
            int phyAddress = ionGetAddr(handle->fd[0]);
            if(0 != phyAddress){
                convertMbToNv21(layer, phyAddress);
            }
        }
    }
}

void sun9iSet(hwc_display_contents_1_t** displays,size_t numDisplays,
		int *releasefencefd){
    hwc_context_t *glbCtx = &gHwcContext;
    unsigned int arg[4] = {0};
    int i;
    int j = 0;
    //fixme: the handle of FbTarget Layer is defference between prepare and set.so we can not update
    //frame data before set().
    //1.reset pre framedata.
    memset(mFrameData->hConfigData, -1, 12*sizeof(int));
    for(i = 0; i < CHANNEL_NUM; i++){
        mFrameData->layer_num[i] = 0;
        for(j = 0; j < LAYER_NUM; j++){
            memset(&(mFrameData->layer_info[i][j]), 0, sizeof(disp_layer_info));
        }
    }
    printDEInfo();

    //2.setup data for every Layer
    disp_channel_t *dispChn;
    layer_t *ly;
    disp_layer_info *layerInfo;  //the layer of HW
    hwc_layer_1_t *layer;        //the layer define in surfaceflinger
    IMG_native_handle_t *handle;
    device_info_t *deviceInfo;
    int *fds = (int *)(mFrameData->hConfigData);
    int fdCnt = 0;
    for(j = 0; j < CHANNEL_NUM; j++){
        dispChn = &mDeModule->dispChn[j];
        deviceInfo = getDeviceInfoByChn(j);
        if(deviceInfo == NULL){
            continue;
        }
        if((deviceInfo->disp.id != glbCtx->secDispId &&
            deviceInfo->disp.id != glbCtx->mainDispId) ||
            deviceInfo->disp.id == INVALID_VALUE){
            continue;
        }
        int zOrder[LAYER_NUM] = {0};
        sortLayer(dispChn,zOrder);
        //setup layer.
        for(i = 0; i < LAYER_NUM; i++){
            ly = findLayerByZorder(dispChn, zOrder[i]);
            if(ly == NULL || ly->assigned != ASSIGNED_SUCCESS){
                continue;
            }
            layerInfo = &(mFrameData->layer_info[j][i]);
            layer = ly->hwc_layer;
            handle = (IMG_native_handle_t *)ly->hwc_layer->handle;
            switch(handle->iFormat){
            case HAL_PIXEL_FORMAT_RGBA_8888:
                layerInfo->fb.format = DISP_FORMAT_ABGR_8888;
                break;
            case HAL_PIXEL_FORMAT_RGBX_8888:
                layerInfo->fb.format = DISP_FORMAT_XBGR_8888;
                break;
            case HAL_PIXEL_FORMAT_RGB_888:
                layerInfo->fb.format = DISP_FORMAT_BGR_888;
                break;
            case HAL_PIXEL_FORMAT_RGB_565:
                layerInfo->fb.format = DISP_FORMAT_RGB_565;
                break;
            case HAL_PIXEL_FORMAT_BGRA_8888:
                layerInfo->fb.format = DISP_FORMAT_ARGB_8888;
                break;
    		case HAL_PIXEL_FORMAT_BGRX_8888:
    			layerInfo->fb.format = DISP_FORMAT_XRGB_8888;
    			break;
            case HAL_PIXEL_FORMAT_YV12:
                layerInfo->fb.format = DISP_FORMAT_YUV420_P;
                break;
            case HAL_PIXEL_FORMAT_YCrCb_420_SP:
                layerInfo->fb.format = DISP_FORMAT_YUV420_SP_VUVU;
                break;
            case HAL_PIXEL_FORMAT_AW_NV12:
                layerInfo->fb.format = DISP_FORMAT_YUV420_SP_UVUV;
                break;
            default:
                ALOGE("Not support format 0x%x in %s", handle->iFormat, __func__);
                goto ERR;
            }
            layerInfo->fb.interlace = layer->interlace ? 1 : 0;
            layerInfo->fb.top_field_first = layer->topFieldFirst ? 1 : 0;
            if(isLayerBlended(layer)){
                layerInfo->alpha_mode = 2;
                layerInfo->alpha_value = layer->planeAlpha;
            }else{
                layerInfo->alpha_mode  = 1;
                layerInfo->alpha_value = 0xff;
            }
            if(isLayerPremult(layer)){
                layerInfo->fb.pre_multiply = 1;
            }
            if(deviceInfo->displayType == DISP_OUTPUT_TYPE_HDMI
                && (deviceInfo->current3DMode == DISPLAY_3D_LEFT_RIGHT_HDMI
                || deviceInfo->current3DMode == DISPLAY_3D_TOP_BOTTOM_HDMI)){
                layerInfo->ck_enable = 0;
            }else{
                layerInfo->ck_enable = 0;
            }
            if(handle->iFormat==HAL_PIXEL_FORMAT_YV12||handle->iFormat==HAL_PIXEL_FORMAT_YCrCb_420_SP){
                layerInfo->fb.size.width= ALIGN(handle->iWidth, YV12_ALIGN);
            }else{
                layerInfo->fb.size.width = ALIGN(handle->iWidth, HW_ALIGN);
            }
            layerInfo->fb.size.height = handle->iHeight;
            switch(layer->videoFormat){
                case HAL_PIXEL_FORMAT_AW_MB420:
                    layerInfo->fb.format = DISP_FORMAT_YUV420_SP_TILE_UVUV;
                    layerInfo->fb.addr[0] = (unsigned int)(layer->videoAddr0) + PHY_OFFSET;
                    layerInfo->fb.addr[1] = (unsigned int)(layer->videoAddr1) + PHY_OFFSET;
                    layerInfo->fb.addr[2] = (unsigned int)(layer->videoAddr2) + PHY_OFFSET;
                    layerInfo->fb.trd_right_addr[0] = (unsigned int)(layer->videoAddr3) + PHY_OFFSET;
                    layerInfo->fb.trd_right_addr[1] = (unsigned int)(layer->videoAddr4) + PHY_OFFSET;
                    layerInfo->fb.trd_right_addr[2] = (unsigned int)(layer->videoAddr5) + PHY_OFFSET;
                    break;
                case HAL_PIXEL_FORMAT_AW_MB411:
                    layerInfo->fb.format = DISP_FORMAT_YUV411_SP_TILE_UVUV;
                    layerInfo->fb.addr[0] = (unsigned int)(layer->videoAddr0) + PHY_OFFSET;
                    layerInfo->fb.addr[1] = (unsigned int)(layer->videoAddr1) + PHY_OFFSET;
                    break;
                case HAL_PIXEL_FORMAT_AW_MB422:
                    layerInfo->fb.format = DISP_FORMAT_YUV422_SP_TILE_UVUV;
                    layerInfo->fb.addr[0] = (unsigned int)(layer->videoAddr0) + PHY_OFFSET;
                    layerInfo->fb.addr[1] = (unsigned int)(layer->videoAddr1) + PHY_OFFSET;
                    break;
                case HAL_PIXEL_FORMAT_AW_YUV_PLANNER420:		// YU12
                    layerInfo->fb.format = DISP_FORMAT_YUV420_P;
                    layerInfo->fb.addr[0] = (unsigned int)(layer->videoAddr0) + PHY_OFFSET;
                    layerInfo->fb.addr[1] = (unsigned int)(layer->videoAddr1) + PHY_OFFSET;
                    layerInfo->fb.addr[2] = (unsigned int)(layer->videoAddr2) + PHY_OFFSET;
                    break;
                default:{
                    layerInfo->fb.addr[0] = ionGetAddr(handle->fd[0]);
        		    if(layerInfo->fb.addr[0] == 0){
        		         goto ERR;
        		    }

        		    if(layerInfo->fb.format == DISP_FORMAT_YUV420_P){			// YV12
        		        layerInfo->fb.addr[2] = layerInfo->fb.addr[0] +
        		                                layerInfo->fb.size.width * layerInfo->fb.size.height;
        		        layerInfo->fb.addr[1] = layerInfo->fb.addr[2] +
        		                                (layerInfo->fb.size.width * layerInfo->fb.size.height)/4;
        		    }else if(layerInfo->fb.format == DISP_FORMAT_YUV420_SP_VUVU
        		        || layerInfo->fb.format == DISP_FORMAT_YUV420_SP_UVUV){	// NV12/NV21
        		        layerInfo->fb.addr[1] = layerInfo->fb.addr[0] +
        		            layerInfo->fb.size.height * layerInfo->fb.size.width;
        		    }
                }
            }
            layerInfo->fb.src_win.x = layer->sourceCrop.left;
            layerInfo->fb.src_win.y = layer->sourceCrop.top;
            layerInfo->fb.src_win.width = layer->sourceCrop.right - layer->sourceCrop.left;
            layerInfo->fb.src_win.height = layer->sourceCrop.bottom - layer->sourceCrop.top;
            ALOGV("1.handle=%x, src_win=(%d,%d,%d,%d), screen_win=(%d,%d,%d,%d)",
                handle,
                layerInfo->fb.src_win.x, layerInfo->fb.src_win.y,
                layerInfo->fb.src_win.width, layerInfo->fb.src_win.height,
                layerInfo->screen_win.x, layerInfo->screen_win.y,
                layerInfo->screen_win.width, layerInfo->screen_win.height);
            int midPoint;
            int bufWidth;
            int bufHeight;
            if(deviceInfo->disp.id == glbCtx->mainDispId){
                bufWidth = glbCtx->mainDispWidth;
                bufHeight = glbCtx->mainDispHeight;
            }else if(deviceInfo->disp.id == glbCtx->secDispId){
                bufWidth = glbCtx->secDispWidth;
                bufHeight = glbCtx->secDispHeight;
            }
            ALOGV("deviceInfo (%d * %d), displayFrame(%d, %d, %d, %d), percent(%d * %d), buf(%d * %d)",
                deviceInfo->varDisplayWidth, deviceInfo->varDisplayHeight,
                layer->displayFrame.left, layer->displayFrame.top, layer->displayFrame.right, layer->displayFrame.bottom,
                deviceInfo->displayPercentW, deviceInfo->displayPercentH,
                bufWidth, bufHeight);
            midPoint = bufWidth >> 1;
            layerInfo->screen_win.x = calcPointByPercent(deviceInfo->displayPercentW, midPoint, layer->displayFrame.left)
        		* deviceInfo->varDisplayWidth / bufWidth;
            layerInfo->screen_win.width = calcPointByPercent(deviceInfo->displayPercentW, midPoint, layer->displayFrame.right)
        		* deviceInfo->varDisplayWidth / bufWidth;
            layerInfo->screen_win.width -= layerInfo->screen_win.x;
            midPoint = bufHeight >> 1;
            layerInfo->screen_win.y = calcPointByPercent(deviceInfo->displayPercentH, midPoint, layer->displayFrame.top)
        		* deviceInfo->varDisplayHeight / bufHeight;
            layerInfo->screen_win.height = calcPointByPercent(deviceInfo->displayPercentH, midPoint, layer->displayFrame.bottom)
        		* deviceInfo->varDisplayHeight / bufHeight;
            layerInfo->screen_win.height -= layerInfo->screen_win.y;
            //add for screan radio
            recomputeForPlatform(glbCtx->screenRadio, layerInfo, handle);
            ALOGV("3.handle=%x, src_win=(%d,%d,%d,%d), screen_win=(%d,%d,%d,%d)",
                handle,
                layerInfo->fb.src_win.x, layerInfo->fb.src_win.y,
                layerInfo->fb.src_win.width, layerInfo->fb.src_win.height,
                layerInfo->screen_win.x, layerInfo->screen_win.y,
                layerInfo->screen_win.width, layerInfo->screen_win.height);
            if(isLayerScaled(deviceInfo, layer) || checkScaleFormat(handle->iFormat)){
                int cutSizeScn, cutSizeSrc;
                hwc_rect_t scnBound;
                layerInfo->mode = DISP_LAYER_WORK_MODE_SCALER;
                scnBound.left = deviceInfo->varDisplayWidth * (100 - deviceInfo->displayPercentW) / 100/ 2;
                scnBound.top = deviceInfo->varDisplayHeight * (100 - deviceInfo->displayPercentH) /100 / 2;
                scnBound.right = scnBound.left + (deviceInfo->varDisplayWidth * deviceInfo->displayPercentW) / 100;
                scnBound.bottom = scnBound.top + (deviceInfo->varDisplayHeight * deviceInfo->displayPercentH) / 100;

                if(layerInfo->fb.src_win.x < 0){
                    cutSizeSrc = (0 - layerInfo->fb.src_win.x);

                    layerInfo->fb.src_win.x += cutSizeSrc;
                    layerInfo->fb.src_win.width -= cutSizeSrc;
                }
                if((layerInfo->fb.src_win.x + layerInfo->fb.src_win.width) > (unsigned int)handle->iWidth){
                    cutSizeSrc = (layerInfo->fb.src_win.x + layerInfo->fb.src_win.width) - handle->iWidth;
                    layerInfo->fb.src_win.width -= cutSizeSrc;
                }
                if(layerInfo->fb.src_win.y < 0){
                    cutSizeSrc = (0 - layerInfo->fb.src_win.y);

                    layerInfo->fb.src_win.y += cutSizeSrc;
                    layerInfo->fb.src_win.height -= cutSizeSrc;
                }
                if((layerInfo->fb.src_win.y + layerInfo->fb.src_win.height) > (unsigned int)handle->iHeight){
                    cutSizeSrc = (layerInfo->fb.src_win.x + layerInfo->fb.src_win.height) - handle->iHeight;
                    layerInfo->fb.src_win.height -= cutSizeSrc;
                }

                if(layerInfo->screen_win.x < scnBound.left){
                    cutSizeScn = (scnBound.left - layerInfo->screen_win.x);
                    cutSizeSrc = cutSizeScn * layerInfo->fb.src_win.width / layerInfo->screen_win.width;

                    layerInfo->fb.src_win.x += cutSizeSrc;
                    layerInfo->fb.src_win.width -= cutSizeSrc;

                    layerInfo->screen_win.x += cutSizeScn;
                    layerInfo->screen_win.width -= cutSizeScn;
                }
                if((layerInfo->screen_win.x + layerInfo->screen_win.width) > (unsigned int)scnBound.right){
                    cutSizeScn = (layerInfo->screen_win.x + layerInfo->screen_win.width) - scnBound.right;
                    cutSizeSrc = cutSizeScn * layerInfo->fb.src_win.width / layerInfo->screen_win.width;

                    layerInfo->fb.src_win.width -= cutSizeSrc;
                    layerInfo->screen_win.width -= cutSizeScn;
                }
                if(layerInfo->screen_win.y < scnBound.top){
                    cutSizeScn = (scnBound.top - layerInfo->screen_win.y);
                    cutSizeSrc = cutSizeScn * layerInfo->fb.src_win.height / layerInfo->screen_win.height;

                    layerInfo->fb.src_win.y += cutSizeSrc;
                    layerInfo->fb.src_win.height -= cutSizeSrc;

                    layerInfo->screen_win.y += cutSizeScn;
                    layerInfo->screen_win.height -= cutSizeScn;
                }
                if((layerInfo->screen_win.y + layerInfo->screen_win.height) > (unsigned int)scnBound.bottom){
                    cutSizeScn = (layerInfo->screen_win.y + layerInfo->screen_win.height) - scnBound.bottom;
                    cutSizeSrc = cutSizeScn * layerInfo->fb.src_win.height / layerInfo->screen_win.height;

                    layerInfo->fb.src_win.height -= cutSizeSrc;
                    layerInfo->screen_win.height -= cutSizeScn;
                }
            }else{
                layerInfo->mode = DISP_LAYER_WORK_MODE_NORMAL;
            }
            layerInfo->pipe = ly->pipeId;
            layerInfo->zorder = ly->zOrder;
            layerConfig3D(deviceInfo, layerInfo, checkScaleFormat(handle->iFormat));
            //update layer num of frame
            mFrameData->layer_num[j] = mFrameData->layer_num[j] + 1;
            if(layer->acquireFenceFd >= 0){
                *(fds+fdCnt) = layer->acquireFenceFd;
                fdCnt++;
            }
        }
        continue;
    ERR:
        ALOGE("update assigned data fail!");
        mFrameData->layer_num[j] = 0;
        memset(&(mFrameData->layer_info[j]), 0, sizeof(disp_layer_info) * LAYER_NUM);
    }


    arg[0] = 0;
    arg[1] = (unsigned int)(mFrameData);
    printDEInfo();
    *releasefencefd = ioctl(glbCtx->displayFd, DISP_CMD_HWC_COMMIT, (unsigned long)arg);
}

void sun9iBeforePrepare(){
    mAllFeUsed = 0;
    mVeFeUsed = 0;
    mUiFeUsed = 0;

    int i = 0;
    int j = 0;
    for(i = 0; i < CHANNEL_NUM; i++){
        disp_channel_t *dispChn = &mDeModule->dispChn[i];

        dispChn->allFeUsed = 0;
        dispChn->uiFeUsed = 0;
        dispChn->veFeUsed = 0;
    }

}

hwc_opr_t sun9iHwcOpr = {
    init : sun9iInit,
 	set : sun9iSet,
 	isLayerAvailable : sun9iIsLayerAvailable,
 	updateFbTarget : sun9iUpdateFbTarget,
 	tryToAssignLayer : sun9iTryToAssignLayer,
 	reset : sun9iReset,
 	postPrepare : sun9iPostPrepare,
 	beforePrepare : sun9iBeforePrepare,
};