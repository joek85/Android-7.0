/*-------------------------------------------------------------------------

-------------------------------------------------------------------------*/
/*-------------------------------------------------------------------------

-------------------------------------------------------------------------*/
/*-------------------------------------------------------------------------

-------------------------------------------------------------------------*/
/*-------------------------------------------------------------------------

-------------------------------------------------------------------------*/
/*************************************************************************/ /*!
@Copyright      Copyright (c) Imagination Technologies Ltd. All Rights Reserved
@License        Strictly Confidential.
*/ /**************************************************************************/

#include "hwc.h"
#define LOG_NDEBUG 0
// PHY_OFFSET is 0x40000000 at A83 platform
#define PHY_OFFSET 0x40000000

#define PLATFORM_CMCCWASU	0x08
#define PLATFORM_ALIYUN		0x09
#define PLATFORM_TVD		0x0A
#define UNKNOWN_PLATFORM	0xFF

#if (BUSINESS_PLATFORM == PLATFORM_CMCCWASU)
	#define MARGIN_DEFAULT_PERCENT_WIDTH 100
	#define MARGIN_DEFAULT_PERCENT_HEIGHT 100
#else
	#define MARGIN_DEFAULT_PERCENT_WIDTH 95
	#define MARGIN_DEFAULT_PERCENT_HEIGHT 95
#endif


SUNXI_hwcdev_context_t gSunxiHwcDevice;
static unsigned int get_ion_address(int sharefd);

static const char * hwc_print_info(AssignDUETO_T eError)
{
    switch(eError)
	{

#define AssignDUETO(x) \
		          case x: \
			            return #x;
    AssignDUETO(I_OVERLAY)
    AssignDUETO(D_NULL_BUF)
    AssignDUETO(D_CONTIG_MEM)
	AssignDUETO(D_VIDEO_PD)
	AssignDUETO(D_CANNT_SCALE)
	AssignDUETO(D_SKIP_LAYER)
    AssignDUETO(D_NO_FORMAT)
    AssignDUETO(D_BACKGROUND)
    AssignDUETO(D_TR_N_0)
    AssignDUETO(D_ALPHA)
    AssignDUETO(D_X_FB)
    AssignDUETO(D_SW_OFTEN)
    AssignDUETO(D_SCALE_OUT)
    AssignDUETO(D_NO_PIPE)
#undef AssignDUETO
		default:
			return "Unknown reason";
	}

}

static void dump_displays(HwcDisContext_t *Localctx)
{
	int  i;
    bool feused;
    SUNXI_hwcdev_context_t *Globctx = &gSunxiHwcDevice;
    layer_info  *AllLayers = Localctx->psAllLayer;
    layer_info  *sunxiLayers = NULL;
    const DisplayInfo *PsDisplayInfo = Localctx->psDisplayInfo;
    static char const* compositionTypeName[] = {
                            "GLES",
                            "HWC",
                            "BKGD",
                            "FB",
                            "UNKN"};
	if(Globctx->hwcdebug & LAYER_DUMP)
	{
        ALOGD("the framecount:%d  display:%d  vsync:%s  mytimestamp:%llu"
           "\n type|CH|VCH|  Scalht |  Scalwt | PL|  handle | phyaddr |  Usage  |  flags  | tr| blend| format  |        source crop      |         frame            |reason\n"
             "-----+--+---+---------+---------+---+---------+---------+---------+---------+---+------+---------+-------------------------+--------------------------|-------------\n",
             Globctx->HWCFramecount+1,PsDisplayInfo->VirtualToHWDisplay,PsDisplayInfo->VsyncEnable?"enable":"disable",PsDisplayInfo->mytimestamp);
		for(i = 0; i < Localctx->numberofLayer ; i++)
		{
            sunxiLayers = AllLayers+i;
            hwc_layer_1_t *l = sunxiLayers->psLayer;
            IMG_native_handle_t* handle = (IMG_native_handle_t*)l->handle;
            ALOGD("%5s|%2d|%3s| %f| %f| %02x| %08x| %08x| %08x| %08x| %02x| %05x| %08x|[%5d,%5d,%5d,%5d]|[%5d,%5d,%5d,%5d]|%s\n",
                    compositionTypeName[l->compositionType],
                    sunxiLayers->hwchannel,
                    sunxiLayers->virchannel >= 0 ? (Localctx->ChannelInfo[sunxiLayers->virchannel].hasVideo ? "Yes" : "No"):"No",
                    sunxiLayers->virchannel >= 0 ? Localctx->ChannelInfo[sunxiLayers->virchannel].HTScaleFactor :0,
                    sunxiLayers->virchannel >= 0 ? Localctx->ChannelInfo[sunxiLayers->virchannel].WTScaleFactor :0,
                    sunxiLayers->virchannel >= 0 ? Localctx->ChannelInfo[sunxiLayers->virchannel].planeAlpha : 0xff,
                    (unsigned int)l->handle,
                    handle == 0 ? 0 : ((handle->uiFlags & PVRSRV_MEM_CONTIGUOUS) ? get_ion_address(handle->fd[0]) : 0),
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
                   sunxiLayers->info != DEFAULT? hwc_print_info(sunxiLayers->info): "NOT_ASSIGNED");

        }
        ALOGD("-----+--+---+---------+---------+---+---------+---------+---------+---------+---+------+---------+-------------------------+--------------------------|------------\n");
    }
}

static inline void reset_layer_type(hwc_display_contents_1_t* list,layer_info *pslayers, int hwctype)
{
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
            pslayers[j].hwchannel = -2;
            pslayers[j].virchannel = -2;
            pslayers[j].HwzOrder  = -1;
            pslayers[j].OrigOrder = -1;
            pslayers[j].is3D = 0;
            pslayers[j].info = DEFAULT;
        }
    }
}
static void inline CalculateFactor(DisplayInfo *PsDisplayInfo, float *XWidthFactor, float *XHighetfactor)
{

    float WidthFactor = (float)PsDisplayInfo->PersentWidth/ 100;
    float Highetfactor = (float)PsDisplayInfo->PersentHeight/ 100;
    if(PsDisplayInfo->InitDisplayWidth && PsDisplayInfo->InitDisplayHeight)
    {
        WidthFactor = (float)(((float)PsDisplayInfo->VarDisplayWidth) / PsDisplayInfo->InitDisplayWidth * PsDisplayInfo->PersentWidth / 100);
        Highetfactor = (float)(((float)PsDisplayInfo->VarDisplayHeight) / PsDisplayInfo->InitDisplayHeight * PsDisplayInfo->PersentHeight / 100);
    }

    *XWidthFactor = WidthFactor;
    *XHighetfactor = Highetfactor;
}

static void
resetLocalInfo(DisplayInfo *psDisplayInfo,HwcDisContext_t * Localctx, hwc_display_contents_1_t *psDisplay, layer_info *AllLayers)
{
    int i = 4, j = 0;
    Localctx->HwCHUsedCnt = -1;
    Localctx->VideoCHCnt = -1;
    Localctx->HaveVideo = 0;
    Localctx->psAllLayer = AllLayers;
    Localctx->numberofLayer = psDisplay->numHwLayers;
    Localctx->psDisplayInfo = psDisplayInfo;
    Localctx->FBhasVideo = 0;
    CalculateFactor(psDisplayInfo,&Localctx->WidthScaleFactor,&Localctx->HighetScaleFactor);

    reset_layer_type(psDisplay,AllLayers,HWC_FRAMEBUFFER);
    while(i--)
    {
        j = 4;
        Localctx->ChannelInfo[i].hasBlend = 0;
        Localctx->ChannelInfo[i].hasVideo= 0;
        Localctx->ChannelInfo[i].WTScaleFactor = 1.0;
        Localctx->ChannelInfo[i].HTScaleFactor = 1.0;
        Localctx->ChannelInfo[i].iCHFormat = 0;
        Localctx->ChannelInfo[i].planeAlpha = 0xff;
        Localctx->ChannelInfo[i].HwLayerCnt = 0;
        while(j--)
        {
            Localctx->ChannelInfo[i].HwLayer[j] = NULL;
        }
    }
}

static inline  void ResetLayerInfo(disp_layer_info* LayerInfo)
{
    memset(LayerInfo,0,sizeof(disp_layer_info));
    LayerInfo->mode = LAYER_MODE_BUFFER;
}

static unsigned int  CntOfLyaerMem(hwc_layer_1_t *psLayer)
{
    unsigned int hight,width;
    IMG_native_handle_t* handle = (IMG_native_handle_t*)psLayer->handle;
    if(handle->iFormat == HAL_PIXEL_FORMAT_YV12 || handle->iFormat == HAL_PIXEL_FORMAT_YCrCb_420_SP)
    {
       width = ALIGN(handle->iWidth, YV12_ALIGN);
    }else{
       width = ALIGN(handle->iWidth, HW_ALIGN);
    }

    return handle->uiBpp * width * handle->iHeight;

}

static inline bool Check3DLayer(const DisplayInfo *PsDisplayInfo)
{
    switch(PsDisplayInfo->Current3DMode)
    {
    case DISPLAY_2D_LEFT:
    case DISPLAY_2D_TOP:
    case DISPLAY_3D_LEFT_RIGHT_HDMI:
    case DISPLAY_3D_TOP_BOTTOM_HDMI:
    case DISPLAY_3D_DUAL_STREAM:
        return 1;
    default:
        return 0;
    }
}

static bool inline ReCountPresent(DisplayInfo *PsDisplayInfo)
{
    if(PsDisplayInfo->VirtualToHWDisplay != -EINVAL)
    {
        if(Check3DLayer(PsDisplayInfo))
        {
            PsDisplayInfo->PersentHeight = 100;
            PsDisplayInfo->PersentWidth = 100;
        }
        else
        {
            PsDisplayInfo->PersentHeight = PsDisplayInfo->SetPersentHeight;
            PsDisplayInfo->PersentWidth = PsDisplayInfo->SetPersentWidth;
        }
    }
    return 0;
}

static void
resetGlobDevice(SUNXI_hwcdev_context_t * Globctx)
{
    int i;
    setup_dispc_data_t* DisplayData=Globctx->pvPrivateData;

    memset(DisplayData, 0, sizeof(setup_dispc_data_t));
	for (i = 0; i < Globctx->NumberofDisp; i++)
	{
        if(Globctx->SunxiDisplay[i].VirtualToHWDisplay != -EINVAL)
        {
            ReCountPresent(&Globctx->SunxiDisplay[i]);
        }
	}
}

static inline int HwcUsageSW(IMG_native_handle_t *psHandle)
{
	return psHandle->usage & (GRALLOC_USAGE_SW_READ_OFTEN |
							  GRALLOC_USAGE_SW_WRITE_OFTEN);
}

static inline int HwcUsageSWwrite(IMG_native_handle_t *psHandle)
{
	return psHandle->usage & GRALLOC_USAGE_SW_WRITE_OFTEN;
}

static inline int HwcUsageProtected(IMG_native_handle_t *psHandle)
{
	return psHandle->usage & GRALLOC_USAGE_PROTECTED;
}


static inline int HwcValidFormat(int format)
{
    switch(format)
    {
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
static inline bool HwcsupportAlpha(int format)
{
    switch(format)
    {
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

static inline int HwcisBlended(hwc_layer_1_t* psLayer)
{
	return (psLayer->blending != HWC_BLENDING_NONE);
}

static inline int HwcisPremult(hwc_layer_1_t* psLayer)
{
    return (psLayer->blending == HWC_BLENDING_PREMULT);
}
static inline bool Hwcisplanealpha(hwc_layer_1_t* psLayer)
{
    return (psLayer->planeAlpha != 0xff);
}

static bool inline check_same_scale(float SRWscaleFac, float SRHscaleFac, float DTWscalFac, float DTHscaleFac )
{
    return (((SRWscaleFac - DTWscalFac) > -0.001) && ((SRWscaleFac - DTWscalFac) < 0.001))
           &&(((SRHscaleFac - DTHscaleFac) > -0.001)&&((SRHscaleFac - DTHscaleFac) < 0.001));
}

static bool HwcisScaled(hwc_layer_1_t *layer,float *WScaleFactor, float * HScaleFactor)
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
        *WScaleFactor = float(layer->displayFrame.right - layer->displayFrame.left) / float(w);
        *HScaleFactor = float(layer->displayFrame.bottom - layer->displayFrame.top) / float(h);
    }
    return ret;
}

static AssignDUETO_T HwcisValidLayer(hwc_layer_1_t *layer)
{
    IMG_native_handle_t *handle = (IMG_native_handle_t *)layer->handle;

    if ((layer->flags & HWC_SKIP_LAYER))
    {
        return D_SKIP_LAYER;
    }

    if (!HwcValidFormat(handle->iFormat))
    {
        return D_NO_FORMAT;
    }

    if (layer->compositionType == HWC_BACKGROUND)
    {
        return D_BACKGROUND;
    }
    if(layer->transform)
    {
        return D_TR_N_0;
    }

    return I_OVERLAY;
}

 int hwc_region_intersect(hwc_rect_t *rect0, hwc_rect_t *rect1)
{
    int mid_x0, mid_y0, mid_x1, mid_y1;
    int mid_diff_x, mid_diff_y;
    int sum_width, sum_height;

    mid_x0 = (rect0->right + rect0->left)/2;
    mid_y0 = (rect0->bottom + rect0->top)/2;
    mid_x1 = (rect1->right + rect1->left)/2;
    mid_y1 = (rect1->bottom + rect1->top)/2;

    mid_diff_x = (mid_x0 >= mid_x1)? (mid_x0 - mid_x1):(mid_x1 - mid_x0);
    mid_diff_y = (mid_y0 >= mid_y1)? (mid_y0 - mid_y1):(mid_y1 - mid_y0);

    sum_width = (rect0->right - rect0->left) + (rect1->right - rect1->left);
    sum_height = (rect0->bottom - rect0->top) + (rect1->bottom - rect1->top);

    if(mid_diff_x < (sum_width/2) && mid_diff_y < (sum_height/2))
    {
        return 1;//return 1 is intersect
    }
    return 0;
}

static inline bool CheckVideoFormat(int format)
{
    switch(format)
    {
    case HAL_PIXEL_FORMAT_YV12:
    case HAL_PIXEL_FORMAT_YCrCb_420_SP:
    case HAL_PIXEL_FORMAT_AW_NV12:
        return 1;
    default:
        return 0;
    }
}

static int HwcCanScale(HwcDisContext_t  *Localctx,hwc_layer_1_t * psLayer)
{
   int src_w,frame_rate,lcd_h,lcd_w,dst_w;
   long long layer_line_peroid,lcd_line_peroid,de_freq;
#ifdef HWC_1_3
    src_w = int (ceilf(psLayer->sourceCropf.right - psLayer->sourceCropf.left));
#else
    src_w = psLayer->sourceCrop.right - psLayer->sourceCrop.left;
#endif
    const DisplayInfo *PsDisplayInfo = Localctx->psDisplayInfo;
    lcd_w = PsDisplayInfo->VarDisplayWidth;
    lcd_h = PsDisplayInfo->VarDisplayHeight;
	dst_w = (int)(psLayer->displayFrame.right * Localctx->WidthScaleFactor) - (int)(psLayer->displayFrame.left * Localctx->WidthScaleFactor);
    frame_rate = PsDisplayInfo->DisplayFps;
    if(PsDisplayInfo->Current3DMode == DISPLAY_3D_LEFT_RIGHT_HDMI)
    {
        dst_w /=2;
    }
    if((DISPLAY_3D_LEFT_RIGHT_HDMI == PsDisplayInfo->Current3DMode)
      || (DISPLAY_2D_LEFT == PsDisplayInfo->Current3DMode))
    {
        if(CheckVideoFormat(((IMG_native_handle_t*)psLayer->handle)->iFormat))
        {
            src_w >>= 1;
        }
    }
    de_freq = PsDisplayInfo->de_clk;
	lcd_line_peroid = 1000000000/frame_rate/lcd_h;
    layer_line_peroid = (src_w > dst_w)?(1000000*((long long)(lcd_w - dst_w + src_w))/(de_freq/1000)):(1000000*((long long)lcd_w)/(de_freq/1000));

    if(((lcd_line_peroid *4/5) < layer_line_peroid) || (src_w > 2048))
    {
        return 0; //can't
    }
    return 1;//can
}
static inline int Find_channel_layer(ChannelInfo_t *ChannelInfo , bool last )
{
   layer_info_t *dispinfo = NULL;
   if(ChannelInfo->HwLayerCnt == 0)
   {
        return -1;
   }
   if(!last)
   {
        dispinfo = ChannelInfo->HwLayer[0];
   }else{
        dispinfo = ChannelInfo->HwLayer[ChannelInfo->HwLayerCnt-1];
   }
   return dispinfo == NULL ? -1 : dispinfo->OrigOrder;
}

static int Match_nofull_channel(HwcDisContext_t *Localctx,int channel,bool isvideo,int emptynum,int format,float SRWscaleFac, float SRHscaleFac, unsigned char planealpha)
{
    ChannelInfo *CH = NULL;
    const DisplayInfo *PsDisplayInfo = Localctx->psDisplayInfo;
    int whilecnt,tmpch;
    (channel >= 0 && channel < PsDisplayInfo->HwChannelNum) ? (whilecnt = 1, tmpch = channel) : (whilecnt = Localctx->HwCHUsedCnt +1 , tmpch = 0);

    while(whilecnt --)
    {
        CH = &Localctx->ChannelInfo[tmpch];
        if( CH->HwLayerCnt != 0
            ?
             ((CH->HwLayerCnt + emptynum) <= PsDisplayInfo->LayerNumofCH
             && isvideo ? ((CH->hasVideo) && (CH->iCHFormat == format)) : (CH->hasVideo ? 0 : 1)
             && CH->planeAlpha == planealpha
             && check_same_scale(CH->WTScaleFactor,CH->HTScaleFactor,SRWscaleFac,SRHscaleFac))
            :
              1 )
        {
            return tmpch;
        }
        tmpch++;
    }
    return -1;
}

int Check_X_List(hwc_layer_1_t *psLayer, HwcDisContext_t *Localctx, int from, int to, int channel, HwcAssignStatus type)
{
    const DisplayInfo *PsDisplayInfo = Localctx->psDisplayInfo;
    layer_info_t *dispinfo = Localctx->psAllLayer;
    dispinfo += from;
    while(from >= 0 && from <= to && to <= Localctx->numberofLayer )
    {
        if(  (dispinfo->psLayer != NULL)
            && ((channel >= -1 && channel < PsDisplayInfo->HwChannelNum ) ? dispinfo->virchannel == channel : 1 )
            && ((type == ASSIGN_GPU || type == ASSIGN_OVERLAY) ? dispinfo->assigned == type : 1)
            && hwc_region_intersect(&dispinfo->psLayer->displayFrame, &psLayer->displayFrame) )
        {
            return 1;
        }
        dispinfo++;
        from++;
    }
    return 0;
}

static inline bool CheckScaleFormat(int format)
{
    switch(format)
    {
    case HAL_PIXEL_FORMAT_YV12:
	case HAL_PIXEL_FORMAT_YCrCb_420_SP:
    case HAL_PIXEL_FORMAT_AW_NV12:
    case HAL_PIXEL_FORMAT_BGRA_8888:
    case HAL_PIXEL_FORMAT_BGRX_8888:
    case HAL_PIXEL_FORMAT_RGBX_8888:
    case HAL_PIXEL_FORMAT_RGBA_8888:
    case HAL_PIXEL_FORMAT_RGB_565:
    case HAL_PIXEL_FORMAT_RGB_888:
        return 1;
    default:
        return 0;
    }
}

static bool isValidVideoFormat(int format)
{
    switch(format)
    {
    case HAL_PIXEL_FORMAT_AW_YUV_PLANNER420:
    case HAL_PIXEL_FORMAT_AW_YVU_PLANNER420:
    case HAL_PIXEL_FORMAT_AW_NV12:
    case HAL_PIXEL_FORMAT_AW_NV21:
        return 1;
    default:
        return 0;
    }
}

static bool check_same_diplay(hwc_display_contents_1_t **psDisplay,int num)
{
    hwc_display_contents_1_t *priDisp = psDisplay[0];
    hwc_display_contents_1_t *cmpDisp = psDisplay[num];
    if(cmpDisp->numHwLayers != priDisp->numHwLayers)
    {
        return 0;
    }
    int cnt = cmpDisp->numHwLayers-1;
    while(cnt--)
    {
        if(cmpDisp->hwLayers[cnt].handle != priDisp->hwLayers[cnt].handle)
        {
            return 0;
        }
    }
    return 1;
}

static int calc_point_byPercent(const unsigned char percent, const int middle_point, const int src_point)
{
    int condition = (src_point > middle_point) ? 1 : 0;
    int length = condition ? (src_point - middle_point) : (middle_point - src_point);
    length = length * percent / 100;
    return condition ? (middle_point + length) : (middle_point - length);
}

static inline long long recalc_coordinate(const unsigned char percent, const long long middle,  const long long coordinate)
{
    long long diff = 0;
    diff = middle - coordinate;
    diff *= percent/100;
    return  middle - diff;
}

void recomputeDisplayFrame(int screenRadio, const hwc_rect_t sourceCrop,
    const int unitedFrameWidth, const int unitedFrameHeight, hwc_rect_t *displayFrame)
{
    switch(screenRadio)
    {
    case SCREEN_AUTO: {
        unsigned int ratio_width, ratio_height;
        unsigned int frame_width = sourceCrop.right - sourceCrop.left;
        unsigned int frame_height = sourceCrop.bottom - sourceCrop.top;
        if(frame_width >= unitedFrameWidth || frame_height >= unitedFrameHeight)
        {
            // scale down
            if(frame_width > unitedFrameWidth)
            {
                frame_height = frame_height * unitedFrameWidth / frame_width;
                frame_width = unitedFrameWidth;
            }
            if(frame_height > unitedFrameHeight)
            {
                frame_width = frame_width * unitedFrameHeight / frame_height;
                frame_height = unitedFrameHeight;
            }
        }
        else
        {
            // scale up
            ratio_width = frame_width * unitedFrameHeight;
            ratio_height = frame_height * unitedFrameWidth;
            if(ratio_width >= ratio_height)
            {
                // scale up until frame_width equal unitedFrameWidth
                frame_height = ratio_height / frame_width;
                frame_width = unitedFrameWidth;
            }
            else
            {
                // scale up until frame_height equal unitedFrameHeight
                frame_width = ratio_width / frame_height;
                frame_height = unitedFrameHeight;
            }
        }
        //ALOGD("###frame[%d,%d], unitedFrame[%d,%d], displayFrame[%d,%d,%d,%d]",
        //    frame_width, frame_height, unitedFrameWidth, unitedFrameHeight,
        //    displayFrame->left, displayFrame->right, displayFrame->top, displayFrame->bottom);
        if(unitedFrameWidth > frame_width)
        {
            ratio_width = frame_width * (displayFrame->right - displayFrame->left) / unitedFrameWidth;
            displayFrame->left += (displayFrame->right - displayFrame->left - ratio_width) >> 1;
            displayFrame->right = displayFrame->left + ratio_width;
        }
        if(unitedFrameHeight > frame_height)
        {
            ratio_height = frame_height * (displayFrame->bottom - displayFrame->top) / unitedFrameHeight;
            displayFrame->top += (displayFrame->bottom - displayFrame->top - ratio_height) >> 1;
            displayFrame->bottom = displayFrame->top + ratio_height;
        }
      }
        break;
    case SCREEN_FULL:
        // here do nothing because the frame of cmcc is allready and always full.
    default:
        break;
    }
}
#if 0
void recomputeForPlatform(int screenRadio, disp_layer_info *layer_info)
{
    switch(screenRadio)
    {
    case SCREEN_AUTO:{
        float radio = ((float)(layer_info->fb.crop.width >> 32)) / (layer_info->fb.crop.height >> 32);
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
            tmp = layer_info->screen_win.width * layer_info->fb.crop.height / layer_info->fb.crop.width;
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
            tmp = layer_info->screen_win.height * layer_info->fb.crop.width / layer_info->fb.crop.height;
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
#endif

static bool resize_layer(const DisplayInfo *PsDisplayInfo, disp_layer_info *layer_info , hwc_layer_1_t *psLayer)
{
    hwc_rect_t sourceCrop;
    hwc_rect_t displayFrame;
    disp_rect screen_win;
	IMG_native_handle_t *handle = (IMG_native_handle_t *)psLayer->handle;
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

    layer_info->fb.crop.x = (long long)(((long long)(sourceCrop.left)) << 32);
    layer_info->fb.crop.width = (long long)(((long long)(sourceCrop.right)) << 32);
    layer_info->fb.crop.width -= layer_info->fb.crop.x;
    layer_info->fb.crop.y = (long long)(((long long)(sourceCrop.top)) << 32);
    layer_info->fb.crop.height = (long long)(((long long)(sourceCrop.bottom)) << 32);
    layer_info->fb.crop.height -= layer_info->fb.crop.y;

    memcpy((void *)&displayFrame, (void *)&(psLayer->displayFrame), sizeof(hwc_rect_t));
    if(CheckVideoFormat(handle->iFormat))
    {
        int unitedFrameWidth = gSunxiHwcDevice.SunxiDisplay[0].InitDisplayWidth;
        int unitedFrameHeight = gSunxiHwcDevice.SunxiDisplay[0].InitDisplayHeight;
        recomputeDisplayFrame(PsDisplayInfo->screenRadio, sourceCrop,
            unitedFrameWidth, unitedFrameHeight, &displayFrame);
    }
    layer_info->screen_win.x = calc_point_byPercent(PsDisplayInfo->PersentWidth, PsDisplayInfo->InitDisplayWidth >> 1,
       displayFrame.left) * PsDisplayInfo->VarDisplayWidth / PsDisplayInfo->InitDisplayWidth;
    layer_info->screen_win.width = calc_point_byPercent(PsDisplayInfo->PersentWidth, PsDisplayInfo->InitDisplayWidth >> 1,
       displayFrame.right) * PsDisplayInfo->VarDisplayWidth / PsDisplayInfo->InitDisplayWidth;
    layer_info->screen_win.width -= layer_info->screen_win.x;
    layer_info->screen_win.width = (0 == layer_info->screen_win.width) ? 1 : layer_info->screen_win.width;

    layer_info->screen_win.y = calc_point_byPercent(PsDisplayInfo->PersentHeight, PsDisplayInfo->InitDisplayHeight >> 1,
       displayFrame.top) * PsDisplayInfo->VarDisplayHeight / PsDisplayInfo->InitDisplayHeight;
    layer_info->screen_win.height = calc_point_byPercent(PsDisplayInfo->PersentHeight, PsDisplayInfo->InitDisplayHeight >> 1,
       displayFrame.bottom) * PsDisplayInfo->VarDisplayHeight / PsDisplayInfo->InitDisplayHeight;
    layer_info->screen_win.height -= layer_info->screen_win.y;
    layer_info->screen_win.height = (0 == layer_info->screen_win.height) ? 1 : layer_info->screen_win.height;

    //ALOGD("screen[%d,%d,%d,%d]", layer_info->screen_win.x, layer_info->screen_win.y,
    //    layer_info->screen_win.width, layer_info->screen_win.height);

    return 0;
}

unsigned int setGbufferBlank(unsigned int format, IMG_native_handle_t *handle)
{
    ATRACE_CALL();
    int ret = -1;
    struct ion_fd_data data ;
    struct ion_fd_data fd_data;
    struct ion_handle_data handle_data;
    struct ion_custom_data custom_data;
    sunxi_cache_range range;
    SUNXI_hwcdev_context_t *Globctx = &gSunxiHwcDevice;
    unsigned int addr_vir = 0;
    unsigned int buffer_size[3] = {0};

    data.fd = handle->fd[0];
    ret = ioctl(Globctx->IonFd, ION_IOC_IMPORT, &data);
    if (ret < 0)
    {
        ALOGE("ION_IOC_IMPORT error, %s", strerror(errno));
        goto out1;
    }
    fd_data.handle = data.handle;
    ret = ioctl(Globctx->IonFd, ION_IOC_MAP, &fd_data);
    if(ret < 0)
    {
        ALOGE("ION_IOC_MAP err=%d, %s", ret, strerror(errno));
        goto out2;
    }

    switch(format)
    {
    case HAL_PIXEL_FORMAT_AW_YVU_PLANNER420:
    case HAL_PIXEL_FORMAT_AW_YUV_PLANNER420:
        buffer_size[0] = ALIGN(handle->iWidth, YV12_ALIGN) * handle->iHeight; // Y
        buffer_size[1] = ALIGN(handle->iWidth >> 1, YV12_ALIGN >> 1) * handle->iHeight; //V&U
        break;
    case HAL_PIXEL_FORMAT_AW_NV21:
    case HAL_PIXEL_FORMAT_AW_NV12:
    default:
        ALOGW("***fixme: the Format[0x%x] is not support at present.", format);
        goto out3;
    }
    addr_vir = (unsigned int)mmap(NULL, buffer_size[0] + buffer_size[1],
                    PROT_READ | PROT_WRITE, MAP_SHARED, fd_data.fd, 0);
    if ((unsigned int)MAP_FAILED == addr_vir)
    {
        ALOGE("ion mmap error, %s", strerror(errno));
        goto out3;
    }

    ALOGW("##### set blank frame!");
    memset((char *)addr_vir, 0x10, buffer_size[0]);
    memset((char *)(addr_vir + buffer_size[0]), 0x80, buffer_size[1]);
    ret = ioctl(Globctx->IonFd, ION_IOC_SYNC, &fd_data);
    if(ret)
    {
        ALOGD("ION_IOC_CUSTOM: FLUSH CACHE FAIL, ret=%d", ret);
    }

out4:
    ret = munmap((void *)addr_vir, buffer_size[0] + buffer_size[1]);
    if(ret)
        ALOGE("munmap err, ret %d\n", ret);

out3:
    close(fd_data.fd);

out2:
    handle_data.handle = data.handle;
    ret = ioctl(Globctx->IonFd, ION_IOC_FREE, &handle_data);
    if(ret)
        ALOGE("ION_IOC_FREE err, ret %d\n", ret);
out1:
    return ret;
}

static unsigned int get_ion_address(int sharefd)
{
    SUNXI_hwcdev_context_t *Globctx = &gSunxiHwcDevice;
    int ret = -1;
    struct ion_handle* tmp2;
    struct ion_custom_data custom_data;
	sunxi_phys_data phys_data;
    ion_handle_data freedata;

    struct ion_fd_data data ;
    data.fd = sharefd;
    ret = ioctl(Globctx->IonFd, ION_IOC_IMPORT, &data);
    if (ret < 0)
    {
        ALOGE("#######ion_import  error#######");
        return 0;
    }
    custom_data.cmd = ION_IOC_SUNXI_PHYS_ADDR;
	phys_data.handle = (void *)data.handle;
	custom_data.arg = (unsigned long)&phys_data;
	ret = ioctl(Globctx->IonFd, ION_IOC_CUSTOM,&custom_data);
	if(ret < 0){
        ALOGE("ION_IOC_CUSTOM(err=%d)",ret);
        return 0;
    }
    freedata.handle = data.handle;
    ret = ioctl(Globctx->IonFd, ION_IOC_FREE, &freedata);
    if(ret < 0){
        ALOGE("ION_IOC_FREE(err=%d)",ret);
        return 0;
    }
    return phys_data.phys_addr;
}

int IonHandleAddRef(hwc_layer_1_t *psLayer)
{
    SUNXI_hwcdev_context_t *Globctx = &gSunxiHwcDevice;
    IMG_native_handle_t* handle = (IMG_native_handle_t*)psLayer->handle;
    ion_list * item;
    ion_list * head = Globctx->IonHandleHead;
    struct ion_fd_data data ;
    int ret = -1;

    if(handle == NULL)
    {
        return -1;
    }

    data.fd = handle->fd[0];
    ret = ioctl(Globctx->IonFd, ION_IOC_IMPORT, &data);
    if (ret < 0)
    {
        ALOGE("#######ion_import  error#######");
        return 0;
    }
    //ALOGD("IonHandleAddRef %d %x %x",Globctx->HWCFramecount, data.handle, handle);

    item = (ion_list *)malloc(sizeof(ion_list));
    item->frame_index = Globctx->HWCFramecount;
    item->fd = data.fd;
    item->handle = (ion_handle *)data.handle;
    if(head == NULL)
    {
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
    Globctx->IonHandleHead = head;

    return 0;
}

int IonHandleDecRef(void)
{
    SUNXI_hwcdev_context_t *Globctx = &gSunxiHwcDevice;
    ion_list * head = Globctx->IonHandleHead;
    ion_list * item;
    ion_handle_data freedata;
    const unsigned char delayFrameCount = 2;

    if(head == NULL)
    {
        return 0;
    }

    item = head->prev;
    while(item && (Globctx->HWCFramecount > delayFrameCount)
        && (item->frame_index < Globctx->HWCFramecount - delayFrameCount))
    {
        int ret = -1;
	    freedata.handle = (ion_user_handle_t)item->handle;
        ret = ioctl(Globctx->IonFd, ION_IOC_FREE, &freedata);
        if(ret < 0)
        {
            ALOGE("ION_IOC_FREE(err=%d)",ret);
            return 0;
        }
        //ALOGD("####################IonHandleDecRef %d %p", item->frame_index, item->handle);

        if(item == head)
        {
            free(item);
            item = NULL;
            head = NULL;
        }else{
            ion_list * item_tmp;
            item_tmp = item->prev;
            item->next->prev = item->prev;
            item->prev->next = item->next;
            free(item);
            item = item_tmp;
        }
        Globctx->IonHandleHead = head;
    }

    return 0;
}

bool match_format(const DisplayInfo *psDisplayInfo, layer_info_t *psHwlayer_info, disp_layer_info *layer_info)
{
    hwc_layer_1_t *layer = psHwlayer_info->psLayer;
    IMG_native_handle_t *handle = (IMG_native_handle_t *)(psHwlayer_info->psLayer->handle);
    bool err = 0;

    layer_info->fb.size[0].width = ALIGN(handle->iWidth, HW_ALIGN);
    layer_info->fb.size[0].height = handle->iHeight;
    layer_info->fb.align[0] = HW_ALIGN;

    switch(layer->videoFormat)
    {
    case HAL_PIXEL_FORMAT_AW_YUV_PLANNER420:		// YU12
        layer_info->fb.format = DISP_FORMAT_YUV420_P;
        layer_info->fb.addr[0] = (unsigned int)(layer->videoAddr0) + PHY_OFFSET;
        layer_info->fb.addr[1] = (unsigned int)(layer->videoAddr1) + PHY_OFFSET;
        layer_info->fb.addr[2] = (unsigned int)(layer->videoAddr2) + PHY_OFFSET;
        layer_info->fb.trd_right_addr[0] = (unsigned int)(layer->videoAddr3) + PHY_OFFSET;
        layer_info->fb.trd_right_addr[1] = (unsigned int)(layer->videoAddr4) + PHY_OFFSET;
        layer_info->fb.trd_right_addr[2] = (unsigned int)(layer->videoAddr5) + PHY_OFFSET;
        layer_info->fb.size[0].width = ALIGN(handle->iWidth, YV12_ALIGN >> 1);
        layer_info->fb.size[1].width = layer_info->fb.size[0].width >> 1;
        layer_info->fb.size[2].width = layer_info->fb.size[0].width >> 1;
        layer_info->fb.size[1].height = handle->iHeight / 2;
        layer_info->fb.size[2].height = handle->iHeight / 2;
        layer_info->fb.align[0] = YV12_ALIGN >> 1;
        layer_info->fb.align[1] = YV12_ALIGN >> 2;
        layer_info->fb.align[2] = YV12_ALIGN >> 2;
        break;
    case HAL_PIXEL_FORMAT_AW_YVU_PLANNER420:		// YV12
        layer_info->fb.format = DISP_FORMAT_YUV420_P;
        layer_info->fb.addr[0] = (unsigned int)(layer->videoAddr0) + PHY_OFFSET;
        layer_info->fb.addr[1] = (unsigned int)(layer->videoAddr2) + PHY_OFFSET;
        layer_info->fb.addr[2] = (unsigned int)(layer->videoAddr1) + PHY_OFFSET;
        layer_info->fb.trd_right_addr[0] = (unsigned int)(layer->videoAddr3) + PHY_OFFSET;
        layer_info->fb.trd_right_addr[1] = (unsigned int)(layer->videoAddr5) + PHY_OFFSET;
        layer_info->fb.trd_right_addr[2] = (unsigned int)(layer->videoAddr4) + PHY_OFFSET;
        layer_info->fb.size[0].width = ALIGN(handle->iWidth, YV12_ALIGN >> 1);
        layer_info->fb.size[1].width = layer_info->fb.size[0].width >> 1;
        layer_info->fb.size[2].width = layer_info->fb.size[0].width >> 1;
        layer_info->fb.size[1].height = handle->iHeight / 2;
        layer_info->fb.size[2].height = handle->iHeight / 2;
        layer_info->fb.align[0] = YV12_ALIGN >> 1;
        layer_info->fb.align[1] = YV12_ALIGN >> 2;
        layer_info->fb.align[2] = YV12_ALIGN >> 2;
        break;
    case HAL_PIXEL_FORMAT_AW_NV12:       // NV12
        layer_info->fb.format = DISP_FORMAT_YUV420_SP_UVUV;
        layer_info->fb.addr[0] = (unsigned int)(layer->videoAddr0) + PHY_OFFSET;
        layer_info->fb.addr[1] = (unsigned int)(layer->videoAddr1) + PHY_OFFSET;
        layer_info->fb.size[0].width = ALIGN(handle->iWidth, YV12_ALIGN >> 1);
        layer_info->fb.size[1].width = ALIGN(layer_info->fb.size[0].width / 2,YV12_ALIGN >> 1);
        layer_info->fb.size[1].height = handle->iHeight / 2;
        layer_info->fb.size[2].height = handle->iHeight / 2;
        layer_info->fb.align[0] = YV12_ALIGN >> 1;
        layer_info->fb.align[1] = YV12_ALIGN >> 1;
        break;
    case HAL_PIXEL_FORMAT_AW_NV21:		 // NV21
        layer_info->fb.format = DISP_FORMAT_YUV420_SP_VUVU;
        layer_info->fb.addr[0] = (unsigned int)(layer->videoAddr0) + PHY_OFFSET;
        layer_info->fb.addr[1] = (unsigned int)(layer->videoAddr1) + PHY_OFFSET;
        layer_info->fb.size[0].width = ALIGN(handle->iWidth, YV12_ALIGN >> 1);
        layer_info->fb.size[1].width = ALIGN(layer_info->fb.size[0].width / 2,YV12_ALIGN >> 1);
        layer_info->fb.size[1].height = handle->iHeight / 2;
        layer_info->fb.size[2].height = handle->iHeight / 2;
        layer_info->fb.align[0] = YV12_ALIGN >> 1;
        layer_info->fb.align[1] = YV12_ALIGN >> 1;
        break;
    default:
        //ALOGD("videoFormat of cedarx: 0x%x", layer->videoFormat);
        layer_info->fb.addr[0] = get_ion_address(handle->fd[0]);
        if(layer_info->fb.addr[0] == 0)
        {
            err = 1;
            goto returnerr;
        }
        switch(handle->iFormat)
        {
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
    }

    if(psHwlayer_info->is3D)
    {
        __display_3d_mode trMode = psDisplayInfo->Current3DMode;

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
        case DISPLAY_3D_DUAL_STREAM:
            layer_info->b_trd_out = 1;
            layer_info->out_trd_mode = DISP_3D_OUT_MODE_FP;
            layer_info->fb.flags = DISP_BF_STEREO_FP;
            break;
        case DISPLAY_2D_DUAL_STREAM:
        default :
            layer_info->b_trd_out = 0;
            layer_info->fb.flags = DISP_BF_NORMAL;
            return 0;
        }
        if(!CheckVideoFormat(handle->iFormat))
        {
            layer_info->fb.flags = DISP_BF_STEREO_FP;
            layer_info->fb.trd_right_addr[0] = layer_info->fb.addr[0] + 12;
        }
    }
returnerr:
    return err;
}

HwcAssignStatus
hwc_try_assign_layer(HwcDisContext_t *Localctx, size_t  singcout,int zOrder)
{

    bool needchannel = 1, isvideo = 0,isalpha = 0, isFB = 0;
    float WscalFac = 1.0, HscaleFac = 1.0;
    int CH= -1, tmCnt1 = 0, tmCnt2 = 0, addLayerCnt = 1, CHdiff = 0,tmpCH = 0 ;
    AssignDUETO_T dueto = I_OVERLAY;
    unsigned char planealpha = 0xff;
    hwc_layer_1_t *psLayer;
    psLayer = Localctx->psAllLayer[singcout].psLayer;

    const DisplayInfo  *PsDisplayInfo = Localctx->psDisplayInfo;
    IMG_native_handle_t* handle = (IMG_native_handle_t*)psLayer->handle;
    ChannelInfo_t *psCH = Localctx->ChannelInfo;

    if(psLayer->compositionType == HWC_FRAMEBUFFER_TARGET )
    {
        if(handle != NULL)
        {
            isFB = 1;
            isalpha = 1;
            goto needchannel;
        }else{
            return ASSIGN_FAILED;
        }
    }
    if(HwcisBlended(psLayer) && singcout == 0)
    {
        psLayer->blending = HWC_BLENDING_NONE;
    }
    if(handle == NULL)
    {
        ALOGV("%s:Buffer handle is NULL", __func__);
        Localctx->UsedFB = ASSIGN_GPU;
        dueto = D_NULL_BUF;
	    goto assign_gpu;
    }
    if((1 == Localctx->psDisplayInfo->VirtualToHWDisplay) && !(CheckVideoFormat(handle->iFormat)))
    {
        Localctx->UsedFB = ASSIGN_GPU;
        dueto = D_SKIP_LAYER;
        goto assign_gpu;
    }
    if(!(handle->uiFlags & PVRSRV_MEM_CONTIGUOUS) && (!isValidVideoFormat(psLayer->videoFormat)))
    {
        ALOGV("%s:not continuous Memory", __func__);
        Localctx->UsedFB = ASSIGN_GPU;
        dueto = D_CONTIG_MEM;
	    goto assign_gpu;
    }

	if(HwcUsageProtected(handle) && !PsDisplayInfo->issecure)
	{
        ALOGV("%s:Video Protected", __func__);
        dueto = D_VIDEO_PD;
	    goto assign_gpu;
	}

    dueto = HwcisValidLayer(psLayer);
    if(dueto != I_OVERLAY)
    {
        ALOGV("HwcisValidLayer:0x%08x", handle->iFormat);
        goto assign_gpu;
    }

    if(HwcUsageSW(handle) && !CheckVideoFormat(handle->iFormat))
    {
        ALOGV("not vedo  and   GRALLOC_USAGE_SW_WRITE_OFTEN");
        dueto = D_SW_OFTEN;
        goto assign_gpu;
    }

    if(HwcisBlended(psLayer))
    {
        planealpha = psLayer->planeAlpha;
        isalpha = 1;//cann't distinguish only pixel alpha
        if(!HwcsupportAlpha(handle->iFormat))
        {
            ALOGV("not surpport alpha layer");
            dueto = D_CANNT_SCALE;
			goto assign_gpu;
        }

        if( HwcisScaled(psLayer, &WscalFac, &HscaleFac)
            || !check_same_scale(Localctx->WidthScaleFactor,Localctx->HighetScaleFactor,1.0,1.0))
	    {
            if(CheckScaleFormat(handle->iFormat))
            {
                if(!HwcCanScale(Localctx, psLayer))
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

        if(Localctx->UsedFB)
        {
            if(Check_X_List(psLayer, Localctx, 0, singcout-1, -1, ASSIGN_GPU))
            {
                dueto = D_X_FB;
                goto assign_gpu;
            }
        }

    }else if(HwcisScaled(psLayer, &WscalFac, &HscaleFac)
               || !check_same_scale(Localctx->WidthScaleFactor,Localctx->HighetScaleFactor,1.0,1.0))
    {
        if(CheckScaleFormat(handle->iFormat))
        {
			if(!HwcCanScale(Localctx, psLayer))
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
    if(CheckVideoFormat(handle->iFormat))
    {
        static int sFormat = 0;
        static int sBlending = 0;
        static float sWidthScale = 0;
        static float sHeightScale = 0;
        if(0 == Localctx->HaveVideo)
        {
            sFormat = handle->iFormat;
            sBlending = psLayer->blending;
            HwcisScaled(psLayer, &sWidthScale , &sHeightScale);
        }
        else
        {
            float llWidthScale, llHeightScale;
            HwcisScaled(psLayer, &llWidthScale, &llHeightScale);
            if ((sFormat != handle->iFormat)
              || (sBlending != psLayer->blending)
              || !check_same_scale(sWidthScale, sHeightScale, llWidthScale, llHeightScale))
            {
                //ALOGD("the second video layer assigned fail!");
                dueto = D_SCALE_OUT;
                goto assign_gpu;
            }
        }
    }

needchannel:
    isvideo = CheckVideoFormat(handle->iFormat);  
    if(is3DMode(PsDisplayInfo->Current3DMode) || isvideo && Check3DLayer(PsDisplayInfo))
    {//ont only the Video has 3D
       addLayerCnt = 2;
    }
    CH = Localctx->HwCHUsedCnt;
    tmpCH = Match_nofull_channel(Localctx, -1, isvideo, addLayerCnt, handle->iFormat, WscalFac, HscaleFac, planealpha);

    while(tmpCH != -1 && CH !=-1 && CH >= tmpCH)
    {// can assigned to the lowest Channel?
        tmCnt1 =  Find_channel_layer(&Localctx->ChannelInfo[CH], 0);
        tmCnt2 =  Find_channel_layer(&Localctx->ChannelInfo[CH], 1);
        if((CH == tmpCH) || (Match_nofull_channel(Localctx, CH, isvideo, addLayerCnt, handle->iFormat, WscalFac, HscaleFac, planealpha) != -1) )
        {
            if(Check_X_List(psLayer, Localctx, tmCnt1, tmCnt2, CH, ASSIGN_OVERLAY))
            {
                if(!isalpha)
                {
                    CHdiff = Localctx->HwCHUsedCnt - CH;
                    needchannel = 0;
                }
                CH = -1;
            }else{
                CHdiff = Localctx->HwCHUsedCnt - CH;
                needchannel = 0;
                CH--;
            }

        }else if(Check_X_List(psLayer, Localctx, tmCnt1, tmCnt2, CH, ASSIGN_OVERLAY))
        {
            CH = -1;
        }else{
            CH--;
        }
    }

    if(needchannel)
    {
        if((isvideo ? (Localctx->HaveVideo ? Localctx->VideoCHCnt < PsDisplayInfo->VideoCHNum -1 : 1) : 1) 
           && Localctx->HwCHUsedCnt < (PsDisplayInfo->HwChannelNum - (isFB ? 0: Localctx->UsedFB) -1))
//here is a fix  :UI has used video channel ,wether reasigend?  could add check video before hwc_try_assign_layer() but must Check_X_List() between the videos
        {
            Localctx->HwCHUsedCnt++;
            isvideo ? Localctx->VideoCHCnt++ :0;
        }else{
            goto assigned_need_resigne;
        }
    }
    if(Localctx->UsedFB && !isalpha && !isFB && Check_X_List(psLayer, Localctx, 0, singcout-1, -1, ASSIGN_GPU ))
    {
        psLayer->hints |= HWC_HINT_CLEAR_FB;
    }

assign_overlay:
    Localctx->HaveVideo += isvideo;
    psCH[Localctx->HwCHUsedCnt - CHdiff].hasVideo = isvideo;
    psCH[Localctx->HwCHUsedCnt - CHdiff].iCHFormat = handle->iFormat;
    psCH[Localctx->HwCHUsedCnt - CHdiff].WTScaleFactor = WscalFac * Localctx->WidthScaleFactor;
    psCH[Localctx->HwCHUsedCnt - CHdiff].HTScaleFactor = HscaleFac * Localctx->HighetScaleFactor;

    psCH[Localctx->HwCHUsedCnt - CHdiff].planeAlpha = planealpha;
    psCH[Localctx->HwCHUsedCnt - CHdiff].HwLayer[psCH[Localctx->HwCHUsedCnt - CHdiff].HwLayerCnt] = &Localctx->psAllLayer[singcout];
    psCH[Localctx->HwCHUsedCnt - CHdiff].HwLayerCnt += addLayerCnt;
    psCH[Localctx->HwCHUsedCnt - CHdiff].isFB = isFB;
    psCH[Localctx->HwCHUsedCnt - CHdiff].hasBlend ? 0: psCH[Localctx->HwCHUsedCnt - CHdiff].hasBlend = isalpha;

    Localctx->psAllLayer[singcout].assigned = ASSIGN_OVERLAY;
    Localctx->psAllLayer[singcout].virchannel = Localctx->HwCHUsedCnt- CHdiff;
    Localctx->psAllLayer[singcout].HwzOrder = zOrder;
    Localctx->psAllLayer[singcout].OrigOrder = singcout;
    Localctx->psAllLayer[singcout].is3D = (addLayerCnt == 2 ? 1 : 0);
    Localctx->psAllLayer[singcout].info = dueto;
    return ASSIGN_OVERLAY;

assign_gpu:

    Localctx->FBhasVideo += (handle != NULL ? CheckVideoFormat(handle->iFormat):0);
    Localctx->psAllLayer[singcout].assigned = ASSIGN_GPU;
    Localctx->psAllLayer[singcout].virchannel = -1;
    Localctx->psAllLayer[singcout].HwzOrder = -1;
    Localctx->psAllLayer[singcout].OrigOrder = singcout;
    Localctx->UsedFB = ASSIGN_GPU;
    Localctx->psAllLayer[singcout].info = dueto;
    return ASSIGN_GPU;

assigned_need_resigne:
    if((!Localctx->UsedFB) || isFB)
    {
        Localctx->UsedFB = ASSIGN_GPU ;
        return ASSIGN_NEEDREASSIGNED;
    }else{
        dueto = D_NO_PIPE;
        goto assign_gpu;
    }
}

int hwc_setup_layer(setup_dispc_data_t* DisplayData, HwcDisContext_t *Localctx)
{
    int CHCnt = 0, LCnt = 0, zOrder = 0 , VideoCnt=0, UiCnt = 0, nubmerlyer = 0;
    disp_layer_info *layer_info;
    const DisplayInfo *PsDisplayInfo = Localctx->psDisplayInfo;
    hwc_layer_1_t *psLayer;
    layer_info_t *psHwlayer_info;
    disp_layer_config *psDisconfig;
    ChannelInfo_t  *psChannelInfo = Localctx->ChannelInfo;
    bool enableLayers = !(PsDisplayInfo->setblank);

    Localctx->HaveVideo ? UiCnt = Localctx->VideoCHCnt+1 :UiCnt = 0;

    if(Localctx->HaveVideo)
    {
        DisplayData->ehancemode[PsDisplayInfo->VirtualToHWDisplay] = PsDisplayInfo->ehancemode;
    }else{
        DisplayData->ehancemode[PsDisplayInfo->VirtualToHWDisplay] = 0;
        DisplayData->ehancemode[PsDisplayInfo->VirtualToHWDisplay] = PsDisplayInfo->ehancemode;
    }
    while(CHCnt < (PsDisplayInfo->VirtualToHWDisplay  ? 2:4))
    {
        LCnt = 0;
        while(LCnt < PsDisplayInfo->LayerNumofCH)
        {
            psHwlayer_info = psChannelInfo[CHCnt].HwLayer[LCnt];
            psDisconfig = &(DisplayData->layer_info[PsDisplayInfo->VirtualToHWDisplay][zOrder]);
            if(psHwlayer_info != NULL)
            {
                psLayer = psHwlayer_info->psLayer;
            }else{
                memset(psDisconfig, 0, sizeof(disp_layer_config));
                goto nulllayer;
            }
            layer_info = &psDisconfig->info;
            if(psLayer == NULL)
            {
                memset(psDisconfig, 0, sizeof(disp_layer_config));
                goto nulllayer;
            }
            if(match_format(PsDisplayInfo, psHwlayer_info, layer_info) || resize_layer(PsDisplayInfo, layer_info, psLayer))
            {
                memset(psDisconfig, 0, sizeof(disp_layer_config));
                goto nulllayer;
            }
            layer_info->alpha_value = psChannelInfo[CHCnt].planeAlpha;
            if( HwcisBlended(psLayer) )
            {
                layer_info->alpha_mode  = 2;

            }else{
                layer_info->alpha_mode  = 1;
            }

            if(psLayer->blending == HWC_BLENDING_PREMULT)
            {
                layer_info->fb.pre_multiply = 1;
            }
#if defined(HOLD_BUFS_UNTIL_NOT_DISPLAYED)
            IonHandleAddRef(psHwlayer_info->psLayer);
            IonHandleDecRef();
#endif
            layer_info->zorder = nubmerlyer;
            psDisconfig->enable = enableLayers;
            nubmerlyer++;
nulllayer:
            psDisconfig->layer_id = LCnt;
            psDisconfig->channel = psChannelInfo[CHCnt].hasVideo ? VideoCnt : UiCnt;
            psHwlayer_info!= NULL ? psHwlayer_info->hwchannel = psDisconfig->channel : 0;
            LCnt++;
            zOrder++;
        }
        psChannelInfo[CHCnt].hasVideo ? VideoCnt++ :UiCnt++;
        CHCnt++;
    }
    return 1;
}

bool sunxi_prepare(hwc_display_contents_1_t **displays ,size_t NumofDisp)
{
    bool  forceSoftwareRendering = 0;
	hwc_display_contents_1_t *psDisplay;
	size_t disp, i;
    unsigned int SizeOfMem = 0;
    HwcAssignStatus AssignStatus;
    int NeedReAssignedLayer = 0;
    hwc_layer_1_t *psLayer;
    int SetOrder = 0;
	SUNXI_hwcdev_context_t *Globctx = &gSunxiHwcDevice;
    setup_dispc_data_t* DisplayData = Globctx->pvPrivateData;
    HwcDisContext_t *Localctx = NULL;
    DisplayInfo   *PsDisplayInfo;
	int err = 0;
    bool needsamedisp;
    unsigned int allreadyCopyAddr = 0;
    resetGlobDevice(Globctx);
    for(disp = 0; disp < NumofDisp; disp++ )
    {
        NeedReAssignedLayer = 0;
        SetOrder = 0;
        psDisplay = displays[disp];
        PsDisplayInfo = &Globctx->SunxiDisplay[disp>=2 ? 0 : disp];
        if( !psDisplay || psDisplay->numHwLayers <= 0 || disp > 2 || PsDisplayInfo->VirtualToHWDisplay == -EINVAL)
        {
            continue;
        }

        if(psDisplay->outbuf != NULL || psDisplay->outbufAcquireFenceFd != 0)
        {
            if (psDisplay->retireFenceFd >= 0)
            {
                close(psDisplay->retireFenceFd);
                psDisplay->retireFenceFd = -1;
            }
            if (psDisplay->outbuf != NULL) {
                psDisplay->outbuf = NULL;
            }
            if (psDisplay->outbufAcquireFenceFd >= 0) {
                close(psDisplay->outbufAcquireFenceFd);
                psDisplay->outbufAcquireFenceFd = -1;
            }
            ALOGV(" Virtual displays are not supported");
        }

        Localctx = &Globctx->DisContext[disp];
        if(NULL != Localctx->psAllLayer)
        {
            free(Localctx->psAllLayer);
        }
        memset(Localctx, 0, sizeof(HwcDisContext_t));

        layer_info  *AllLayers =(layer_info*)calloc(psDisplay->numHwLayers, sizeof(layer_info));
        memset(AllLayers,0,(sizeof(layer_info)) * psDisplay->numHwLayers);
#if defined (HWC_1_3) && defined (WB)
        if(disp > 0)
        {//need check
           if(DisplayData->SameDisp[disp-1] = check_same_diplay(displays, disp))
           {
                reset_layer_type(psDisplay,AllLayers,HWC_OVERLAY);
                free(AllLayers);
                continue;
           }else{
                if(disp == VIRTUAL_DISPLAY_ID_BASE)
                {
                    reset_layer_type(psDisplay, AllLayers, HWC_FRAMEBUFFER);
                    free(AllLayers);
                    continue;
                }
           }
        }
#endif

ReAssignedLayer:
        resetLocalInfo(PsDisplayInfo, Localctx, psDisplay, AllLayers);

        for(i = 0; i < psDisplay->numHwLayers; i++)
        {
            psLayer = &psDisplay->hwLayers[i];
            if((NULL != psLayer->handle) && (HAL_PIXEL_FORMAT_AW_FORCE_GPU == psLayer->videoFormat))
            {
                forceSoftwareRendering = 1;
                break;
            }
        }

    	for(i = (forceSoftwareRendering ? psDisplay->numHwLayers-1 : 0); i < psDisplay->numHwLayers; i++)
    	{

    		psLayer = &psDisplay->hwLayers[i];
            if(i >= psDisplay->numHwLayers-1)
            {
                if( psDisplay->numHwLayers == 1|| Localctx->UsedFB
                    || psLayer->handle == NULL ||forceSoftwareRendering
                    || SizeOfMem > CntOfLyaerMem(&psDisplay->hwLayers[psDisplay->numHwLayers-1]) )

                {
                    if((Globctx->ForceGPUComp && !Localctx->HaveVideo)
                        || forceSoftwareRendering || (psLayer->handle == NULL && Localctx->UsedFB))
                    {
                        ALOGD("Force GPU Composer");
                        SetOrder = 0;
                        resetLocalInfo(PsDisplayInfo, Localctx, psDisplay, AllLayers);
                    }
                    if(Globctx->ForceGPUComp && Localctx->HaveVideo && !Localctx->UsedFB && psDisplay->numHwLayers != 1)
                    {
                        break;
                    }
                }else{
                        break;
                }
            }
            AssignStatus = hwc_try_assign_layer(Localctx, i, SetOrder);
    	    switch (AssignStatus)
    		{
                case ASSIGN_OVERLAY:
                    if(psLayer->compositionType == HWC_FRAMEBUFFER)
                    {
                        psLayer->compositionType = HWC_OVERLAY;
                        if(Globctx->ForceGPUComp && !Localctx->UsedFB)
                        {
                            SizeOfMem += CntOfLyaerMem(psLayer);
                        }
                    }
                    SetOrder++;
                break;
    			case ASSIGN_GPU :

                break;
                case ASSIGN_FAILED:
                    ALOGD("Use GPU composite FB failed ");
                    resetLocalInfo(PsDisplayInfo, Localctx, psDisplay, AllLayers);
                    SetOrder=0;
                break;
                case ASSIGN_NEEDREASSIGNED :
    			    if(NeedReAssignedLayer == 0)
    			    {
                        NeedReAssignedLayer++;
                        SizeOfMem = 0;
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
        DisplayData->layer_num[PsDisplayInfo->VirtualToHWDisplay] = SetOrder;

        for(i = 0;i < psDisplay->numHwLayers; i++)
        {
            psLayer = &psDisplay->hwLayers[i];
            IMG_native_handle_t *handle = (IMG_native_handle_t *)psLayer->handle;
            if((NULL != handle) && (isValidVideoFormat(psLayer->videoFormat))
              && (HWC_FRAMEBUFFER == psLayer->compositionType))
            {
                if(allreadyCopyAddr != 0)
                {
                   //fixme: think about more than one gbuffer to setblank
                   continue;
                }
                if(0 == setGbufferBlank(psLayer->videoFormat, handle))
                {
                   allreadyCopyAddr++;
                }
            }
        }
    }
  return 0 ;
}


bool sunxi_set(hwc_display_contents_1_t** displays,size_t numDisplays, int *releasefencefd, int *retirefencefd)
{
    SUNXI_hwcdev_context_t *Globctx = &gSunxiHwcDevice;
    setup_dispc_data_t* DisplayData = Globctx->pvPrivateData;
    HwcDisContext_t *Localctx = NULL;
    DisplayInfo   *PsDisplayInfo;
    int HwlayerCnt;
    int FdCnt = 0,ret = -1 ;
    size_t disp, i;
    hwc_display_contents_1_t* psDisplay;
    hwc_layer_1_t *psLayer;
    unsigned long arg[4] = {0};
    int returnfenceFd[2] = {-1,-1};

    HwlayerCnt = DisplayData->layer_num[0] + DisplayData->layer_num[1];
    int Fds[HwlayerCnt];
    memset(Fds, -1, HwlayerCnt);
    for(disp = 0; disp < numDisplays; disp++)
    {
        Localctx = &Globctx->DisContext[disp > 1 ? 0 : disp];
        psDisplay = displays[disp];
        PsDisplayInfo = &Globctx->SunxiDisplay[disp > 1 ? 0 : disp];
    	if(!psDisplay || psDisplay->numHwLayers <= 0 || disp > 2 || PsDisplayInfo->VirtualToHWDisplay == -EINVAL)
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
                    *(Fds + FdCnt) = psLayer->acquireFenceFd;
                    FdCnt++;
                }
	    	}
        }
        hwc_setup_layer(DisplayData, Localctx);
        dump_displays(Localctx);
    }

    DisplayData->aquireFenceCnt = FdCnt;
    DisplayData->aquireFenceFd = Fds;
    DisplayData->returnfenceFd = returnfenceFd;
    DisplayData->androidfrmnum = Globctx->HWCFramecount;
    if(Globctx->DetectError == 0)
    {
        arg[0] = 0;
        arg[1] = (unsigned int)(Globctx->pvPrivateData);
        ret = ioctl(Globctx->DisplayFd, DISP_HWC_COMMIT, (unsigned long)arg);
    }
    if(!ret)
    {
        *releasefencefd = returnfenceFd[0];
        *retirefencefd = returnfenceFd[1];
    }
    Globctx->HWCFramecount++;
    if(0 != Globctx->exchangeDispChannel)
    {
        if(0 == pthread_mutex_trylock(&Globctx->lock))
        {
            ALOGD("exchanging ...");
            DisplayInfo psDisplayInfo; // fixme: we should exchange the pointers of SunxiDisplay[0/1], but not the SunxiDisplays
            int initDisplayWidth1 = Globctx->SunxiDisplay[1].InitDisplayWidth;
            int initDisplayHeight1 = Globctx->SunxiDisplay[1].InitDisplayHeight;
            int setPercentWidth = Globctx->SunxiDisplay[0].SetPersentWidth;
            int setPercentHeight = Globctx->SunxiDisplay[0].SetPersentHeight;
            memcpy((void *)&psDisplayInfo, (void *)(&Globctx->SunxiDisplay[0]), sizeof(DisplayInfo));
            memcpy((void *)(&Globctx->SunxiDisplay[0]), (void *)(&Globctx->SunxiDisplay[1]), sizeof(DisplayInfo));
            memcpy((void *)(&Globctx->SunxiDisplay[1]), (void *)&psDisplayInfo, sizeof(DisplayInfo));
            Globctx->SunxiDisplay[0].InitDisplayWidth = psDisplayInfo.InitDisplayWidth;
            Globctx->SunxiDisplay[0].InitDisplayHeight = psDisplayInfo.InitDisplayHeight;
            Globctx->SunxiDisplay[1].InitDisplayWidth = initDisplayWidth1;
            Globctx->SunxiDisplay[1].InitDisplayHeight = initDisplayHeight1;
            Globctx->SunxiDisplay[0].SetPersentWidth = setPercentWidth;
            Globctx->SunxiDisplay[1].SetPersentWidth = setPercentWidth;
            Globctx->SunxiDisplay[0].SetPersentHeight = setPercentHeight;
            Globctx->SunxiDisplay[1].SetPersentHeight = setPercentHeight;
            Globctx->exchangeDispChannel = 0;
            pthread_mutex_unlock(&Globctx->lock);
        }
    }
    return 1;
}

int initDisplayDevice(int disp, int hwDisp, disp_output *dispOutput)
{
    SUNXI_hwcdev_context_t *Globctx = &gSunxiHwcDevice;
    DisplayInfo *psDisplayInfo = &Globctx->SunxiDisplay[disp];
    int refreshRate, xdpi, ydpi, vsync_period;
    int tvmode4sysrsl;

    switch(dispOutput->type)
    {
    case DISP_OUTPUT_TYPE_LCD:
        struct fb_var_screeninfo info;
        if (ioctl(Globctx->FBFd, FBIOGET_VSCREENINFO, &info) == -1)
        {
            ALOGE("FBIOGET_VSCREENINFO ioctl failed: %s", strerror(errno));
            return -1;
        }
        refreshRate = 1000000000000LLU /
            (
            uint64_t( info.upper_margin + info.lower_margin + info.vsync_len + info.yres )
            * ( info.left_margin  + info.right_margin + info.hsync_len + info.xres )
            * info.pixclock
            );
		if (refreshRate == 0)
		{
			ALOGW("invalid refresh rate, assuming 60 Hz");
			refreshRate = 60;
		}
		if(info.width == 0)
		{
			psDisplayInfo->DiplayDPI_X = 160000;
		}
		else
		{
			psDisplayInfo->DiplayDPI_X = 1000 * (info.xres * 25.4f) / info.width;
		}
		if(info.height == 0)
		{
			psDisplayInfo->DiplayDPI_Y = 160000;
		}
		else
		{
			 psDisplayInfo->DiplayDPI_Y = 1000 * (info.yres * 25.4f) / info.height;
		}

        psDisplayInfo->DisplayFps = refreshRate;
        psDisplayInfo->DisplayVsyncP = 1000000000 / refreshRate;
        psDisplayInfo->InitDisplayWidth = info.xres;
        psDisplayInfo->InitDisplayHeight = info.yres;
        psDisplayInfo->VarDisplayWidth = info.xres;
        psDisplayInfo->VarDisplayHeight = info.yres;
        psDisplayInfo->issecure = 1;
        break;
    case DISP_OUTPUT_TYPE_TV:
    case DISP_OUTPUT_TYPE_HDMI:
        psDisplayInfo->VarDisplayWidth = get_info_mode(dispOutput->mode,WIDTH);
        psDisplayInfo->VarDisplayHeight = get_info_mode(dispOutput->mode,HEIGHT);
        psDisplayInfo->DisplayFps = get_info_mode(dispOutput->mode, REFRESHRAE);
        psDisplayInfo->DisplayVsyncP = 1000000000 / psDisplayInfo->DisplayFps;
        if((HWC_DISPLAY_PRIMARY == disp) && (0 == isDisplayP2P()))
        {
            //not point to point
            tvmode4sysrsl = getTvMode4SysResolution();
            psDisplayInfo->InitDisplayWidth = get_info_mode(tvmode4sysrsl, WIDTH);
            psDisplayInfo->InitDisplayHeight = get_info_mode(tvmode4sysrsl, HEIGHT);
        }
        else // i want that external display is display-p2p
        {
            psDisplayInfo->InitDisplayWidth = psDisplayInfo->VarDisplayWidth;
            psDisplayInfo->InitDisplayHeight = psDisplayInfo->VarDisplayHeight;
        }
        psDisplayInfo->DiplayDPI_X = 213000;
        psDisplayInfo->DiplayDPI_Y = 213000;
		psDisplayInfo->issecure = HAS_HDCP;
		break;
    case DISP_OUTPUT_TYPE_VGA:
    default:
        psDisplayInfo->VirtualToHWDisplay = -EINVAL;
        ALOGE("%s:%d, invalid output type %d", __func__, __LINE__, dispOutput->type);
        return -1;
    }

    if(-1 == getDispMarginFromFile(&psDisplayInfo->SetPersentWidth,
                 &psDisplayInfo->SetPersentHeight))
    {
         psDisplayInfo->SetPersentWidth = MARGIN_DEFAULT_PERCENT_WIDTH;
         psDisplayInfo->SetPersentHeight = MARGIN_DEFAULT_PERCENT_HEIGHT;
    }
    psDisplayInfo->VirtualToHWDisplay = hwDisp;
    psDisplayInfo->DisplayType = dispOutput->type;
    psDisplayInfo->DisplayMode = dispOutput->mode;
    psDisplayInfo->setDispMode = dispOutput->mode;
    psDisplayInfo->HwChannelNum = hwDisp ? 2 : NUMCHANNELOFDSP;
    psDisplayInfo->LayerNumofCH = NUMLAYEROFCHANNEL;
    psDisplayInfo->VideoCHNum = NUMCHANNELOFVIDEO;
    psDisplayInfo->VsyncEnable = 1;
    psDisplayInfo->ehancemode = 0;
    psDisplayInfo->Current3DMode = DISPLAY_2D_ORIGINAL;
    psDisplayInfo->de_clk = 224000000;
    psDisplayInfo->screenRadio = SCREEN_FULL;

    return 0;
}

static int InitDisplayDeviceInfo()
{
    SUNXI_hwcdev_context_t *Globctx = &gSunxiHwcDevice;
    int arg[4] = {0};
    int hwDisp = 0;
    int disp = HWC_DISPLAY_PRIMARY;
    unsigned char hasDispCnt = 0;
    disp_output dispOutput[NUMBEROFDISPLAY];

    // 1. get output types form disp-driver.
    for(; hwDisp < NUMBEROFDISPLAY; hwDisp++)
    {
        arg[0] = hwDisp;
        arg[1] = (int)&dispOutput[hwDisp];
        ioctl(Globctx->DisplayFd, DISP_GET_OUTPUT, arg);
        ALOGD("hwDisp=%d, dispOutput[%d, %d]", hwDisp,
            dispOutput[hwDisp].type, dispOutput[hwDisp].mode);
        switch(dispOutput[hwDisp].type)
        {
        case DISP_OUTPUT_TYPE_LCD:
        case DISP_OUTPUT_TYPE_TV:
        case DISP_OUTPUT_TYPE_VGA:
        case DISP_OUTPUT_TYPE_HDMI:
            hasDispCnt++;
            break;
        default:
            ALOGV("Dont get any valid display output type in hwDisp[%d]!", hwDisp);
            memset(&dispOutput[hwDisp], 0, sizeof(disp_output));
        }
    }
    if((hasDispCnt > 2) || (hasDispCnt == 0))
    {
        // we donot support three display-device initial at present, but fixme in the future.
        ALOGE("hwc: has none or more than 2 display device to initial !!!");
        return -1;
    }

    // 2. init the main-disp, and init the 2nd-disp if neccessery.
    //hwDisp = getMainDisplay();
    if(hwDisp >=0 && hwDisp < NUMBEROFDISPLAY && (0 != dispOutput[hwDisp].type))
    {
        //hwcDisplay[disp] = hwDisp;
        initDisplayDevice(disp, hwDisp, dispOutput + hwDisp);
        disp++;
        memset(&dispOutput[hwDisp], 0, sizeof(disp_output));
        hasDispCnt--;
    }
    for(hwDisp = 0;
        (hasDispCnt!=0) && (hwDisp<NUMBEROFDISPLAY) && (disp<HWC_NUM_PHYSICAL_DISPLAY_TYPES);
        hwDisp++)
    {
        if(0 != dispOutput[hwDisp].type)
        {
            initDisplayDevice(disp, hwDisp, dispOutput + hwDisp);
            disp++;
            memset(&dispOutput[hwDisp], 0, sizeof(disp_output));
            hasDispCnt--;
        }
    }

    arg[0] = Globctx->SunxiDisplay[HWC_DISPLAY_PRIMARY].VirtualToHWDisplay;
    arg[1] = 1;
    ioctl(Globctx->DisplayFd, DISP_VSYNC_EVENT_EN,(unsigned long)arg);
    _hwc_device_set_enhancemode(HWC_DISPLAY_PRIMARY, 1, 0);

    return 0;
}

SUNXI_hwcdev_context_t* hwc_create_device(void)
{
    SUNXI_hwcdev_context_t *Globctx = &gSunxiHwcDevice;

    unsigned long arg[4] = {0};
    int DispCnt;

    property_set("persist.sys.disp_init_exit", "1");
    Globctx->dispPolicy = getDispPolicy();

    Globctx->DisplayFd = open("/dev/disp", O_RDWR);
    if (Globctx->DisplayFd < 0)
    {
        ALOGE( "Failed to open disp device, ret:%d, errno: %d\n", Globctx->DisplayFd, errno);
    }

    Globctx->FBFd = open("/dev/graphics/fb0", O_RDWR);
    if (Globctx->FBFd < 0)
    {
        ALOGE( "Failed to open fb0 device, ret:%d, errno:%d\n", Globctx->FBFd, errno);
    }
    Globctx->IonFd = open("/dev/ion",O_RDWR);
    if(Globctx->IonFd < 0)
    {
        ALOGE( "Failed to open  ion device, ret:%d, errno:%d\n", Globctx->IonFd, errno);
    }
    Globctx->NumberofDisp = NUMBEROFDISPLAY;

    Globctx->SunxiDisplay =(DisplayInfo* )calloc(Globctx->NumberofDisp, sizeof(DisplayInfo));
    memset(Globctx->SunxiDisplay, 0, Globctx->NumberofDisp * sizeof(DisplayInfo));
    for(DispCnt = 0; DispCnt < Globctx->NumberofDisp; DispCnt++)
    {
        Globctx->SunxiDisplay[DispCnt].VirtualToHWDisplay = -EINVAL;
    }

    Globctx->DisContext =(HwcDisContext_t* )calloc(Globctx->NumberofDisp, sizeof(HwcDisContext_t));
    memset(Globctx->DisContext, 0, Globctx->NumberofDisp * sizeof(HwcDisContext_t));

    Globctx->pvPrivateData =(setup_dispc_data_t* )calloc(1, sizeof(setup_dispc_data_t));
    memset(Globctx->pvPrivateData, 0, sizeof(setup_dispc_data_t));

    InitDisplayDeviceInfo();
    Globctx->ban4k = check4KBan();
    Globctx->CanForceGPUCom = 1;
    Globctx->ForceGPUComp = 0;
	Globctx->fBeginTime = 0.0;
    Globctx->uiBeginFrame = 0;
    Globctx->hwcdebug = 0;
    Globctx->IonHandleHead = NULL;
    Globctx->exchangeDispChannel = 0;
    pthread_mutex_init(&Globctx->lock, NULL);

    ALOGD("####ban4k=%d",Globctx->ban4k);
    ALOGD( "Primary Display[%d]: Type:%d Mode:%d  Width:[%d]  Height:[%d] ",
        Globctx->SunxiDisplay[0].VirtualToHWDisplay,
        Globctx->SunxiDisplay[0].DisplayType,
        Globctx->SunxiDisplay[0].DisplayMode,
        Globctx->SunxiDisplay[0].VarDisplayWidth,
        Globctx->SunxiDisplay[0].VarDisplayHeight);
    ALOGD( "External Display[%d]: Type:%d Mode:%d  Width:[%d]  Height:[%d] ",
        Globctx->SunxiDisplay[1].VirtualToHWDisplay,
        Globctx->SunxiDisplay[1].DisplayType,
        Globctx->SunxiDisplay[1].DisplayMode,
        Globctx->SunxiDisplay[1].VarDisplayWidth,
        Globctx->SunxiDisplay[1].VarDisplayHeight);

    pthread_create(&Globctx->sVsyncThread, NULL, VsyncThreadWrapper, Globctx);

	return (SUNXI_hwcdev_context_t*)Globctx;
}

int hwc_destroy_device()
{
	SUNXI_hwcdev_context_t *Globctx = &gSunxiHwcDevice;

	close(Globctx->DisplayFd);
	close(Globctx->FBFd);
    close(Globctx->IonFd);
	free(Globctx->pvPrivateData);
    free(Globctx->SunxiDisplay);
	return 1;
}
