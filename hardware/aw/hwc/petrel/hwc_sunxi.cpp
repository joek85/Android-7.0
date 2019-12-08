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
// PHY_OFFSET is 0x40000000 at A83 platform
#define PHY_OFFSET 0x40000000

#define PLATFORM_CMCCWASU 0x08
#define PLATFORM_ALIYUN 0x09
#define PLATFORM_TVD 0x0A
#define UNKNOWN_PLATFORM 0xFF

SUNXI_hwcdev_context_t gSunxiHwcDevice;
static unsigned int get_ion_address(int sharefd);

static const char * hwc_print_info(AssignDUETO_T eError)
{
    switch(eError) {
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
    AssignDUETO(D_MEM_LIMIT)
    #undef AssignDUETO
    default:
        return "Unknown reason";
    }
}

static unsigned int get_sunxi_metadata_flag(buffer_handle_t handle)
{
	if (NULL != handle) {
#if GRALLOC_SUNXI_METADATA_BUF
		private_handle_t *hdl = (private_handle_t *)handle;
		int ret = 0;

		SUNXI_hwcdev_context_t *Globctx = &gSunxiHwcDevice;
		ion_user_handle_t ion_handle;
		unsigned char *map_ptr = NULL;
		int map_fd = -1;
		unsigned int flag = 0;

		if (0 == hdl->ion_metadata_size) {
			ALOGW("ion_metadata_size is 0");
			return 0;
		}

		ret = ion_import(Globctx->IonFd, hdl->metadata_fd, &ion_handle);
		if (ret) {
			ALOGD("ion import failed, ret=%d, metadata_fd=%d!",
				ret, hdl->metadata_fd);
			return 0;
		}
		ret = ion_map(Globctx->IonFd, ion_handle, hdl->ion_metadata_size,
			PROT_READ, MAP_SHARED, 0, &map_ptr, &map_fd);
		if (ret) {
			ALOGE("ion_map failed, ret=%d\n", ret);
			if (0 <= map_fd)
				close(map_fd);
			ion_free(Globctx->IonFd, ion_handle);
			return 0;
		}

		struct sunxi_metadata *ptr = (struct sunxi_metadata *)map_ptr;
		flag = ptr->flag;

		struct afbc_header *p = &(ptr->afbc_head);
		/* ALOGD("&&&&& afbc header:");
		ALOGD("%u,%u,%u,%u;\n"
			"%u,%u,%u,%u;\n"
			"%u,%u,%u,%u;\n"
			"%u,%u,%u,%u\n"
			"%u,%u,%u @@@.",
			p->signature, p->filehdr_size, p->version, p->body_size,
			p->ncomponents, p->header_layout, p->yuv_transform, p->block_split,
			p->inputbits[0], p->inputbits[1], p->inputbits[2], p->inputbits[3],
			p->block_width, p->block_height, p->width, p->height,
			p->left_crop, p->top_crop, p->block_layout);
		*/

		ret = munmap(map_ptr, hdl->ion_metadata_size);
		if (0 != ret) {
			ALOGD("munmap sunxi metadata failed ! ret=%d", ret);
		}
		close(map_fd);
		ion_free(Globctx->IonFd, ion_handle);
		return flag;
#endif
	} else {
		ALOGE("%s,%d", __FUNCTION__, __LINE__);
	}
	return 0;
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
        "SIDE",
        "CURS",
        "UNKN"
    };
    if(Globctx->hwcdebug & LAYER_DUMP) {
        if(F_HWDISP_ID == PsDisplayInfo->VirtualToHWDisplay) {
            ALOGD("###--- THIS IS F DISPLAY ID. ---###");
            return;
        }
        ALOGD("the framecount:%d  display:%d  vsync:%s  mytimestamp:%llu"
            "\n type|CH|VCH|  Scalht |  Scalwt | PL  | bandwid |  handle | phyaddr |"
            "  Usage  |  flags  | tr| blend| format  |        source crop      |"
            "         frame            |dataspace|reason\n"
            "-----+--+---+---------+---------+-----+---------+---------+---------+---"
            "------+---------+---+------+---------+-------------------------+---------"
            "-----------------|----------|-------\n",
            Globctx->HWCFramecount+1, PsDisplayInfo->VirtualToHWDisplay,
            PsDisplayInfo->VsyncEnable ? "enable" : "disable", PsDisplayInfo->mytimestamp);
        for(i = 0; i < Localctx->numberofLayer ; i++) {
            sunxiLayers = AllLayers+i;
            hwc_layer_1_t *l = sunxiLayers->psLayer;
            private_handle_t* handle = (private_handle_t*)l->handle;
            ALOGD("%5s|%2d|%3s| %f| %f|0x%03x| %08x| %08x| %08x| %08x| %08x|"
                " %02x| %05x| %08x|[%5d,%5d,%5d,%5d]|[%5d,%5d,%5d,%5d]|%08x|%s\n",
                compositionTypeName[l->compositionType],
                sunxiLayers->hwchannel,
                sunxiLayers->virchannel >= 0 ?
                    (Localctx->ChannelInfo[sunxiLayers->virchannel].hasVideo?"Yes":"No") : "No",
                sunxiLayers->virchannel >= 0 ?
                    Localctx->ChannelInfo[sunxiLayers->virchannel].HTScaleFactor : 0,
                sunxiLayers->virchannel >= 0 ?
                    Localctx->ChannelInfo[sunxiLayers->virchannel].WTScaleFactor : 0,
                sunxiLayers->virchannel >= 0 ?
                    Localctx->ChannelInfo[sunxiLayers->virchannel].planeAlpha : 0xf00,
                sunxiLayers->virchannel >= 0 ? sunxiLayers->bandwidth : 0,
                (unsigned int)(l->handle),
                handle == 0 ? 0 :
                    (((handle->flags & private_handle_t::PRIV_FLAGS_USES_CONFIG)
                        && (Globctx->iomem_type != IOMEM_TYPE_IOMMU)) ?
                        get_ion_address(handle->share_fd) : 0),
                handle == 0 ? 0 : handle->usage,
                l->flags,
                l->transform,
                l->blending,
                handle==0?0:handle->format,
                l->sourceCrop.left,
                l->sourceCrop.top,
                l->sourceCrop.right,
                l->sourceCrop.bottom,
                l->displayFrame.left,
                l->displayFrame.top,
                l->displayFrame.right,
                l->displayFrame.bottom,
                l->dataspace,
                sunxiLayers->info != DEFAULT? hwc_print_info(sunxiLayers->info): "NOT_ASSIGNED");
        }
        ALOGD("-----+--+---+---------+---------+---+---------+---------+"
            "---------+---------+---+------+---------+------------------"
            "-------+--------------------------|------------\n");
    }
}

static inline void reset_layer_type(hwc_display_contents_1_t* list,
    layer_info *pslayers, int hwctype)
{
    unsigned int j = 0;
    if (list && list->numHwLayers >= 1) {
        for(j = 0; j < list->numHwLayers; j++) {
            if(list->hwLayers[j].compositionType != HWC_FRAMEBUFFER_TARGET) {
                list->hwLayers[j].compositionType = hwctype;
            }
            pslayers[j].psLayer = &list->hwLayers[j];
            pslayers[j].assigned = ASSIGN_INIT;
            pslayers[j].hwchannel = -2;
            pslayers[j].virchannel = -2;
            pslayers[j].HwzOrder  = -1;
            pslayers[j].OrigOrder = -1;
            pslayers[j].is3D = 0;
            pslayers[j].is_dim_layer = 0;
            pslayers[j].info = DEFAULT;
        }
    }
}
static void inline CalculateFactor(DisplayInfo *PsDisplayInfo,
    float *XWidthFactor, float *XHighetfactor)
{
    float WidthFactor = (float)PsDisplayInfo->PersentWidth/ 100;
    float Highetfactor = (float)PsDisplayInfo->PersentHeight/ 100;
    if(PsDisplayInfo->InitDisplayWidth && PsDisplayInfo->InitDisplayHeight) {
        WidthFactor = (float)(((float)PsDisplayInfo->VarDisplayWidth)
            / PsDisplayInfo->InitDisplayWidth * PsDisplayInfo->PersentWidth / 100);
        Highetfactor = (float)(((float)PsDisplayInfo->VarDisplayHeight)
            / PsDisplayInfo->InitDisplayHeight * PsDisplayInfo->PersentHeight / 100);
    }
    *XWidthFactor = WidthFactor;
    *XHighetfactor = Highetfactor;
}

static void inline CalculateLayerScaleFactor(HwcDisContext_t *localctx, hwc_layer_1_t *l,
    float *xscale, float *yscale)
{
    int cropWidth  = l->sourceCrop.right  - l->sourceCrop.left;
    int cropHeight = l->sourceCrop.bottom - l->sourceCrop.top;

    if (cropWidth == 0 || cropWidth == 1)
        *xscale = 1.0f;
    else
        *xscale = ((float)(l->displayFrame.right - l->displayFrame.left))
                            / ((float)cropWidth) * localctx->WidthScaleFactor;

    if (cropHeight == 0 || cropHeight == 1)
        *yscale = 1.0f;
    else
        *yscale = ((float)(l->displayFrame.bottom - l->displayFrame.top))
                            / ((float)cropHeight) * localctx->HighetScaleFactor;
}

static void resetLocalInfo(DisplayInfo *psDisplayInfo, HwcDisContext_t * Localctx,
    hwc_display_contents_1_t *psDisplay, layer_info *AllLayers)
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
    psDisplayInfo->bandwidth.available = psDisplayInfo->bandwidth.max
        - psDisplayInfo->bandwidth.video - psDisplayInfo->bandwidth.fb;
	psDisplayInfo->hw_input = HW_INPUT_SDR;
    reset_layer_type(psDisplay,AllLayers,HWC_FRAMEBUFFER);
    while(i--) {
        j = 4;
        Localctx->ChannelInfo[i].hasBlend = 0;
        Localctx->ChannelInfo[i].hasVideo= 0;
        Localctx->ChannelInfo[i].WTScaleFactor = 1.0;
        Localctx->ChannelInfo[i].HTScaleFactor = 1.0;
        Localctx->ChannelInfo[i].iCHFormat = 0;
        Localctx->ChannelInfo[i].planeAlpha = 0xff;
        Localctx->ChannelInfo[i].HwLayerCnt = 0;
        while(j--){
            Localctx->ChannelInfo[i].HwLayer[j] = NULL;
        }
    }
}

static int inline cal_layer_mem(hwc_layer_1_t *psLayer)
{
    if (!psLayer || !psLayer->handle)
        return 0;

    int bpp;
    int format = (psLayer->compositionType == HWC_FRAMEBUFFER_TARGET) ?
        HAL_PIXEL_FORMAT_BGRA_8888 : ((private_handle_t*)(psLayer->handle))->format;
    switch(format) {
    case HAL_PIXEL_FORMAT_RGBA_8888:
    case HAL_PIXEL_FORMAT_RGBX_8888:
    case HAL_PIXEL_FORMAT_BGRA_8888:
    case HAL_PIXEL_FORMAT_BGRX_8888:
        bpp = 32;
        break;
    case HAL_PIXEL_FORMAT_RGB_888:
        bpp = 24;
        break;
    case HAL_PIXEL_FORMAT_RGB_565:
        bpp = 16;
        break;
    case HAL_PIXEL_FORMAT_YCrCb_420_SP:
    case HAL_PIXEL_FORMAT_YV12:
    case HAL_PIXEL_FORMAT_AW_NV12_10bit:
    case HAL_PIXEL_FORMAT_AW_NV21_10bit:
	case HAL_PIXEL_FORMAT_AW_YV12_10bit:
	case HAL_PIXEL_FORMAT_AW_I420_10bit:
	case HAL_PIXEL_FORMAT_AW_NV12:
       bpp = 12; // 1.5 * 8
       break;
    case HAL_PIXEL_FORMAT_AW_P010_UV:
    case HAL_PIXEL_FORMAT_AW_P010_VU:
        bpp = 24; // 3 * 8
        break;
    default:
        bpp = 32;
        ALOGD("cal_layer_mem: format=%d", format);
    }
    return (psLayer->sourceCrop.right - psLayer->sourceCrop.left)
        * (psLayer->sourceCrop.bottom - psLayer->sourceCrop.top)
        * bpp / 8;
}

static inline bool Check3DLayer(const DisplayInfo *PsDisplayInfo)
{
    switch(PsDisplayInfo->Current3DMode) {
    case DISPLAY_2D_LEFT:
    case DISPLAY_2D_TOP:
    case DISPLAY_3D_LEFT_RIGHT_HDMI:
    case DISPLAY_3D_TOP_BOTTOM_HDMI:
    case DISPLAY_3D_DUAL_STREAM:
    case DISPLAY_3D_LEFT_RIGHT_ALL:
    case DISPLAY_3D_TOP_BOTTOM_ALL:
        return 1;
    default:
        return 0;
    }
}

static bool inline ReCountPresent(DisplayInfo *PsDisplayInfo)
{
    if(PsDisplayInfo->VirtualToHWDisplay != EVALID_HWDISP_ID) {
        PsDisplayInfo->PersentHeight = PsDisplayInfo->SetPersentHeight;
        PsDisplayInfo->PersentWidth = PsDisplayInfo->SetPersentWidth;
    }
    return 0;
}

static void setup_dispc_data_reset(setup_dispc_data_t *pdata);
static void resetGlobDevice(SUNXI_hwcdev_context_t * Globctx)
{
    int i;
    setup_dispc_data_reset(&Globctx->PrivateData[0]);
    setup_dispc_data_reset(&Globctx->PrivateData[1]);
    for (i = 0; i < Globctx->NumberofDisp; i++) {
        if(Globctx->SunxiDisplay[i].VirtualToHWDisplay != EVALID_HWDISP_ID) {
            ReCountPresent(&Globctx->SunxiDisplay[i]);
        }
        HwcDisContext_t * localctx = &Globctx->DisContext[i];
        layer_info *cache_layer = localctx->psAllLayer;
        unsigned int cache_layer_cnt = localctx->allocNumLayer;
        memset(localctx, 0, sizeof(HwcDisContext_t));
        localctx->psAllLayer = cache_layer;
        localctx->allocNumLayer = cache_layer_cnt;
    }
}

static inline int HwcUsageSW(private_handle_t *psHandle)
{
    return (((psHandle->usage & GRALLOC_USAGE_SW_READ_MASK) == GRALLOC_USAGE_SW_READ_OFTEN)
        || ((psHandle->usage & GRALLOC_USAGE_SW_WRITE_MASK) == GRALLOC_USAGE_SW_WRITE_OFTEN));
}

static inline int HwcUsageSWwrite(private_handle_t *psHandle)
{
    return psHandle->usage & GRALLOC_USAGE_SW_WRITE_OFTEN;
}

static inline int HwcUsageProtected(private_handle_t *psHandle)
{
    return psHandle->usage & GRALLOC_USAGE_PROTECTED;
}

static inline bool HwcsupportAlpha(int format)
{
    switch(format) {
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

static bool inline check_same_scale(float SRWscaleFac, float SRHscaleFac,
    float DTWscalFac, float DTHscaleFac )
{
    return (((SRWscaleFac - DTWscalFac) > -0.001) && ((SRWscaleFac - DTWscalFac) < 0.001))
        &&(((SRHscaleFac - DTHscaleFac) > -0.001)&&((SRHscaleFac - DTHscaleFac) < 0.001));
}

static bool HwcisScaled(hwc_layer_1_t *layer,float *WScaleFactor, float * HScaleFactor)
{
    bool ret = 0;
    int w = layer->sourceCrop.right - layer->sourceCrop.left;
    int h = layer->sourceCrop.bottom - layer->sourceCrop.top;
    ret = ((layer->displayFrame.right - layer->displayFrame.left) != w)
        || ((layer->displayFrame.bottom - layer->displayFrame.top) != h);
    if(ret) {
        *WScaleFactor = float(layer->displayFrame.right - layer->displayFrame.left) / float(w);
        *HScaleFactor = float(layer->displayFrame.bottom - layer->displayFrame.top) / float(h);
    } else {
        *WScaleFactor = 1;
        *HScaleFactor = 1;
    }
    return ret;
}

static AssignDUETO_T HwcisValidLayer(hwc_layer_1_t *layer)
{
    private_handle_t *handle = (private_handle_t *)layer->handle;

    if (handle->format >= 0x100
        && handle->format <= 0x107
        && HAL_PIXEL_FORMAT_AW_NV12 != handle->format) {
        return D_NO_FORMAT;
    }

    if ((layer->flags & HWC_SKIP_LAYER)) {
        return D_SKIP_LAYER;
    }
    if (layer->compositionType == HWC_BACKGROUND) {
        return D_BACKGROUND;
    }
    if(layer->transform) {
        return D_TR_N_0;
    }
    return I_OVERLAY;
}

static int is_awformat_buf(buffer_handle_t handle)
{
	if (NULL != handle) {
		private_handle_t *hdl = (private_handle_t *)handle;
		if ((0x100 <= hdl->format)
			&& (0x1FF >= hdl->format)) {
			return 1;
		}
	}
	return 0;
}

static int is_gpuformat_buf(buffer_handle_t handle)
{
	if (NULL != handle) {
		private_handle_t *hdl = (private_handle_t *)handle;
		if (!is_awformat_buf(handle)
			|| (HAL_PIXEL_FORMAT_AW_NV12 == hdl->format)) {
			return 1;
		}
	}
	return 0;
}

static int is_afbc_buf(buffer_handle_t handle)
{
	if (NULL != handle) {
#if GRALLOC_SUNXI_METADATA_BUF
		private_handle_t *hdl = (private_handle_t *)handle;
		int metadata_fd = hdl->metadata_fd;
		unsigned int flag = hdl->ion_metadata_flag;
		if ((0 <= metadata_fd)
			&& (flag & SUNXI_METADATA_FLAG_AFBC_HEADER)) {
			return 1;
		}
#endif
	}
	return 0;
}

int is_skip_gpu_buf(buffer_handle_t handle)
{
	if (is_afbc_buf(handle)
		|| !is_gpuformat_buf(handle))
		return 1;
	else
		return 0;
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

    if(mid_diff_x < (sum_width/2) && mid_diff_y < (sum_height/2)) {
        return 1;//return 1 is intersect
    }
    return 0;
}

static inline bool CheckVideoFormat(int format)
{
    switch(format) {
    case HAL_PIXEL_FORMAT_YV12:
    case HAL_PIXEL_FORMAT_YCrCb_420_SP:
    case HAL_PIXEL_FORMAT_AW_NV12_10bit:
    case HAL_PIXEL_FORMAT_AW_NV21_10bit:
    case HAL_PIXEL_FORMAT_AW_YV12_10bit:
    case HAL_PIXEL_FORMAT_AW_I420_10bit:
    case HAL_PIXEL_FORMAT_AW_P010_UV:
    case HAL_PIXEL_FORMAT_AW_P010_VU:
    case HAL_PIXEL_FORMAT_AW_P210_UV:
    case HAL_PIXEL_FORMAT_AW_P210_VU:
    case HAL_PIXEL_FORMAT_AW_NV12:
        return true;
    default:
        return false;
    }
}

static int HwcCanScale(HwcDisContext_t  *Localctx,hwc_layer_1_t * psLayer)
{
    int src_w,frame_rate,lcd_h,lcd_w,dst_w;
    long long layer_line_peroid,lcd_line_peroid,de_freq;
    const DisplayInfo *PsDisplayInfo = Localctx->psDisplayInfo;

    src_w = psLayer->sourceCrop.right - psLayer->sourceCrop.left;
    lcd_w = PsDisplayInfo->VarDisplayWidth;
    lcd_h = PsDisplayInfo->VarDisplayHeight;
    dst_w = (int)(psLayer->displayFrame.right * Localctx->WidthScaleFactor)
        - (int)(psLayer->displayFrame.left * Localctx->WidthScaleFactor);
    frame_rate = PsDisplayInfo->DisplayFps;
    if((PsDisplayInfo->Current3DMode == DISPLAY_3D_LEFT_RIGHT_HDMI)
        || (PsDisplayInfo->Current3DMode == DISPLAY_3D_LEFT_RIGHT_ALL)) {
        dst_w /=2;
    }
    if((DISPLAY_3D_LEFT_RIGHT_HDMI == PsDisplayInfo->Current3DMode)
        || (DISPLAY_2D_LEFT == PsDisplayInfo->Current3DMode)
        || (DISPLAY_3D_LEFT_RIGHT_ALL == PsDisplayInfo->Current3DMode)) {
        if(CheckVideoFormat(((private_handle_t*)psLayer->handle)->format)) {
            src_w >>= 1;
        }
    }
    de_freq = PsDisplayInfo->de_clk;
    lcd_line_peroid = 1000000000/frame_rate/lcd_h;
    layer_line_peroid = (src_w > dst_w) ? (1000000 * ((long long)(lcd_w - dst_w + src_w))
        /(de_freq/1000)):(1000000*((long long)lcd_w)/(de_freq/1000));

    if((lcd_line_peroid * 85 / 100) < layer_line_peroid) {
        return 0; //can't
    }
    return 1;//can
}

#define max(a,b) ((a) > (b) ? (a) : (b))
#define min(a,b) ((a) < (b) ? (a) : (b))

/*
 * check if de can handle ui scale.
 *
 * input params:
 * ovlw, ovlh : source size
 * outw, outh : frame size
 * lcdw, lcdh : screen size
 * lcd_fps : frame rate in Hz
 * de_freq : de clock freqence in Hz
 *
 * return:
 * 1 -- can use de scale
 * 0 -- can NOT use de scale
 */
static int de_scale_capability_detect(
            unsigned int ovlw, unsigned int ovlh,
            unsigned int outw, unsigned int outh,
            unsigned long long lcdw,
            unsigned long long lcdh,
            unsigned long long lcd_fps,
            unsigned long long de_freq)
{
    unsigned long long de_freq_req;
    unsigned long long lcd_freq;
    unsigned long long layer_cycle_num;
    unsigned int dram_efficience = 95;

    /*
     *  ovlh > outh : vertical scale down
     *  ovlh < outh : vertical scale up
     */
    if (ovlh > outh)
        layer_cycle_num = max(ovlw, outw) + max((ovlh - outh) * ovlw / outh, lcdw - outw);
    else
        layer_cycle_num = max(ovlw, outw) + (lcdw - outw);

    lcd_freq = (lcdh) * (lcdw) * (lcd_fps) * 10 / 9;
    de_freq_req = lcd_freq * layer_cycle_num * 100 / dram_efficience;
    de_freq_req = de_freq_req / lcdw;

    if (de_freq > de_freq_req)
        return 1;
    else
        return 0;
}

static int hwc_ui_scaler_check(hwc_layer_1_t *layer, const DisplayInfo *info)
{
#ifdef HWC_1_3
    int src_w = ceil(layer->sourceCropf.right - layer->sourceCropf.left);
    int src_h = ceil(layer->sourceCropf.bottom - layer->sourceCropf.top);
    int out_w = layer->displayFrame.right - layer->displayFrame.left;
    int out_h = layer->displayFrame.bottom - layer->displayFrame.top;
#else
    int src_w = layer->sourceCrop.right - layer->sourceCrop.left;
    int src_h = layer->sourceCrop.bottom - layer->sourceCrop.top;
    int out_w = layer->displayFrame.right - layer->displayFrame.left;
    int out_h = layer->displayFrame.bottom - layer->displayFrame.top;
#endif

    out_w = out_w * info->PersentWidth  * info->VarDisplayWidth  / info->InitDisplayWidth  / 100;
    out_h = out_h * info->PersentHeight * info->VarDisplayHeight / info->InitDisplayHeight / 100;
    out_w = out_w == 0 ? 1 : out_w;
    out_h = out_h == 0 ? 1 : out_h;

    return de_scale_capability_detect(src_w, src_h, out_w, out_h,
            info->VarDisplayWidth, info->VarDisplayHeight,
            info->fps, info->de_clk);
}

static inline int Find_channel_layer(ChannelInfo_t *ChannelInfo , bool last )
{
    layer_info_t *dispinfo = NULL;
    if(ChannelInfo->HwLayerCnt == 0) {
        return -1;
    }
    if(!last) {
        dispinfo = ChannelInfo->HwLayer[0];
    } else {
        dispinfo = ChannelInfo->HwLayer[ChannelInfo->HwLayerCnt-1];
    }
    int index = last ? ChannelInfo->HwLayerCnt-1 : 0;
    while (!dispinfo && index > 0) {
        index--;
        dispinfo = ChannelInfo->HwLayer[index];
    }
    return dispinfo == NULL ? -1 : dispinfo->OrigOrder;
}

static int Match_nofull_channel(HwcDisContext_t *Localctx, int channel,
    bool isvideo, bool is_dim_layer, int emptynum, int format, float SRWscaleFac, float SRHscaleFac,
    unsigned char planealpha)
{
    ChannelInfo *CH = NULL;
    const DisplayInfo *PsDisplayInfo = Localctx->psDisplayInfo;
    int whilecnt,tmpch;

    if(channel >= 0 && channel < PsDisplayInfo->HwChannelNum) {
        whilecnt = 1;
        tmpch = channel;
    } else {
        whilecnt = Localctx->HwCHUsedCnt +1;
        tmpch = 0;
    }

    while(whilecnt --) {
        CH = &Localctx->ChannelInfo[tmpch];
        if((CH->HwLayerCnt == 0) ||
            (((CH->HwLayerCnt + emptynum) <= PsDisplayInfo->LayerNumofCH)
            && (isvideo ? ((CH->hasVideo) && (CH->iCHFormat == format)) : (CH->hasVideo ? 0 : 1))
            && (CH->planeAlpha == planealpha)
            && (is_dim_layer ||
                check_same_scale(CH->WTScaleFactor, CH->HTScaleFactor, SRWscaleFac, SRHscaleFac)))) {
            return tmpch;
        }
        tmpch++;
    }
    return -1;
}

int Check_X_List(hwc_layer_1_t *psLayer, HwcDisContext_t *Localctx, int from, int to,
    int channel, HwcAssignStatus type)
{
    const DisplayInfo *PsDisplayInfo = Localctx->psDisplayInfo;
    layer_info_t *dispinfo = Localctx->psAllLayer;
    dispinfo += from;
    while(from >= 0 && from <= to && to <= Localctx->numberofLayer) {
        if((dispinfo->psLayer != NULL)
            && ((channel >= -1 && channel < PsDisplayInfo->HwChannelNum ) ?
                dispinfo->virchannel == channel : 1)
            && ((type == ASSIGN_GPU || type == ASSIGN_OVERLAY) ?
                dispinfo->assigned == type : 1)
            && (dispinfo->is_dim_layer
                || hwc_region_intersect(&dispinfo->psLayer->displayFrame, &psLayer->displayFrame))) {
            return 1;
        }
        dispinfo++;
        from++;
    }
    return 0;
}

static inline bool isValidUiFormat(int format)
{
    switch(format)
    {
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

static bool check_same_diplay(hwc_display_contents_1_t **psDisplay,int num)
{
    hwc_display_contents_1_t *priDisp = psDisplay[0];
    hwc_display_contents_1_t *cmpDisp = psDisplay[num];
    if(cmpDisp->numHwLayers != priDisp->numHwLayers) {
        return 0;
    }
    int cnt = cmpDisp->numHwLayers-1;
    while(cnt--) {
        if(cmpDisp->hwLayers[cnt].handle != priDisp->hwLayers[cnt].handle) {
            return 0;
        }
    }
    return 1;
}

static int calc_point_byPercent(const unsigned char percent,
    const int middle_point, const int src_point)
{
    int condition = (src_point > middle_point) ? 1 : 0;
    int length = condition ? (src_point - middle_point) : (middle_point - src_point);
    length = length * percent / 100;
    return condition ? (middle_point + length) : (middle_point - length);
}

void recomputeDisplayFrame(int screenRadio, const hwc_rect_t sourceCrop,
    const int unitedFrameWidth, const int unitedFrameHeight, hwc_rect_t *displayFrame)
{
    switch(screenRadio) {
    case SCREEN_AUTO: {
        unsigned int ratio_width, ratio_height;
        int frame_width = sourceCrop.right - sourceCrop.left;
        int frame_height = sourceCrop.bottom - sourceCrop.top;
        if(frame_width >= unitedFrameWidth || frame_height >= unitedFrameHeight) {
            // scale down
            if(frame_width > unitedFrameWidth) {
                frame_height = frame_height * unitedFrameWidth / frame_width;
                frame_width = unitedFrameWidth;
            }
            if(frame_height > unitedFrameHeight) {
                frame_width = frame_width * unitedFrameHeight / frame_height;
                frame_height = unitedFrameHeight;
            }
        } else {
            // scale up
            ratio_width = frame_width * unitedFrameHeight;
            ratio_height = frame_height * unitedFrameWidth;
            if(ratio_width >= ratio_height) {
                // scale up until frame_width equal unitedFrameWidth
                frame_height = ratio_height / frame_width;
                frame_width = unitedFrameWidth;
            } else {
                // scale up until frame_height equal unitedFrameHeight
                frame_width = ratio_width / frame_height;
                frame_height = unitedFrameHeight;
            }
        }
        //ALOGD("###frame[%d,%d], unitedFrame[%d,%d], displayFrame[%d,%d,%d,%d]",
        //    frame_width, frame_height, unitedFrameWidth, unitedFrameHeight,
        //    displayFrame->left, displayFrame->right, displayFrame->top, displayFrame->bottom);
        if(unitedFrameWidth > frame_width) {
            ratio_width = frame_width * (displayFrame->right - displayFrame->left)
                / unitedFrameWidth;
            displayFrame->left += (displayFrame->right - displayFrame->left - ratio_width) >> 1;
            displayFrame->right = displayFrame->left + ratio_width;
        }
        if(unitedFrameHeight > frame_height) {
            ratio_height = frame_height * (displayFrame->bottom - displayFrame->top)
                / unitedFrameHeight;
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

static bool resize_layer(const DisplayInfo *PsDisplayInfo,
    disp_layer_info_t *layer_info , hwc_layer_1_t *psLayer)
{
    hwc_rect_t sourceCrop;
    hwc_rect_t displayFrame;
    disp_rect screen_win;
    private_handle_t * handle = (private_handle_t *)psLayer->handle;

    sourceCrop.left =   psLayer->sourceCrop.left < 0 ? 0 : psLayer->sourceCrop.left;
    sourceCrop.right =  psLayer->sourceCrop.right < 0 ? 0 : psLayer->sourceCrop.right;
    sourceCrop.top =    psLayer->sourceCrop.top < 0 ? 0 : psLayer->sourceCrop.top;
    sourceCrop.bottom = psLayer->sourceCrop.bottom < 0 ? 0 : psLayer->sourceCrop.bottom;

    layer_info->fb.crop.x = (long long)(((long long)(sourceCrop.left)) << 32);
    layer_info->fb.crop.width = (long long)(((long long)(sourceCrop.right)) << 32);
    layer_info->fb.crop.width -= layer_info->fb.crop.x;
    layer_info->fb.crop.y = (long long)(((long long)(sourceCrop.top)) << 32);
    layer_info->fb.crop.height = (long long)(((long long)(sourceCrop.bottom)) << 32);
    layer_info->fb.crop.height -= layer_info->fb.crop.y;

    memcpy((void *)&displayFrame, (void *)&(psLayer->displayFrame), sizeof(hwc_rect_t));
    if(CheckVideoFormat(handle->format)) {
        int unitedFrameWidth = gSunxiHwcDevice.SunxiDisplay[0].InitDisplayWidth;
        int unitedFrameHeight = gSunxiHwcDevice.SunxiDisplay[0].InitDisplayHeight;
        recomputeDisplayFrame(PsDisplayInfo->screenRadio, sourceCrop,
        unitedFrameWidth, unitedFrameHeight, &displayFrame);
    }
    layer_info->screen_win.x = calc_point_byPercent(PsDisplayInfo->PersentWidth,
        PsDisplayInfo->InitDisplayWidth >> 1, displayFrame.left)
        * PsDisplayInfo->VarDisplayWidth / PsDisplayInfo->InitDisplayWidth;
    layer_info->screen_win.width = calc_point_byPercent(PsDisplayInfo->PersentWidth,
        PsDisplayInfo->InitDisplayWidth >> 1, displayFrame.right)
        * PsDisplayInfo->VarDisplayWidth / PsDisplayInfo->InitDisplayWidth;
    layer_info->screen_win.width -= layer_info->screen_win.x;
    layer_info->screen_win.width = (0 == layer_info->screen_win.width) ?
        1 : layer_info->screen_win.width;

    layer_info->screen_win.y = calc_point_byPercent(PsDisplayInfo->PersentHeight,
        PsDisplayInfo->InitDisplayHeight >> 1, displayFrame.top)
        * PsDisplayInfo->VarDisplayHeight / PsDisplayInfo->InitDisplayHeight;
    layer_info->screen_win.height = calc_point_byPercent(PsDisplayInfo->PersentHeight,
        PsDisplayInfo->InitDisplayHeight >> 1, displayFrame.bottom)
        * PsDisplayInfo->VarDisplayHeight / PsDisplayInfo->InitDisplayHeight;
    layer_info->screen_win.height -= layer_info->screen_win.y;
    layer_info->screen_win.height = (0 == layer_info->screen_win.height) ?
        1 : layer_info->screen_win.height;

    //ALOGD("screen[%d,%d,%d,%d]", layer_info->screen_win.x, layer_info->screen_win.y,
    //    layer_info->screen_win.width, layer_info->screen_win.height);
    return 0;
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
    if (ret < 0) {
        ALOGE("#######ion_import  error#######");
        return 0;
    }
    custom_data.cmd = ION_IOC_SUNXI_PHYS_ADDR;
    phys_data.handle = data.handle;
    custom_data.arg = (unsigned long)&phys_data;
    ret = ioctl(Globctx->IonFd, ION_IOC_CUSTOM,&custom_data);
    if(ret < 0) {
        ALOGE("ION_IOC_CUSTOM(err=%d)",ret);
        return 0;
    }
    freedata.handle = data.handle;
    ret = ioctl(Globctx->IonFd, ION_IOC_FREE, &freedata);
    if(ret < 0) {
        ALOGE("ION_IOC_FREE(err=%d)",ret);
        return 0;
    }
    return phys_data.phys_addr;
}

int IonHandleAddRef(disp_layer_info_t *layer_info, hwc_layer_1_t *psLayer)
{
    SUNXI_hwcdev_context_t *Globctx = &gSunxiHwcDevice;
    private_handle_t* handle = (private_handle_t*)psLayer->handle;
    ion_list * item;
    struct ion_fd_data data ;

    if(handle == NULL) {
        return -1;
    }

    if(Globctx->ionListCurNum == Globctx->ionListTotalNum) {
        const int alloc_num = 4;
        int i;
        int err = 0;
        ion_list *ionCurValidNode = NULL;
        ion_list * head = Globctx->IonHandleHead;

        if(NULL != Globctx->ionCurValidNode) {
            ionCurValidNode = Globctx->ionCurValidNode->prev;
        }

        for(i = 0; i < alloc_num; ++i) {
            item = (ion_list *)malloc(sizeof(ion_list));
            if(NULL == item) {
                ALOGE("%s,line=%d: malloc failed", __func__, __LINE__);
                ++err;
                continue;
            }
            memset((void *)item, 0, sizeof(*item));
            item->fd = -1;
            if(NULL != head) {
                item->prev = head->prev;
                item->next = head;
                item->prev->next = item;
                head->prev = item;
            } else {
                item->prev = item;
                item->next = item;
                head = item;
                Globctx->IonHandleHead = item;
                Globctx->ionCurValidNode = item;
            }
            ++(Globctx->ionListTotalNum);
            ALOGD("total alloc num %d", Globctx->ionListTotalNum);
        }
        if(alloc_num == err) {
            ALOGE("%s,line=%d: malloc failed", __func__, __LINE__);
            return -1;
        }
        if(NULL != ionCurValidNode)
        Globctx->ionCurValidNode = ionCurValidNode->next;
    }

    if (ion_import(Globctx->IonFd, handle->share_fd, &(data.handle))) {
        ALOGE("%s: ion_import error, share_fd=%d",
            __func__, handle->share_fd);
        return -1;
    };
    if (ion_share(Globctx->IonFd, data.handle, &(data.fd))) {
        ALOGE("%s: ion_import error, share_fd=%d",
            __func__, handle->share_fd);
        ion_free(Globctx->IonFd, data.handle);
        return -1;
    }
    //ALOGD("IonHandleAddRef frame=%d, ion_handle=%x, new_fd=%d. old_fd=%d",
    //    Globctx->HWCFramecount, data.handle, data.fd, handle->share_fd);
    layer_info->fb.fd = data.fd;

    item = Globctx->ionCurValidNode;
    item->has_ref = 1;
    item->frame_index = Globctx->HWCFramecount;
    if (0 <= item->fd) {
        ALOGD("%s: close share_fd=%d", __func__, item->fd);
        close(item->fd);
    }
    item->fd = data.fd;
    item->handle = data.handle;
    ++(Globctx->ionListCurNum);
    Globctx->ionCurValidNode = Globctx->ionCurValidNode->next;

    return 0;
}

int IonHandleDecRef(char dec_all)
{
    SUNXI_hwcdev_context_t *Globctx = &gSunxiHwcDevice;
    ion_list *head = Globctx->IonHandleHead;
    const unsigned char delayFrameCount = 2;

    while ((NULL != head)
        && (0 != head->has_ref)
        && ((delayFrameCount + head->frame_index < Globctx->HWCFramecount)
        || (0 != dec_all))) {

            //ALOGD("#######IonHandleDecRef frame=%d ion_handle=%d, fd=%d",
            //    head->frame_index, head->handle, head->fd);
            if (0 <= head->fd) {
                close(head->fd);
                head->fd = -1;
            }
            if (ion_free(Globctx->IonFd, head->handle)) {
                ALOGE("%s:line=%d. ion_free err", __func__, __LINE__);
            }
            head->handle = 0;
            head->frame_index = 0;
            head->has_ref = 0;
            head = head->next;
            --(Globctx->ionListCurNum);
    }
    Globctx->IonHandleHead = head;

    return 0;
}

static int setup_sunxi_metadata(layer_info_t *psHwlayer_info,
	disp_layer_info_t *layer_info)
{
#if (GRALLOC_SUNXI_METADATA_BUF & (DE_VERSION == 30))
	private_handle_t *handle = (private_handle_t *)(psHwlayer_info->psLayer->handle);
	layer_info->fb.metadata_flag = handle->ion_metadata_flag;
	layer_info->fb.metadata_size = handle->ion_metadata_size;
	layer_info->fb.metadata_fd = handle->metadata_fd;
	if (is_afbc_buf(handle))
		layer_info->fb.fbd_en = 1;
	#if 0
	ALOGD("fbd_en=%d, metadata_flag=0x%x, metadata_size=%d, metadata__buf[0x%llx, %x]",
		layer_info->fb.fbd_en,
		layer_info->fb.metadata_flag,
		layer_info->fb.metadata_size,
		layer_info->fb.metadata_buf,
		get_ion_address(handle->metadata_fd));
	#endif
#else
	psHwlayer_info, layer_info;
#endif /* GRALLOC_SUNXI_METADATA_BUF */
	return 0;
}

static int setup_hwlayer_dataspace(
	disp_layer_info_t *layer_info, int32_t dataspace)
{
#if (DE_VERSION == 30)
	unsigned int transfer = (dataspace & HAL_DATASPACE_TRANSFER_MASK)
		 >> HAL_DATASPACE_TRANSFER_SHIFT;
	unsigned int standard = (dataspace & HAL_DATASPACE_STANDARD_MASK)
		 >> HAL_DATASPACE_STANDARD_SHIFT;
	unsigned int range = (HAL_DATASPACE_RANGE_FULL
		!= (dataspace & HAL_DATASPACE_RANGE_MASK)) ? 0 : 1; /* 0: limit. 1: full */

	/* color space table [standard][range] */
	const disp_color_space cs_table[][2] = {
		{DISP_UNDEF, DISP_UNDEF_F},
		{DISP_BT709, DISP_BT709_F},
		{DISP_BT470BG, DISP_BT470BG_F},
		{DISP_BT470BG, DISP_BT470BG_F},
		{DISP_BT601, DISP_BT601_F},
		{DISP_BT601, DISP_BT601_F},
		{DISP_BT2020NC, DISP_BT2020NC_F},
		{DISP_BT2020C, DISP_BT2020C_F},
		{DISP_FCC, DISP_FCC_F},
		{DISP_BT709, DISP_BT709_F}
	};
	if ((range < sizeof(cs_table[0]) / sizeof(cs_table[0][0]))
		&& (standard < sizeof(cs_table) / sizeof(cs_table[0]))) {
		layer_info->fb.color_space = cs_table[standard][range];
	} else {
		ALOGD("unknown dataspace standard(0x%x) range(0x%x)", standard, range);
		layer_info->fb.color_space = range ? DISP_UNDEF_F : DISP_UNDEF;
	}

	const disp_eotf eotf_table[] = {
		DISP_EOTF_UNDEF,
		DISP_EOTF_LINEAR,
		DISP_EOTF_IEC61966_2_1,
		DISP_EOTF_BT601,
		DISP_EOTF_GAMMA22,
		DISP_EOTF_GAMMA28,
		DISP_EOTF_SMPTE2084,
		DISP_EOTF_ARIB_STD_B67
	};
	if (transfer < sizeof(eotf_table) / sizeof(eotf_table[0])) {
		layer_info->fb.eotf = eotf_table[transfer];
	} else {
		ALOGD("unknown dataspace Transfer(0x%x)", transfer);
		layer_info->fb.eotf = DISP_EOTF_UNDEF;
	}
	//ALOGD("layer_info_fb: eotf=%d, cs=%d",
	//	layer_info->fb.eotf, layer_info->fb.color_space);
#else
	layer_info, dataspace;
#endif
	return 0;
}

bool match_format(const DisplayInfo *psDisplayInfo,
    layer_info_t *psHwlayer_info, disp_layer_info_t *layer_info)
{
    SUNXI_hwcdev_context_t *Globctx = &gSunxiHwcDevice;
    hwc_layer_1_t *layer = psHwlayer_info->psLayer;
    private_handle_t *handle = (private_handle_t *)(psHwlayer_info->psLayer->handle);
    __display_3d_mode trMode = psDisplayInfo->Current3DMode;
    bool isVideo = CheckVideoFormat(handle->format);
    bool err = 0;

    layer_info->fb.size[0].width = handle->width;
    layer_info->fb.size[0].height = handle->height;
    layer_info->fb.align[0] = HW_ALIGN;

    switch(handle->format) {
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
    case HAL_PIXEL_FORMAT_AW_YV12_10bit:
    case HAL_PIXEL_FORMAT_AW_I420_10bit:
        //ALOGD("match format(0x%x)", handle->format);
        layer_info->fb.format = DISP_FORMAT_YUV420_P;
        layer_info->fb.size[0].width = ALIGN(handle->width, YV12_ALIGN);
        layer_info->fb.size[1].width = ALIGN(layer_info->fb.size[0].width / 2,YV12_ALIGN);
        layer_info->fb.size[2].width = ALIGN(layer_info->fb.size[0].width / 2,YV12_ALIGN);
        layer_info->fb.size[1].height = handle->height/ 2;
        layer_info->fb.size[2].height = handle->height/ 2;
        layer_info->fb.align[0] = YV12_ALIGN;
        layer_info->fb.align[1] = YV12_ALIGN;
        layer_info->fb.align[2] = YV12_ALIGN;
        break;
    case HAL_PIXEL_FORMAT_YCrCb_420_SP:
    case HAL_PIXEL_FORMAT_AW_NV21_10bit:
        //ALOGD("match format(0x%x)", handle->format);
        layer_info->fb.format = DISP_FORMAT_YUV420_SP_VUVU;
        layer_info->fb.size[0].width = ALIGN(handle->width, YV12_ALIGN);
        layer_info->fb.size[1].width = layer_info->fb.size[0].width / 2;
        layer_info->fb.size[1].height = handle->height/ 2;
        layer_info->fb.align[0] = 0;
        layer_info->fb.align[1] = 0;
        break;
    case HAL_PIXEL_FORMAT_AW_NV12:
    case HAL_PIXEL_FORMAT_AW_NV12_10bit:
        layer_info->fb.format = DISP_FORMAT_YUV420_SP_UVUV;
        layer_info->fb.size[0].width = ALIGN(handle->width, YV12_ALIGN);
        layer_info->fb.size[1].width = layer_info->fb.size[0].width / 2;
        layer_info->fb.size[1].height = handle->height/ 2;
        layer_info->fb.align[0] = 0;
        layer_info->fb.align[1] = 0;
        break;
    case HAL_PIXEL_FORMAT_AW_P010_UV:
        layer_info->fb.format = DISP_FORMAT_YUV420_SP_UVUV_10BIT;
        layer_info->fb.align[0] = YV12_ALIGN;//fixme
        layer_info->fb.align[1] = YV12_ALIGN;//fixme
        layer_info->fb.size[1].width = handle->width / 2;
        layer_info->fb.size[1].height = handle->height / 2;
        break;
    case HAL_PIXEL_FORMAT_AW_P010_VU:
        layer_info->fb.format = DISP_FORMAT_YUV420_SP_VUVU_10BIT;
        layer_info->fb.align[0] = YV12_ALIGN;//fixme
        layer_info->fb.align[1] = YV12_ALIGN;//fixme
        layer_info->fb.size[1].width = handle->width / 2;
        layer_info->fb.size[1].height = handle->height / 2;
        break;
    default:
        ALOGE("Not support format 0x%x in %s", handle->format, __FUNCTION__);
        err = 1;
    }

	setup_sunxi_metadata(psHwlayer_info, layer_info);
	setup_hwlayer_dataspace(layer_info, layer->dataspace);

    if(psHwlayer_info->is3D) {
        int all_3d = 0;
        switch(trMode) {
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
        case DISPLAY_3D_LEFT_RIGHT_ALL:
            layer_info->b_trd_out = 1;
            layer_info->out_trd_mode = DISP_3D_OUT_MODE_FP;
            layer_info->fb.flags = DISP_BF_STEREO_SSH;
            all_3d = 1;
            break;
        case DISPLAY_3D_TOP_BOTTOM_ALL:
            layer_info->b_trd_out = 1;
            layer_info->out_trd_mode = DISP_3D_OUT_MODE_FP;
            layer_info->fb.flags = DISP_BF_STEREO_TB;
            all_3d = 1;
            break;
        case DISPLAY_2D_DUAL_STREAM:
        default :
            layer_info->b_trd_out = 0;
            layer_info->fb.flags = DISP_BF_NORMAL;
            return 0;
        }
        if(!isVideo && (0 == all_3d)) {
            layer_info->fb.flags = DISP_BF_STEREO_2D_DEPTH;
            layer_info->fb.depth = 3; // fixme
        }
    } else {
        layer_info->b_trd_out = 0;
        layer_info->fb.flags = DISP_BF_NORMAL;
        switch(trMode) {
        case DISPLAY_2D_LEFT:
        case DISPLAY_3D_LEFT_RIGHT_HDMI:
            if (!isVideo)
                break;
        case DISPLAY_3D_LEFT_RIGHT_ALL:
            layer_info->b_trd_out = 0;
            layer_info->fb.flags = DISP_BF_STEREO_SSH;
            break;
        case DISPLAY_2D_TOP:
        case DISPLAY_3D_TOP_BOTTOM_HDMI:
            if (!isVideo)
                break;
        case DISPLAY_3D_TOP_BOTTOM_ALL:
            layer_info->b_trd_out = 0;
            layer_info->fb.flags = DISP_BF_STEREO_TB;
            break;
        default :
            return 0;
        }
    }

returnerr:
    return err;
}

static bool check_dim_layer(hwc_layer_1_t *hwclayer) {
    bool is_dim_layer = 1;
    if (hwclayer->handle != 0 ||
            hwclayer->sourceCrop.bottom != -1 || hwclayer->sourceCrop.right != -1) {
        is_dim_layer = 0;
    }
    return is_dim_layer;
}

static bool setup_dim_layer(hwc_layer_1_t *psLayer, disp_layer_info_t *layer_info, const DisplayInfo *dispDevInfo)
{
    layer_info->mode = LAYER_MODE_COLOR;
    layer_info->fb.size[0].height = 720;
    layer_info->fb.size[0].width  = 1280;
    layer_info->fb.align[0] = 0;
    layer_info->fb.crop.x = 0;
    layer_info->fb.crop.y = 0;
    layer_info->fb.crop.width = ((long long)1280)<<32;
    layer_info->fb.crop.height = ((long long)720)<<32;
    layer_info->color = ((unsigned int)psLayer->planeAlpha)<<24;
    layer_info->screen_win.x = 0;
    layer_info->screen_win.y = 0;
    layer_info->screen_win.width  = dispDevInfo->VarDisplayWidth;
    layer_info->screen_win.height = dispDevInfo->VarDisplayHeight;
    return 1;
}

HwcAssignStatus
hwc_try_assign_layer(HwcDisContext_t *Localctx, size_t  singcout,int zOrder)
{
    SUNXI_hwcdev_context_t *Globctx = &gSunxiHwcDevice;
    bool needchannel = 1, isvideo = 0,isalpha = 0, isFB = 0, is_dim_layer = 0;
    float WscalFac = 1.0, HscaleFac = 1.0;
    int CH= -1, tmCnt1 = 0, tmCnt2 = 0, addLayerCnt = 1, CHdiff = 0,tmpCH = 0 ;
    int layer_fmt;
    AssignDUETO_T dueto = I_OVERLAY;
    unsigned char planealpha = 0xff;
    hwc_layer_1_t *psLayer;
    psLayer = Localctx->psAllLayer[singcout].psLayer;

    const DisplayInfo  *PsDisplayInfo = Localctx->psDisplayInfo;
    private_handle_t* handle = (private_handle_t*)psLayer->handle;
    ChannelInfo_t *psCH = Localctx->ChannelInfo;
    int bandwidth = 0;
    bandwidth_t *pBandwidth = const_cast<bandwidth_t *>(&(PsDisplayInfo->bandwidth));

    if(psLayer->compositionType == HWC_FRAMEBUFFER_TARGET) {
        layer_fmt = HAL_PIXEL_FORMAT_BGRA_8888;
        if(handle != NULL) {
            isFB = 1;
            isalpha = 1;
            goto needchannel;
        } else {
            if (Localctx->UsedFB) {
                isFB = 1;
                isalpha = 1;
                goto needchannel;
            } else {
                return ASSIGN_INIT;
            }
        }
    }
    if(HwcisBlended(psLayer) && singcout == 0) {
        psLayer->blending = HWC_BLENDING_NONE;
    }

    is_dim_layer = check_dim_layer(psLayer);
    if (is_dim_layer) {
        if (Check_X_List(psLayer, Localctx, 0, singcout-1, -1, ASSIGN_GPU)) {
            dueto = D_X_FB;
            goto assign_gpu;
        } else {
            isalpha = 1;
            planealpha = psLayer->planeAlpha;
            goto needchannel;
        }
    }

    if(handle == NULL) {
        ALOGV("%s:Buffer handle is NULL", __func__);
        dueto = D_NULL_BUF;
        goto assign_gpu;
    }

    CalculateLayerScaleFactor(Localctx, psLayer, &WscalFac, &HscaleFac);
	isvideo = CheckVideoFormat(handle->format);

    if((IOMEM_TYPE_IOMMU != Globctx->iomem_type)
        && !(handle->flags& private_handle_t::PRIV_FLAGS_USES_CONFIG)) {
        ALOGV("%s:not continuous Memory", __func__);
        dueto = D_CONTIG_MEM;
        goto assign_gpu;
    }

    if(HwcUsageProtected(handle) && !PsDisplayInfo->issecure) {
        ALOGV("%s:Video Protected", __func__);
        dueto = D_VIDEO_PD;
        goto assign_gpu;
    }


	#if GRALLOC_SUNXI_METADATA_BUF
	if (handle->usage & GRALLOC_USAGE_METADATA_BUF) {
		if (isvideo) {
			unsigned int metadata_buf_flag = get_sunxi_metadata_flag(handle);
			unsigned int old_flag = handle->ion_metadata_flag;
			handle->ion_metadata_flag = metadata_buf_flag;
		} else {
			handle->ion_metadata_flag = 0;
		}
	} else {
		handle->ion_metadata_flag = 0;
	}
	#endif // #if GRALLOC_SUNXI_METADATA_BUF

    if(isvideo && !HwcCanScale(Localctx, psLayer)) {
        ALOGV("Scale can not used");
        dueto = D_SCALE_OUT;
        goto assign_gpu;
    }


    if (is_skip_gpu_buf(handle)) {
        if (psLayer->flags & HWC_SKIP_LAYER)
        psLayer->flags &= ~HWC_SKIP_LAYER;
    }

    dueto = HwcisValidLayer(psLayer);
    if(dueto != I_OVERLAY) {
        ALOGV("HwcisValidLayer:0x%08x", handle->format);
        goto assign_gpu;
    }

    if (isValidUiFormat(handle->format)) {
        if (!hwc_ui_scaler_check(psLayer, PsDisplayInfo)) {
            ALOGV("de can not scale ui layer");
            dueto = D_CANNT_SCALE;
            goto assign_gpu;
        }
    }

    if (is_afbc_buf(handle)) {
        if (!hwc_ui_scaler_check(psLayer, PsDisplayInfo)) {
            //ALOGD("de cannot scale afbc layer");
            dueto = D_CANNT_SCALE;
            goto assign_gpu;
        }
        // do not assign afbc buffer to DE1
        if (PsDisplayInfo->VirtualToHWDisplay == 1) {
            dueto = D_NO_FORMAT;
            goto assign_gpu;
        }
    }

    if(HwcisBlended(psLayer)) {
        planealpha = psLayer->planeAlpha;
        isalpha = 1;//cann't distinguish only pixel alpha
        if(!HwcsupportAlpha(handle->format)) {
            ALOGV("not surpport alpha layer");
            dueto = D_ALPHA;
            goto assign_gpu;
        }
    }

    if(Localctx->UsedFB) {
        if(Check_X_List(psLayer, Localctx, 0, singcout-1, -1, ASSIGN_GPU)) {
            dueto = D_X_FB;
            goto assign_gpu;
        }
    }

    if(CheckVideoFormat(handle->format)) {
        static int sFormat = 0;
        static int sBlending = 0;
        static float sWidthScale = 0;
        static float sHeightScale = 0;
        if(0 == Localctx->HaveVideo) {
            sFormat = handle->format;
            sBlending = psLayer->blending;
            HwcisScaled(psLayer, &sWidthScale , &sHeightScale);
        } else {
            float llWidthScale, llHeightScale;
            HwcisScaled(psLayer, &llWidthScale, &llHeightScale);
            if ((sFormat != handle->format)
                || (sBlending != psLayer->blending)
                || !check_same_scale(sWidthScale, sHeightScale, llWidthScale, llHeightScale)) {
                //ALOGD("the second video layer assigned fail!");
                dueto = D_SCALE_OUT;
                goto assign_gpu;
            }
        }
    } else {
        bandwidth = cal_layer_mem(psLayer);
        if(bandwidth > pBandwidth->available) {
            dueto = D_MEM_LIMIT;
            goto assign_gpu;
        }
    }

needchannel:
    layer_fmt = (isFB || is_dim_layer) ? HAL_PIXEL_FORMAT_BGRA_8888 : handle->format;
    if ((DISP_OUTPUT_TYPE_HDMI == PsDisplayInfo->DisplayType)
        && (is3DMode(PsDisplayInfo->Current3DMode)
        || (isvideo && Check3DLayer(PsDisplayInfo)))) {
        //dont only the Video has 3D
        addLayerCnt = 2;
    }

    CH = Localctx->HwCHUsedCnt;
    tmpCH = Match_nofull_channel(Localctx, -1, isvideo, is_dim_layer,
                addLayerCnt, layer_fmt, WscalFac, HscaleFac, planealpha);

    while(tmpCH != -1 && CH !=-1 && CH >= tmpCH) {
        // can assigned to the lowest Channel?
        tmCnt1 =  Find_channel_layer(&Localctx->ChannelInfo[CH], 0);
        tmCnt2 =  Find_channel_layer(&Localctx->ChannelInfo[CH], 1);
        if((CH == tmpCH)
            || (Match_nofull_channel(Localctx, CH, isvideo, is_dim_layer, addLayerCnt,
                layer_fmt, WscalFac, HscaleFac, planealpha) != -1)) {
            if(Check_X_List(psLayer, Localctx, tmCnt1, tmCnt2, CH, ASSIGN_OVERLAY)) {
                if(!isalpha) {
                    CHdiff = Localctx->HwCHUsedCnt - CH;
                    needchannel = 0;
                }
                CH = -1;
            } else {
                CHdiff = Localctx->HwCHUsedCnt - CH;
                needchannel = 0;
                CH--;
            }
        } else if (Check_X_List(psLayer, Localctx, tmCnt1, tmCnt2, CH, ASSIGN_OVERLAY)) {
            CH = -1;
        } else {
            CH--;
        }
    }

    if(needchannel) {
        if((isvideo ? (Localctx->HaveVideo ?
                Localctx->VideoCHCnt < PsDisplayInfo->VideoCHNum -1 : 1) : 1)
            && Localctx->HwCHUsedCnt < (PsDisplayInfo->HwChannelNum
                - (isFB ? 0: Localctx->UsedFB) -1)) {
            Localctx->HwCHUsedCnt++;
            isvideo ? Localctx->VideoCHCnt++ :0;
        }  else {
            goto assigned_need_resigne;
        }
    }
    if(Localctx->UsedFB
        && !isalpha
        && !isFB
        && Check_X_List(psLayer, Localctx, 0, singcout-1, -1, ASSIGN_GPU )) {
        psLayer->hints |= HWC_HINT_CLEAR_FB;
    }
    if (is_dim_layer) {
        psLayer->flags &= (~HWC_SKIP_LAYER);
    }

assign_overlay:
    Localctx->HaveVideo += isvideo;
    psCH[Localctx->HwCHUsedCnt - CHdiff].hasVideo = isvideo;
    psCH[Localctx->HwCHUsedCnt - CHdiff].iCHFormat = layer_fmt;
    psCH[Localctx->HwCHUsedCnt - CHdiff].WTScaleFactor = WscalFac;
    psCH[Localctx->HwCHUsedCnt - CHdiff].HTScaleFactor = HscaleFac;

    psCH[Localctx->HwCHUsedCnt - CHdiff].planeAlpha = planealpha;
    psCH[Localctx->HwCHUsedCnt - CHdiff].HwLayer[psCH[Localctx->HwCHUsedCnt - CHdiff].HwLayerCnt] =
        &Localctx->psAllLayer[singcout];
    psCH[Localctx->HwCHUsedCnt - CHdiff].HwLayerCnt += addLayerCnt;
    psCH[Localctx->HwCHUsedCnt - CHdiff].isFB = isFB;
    psCH[Localctx->HwCHUsedCnt - CHdiff].hasBlend ?
        0 : psCH[Localctx->HwCHUsedCnt - CHdiff].hasBlend = isalpha;

    Localctx->psAllLayer[singcout].assigned = ASSIGN_OVERLAY;
    Localctx->psAllLayer[singcout].virchannel = Localctx->HwCHUsedCnt- CHdiff;
    Localctx->psAllLayer[singcout].HwzOrder = zOrder;
    Localctx->psAllLayer[singcout].OrigOrder = singcout;
    Localctx->psAllLayer[singcout].is3D = (addLayerCnt == 2 ? 1 : 0);
    Localctx->psAllLayer[singcout].info = dueto;
    Localctx->psAllLayer[singcout].is_dim_layer = is_dim_layer;
    Localctx->psAllLayer[singcout].bandwidth = (bandwidth ? bandwidth : cal_layer_mem(psLayer));

    if((!isvideo) && (!isFB)) {
        pBandwidth->available -= bandwidth;
        //ALOGD("ui overlay: bandwidth=%d, available=%d",
        //    bandwidth, pBandwidth->available);
    }
    if(psLayer->flags & HWC_IS_CURSOR_LAYER) {
        //ALOGD("need assign cursor layer");
        return ASSIGN_CURSOR_OVERLAY;
    }
    return ASSIGN_OVERLAY;

assign_gpu:

    Localctx->FBhasVideo += (handle != NULL ? CheckVideoFormat(handle->format):0);
    Localctx->psAllLayer[singcout].assigned = ASSIGN_GPU;
    Localctx->psAllLayer[singcout].virchannel = -1;
    Localctx->psAllLayer[singcout].HwzOrder = -1;
    Localctx->psAllLayer[singcout].OrigOrder = singcout;
    Localctx->UsedFB = ASSIGN_GPU;
    Localctx->psAllLayer[singcout].info = dueto;
    Localctx->psAllLayer[singcout].bandwidth = 0;

    if(isvideo) {
        pBandwidth->video -= (bandwidth ? bandwidth : cal_layer_mem(psLayer));
        pBandwidth->available += bandwidth;
        //ALOGD("video to fb: bandwidth=%d, video=%d, available=%d",
        //   bandwidth, pBandwidth->video, pBandwidth->available);
    }
    if(!pBandwidth->fb) {
       pBandwidth->fb = PsDisplayInfo->InitDisplayWidth
       * PsDisplayInfo->InitDisplayHeight * 4;
       if(pBandwidth->available < pBandwidth->fb) {
           //ALOGD("assigned_need_resigne: need mem for fb");
           Localctx->UsedFB = ASSIGN_GPU;
           return ASSIGN_NEEDREASSIGNED;
       } else {
           pBandwidth->available -= pBandwidth->fb;
           //ALOGD("pBandwidth->fb=%d, available=%d",
           //    pBandwidth->fb, pBandwidth->available);
       }
    }
    return ASSIGN_GPU;

assigned_need_resigne:
    if((!Localctx->UsedFB) || isFB) {
        Localctx->UsedFB = ASSIGN_GPU ;
        if(!pBandwidth->fb) {
            pBandwidth->fb = PsDisplayInfo->InitDisplayWidth
                * PsDisplayInfo->InitDisplayHeight * 4;
        }
        return ASSIGN_NEEDREASSIGNED;
    } else {
        dueto = D_NO_PIPE;
        goto assign_gpu;
    }
}

static void blank_writeback_display(void)
{
#ifdef HWC_WRITEBACK_ENABLE
    /*
     * When switching output mode (sdr -> hdr or hdr -> sdr) width
     * writeback enabled, The writeback output is flashing. Blank
     * the external display before the switching operation
     */
    if (hwc_writeback_is_enable()) {
        blank_disp(1);
    }
#endif
}

int tv_auto_output(DisplayInfo *PsDisplayInfo)
{
	if ((NULL == PsDisplayInfo)
		|| (HW_CHANNEL_PRIMARY != PsDisplayInfo->VirtualToHWDisplay))
		return 0;

	if (DISPLAY_OUTPUT_DATASPACE_MODE_AUTO == PsDisplayInfo->dataspace_mode) {
		//ALOGD("hw_input=%d, actual_dataspce_mode=%d",
		//	PsDisplayInfo->hw_input, PsDisplayInfo->actual_dataspce_mode);

		if ((PsDisplayInfo->hw_input == HW_INPUT_HDR)
			&& (PsDisplayInfo->actual_dataspce_mode != DISPLAY_OUTPUT_DATASPACE_MODE_HDR)) {
			if (TV_CAPABILITY_HDR & PsDisplayInfo->tv_capabilities) {
				ALOGD("tv auto output: sdr --> hdr");
				disp_device_config cfg;
				getDispDeviceConfigs(PsDisplayInfo->VirtualToHWDisplay, &cfg);
				cfg.eotf = DISP_EOTF_SMPTE2084;
				if (TV_CAPABILITY_WCG & PsDisplayInfo->tv_capabilities)
					cfg.cs = DISP_BT2020NC;
				if (DISPLAY_OUTPUT_FORMAT_AUTO == PsDisplayInfo->setOutputformat) {
					if (is_yuv420_sampling_mode(PsDisplayInfo->DisplayMode)) {
						sunxi_hdmi_capability_t cap;
						if (getHdmiCapability(&cap)) {
							ALOGE("getHdmiCapability for tv_auto_output error");
							cap.pixel_encoding = 0;
						}
						cfg.format = DISP_CSC_TYPE_YUV420;
						if (cap.pixel_encoding & PIXEL_ENCODING_YUV420_10BIT)
							cfg.bits = DISP_DATA_10BITS;
						else
							cfg.bits = DISP_DATA_8BITS;
					} else {
						cfg.format = DISP_CSC_TYPE_YUV422;
						cfg.bits = DISP_DATA_10BITS;
					}
				}
				blank_writeback_display();
				int ret = setDispDeviceConfigs(PsDisplayInfo->VirtualToHWDisplay, &cfg);
				if (!ret) {

					char const *fileName = ENHANCE_MODE_ATTR;
					char values[4];
					sprintf(values, "%d", 0);
					writeStringToAttrFile(fileName, values, strlen(values));

					PsDisplayInfo->actual_dataspce_mode = DISPLAY_OUTPUT_DATASPACE_MODE_HDR;
					PsDisplayInfo->output_format = cfg.format;
					PsDisplayInfo->output_bits = cfg.bits;
				} else {
					ALOGD("%s,%d, ret=%d", __FUNCTION__, __LINE__, ret);
					return -1;
				}
			} else {
				ALOGV("%s,%d", __FUNCTION__, __LINE__);
				return -1;
			}
		} else if ((PsDisplayInfo->hw_input == HW_INPUT_WCG)
			&& (PsDisplayInfo->actual_dataspce_mode != DISPLAY_OUTPUT_DATASPACE_MODE_WCG)) {
			if (TV_CAPABILITY_WCG & PsDisplayInfo->tv_capabilities) {
				ALOGD("tv auto output: sdr --> wcg");
				disp_device_config cfg;
				getDispDeviceConfigs(PsDisplayInfo->VirtualToHWDisplay, &cfg);
				cfg.cs = DISP_BT2020NC;
				if (DISPLAY_OUTPUT_FORMAT_AUTO == PsDisplayInfo->setOutputformat) {
					if (is_yuv420_sampling_mode(PsDisplayInfo->DisplayMode)) {
						sunxi_hdmi_capability_t cap;
						if (getHdmiCapability(&cap)) {
							ALOGE("getHdmiCapability for tv_auto_output error");
							cap.pixel_encoding = 0;
						}
						cfg.format = DISP_CSC_TYPE_YUV420;
						if (cap.pixel_encoding & PIXEL_ENCODING_YUV420_10BIT)
							cfg.bits = DISP_DATA_10BITS;
						else
							cfg.bits = DISP_DATA_8BITS;
					} else {
						cfg.format = DISP_CSC_TYPE_YUV422;
						cfg.bits = DISP_DATA_10BITS;
					}
				}
				blank_writeback_display();
				int ret = setDispDeviceConfigs(PsDisplayInfo->VirtualToHWDisplay, &cfg);
				if (!ret) {

					char const *fileName = ENHANCE_MODE_ATTR;
					char values[4];
					int enhance_mode = getValueFromProperty(ENHANCE_MODE_PROPERTY) % 3;
					sprintf(values, "%d", enhance_mode);
					writeStringToAttrFile(fileName, values, strlen(values));

					PsDisplayInfo->actual_dataspce_mode = DISPLAY_OUTPUT_DATASPACE_MODE_WCG;
					PsDisplayInfo->output_format = cfg.format;
					PsDisplayInfo->output_bits = cfg.bits;
				} else {
					ALOGD("%s,%d, ret=%d", __FUNCTION__, __LINE__, ret);
					return -1;
				}
			} else {
				ALOGV("%s,%d", __FUNCTION__, __LINE__);
				return -1;
			}

		} else if ((PsDisplayInfo->hw_input == HW_INPUT_SDR)
			&& (PsDisplayInfo->actual_dataspce_mode != DISPLAY_OUTPUT_DATASPACE_MODE_SDR)) {
			ALOGD("tv auto output: hdr/wcg --> sdr");
			disp_device_config cfg;
			getDispDeviceConfigs(PsDisplayInfo->VirtualToHWDisplay, &cfg);
			cfg.eotf = DISP_EOTF_GAMMA22;
			cfg.cs = PsDisplayInfo->VarDisplayHeight >= 720 ? DISP_BT709 : DISP_BT601;
			if (DISPLAY_OUTPUT_FORMAT_AUTO == PsDisplayInfo->setOutputformat) {
				if (is_yuv420_sampling_mode(PsDisplayInfo->DisplayMode)) {
					cfg.format = DISP_CSC_TYPE_YUV420;
					cfg.bits = DISP_DATA_8BITS;
				} else {
					cfg.format = DISP_CSC_TYPE_YUV444;
					cfg.bits = DISP_DATA_8BITS;
				}
			}
			blank_writeback_display();
			int ret = setDispDeviceConfigs(PsDisplayInfo->VirtualToHWDisplay, &cfg);
			if (!ret) {

				char const *fileName = ENHANCE_MODE_ATTR;
				char values[4];
				int enhance_mode = getValueFromProperty(ENHANCE_MODE_PROPERTY) % 3;
				sprintf(values, "%d", enhance_mode);
				writeStringToAttrFile(fileName, values, strlen(values));

				PsDisplayInfo->actual_dataspce_mode = DISPLAY_OUTPUT_DATASPACE_MODE_SDR;
				PsDisplayInfo->output_format = cfg.format;
				PsDisplayInfo->output_bits = cfg.bits;
			} else {
				ALOGV("setDispDeviceConfigs for sdr mode fail, ret=%d", ret);
				return -1;
			}
		}
	}

	return 0;
}

int hwc_setup_layer(setup_dispc_data_t* DisplayData, HwcDisContext_t *Localctx)
{
    int CHCnt = 0, LCnt = 0, zOrder = 0 , VideoCnt=0, UiCnt = 0, nubmerlyer = 0;
    unsigned int videoCropHeight = 0;
    disp_layer_info_t *layer_info;
    const DisplayInfo *PsDisplayInfo = Localctx->psDisplayInfo;
    hwc_layer_1_t *psLayer;
    layer_info_t *psHwlayer_info;
    disp_layer_t *psDisconfig;
    int *pfencefd;
    ChannelInfo_t  *psChannelInfo = Localctx->ChannelInfo;
#ifndef HWC_WRITEBACK_ENABLE
    bool enableLayers = !(PsDisplayInfo->setblank);
#else
    // enable layer for writeback even though it's plugout!
    bool enableLayers = (!(PsDisplayInfo->setblank))||(PsDisplayInfo->opened_for_wb);
#endif

    Localctx->HaveVideo ? UiCnt = Localctx->VideoCHCnt+1 :UiCnt = 0;

    if(Localctx->HaveVideo) {
        DisplayData->ehancemode = PsDisplayInfo->ehancemode;
    } else {
        DisplayData->ehancemode = 0;
        DisplayData->ehancemode = PsDisplayInfo->ehancemode;
    }

    while (CHCnt < (PsDisplayInfo->VirtualToHWDisplay ? 2 : 4)) {
        LCnt = 0;
        while (LCnt < PsDisplayInfo->LayerNumofCH) {
            psHwlayer_info = psChannelInfo[CHCnt].HwLayer[LCnt];
            psDisconfig = &DisplayData->hwclayer[nubmerlyer];
            pfencefd = &DisplayData->fencefd[nubmerlyer];

            if (psHwlayer_info != NULL) {
                psLayer = psHwlayer_info->psLayer;
            } else {
                memset(psDisconfig, 0, sizeof(*psDisconfig));
                goto nulllayer;
            }
            if (psLayer == NULL) {
                memset(psDisconfig, 0, sizeof(*psDisconfig));
                goto nulllayer;
            }
            layer_info = &psDisconfig->info;
            layer_info->zorder = nubmerlyer;
            if (psHwlayer_info->is_dim_layer) {
                setup_dim_layer(psLayer, layer_info, PsDisplayInfo);
                layer_info->fb.pre_multiply = 0;
                layer_info->alpha_mode      = 0;
            } else {
                if (NULL == psLayer->handle) {
                    memset(psDisconfig, 0, sizeof(*psDisconfig));
                    goto nulllayer;
                }
                if (match_format(PsDisplayInfo, psHwlayer_info, layer_info) ||
                        resize_layer(PsDisplayInfo, layer_info, psLayer)) {
                    memset(psDisconfig, 0, sizeof(*psDisconfig));
                    goto nulllayer;
                }

                if (psLayer->blending == HWC_BLENDING_PREMULT) {
                    layer_info->alpha_mode = 0;
                    layer_info->fb.pre_multiply = 1;
                } else if (psLayer->blending == HWC_BLENDING_COVERAGE) {
                    layer_info->alpha_mode = 0;
                } else {
                    layer_info->alpha_mode = 1;
                    layer_info->alpha_value = 0xFF;
                }
                if ((DISPLAY_3D_TOP_BOTTOM_HDMI == PsDisplayInfo->Current3DMode)
                    && CheckVideoFormat(((private_handle_t *)psLayer->handle)->format)) {
                    if (0 < PsDisplayInfo->videoCropHeightFor3D) {
                        nubmerlyer++;
                        DisplayData->layer_num++;
                        videoCropHeight = PsDisplayInfo->videoCropHeightFor3D;
                        //ALOGD("videoCropHeightFor3D=%d", videoCropHeight);
                    }
                }
                if (0 > IonHandleAddRef(layer_info, psHwlayer_info->psLayer)) {
                    memset(psDisconfig, 0, sizeof(*psDisconfig));
                    goto nulllayer;
                }
                IonHandleDecRef(0);
            }

            psDisconfig->enable = enableLayers;
            *pfencefd = psLayer->acquireFenceFd;
            nubmerlyer++;
            DisplayData->layer_num++;
nulllayer:
            psDisconfig->layer_id = LCnt;
            psDisconfig->channel = psChannelInfo[CHCnt].hasVideo ? VideoCnt : UiCnt;
            psHwlayer_info != NULL ? psHwlayer_info->hwchannel = psDisconfig->channel : 0;
            LCnt++;
            zOrder++;
        }
        psChannelInfo[CHCnt].hasVideo ? VideoCnt++ : UiCnt++;
        CHCnt++;
    }

    if (0 < videoCropHeight) {
        //ALOGD("############## Todo for dual 3D");
        disp_layer_t *left_buf = &DisplayData->hwclayer[0];
        disp_layer_t *right_buf = &DisplayData->hwclayer[1];
        DisplayData->fencefd[1] = -1;
        left_buf->info.b_trd_out = 0;
        left_buf->info.out_trd_mode = (enum disp_3d_out_mode)0;
        left_buf->info.fb.flags = DISP_BF_NORMAL;
        left_buf->info.fb.crop.height = (long long)videoCropHeight << 31;
        memcpy((void *)right_buf, (void *)left_buf, sizeof(disp_layer_t));
        right_buf->info.fb.crop.y = (long long)(right_buf->info.fb.size[0].height) << 31;
        right_buf->info.screen_win.y += PsDisplayInfo->VarDisplayHeight;
        right_buf->info.zorder++;
        right_buf->layer_id++;
    }
    tv_auto_output(const_cast<DisplayInfo *>(PsDisplayInfo));

    return 1;
}

static bool check_hw_restricted(
	DisplayInfo *PsDisplayInfo, hwc_layer_1_t *psLayer)
{

	if ((NULL == psLayer) || (NULL == PsDisplayInfo)
		|| (HW_CHANNEL_PRIMARY != PsDisplayInfo->VirtualToHWDisplay))
		return 0;

	int hwChannelNum = PsDisplayInfo->HwChannelNum;
	int dataspace_mode = 0;

	if (DISPLAY_OUTPUT_DATASPACE_MODE_AUTO == PsDisplayInfo->dataspace_mode) {
		int transfer = psLayer->dataspace & HAL_DATASPACE_TRANSFER_MASK;
		int standard = psLayer->dataspace & HAL_DATASPACE_STANDARD_MASK;
		if ((HAL_DATASPACE_TRANSFER_ST2084 == transfer)
			|| (HAL_DATASPACE_TRANSFER_HLG == transfer))
			PsDisplayInfo->hw_input = HW_INPUT_HDR;
		else if (HAL_DATASPACE_STANDARD_BT2020 == standard)
			PsDisplayInfo->hw_input = HW_INPUT_WCG;
		if ((HW_INPUT_HDR == PsDisplayInfo->hw_input)
			&& (PsDisplayInfo->tv_capabilities & TV_CAPABILITY_HDR))
			dataspace_mode = DISPLAY_OUTPUT_DATASPACE_MODE_HDR;
		else if ((HW_INPUT_WCG == PsDisplayInfo->hw_input)
			&& (PsDisplayInfo->tv_capabilities & TV_CAPABILITY_WCG))
			dataspace_mode = DISPLAY_OUTPUT_DATASPACE_MODE_WCG;
		else
			dataspace_mode = DISPLAY_OUTPUT_DATASPACE_MODE_SDR;
	} else {
		dataspace_mode = PsDisplayInfo->dataspace_mode;
	}

	if (DISPLAY_OUTPUT_DATASPACE_MODE_SDR != dataspace_mode) {
		PsDisplayInfo->HwChannelNum = 2;
	}
	//ALOGD("dataspace_mode=%d %d", PsDisplayInfo->dataspace_mode, dataspace_mode);

	return hwChannelNum != PsDisplayInfo->HwChannelNum;
}

bool sunxi_prepare(hwc_display_contents_1_t **displays ,size_t NumofDisp)
{
    bool  forceSoftwareRendering = 0;
    hwc_display_contents_1_t *psDisplay;
    size_t disp, i;
    HwcAssignStatus AssignStatus;
    int NeedReAssignedLayer = 0;
    hwc_layer_1_t *psLayer;
    int SetOrder = 0;
    SUNXI_hwcdev_context_t *Globctx = &gSunxiHwcDevice;
    setup_dispc_data_t* DisplayData;
    HwcDisContext_t *Localctx = NULL;
    DisplayInfo   *PsDisplayInfo;
    int err = 0;
    bool needsamedisp;
    unsigned int allreadyCopyAddr = 0;
    bandwidth_t bandwidthRecord;
    bool is_dual_display = displays[1] ? 1 : 0;

    resetGlobDevice(Globctx);
#ifdef HWC_WRITEBACK_ENABLE
    hwc_display_contents_1_t* contents[2];
    size_t dispcnt = NumofDisp;
    hwc_writeback_prepare(Globctx, displays, NumofDisp, &contents[0], &dispcnt);
    NumofDisp = dispcnt;
    displays = &contents[0];
#endif
    for(disp = 0; disp < NumofDisp; disp++)
    {
        NeedReAssignedLayer = 0;
        SetOrder = 0;
        psDisplay = displays[disp];

        PsDisplayInfo = &Globctx->SunxiDisplay[disp>=2 ? 0 : disp];
        if( !psDisplay || psDisplay->numHwLayers <= 0 || disp > 2
            || PsDisplayInfo->VirtualToHWDisplay == EVALID_HWDISP_ID) {
            //ALOGD("psDisplay=%p", psDisplay);
            //ALOGD_IF(psDisplay, "numHwLayers=%d", psDisplay->numHwLayers);
            continue;
        }
        if(F_HWDISP_ID == PsDisplayInfo->VirtualToHWDisplay) {
            for(i = 0; i < psDisplay->numHwLayers; i++) {
                if(psDisplay->hwLayers[i].compositionType != HWC_FRAMEBUFFER_TARGET) {
                    psDisplay->hwLayers[i].compositionType = HWC_OVERLAY;
                }
            }
            continue;
        }

        if(psDisplay->outbuf != NULL || psDisplay->outbufAcquireFenceFd != 0) {
            if (psDisplay->retireFenceFd >= 0) {
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
        layer_info *AllLayers = Localctx->psAllLayer;
        unsigned int alloc_num_layer = Localctx->allocNumLayer;
        if(psDisplay->numHwLayers > alloc_num_layer) {
            if(NULL != AllLayers) {
                free(AllLayers);
            }
            AllLayers = (layer_info*)calloc(psDisplay->numHwLayers, sizeof(layer_info));
            if(NULL != AllLayers) {
                alloc_num_layer = psDisplay->numHwLayers;
                ALOGD("realloc %d for AllLayers", alloc_num_layer);
            } else {
                ALOGE("calloc for AllLayers failed. num=%d, sizeof=%d\n",
                psDisplay->numHwLayers, sizeof(layer_info));
                Localctx->psAllLayer = NULL;
                Localctx->allocNumLayer = 0;
                return -1;
            }
        }
        memset((void *)AllLayers, 0,
        psDisplay->numHwLayers * sizeof(layer_info));
        memset((void *)Localctx, 0, sizeof(HwcDisContext_t));
        Localctx->psAllLayer = AllLayers;
        Localctx->allocNumLayer = alloc_num_layer;

#ifdef HWC_WRITEBACK_ENABLE
        if (hwc_writeback_get_display_id() == disp)
            continue;
#endif

        PsDisplayInfo->bandwidth.max = getMemLimit();
        PsDisplayInfo->bandwidth.fb = 0;
        PsDisplayInfo->bandwidth.video = 0;
        for(i = 0; i < psDisplay->numHwLayers; i++) {
            psLayer = &psDisplay->hwLayers[i];
            if((0 == NeedReAssignedLayer) && (NULL != psLayer->handle)
                && CheckVideoFormat(((private_handle_t *)psLayer->handle)->format)) {
                PsDisplayInfo->bandwidth.video += cal_layer_mem(psLayer);
            }
        }
        if (is_dual_display) {
            // fixme
            if (1 == PsDisplayInfo->VirtualToHWDisplay) {
                int memLimit = PsDisplayInfo->bandwidth.video + 1658880;// 1658880 = 720 * 576 * 4;
                if (memLimit < PsDisplayInfo->bandwidth.max) {
                    //ALOGD("limit cvbs bandwidth for dual display.");
                    PsDisplayInfo->bandwidth.max = memLimit;
                }
            }
        }

		PsDisplayInfo->HwChannelNum =
			PsDisplayInfo->VirtualToHWDisplay ? 2 : NUMCHANNELOFDSP;

ReAssignedLayer:
        resetLocalInfo(PsDisplayInfo, Localctx, psDisplay, AllLayers);

        for(i = (forceSoftwareRendering ? psDisplay->numHwLayers-1 : 0);
            i < psDisplay->numHwLayers; i++) {
            psLayer = &psDisplay->hwLayers[i];
            if(i >= psDisplay->numHwLayers-1) {
                if( psDisplay->numHwLayers == 1|| Localctx->UsedFB
                    || psLayer->handle == NULL ||forceSoftwareRendering) {
                    if((Globctx->ForceGPUComp && !Localctx->HaveVideo)
                        || forceSoftwareRendering) {
                        ALOGD("Force GPU Composer");
                        SetOrder = 0;
                        resetLocalInfo(PsDisplayInfo, Localctx, psDisplay, AllLayers);
                    }
                    if(Globctx->ForceGPUComp
                        && Localctx->HaveVideo
                        && !Localctx->UsedFB
                        && psDisplay->numHwLayers != 1) {
                        break;
                    }
                } else {
                    break;
                }
            }
            AssignStatus = hwc_try_assign_layer(Localctx, i, SetOrder);
            switch (AssignStatus) {
            case ASSIGN_OVERLAY:
                if(psLayer->compositionType == HWC_FRAMEBUFFER) {
                    psLayer->compositionType = HWC_OVERLAY;
					if (check_hw_restricted(PsDisplayInfo, psLayer)) {
						goto ReAssignedLayer;
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
                if(NeedReAssignedLayer == 0) {
                    NeedReAssignedLayer++;
                    goto ReAssignedLayer;
                } else {
                    forceSoftwareRendering = 1;
                    goto ReAssignedLayer;
                }
                break;
            case ASSIGN_CURSOR_OVERLAY:
                if(psLayer->compositionType == HWC_FRAMEBUFFER) {
                    psLayer->compositionType = HWC_CURSOR_OVERLAY;
                }
                SetOrder++;
                break;
            default:
                //ALOGE("No choice in assign layers");
                break;
            }
        }

        int deviceid = PsDisplayInfo->VirtualToHWDisplay;
        DisplayData = &Globctx->PrivateData[deviceid];
        DisplayData->deviceid  = deviceid;

    }

    return 0 ;
}

static int commit_layers(setup_dispc_data_t *displayData,
	int *releasefencefd, int *retirefencefd)
{
	SUNXI_hwcdev_context_t *Globctx = &gSunxiHwcDevice;
	int i, ret;
	unsigned long arg[4];

	if (0 == displayData->layer_num)
		return -1;

	// wait for acquire fence.
	ret = 0;
	for (i = 0; i < displayData->layer_num; ++i) {
		if (0 <= displayData->fencefd[i]) {
			int timeout = 1000; /* 1000 ms */
			timeout = sync_wait(displayData->fencefd[i], timeout);
			if (0 > timeout) {
				ALOGE("fence(%d) timeout", displayData->fencefd[i]);
				ret = -1;
			}
		}
	}
	if (0 != ret) {
		*releasefencefd = -1;
		*retirefencefd = -1;
		blank_disp(displayData->deviceid);
		return ret;
	}

	/* commit layer to de, begin */
	arg[0] = displayData->deviceid;

	// make disabled layers
	unsigned int disabled_layer_mask = ~0x0;
	for (i = 0; i < displayData->layer_num; ++i) {
		disp_layer_t *cfg = &(displayData->hwclayer[i]);
		unsigned int id = cfg->channel * 4 + cfg->layer_id;
		disabled_layer_mask &= (~(0x1 << id));
	}
	int chn = displayData->deviceid ? 2 : 4;
	disp_layer_t *disabled_cfg =
		&(displayData->hwclayer[displayData->layer_num]);
	for (i = 0; i < chn; ++i) {
		unsigned j;
		for (j = 0; j < 4; ++j) {
			unsigned int id = i * 4 + j;
			if (disabled_layer_mask & (0x1 << id)) {
				disabled_cfg->channel = i;
				disabled_cfg->layer_id = j;
				disabled_cfg++;
			}
		}
	}

	arg[1] = 1; // shadow protect true
	ioctl(Globctx->DisplayFd, DISP_SHADOW_PROTECT, (unsigned long)arg);

	arg[1] = (unsigned long)(displayData->hwclayer);
	arg[2] = (unsigned long)(displayData->deviceid ? 8 : 16);
	ret = ioctl(Globctx->DisplayFd, COMMIT_LAYER_CMD, (unsigned long)arg);
	if (!ret) {
		int fencefd = -1;

		arg[1] = HWC_COMMIT_FRAME_NUM_INC;
		arg[2] = HWC_SYNC_TIMELINE_DEFAULT;
		arg[3] = 1;
		fencefd = ioctl(Globctx->DisplayFd, DISP_HWC_COMMIT, (unsigned long)arg);
		if (0 <= fencefd) {
			*releasefencefd = fencefd;
		} else {
			*releasefencefd = -1;
		}
	} else {
		ALOGE("DISP_LAYER_SET_CONFIG failed !");
	}

	arg[1] = 0; // shadow protect false
	ioctl(Globctx->DisplayFd, DISP_SHADOW_PROTECT, (unsigned long)arg);
	/* commit layer to de, end */

	return ret;
}

static int skip_blackscreen_before_bootanimation(hwc_display_contents_1_t *psDisplay)
{
    static int need_skip = 1;

    if (!need_skip)
        return 0;
    else if (need_skip && psDisplay && psDisplay->numHwLayers > 1) {
        need_skip = 0;
        return 0;
    }

    if (!psDisplay || psDisplay->numHwLayers <= 1) {
        if (psDisplay && psDisplay->numHwLayers == 1) {
            ALOGD("skip layer and close acquire fencefd %d", psDisplay->hwLayers[0].acquireFenceFd);
            close(psDisplay->hwLayers[0].acquireFenceFd);
            psDisplay->hwLayers[0].acquireFenceFd = -1;
        }
    }
    return need_skip;
}

bool sunxi_set(hwc_display_contents_1_t** displays,size_t numDisplays,
    int *releasefencefd, int *retirefencefd)
{
    SUNXI_hwcdev_context_t *Globctx = &gSunxiHwcDevice;
    setup_dispc_data_t* DisplayData;
    HwcDisContext_t *Localctx = NULL;
    DisplayInfo   *PsDisplayInfo;
    int HwlayerCnt;
    int FdCnt = 0,ret = -1 ;
    size_t disp;
    hwc_display_contents_1_t* psDisplay;
    hwc_layer_1_t *psLayer;
    unsigned long arg[4] = {0};
    int returnfenceFd[2] = {-1,-1};
    int fencefd_cnt = 0;

    if (2 < numDisplays)
        numDisplays = 2;

#ifdef HWC_WRITEBACK_ENABLE
    hwc_display_contents_1_t *contents[2];
    if (hwc_writeback_is_enable()
        && hwc_writeback_get_display_id() == HW_CHANNEL_PRIMARY) {
        numDisplays = 2;
        contents[0] = displays[0];
        contents[1] = displays[0];
    } else {
        contents[0] = displays[0];
        contents[1] = displays[1];
    }
    displays = &contents[0];
#endif

    for(disp = 0; disp < numDisplays; disp++) {
        int hwDisp;

        Localctx = &Globctx->DisContext[disp];
        PsDisplayInfo = &Globctx->SunxiDisplay[disp];
        hwDisp = PsDisplayInfo->VirtualToHWDisplay;


        *releasefencefd = -1;
        *retirefencefd = -1;
        psDisplay = displays[disp];

        if (!psDisplay || skip_blackscreen_before_bootanimation(psDisplay))
            continue;

        if (Globctx->screenOff) {
            /* If primary display has been blank, Do not commit
             * any layer to driver, Or it will lead to a dma_map crash error
             */
            goto __update_fence;
        }
        if (hwDisp != EVALID_HWDISP_ID && hwDisp != F_HWDISP_ID) {
#ifdef HWC_WRITEBACK_ENABLE
            if (hwc_writeback_is_enable() && hwc_writeback_get_display_id() == disp) {
                DisplayData = &Globctx->PrivateData[hwDisp];
                if (hwc_writeback_buffer_setup(Globctx, DisplayData) == 0) {
                    DisplayData->androidfrmnum = Globctx->HWCFramecount;
                    commit_layers(DisplayData, releasefencefd, retirefencefd);
                    if (*releasefencefd >= 0) {
                        close(*releasefencefd);
                        *releasefencefd = -1;
                    }
                }
                hwc_writeback_buffer_clear(Globctx);
                goto __update_fence;
            }
#endif
            DisplayData = &Globctx->PrivateData[hwDisp];
            hwc_setup_layer(DisplayData, Localctx);
            DisplayData->androidfrmnum = Globctx->HWCFramecount;
            commit_layers(DisplayData, releasefencefd, retirefencefd);
            dump_displays(Localctx);
        } else {
            ALOGD("not sunxi set: disp=%d, hwDisp=%d", disp, hwDisp);
        }

__update_fence:
        unsigned int i;

        for (i = 0; i < psDisplay->numHwLayers; ++i) {
            if (psDisplay->hwLayers[i].acquireFenceFd >= 0) {
                close(psDisplay->hwLayers[i].acquireFenceFd);
                psDisplay->hwLayers[i].acquireFenceFd = -1;
            }
            if (psDisplay->hwLayers[i].releaseFenceFd >= 0) {
                close(psDisplay->hwLayers[i].releaseFenceFd);
                psDisplay->hwLayers[i].releaseFenceFd = -1;
            }
            if (psDisplay->hwLayers[i].compositionType == HWC_OVERLAY) {
                if (*releasefencefd >= 0 && hwDisp < NUMBEROFDISPLAY) {
                    psDisplay->hwLayers[i].releaseFenceFd = dup(*releasefencefd);
                }
            } else if (psDisplay->hwLayers[i].compositionType == HWC_FRAMEBUFFER_TARGET) {
                //ALOGD("disp(%d): usedFb=%d", disp, Localctx->UsedFB);
                if (Localctx->UsedFB) {
                    psDisplay->hwLayers[i].releaseFenceFd = dup(*releasefencefd);
                }
            }
        }
        if(*releasefencefd >= 0) {
            close(*releasefencefd);
            *releasefencefd = -1;
        }
        if (psDisplay->retireFenceFd >= 0) {
            ALOGD("close disp(%d) retireFenceFd(%d)",
            disp, psDisplay->retireFenceFd);
            close(psDisplay->retireFenceFd);
            psDisplay->retireFenceFd = -1;
        }

    }

    Globctx->HWCFramecount++;
    if(0 != Globctx->exchangeDispChannel) {
        if(0 == pthread_mutex_trylock(&Globctx->lock)) {
            ALOGD("exchanging ...");
            // fixme: we should exchange the pointers of SunxiDisplay[0/1],
            // but not the SunxiDisplays
            DisplayInfo psDisplayInfo;
            memcpy((void *)&psDisplayInfo, (void *)(&Globctx->SunxiDisplay[0]),
                sizeof(DisplayInfo));
            memcpy((void *)(&Globctx->SunxiDisplay[0]),
                (void *)(&Globctx->SunxiDisplay[1]), sizeof(DisplayInfo));
            memcpy((void *)(&Globctx->SunxiDisplay[1]),
                (void *)&psDisplayInfo, sizeof(DisplayInfo));

            Globctx->SunxiDisplay[0].InitDisplayWidth  = Globctx->InitDisplaySize[HWC_DISPLAY_PRIMARY][0];
            Globctx->SunxiDisplay[0].InitDisplayHeight = Globctx->InitDisplaySize[HWC_DISPLAY_PRIMARY][1];

            if (Globctx->SunxiDisplay[1].VirtualToHWDisplay == HDMI_USED) {
                Globctx->SunxiDisplay[1].InitDisplayWidth  = Globctx->InitDisplaySize[HWC_DISPLAY_PRIMARY][0];
                Globctx->SunxiDisplay[1].InitDisplayHeight = Globctx->InitDisplaySize[HWC_DISPLAY_PRIMARY][1];
            } else {
                Globctx->SunxiDisplay[1].InitDisplayWidth  = Globctx->InitDisplaySize[HWC_DISPLAY_EXTERNAL][0];
                Globctx->SunxiDisplay[1].InitDisplayHeight = Globctx->InitDisplaySize[HWC_DISPLAY_EXTERNAL][1];
            }
            ALOGD("set display size %dx%d, VirtualToHWDisplay %d",
                Globctx->SunxiDisplay[1].InitDisplayWidth,
                Globctx->SunxiDisplay[1].InitDisplayHeight,
                Globctx->SunxiDisplay[1].VirtualToHWDisplay);

            /* sync the vsyncEnable of primary display */
            Globctx->SunxiDisplay[0].VsyncEnable = Globctx->SunxiDisplay[1].VsyncEnable;
            arg[0] = Globctx->SunxiDisplay[0].VirtualToHWDisplay;
            arg[1] = Globctx->SunxiDisplay[0].VsyncEnable;
            ioctl(Globctx->DisplayFd, DISP_VSYNC_EVENT_EN,(unsigned long)arg);
            /* force disable vsync of external display */
            Globctx->SunxiDisplay[1].VsyncEnable = 0;
            arg[0] = Globctx->SunxiDisplay[1].VirtualToHWDisplay;
            arg[1] = Globctx->SunxiDisplay[1].VsyncEnable;
            ioctl(Globctx->DisplayFd, DISP_VSYNC_EVENT_EN,(unsigned long)arg);

            Globctx->exchangeDispChannel = 0;
            pthread_mutex_unlock(&Globctx->lock);
        }
    }

    if (0 != (Globctx->blankHwdisp & 0x80000000)) {
        unsigned int tempHwDisp = Globctx->blankHwdisp & ~0x80000000;
        if(0 == pthread_mutex_trylock(&Globctx->lock)) {
            if (tempHwDisp < NUMBEROFDISPLAY) {
                ALOGD("do blank Hwdisp=%d", tempHwDisp);
                blank_disp(tempHwDisp);
            } else {
                ALOGD("cant blank Hwdisp=%d", tempHwDisp);
            }
            Globctx->blankHwdisp = 0;
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

    switch(dispOutput->type) {
    case DISP_OUTPUT_TYPE_LCD:
        struct fb_var_screeninfo info;
        if (ioctl(Globctx->FBFd, FBIOGET_VSCREENINFO, &info) == -1) {
            ALOGE("FBIOGET_VSCREENINFO ioctl failed: %s", strerror(errno));
            return -1;
        }
        refreshRate = 1000000000000LLU /
            (
            uint64_t( info.upper_margin + info.lower_margin + info.vsync_len + info.yres )
            * ( info.left_margin  + info.right_margin + info.hsync_len + info.xres )
            * info.pixclock
            );
        if (refreshRate == 0) {
            ALOGW("invalid refresh rate, assuming 60 Hz");
            refreshRate = 60;
        }
        if(info.width == 0) {
            psDisplayInfo->DiplayDPI_X = 160000;
        } else {
            psDisplayInfo->DiplayDPI_X = 1000 * (info.xres * 25.4f) / info.width;
        }
        if(info.height == 0) {
            psDisplayInfo->DiplayDPI_Y = 160000;
        } else {
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
        if((HWC_DISPLAY_PRIMARY == disp) && (0 == isDisplayP2P())) {
            //not point to point
            tvmode4sysrsl = getTvMode4SysResolution();
            psDisplayInfo->InitDisplayWidth = get_info_mode(tvmode4sysrsl, WIDTH);
            psDisplayInfo->InitDisplayHeight = get_info_mode(tvmode4sysrsl, HEIGHT);
        } else {
            // i want that external display is display-p2p
            if(0 == MAX_INIT_DISP_WIDTH) {
                psDisplayInfo->InitDisplayWidth = psDisplayInfo->VarDisplayWidth;
            } else {
                psDisplayInfo->InitDisplayWidth =
                    (psDisplayInfo->VarDisplayWidth <= MAX_INIT_DISP_WIDTH) ?
                    psDisplayInfo->VarDisplayWidth : MAX_INIT_DISP_WIDTH;
            }
            if(0 == MAX_INIT_DISP_HEIGHT) {
                psDisplayInfo->InitDisplayHeight = psDisplayInfo->VarDisplayHeight;
            } else {
                psDisplayInfo->InitDisplayHeight =
                    (psDisplayInfo->VarDisplayHeight <= MAX_INIT_DISP_HEIGHT) ?
                    psDisplayInfo->VarDisplayHeight : MAX_INIT_DISP_HEIGHT;
            }
        }
        psDisplayInfo->DiplayDPI_X = 213000;
        psDisplayInfo->DiplayDPI_Y = 213000;
        psDisplayInfo->issecure = HAS_HDCP;
        break;
    case DISP_OUTPUT_TYPE_VGA:
        psDisplayInfo->VarDisplayWidth = getVgaInfo(dispOutput->mode,WIDTH);
        psDisplayInfo->VarDisplayHeight = getVgaInfo(dispOutput->mode,HEIGHT);
        psDisplayInfo->DisplayFps = getVgaInfo(dispOutput->mode, REFRESHRAE);
        psDisplayInfo->DisplayVsyncP = 1000000000 / psDisplayInfo->DisplayFps;
        if((HWC_DISPLAY_PRIMARY == disp) && (0 == isDisplayP2P())) {
            //not point to point
            tvmode4sysrsl = getTvMode4SysResolution();
            psDisplayInfo->InitDisplayWidth = get_info_mode(tvmode4sysrsl, WIDTH);
            psDisplayInfo->InitDisplayHeight = get_info_mode(tvmode4sysrsl, HEIGHT);
        } else {
            // i want that external display is display-p2p
            if(0 == MAX_INIT_DISP_WIDTH) {
            psDisplayInfo->InitDisplayWidth = psDisplayInfo->VarDisplayWidth;
            } else {
                psDisplayInfo->InitDisplayWidth =
                    (psDisplayInfo->VarDisplayWidth <= MAX_INIT_DISP_WIDTH) ?
                    psDisplayInfo->VarDisplayWidth : MAX_INIT_DISP_WIDTH;
            }
            if(0 == MAX_INIT_DISP_HEIGHT) {
                psDisplayInfo->InitDisplayHeight = psDisplayInfo->VarDisplayHeight;
            } else {
                psDisplayInfo->InitDisplayHeight =
                    (psDisplayInfo->VarDisplayHeight <= MAX_INIT_DISP_HEIGHT) ?
                    psDisplayInfo->VarDisplayHeight : MAX_INIT_DISP_HEIGHT;
            }
        }
        psDisplayInfo->DiplayDPI_X = 213000;
        psDisplayInfo->DiplayDPI_Y = 213000;
        psDisplayInfo->issecure = 1;
        break;
    default:
        psDisplayInfo->VirtualToHWDisplay = EVALID_HWDISP_ID;
        ALOGE("%s:%d, invalid output type %d", __func__, __LINE__, dispOutput->type);
        return -1;
    }

    if(getDispMarginFromFile(dispOutput->type,
            &psDisplayInfo->SetPersentWidth, &psDisplayInfo->SetPersentHeight)) {
        psDisplayInfo->SetPersentWidth  = MARGIN_DEFAULT_PERCENT_WIDTH;
        psDisplayInfo->SetPersentHeight = MARGIN_DEFAULT_PERCENT_HEIGHT;
    }

    psDisplayInfo->VirtualToHWDisplay = hwDisp;
    psDisplayInfo->DisplayType = dispOutput->type;
    psDisplayInfo->DisplayMode = dispOutput->mode;
    psDisplayInfo->setDispMode = dispOutput->mode;
    psDisplayInfo->HwChannelNum = hwDisp ? 2 : NUMCHANNELOFDSP;
    psDisplayInfo->LayerNumofCH = NUMLAYEROFCHANNEL;
    psDisplayInfo->VideoCHNum = NUMCHANNELOFVIDEO;
    psDisplayInfo->VsyncEnable = 0;
    psDisplayInfo->ehancemode = 0;
    psDisplayInfo->Current3DMode = DISPLAY_2D_ORIGINAL;
    psDisplayInfo->de_clk = 432000000;
    psDisplayInfo->screenRadio = SCREEN_FULL;
    psDisplayInfo->bandwidth.max = getMemLimit();

    /* get de_clk and fps from driver */
    get_de_freq_and_fps(psDisplayInfo);

	checkinTvConfig(psDisplayInfo);
	checkEnhanceMode(psDisplayInfo);
    return 0;
}

static int InitDisplayDeviceInfo()
{
    SUNXI_hwcdev_context_t *Globctx = &gSunxiHwcDevice;
    unsigned long arg[4] = {0};
    int hwDisp = 0;
    int disp = HWC_DISPLAY_PRIMARY;
    unsigned char hasDispCnt = 0;
    disp_output dispOutput[NUMBEROFDISPLAY];

    // 1. get output types form disp-driver.
    for(; hwDisp < NUMBEROFDISPLAY; hwDisp++) {
        arg[0] = hwDisp;
        arg[1] = (unsigned long)&dispOutput[hwDisp];
        ioctl(Globctx->DisplayFd, DISP_GET_OUTPUT, arg);
        ALOGD("hwDisp=%d, dispOutput[%d, %d]", hwDisp,
        dispOutput[hwDisp].type, dispOutput[hwDisp].mode);
        switch(dispOutput[hwDisp].type) {
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
    if((hasDispCnt > 2) || (hasDispCnt == 0)) {
        // we donot support three display-device initial at present, but fixme in the future.
        ALOGE("hwc: has none or more than 2 display device to initial !!!");
        return -1;
    }

    // 2. init the main-disp, and init the 2nd-disp if neccessery.
    //hwDisp = getMainDisplay();
    if(hwDisp >=0 && hwDisp < NUMBEROFDISPLAY && (0 != dispOutput[hwDisp].type)) {
        //hwcDisplay[disp] = hwDisp;
        saveDispModeToFile(dispOutput[hwDisp].type, dispOutput[hwDisp].mode);
        initDisplayDevice(disp, hwDisp, dispOutput + hwDisp);
        disp++;
        memset(&dispOutput[hwDisp], 0, sizeof(disp_output));
        hasDispCnt--;
    }
    for(hwDisp = 0;
        (hasDispCnt!=0) && (hwDisp<NUMBEROFDISPLAY) && (disp<HWC_NUM_PHYSICAL_DISPLAY_TYPES);
        hwDisp++) {
        if(0 != dispOutput[hwDisp].type) {
            saveDispModeToFile(dispOutput[hwDisp].type, dispOutput[hwDisp].mode);
            initDisplayDevice(disp, hwDisp, dispOutput + hwDisp);
            disp++;
            memset(&dispOutput[hwDisp], 0, sizeof(disp_output));
            hasDispCnt--;
        }
    }

    Globctx->SunxiDisplay[HWC_DISPLAY_PRIMARY].VsyncEnable = 1;
    arg[0] = Globctx->SunxiDisplay[HWC_DISPLAY_PRIMARY].VirtualToHWDisplay;
    arg[1] = 1;
    ioctl(Globctx->DisplayFd, DISP_VSYNC_EVENT_EN,(unsigned long)arg);

    #ifdef SYSRSL_SWITCH_ON_EX
    if((DISP_DUAL_POLICY != Globctx->dispPolicy)
        && (isDisplayP2P())) {
        const char *density = "persist.sys.disp_density";
        memcpy((void *)&(Globctx->SunxiDisplay[HWC_DISPLAY_EXTERNAL]),
        (void *)&(Globctx->SunxiDisplay[HWC_DISPLAY_PRIMARY]),
        sizeof(DisplayInfo));
        switch(Globctx->SunxiDisplay[HWC_DISPLAY_PRIMARY].DisplayMode) {
        case DISP_TV_MOD_3840_2160P_30HZ:
        case DISP_TV_MOD_3840_2160P_60HZ:
        case DISP_TV_MOD_3840_2160P_50HZ:
        case DISP_TV_MOD_1080P_50HZ:
        case DISP_TV_MOD_1080P_60HZ:
        case DISP_TV_MOD_1080P_24HZ:
        case DISP_TV_MOD_1080I_60HZ:
        case DISP_TV_MOD_1080I_50HZ:
            property_set(density, "240");
            break;
        case DISP_TV_MOD_720P_50HZ:
        case DISP_TV_MOD_720P_60HZ:
            property_set(density, "160");
            break;
        case DISP_TV_MOD_576P:
        case DISP_TV_MOD_576I:
            property_set(density, "128");
            break;
        case DISP_TV_MOD_480P:
        case DISP_TV_MOD_480I:
            property_set(density, "106");
            break;
        default:;
        }
        Globctx->SunxiDisplay[HWC_DISPLAY_PRIMARY].VirtualToHWDisplay = F_HWDISP_ID;
        Globctx->SunxiDisplay[HWC_DISPLAY_PRIMARY].DisplayType = DISP_OUTPUT_TYPE_NONE;
    }
    #endif

    return 0;
}

static int setup_dispc_data_alloc(setup_dispc_data_t *pdata)
{
    char *alloc;
    int size = sizeof(disp_layer_t) * 16 + sizeof(int) * 16;
    alloc = (char *)calloc(1, size);
    if (alloc) {
        memset(alloc, 0, size);
        pdata->hwclayer = (disp_layer_t *)alloc;
        pdata->fencefd  = (int *)(alloc + sizeof(disp_layer_t) * 16);
        return 0;
    }

    ALOGE("%s: failed, size %d bytes\n", __func__, size);
    return -ENOMEM;
}

static int setup_dispc_data_free(setup_dispc_data_t *pdata)
{
    free(pdata->hwclayer);
    return 0;
}

static inline void setup_dispc_data_reset(setup_dispc_data_t *pdata)
{
    int size = sizeof(disp_layer_t) * 16 + sizeof(int) * 16;
    memset(pdata->hwclayer, 0, size);
    pdata->layer_num     = 0;
    pdata->needWB        = 0;
    pdata->ehancemode    = 0;
    pdata->androidfrmnum = 0;
}

SUNXI_hwcdev_context_t* hwc_create_device(void)
{
    SUNXI_hwcdev_context_t *Globctx = &gSunxiHwcDevice;

    unsigned long arg[4] = {0};
    int DispCnt;

    property_set("persist.sys.disp_init_exit", "1");
    Globctx->dispPolicy = getDispPolicy();

    Globctx->DisplayFd = open("/dev/disp", O_RDWR);
    if (Globctx->DisplayFd < 0) {
        ALOGE( "Failed to open disp device, ret:%d, errno: %d\n", Globctx->DisplayFd, errno);
    }

    Globctx->FBFd = open("/dev/graphics/fb0", O_RDWR);
    if (Globctx->FBFd < 0) {
        ALOGE( "Failed to open fb0 device, ret:%d, errno:%d\n", Globctx->FBFd, errno);
    }
    Globctx->IonFd = open("/dev/ion",O_RDWR);
    if(Globctx->IonFd < 0) {
        ALOGE( "Failed to open  ion device, ret:%d, errno:%d\n", Globctx->IonFd, errno);
    }
    Globctx->NumberofDisp = NUMBEROFDISPLAY;

    Globctx->SunxiDisplay =(DisplayInfo* )calloc(Globctx->NumberofDisp, sizeof(DisplayInfo));
    memset(Globctx->SunxiDisplay, 0, Globctx->NumberofDisp * sizeof(DisplayInfo));
    for(DispCnt = 0; DispCnt < Globctx->NumberofDisp; DispCnt++) {
        Globctx->SunxiDisplay[DispCnt].VirtualToHWDisplay = EVALID_HWDISP_ID;
    }

    Globctx->DisContext =(HwcDisContext_t* )calloc(Globctx->NumberofDisp,
        sizeof(HwcDisContext_t));
    memset(Globctx->DisContext, 0, Globctx->NumberofDisp * sizeof(HwcDisContext_t));

    setup_dispc_data_alloc(&Globctx->PrivateData[0]);
    setup_dispc_data_alloc(&Globctx->PrivateData[1]);
    Globctx->PrivateData[0].deviceid = 0;
    Globctx->PrivateData[1].deviceid = 1;

    arg[0] = 0;
    arg[1] = HWC_START;
    arg[2] = START_CLIENT_HWC_1_X;
    if (ioctl(Globctx->DisplayFd, DISP_HWC_COMMIT, (unsigned long)arg)) {
        ALOGE("start devcomposer failed !!!");
    }
    arg[0] = 0;
    arg[1] = HWC_SYNC_TIMELINE_SET_OFFSET;
    arg[2] = HWC_SYNC_TIMELINE_DEFAULT;
    arg[3] = 1;
    if (ioctl(Globctx->DisplayFd, DISP_HWC_COMMIT, (unsigned long)arg)) {
        ALOGE("HWC_SYNC_TIMELINE_SET_OFFSET disp0_0 failed !");
    }
    arg[0] = 1;
    arg[1] = HWC_SYNC_TIMELINE_SET_OFFSET;
    arg[2] = HWC_SYNC_TIMELINE_DEFAULT;
    arg[3] = 1;
    if (ioctl(Globctx->DisplayFd, DISP_HWC_COMMIT, (unsigned long)arg)) {
        ALOGE("HWC_SYNC_TIMELINE_SET_OFFSET disp1_0 failed !");
    }

    InitDisplayDeviceInfo();
    Globctx->iomem_type = getIomenType(IOMEM_TYPE_CMA);
    Globctx->ban4k = check4KBan();
    Globctx->CanForceGPUCom = 1;
    Globctx->ForceGPUComp = 0;
    Globctx->fBeginTime = 0.0;
    Globctx->uiBeginFrame = 0;
    Globctx->hwcdebug = 0;
    Globctx->IonHandleHead = NULL;
    Globctx->exchangeDispChannel = 0;
    pthread_mutex_init(&Globctx->lock, NULL);

    ALOGD("####ban4k=%d, iomem_type=%s", Globctx->ban4k,
        Globctx->iomem_type == IOMEM_TYPE_IOMMU ? "iommu" : "cma");
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

#ifdef HWC_WRITEBACK_ENABLE
    hwc_writeback_init(Globctx);
#endif
    return (SUNXI_hwcdev_context_t*)Globctx;
}

int hwc_destroy_device()
{
    SUNXI_hwcdev_context_t *Globctx = &gSunxiHwcDevice;
    int i;

    close(Globctx->DisplayFd);
    close(Globctx->FBFd);
    setup_dispc_data_free(&Globctx->PrivateData[0]);
    setup_dispc_data_free(&Globctx->PrivateData[1]);
    free(Globctx->SunxiDisplay);

    if(NULL != Globctx->DisContext) {
        for(i = 0; i < Globctx->NumberofDisp; ++i) {
            if(NULL != Globctx->DisContext[i].psAllLayer) {
                free(Globctx->DisContext[i].psAllLayer);
                Globctx->DisContext[i].psAllLayer = NULL;
                Globctx->DisContext[i].allocNumLayer = 0;
            }
        }
        free(Globctx->DisContext);
        Globctx->DisContext = NULL;
    }

    IonHandleDecRef(1);
    for(; Globctx->ionListTotalNum > 0; --(Globctx->ionListTotalNum)) {
        ion_list *head = Globctx->IonHandleHead;
        Globctx->IonHandleHead = head->next;
        free(head);
    }
    Globctx->IonHandleHead = NULL;
    close(Globctx->IonFd);

    return 1;
}
