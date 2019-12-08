
#include <cutils/log.h>
#include "hwc.h"

#define _debug_tag_ "hwc-capture "
#define logerr(fmt, args...)  ALOGE(_debug_tag_ fmt, ##args)
#define loginfo(fmt, args...) ALOGD(_debug_tag_ fmt, ##args)

#define DEFAULT_WRITEBACK_WIDTH     (720)
#define DEFAULT_WRITEBACK_HEIGHT    (576)

enum writeback_state {
    WRITEBACK_IDLE = 1,
    WRITEBACK_TO_PRIMARY_DISP,
    WRITEBACK_TO_EXTERNAL_DISP,
};

struct writeback_context {
    int enable;
    enum writeback_state state;
    int writeback_to;
    int primary_channel_enable;

    int default_width;
    int default_height;
};

static struct writeback_context wbcontext;

static int
is_compressed_video_layer(hwc_layer_1_t *layer) {

    if (!layer || !layer->handle)
        return 0;
    private_handle_t *handle = (private_handle_t *)(layer->handle);
    int format = handle->format;
    int src_width;

#ifdef GRALLOC_SUNXI_METADATA_BUF
    if (handle->ion_metadata_flag & SUNXI_METADATA_FLAG_AFBC_HEADER)
        return 1;
#endif

    /*
     * If the video size if large then 3840 pixel,
     * Use writeback content for external display.
     */
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
        src_width = layer->sourceCrop.right - layer->sourceCrop.left;
        return (src_width >= 3840);
        return 1;
    default:
        return 0;
    }
    return 0;
}

static int
is_interlaced_mode(int mode) {
    int result = 0;
    switch (mode) {
    case DISP_TV_MOD_480I:
    case DISP_TV_MOD_576I:
    case DISP_TV_MOD_1080I_50HZ:
    case DISP_TV_MOD_1080I_60HZ:
        result = 1;
        break;
    default:
        result = 0;
        break;
    }
    return result;
}

static int
hwc_writeback_capability(const SUNXI_hwcdev_context_t *context) {
    DisplayInfo *info = &context->SunxiDisplay[0];
    if (info->VirtualToHWDisplay == HDMI_USED
        && info->DisplayType == DISP_OUTPUT_TYPE_HDMI && is_interlaced_mode(info->DisplayMode)) {
        return 0;
    }
    return 1;
}

int
hwc_writeback_init(SUNXI_hwcdev_context_t *context) {

    context->capture = hwc_capture_init(0);
    if (!context->capture) {
        logerr("hwc capture module init error.");
        return -1;
    }
    wbcontext.enable = 0;
    wbcontext.state = WRITEBACK_IDLE;
    wbcontext.writeback_to = -1;
    wbcontext.primary_channel_enable = 0;

    wbcontext.default_width  = DEFAULT_WRITEBACK_WIDTH;
    wbcontext.default_height = DEFAULT_WRITEBACK_HEIGHT;
    loginfo("hwc capture init success");
    return 0;
}

static int close_unuse_layer_fence(hwc_display_contents_1_t *layer)
{
    if (!layer)
        return 0;
    for (size_t i = 0; i < layer->numHwLayers; i++) {
        if (layer->hwLayers[i].acquireFenceFd >= 0) {
            close(layer->hwLayers[i].acquireFenceFd);
            layer->hwLayers[i].acquireFenceFd = -1;
        }
    }
    return 0;
}

int
hwc_writeback_prepare(const SUNXI_hwcdev_context_t *context,
        hwc_display_contents_1_t **contents, size_t dispcnt,
        hwc_display_contents_1_t **fixup_contents, size_t *fixup_dispcnt) {

    size_t dispid;
    int need_writeback = 0;
    int external_display = 0;
    DisplayInfo *info;

    if (!context->capture) {
        logerr("hwc capture mode init error");
        return -1;
    }

    fixup_contents[0] = contents[0];
    fixup_contents[1] = contents[1];
    *fixup_dispcnt = dispcnt;

    if (hwc_writeback_capability(context) != 1) {
        goto __exit_wb;
    }

    dispcnt = dispcnt > 2 ? 2 : dispcnt;
    for (dispid = 0; dispid < dispcnt; dispid++) {
        info = &context->SunxiDisplay[dispid];
        if (info->VirtualToHWDisplay != HW_CHANNEL_EXTERNAL)
            continue;

        hwc_layer_1_t *hwclayer;
        hwc_display_contents_1_t *gather = contents[dispid];
        if (!gather || gather->numHwLayers <= 0)
            continue;

        /*
         * Check external display's layer, If there's any layer which
         * can't handle by display-engine, Then enable writeback.
         */
        for (size_t i = 0; i < gather->numHwLayers; i++) {
            hwclayer = &gather->hwLayers[i];
            if (is_compressed_video_layer(hwclayer)) {
                need_writeback = 1;
                external_display = dispid;
                break;
            }
        }
    }

    if (need_writeback) {
        switch (external_display) {
        case 0:
            /*
             * If the external_display equal to 0,
             * It means that there is only external diplay present now.
             * Copy layer from disp.0 to disp.1
             */
            fixup_contents[0] = contents[0];
            fixup_contents[1] = contents[0];
            *fixup_dispcnt = 2;
            close_unuse_layer_fence(contents[1]);

            /*
             * Open primary channel to composer layer
             */
            info = &context->SunxiDisplay[1];
            if (!wbcontext.primary_channel_enable) {
                hwcOutputSwitch(info, DISP_OUTPUT_TYPE_HDMI, DISP_TV_MOD_720P_60HZ);
                wbcontext.primary_channel_enable = 1;
                ALOGD("open primary channel");
            }

            /* correct the display device info */
            info->opened_for_wb = 1;
            info->VirtualToHWDisplay = 0;
            break;
        case 1:
            fixup_contents[0] = contents[0];
            fixup_contents[1] = contents[1];
            *fixup_dispcnt = dispcnt;

            /* FIXME: */
            info = &context->SunxiDisplay[0];
            info->opened_for_wb = 0;
            wbcontext.primary_channel_enable = 0;
            break;
        default:
            break;
        }
    } else {
__exit_wb:
        fixup_contents[0] = contents[0];
        fixup_contents[1] = dispcnt > 1 ? contents[1] : NULL;
        context->SunxiDisplay[0].opened_for_wb = 0;
        context->SunxiDisplay[1].opened_for_wb = 0;

        if (wbcontext.primary_channel_enable
                && context->SunxiDisplay[0].VirtualToHWDisplay == 1) {
            info = &context->SunxiDisplay[1];
            blank_disp(0);
            info->VirtualToHWDisplay = EVALID_HWDISP_ID;
//          hwcOutputSwitch(info, DISP_OUTPUT_TYPE_NONE, 0);
            wbcontext.primary_channel_enable = 0;
            ALOGD("close primary channel");
        }
    }

    struct hwc_capture_context *capture = context->capture;
    if (need_writeback) {
        wbcontext.writeback_to = external_display;

        if (!capture->is_enable(capture)) {
            capture_config_t config;
            config.width  = wbcontext.default_width;
            config.height = wbcontext.default_height;
            config.format = DISP_FORMAT_YUV420_P;
            int retval = capture->enable(capture, &config);
            if (retval) {
                capture->disable(capture);
                wbcontext.writeback_to = -1;
            }
            loginfo("capture enable: %s", retval ? "FAILED" : "OK");
        }

        /*
         * If we use writeback buffer for this dispid, The SurfaceFlinger
         * no need to composition any layer for this display, So set layer
         * type with HWC_FRAMEBUFFER to fake SurfaceFlinger.
         */
        hwc_display_contents_1_t *gather = contents[external_display];
        for (size_t i = 0; i < gather->numHwLayers; i++) {
            if (gather->hwLayers[i].compositionType != HWC_FRAMEBUFFER_TARGET) {
                gather->hwLayers[i].compositionType = HWC_OVERLAY;
            }
        }
    } else {
        wbcontext.writeback_to = -1;
        if (capture->is_enable(capture)) {
            int retval = capture->disable(capture);
            loginfo("capture close: %s", retval ? "FAILED" :"OK");
        }

        if (capture->buffer_refs_count(capture)) {
            capture->buffer_refs_decrease(capture);
            loginfo("decrease buffer refs");
        }
    }
    return 0;
}

int
hwc_writeback_is_enable(void) {
    return (wbcontext.writeback_to == 0 || wbcontext.writeback_to == 1);
}

size_t
hwc_writeback_get_display_id(void) {
    return (size_t)wbcontext.writeback_to;
}

static int
__recalculate_position(const SUNXI_hwcdev_context_t *context,
        struct disp_rect *crop, struct disp_rect *screen_window) {

    int src_disp_id = wbcontext.writeback_to == 0 ? 1 : 0;
    DisplayInfo *dispinfo_src = &context->SunxiDisplay[src_disp_id];
    DisplayInfo *dispinfo_dst = &context->SunxiDisplay[wbcontext.writeback_to];

    // TODO: consider that when SetPersentWidth is not equal to SetPersentHeight.
    if (dispinfo_src->SetPersentWidth == dispinfo_dst->SetPersentWidth) {
        crop->x = 0;
        crop->y = 0;
        crop->width  = wbcontext.default_width;
        crop->height = wbcontext.default_height;

        screen_window->x = 0;
        screen_window->y = 0;
        screen_window->width  = dispinfo_dst->VarDisplayWidth;
        screen_window->height = dispinfo_dst->VarDisplayHeight;
    } else if (dispinfo_src->SetPersentWidth > dispinfo_dst->SetPersentWidth) {
        crop->x = 0;
        crop->y = 0;
        crop->width  = wbcontext.default_width;
        crop->height = wbcontext.default_height;

        float factor = (float)dispinfo_dst->SetPersentWidth / dispinfo_src->SetPersentWidth;
        int width  = ceil(dispinfo_dst->VarDisplayWidth  * factor);
        int height = ceil(dispinfo_dst->VarDisplayHeight * factor);

        screen_window->x = (dispinfo_dst->VarDisplayWidth  - width ) / 2;
        screen_window->y = (dispinfo_dst->VarDisplayHeight - height) / 2;
        screen_window->width  = width;
        screen_window->height = height;
    } else {
        int crop_w = wbcontext.default_width  * dispinfo_src->SetPersentWidth  / 100;
        int crop_h = wbcontext.default_height * dispinfo_src->SetPersentHeight / 100;
        int frame_w = dispinfo_dst->VarDisplayWidth  * dispinfo_dst->SetPersentWidth  / 100;
        int frame_h = dispinfo_dst->VarDisplayHeight * dispinfo_dst->SetPersentHeight / 100;

        crop->x = (wbcontext.default_width  - crop_w) / 2;
        crop->y = (wbcontext.default_height - crop_h) / 2;
        crop->width  = crop_w;
        crop->height = crop_h;

        screen_window->x = (dispinfo_dst->VarDisplayWidth  - frame_w) / 2;
        screen_window->y = (dispinfo_dst->VarDisplayHeight - frame_h) / 2;
        screen_window->width  = frame_w;
        screen_window->height = frame_h;
    }
    return 0;
}

int
hwc_writeback_buffer_setup(const SUNXI_hwcdev_context_t *context, setup_dispc_data_t *out) {
    struct disp_capture_handle cptrhdl;
    struct hwc_capture_context *capture = context->capture;
    DisplayInfo *dispinfo = &context->SunxiDisplay[wbcontext.writeback_to];

    /*
     * acquire writeback buffer from capture module.
     */
    capture->acquire_buffer(capture, &cptrhdl);
    if (cptrhdl.handle >= 0 && cptrhdl.handle < MAX_BUFFER_COUNT) {
        int ioctl_ret = 0;
        ioctl_ret = capture->release_buffer(capture, &capture->current);
        memcpy(&capture->current, &cptrhdl, sizeof(struct disp_capture_handle));
    } else {
        if (capture->current.handle >= 0 && capture->current.handle < MAX_BUFFER_COUNT) {
            memcpy(&cptrhdl, &capture->current, sizeof(struct disp_capture_handle));
            cptrhdl.fencefd = -1;
            logerr("no ready buffer, repeat: %d", capture->current.handle);
        } else {
            logerr("no valid handle");
            return -1;
        }
    }

    /*
     * Use the writeback buffer to replace layers from
     * SurfaceFlinger.
     */
    disp_layer_t *config = &out->hwclayer[0];
    struct disp_capture_buffer *buffer = capture->slots[cptrhdl.handle].buf;

    out->layer_num = 1;
    out->fencefd[0] = cptrhdl.fencefd;
    out->androidfrmnum = context->HWCFramecount;

    config->enable = 1;
    if (buffer->format == DISP_FORMAT_ARGB_8888) {
        config->channel  = 1;
        config->layer_id = 0;
    } else {
        config->channel  = 0;
        config->layer_id = 0;
    }

    config->info.fb.fd = buffer->fd;
    config->info.fb.format  = buffer->format;

    config->info.fb.size[0].width  = buffer->width;
    config->info.fb.size[0].height = buffer->height;
    if (buffer->format == DISP_FORMAT_YUV420_P) {
        config->info.fb.size[1].width  = buffer->width  / 2;
        config->info.fb.size[1].height = buffer->height / 2;
        config->info.fb.size[2].width  = buffer->width  / 2;
        config->info.fb.size[2].height = buffer->height / 2;

        config->info.fb.align[0] = 4;
        config->info.fb.align[1] = 4;
    } else if (buffer->format == DISP_FORMAT_YUV420_SP_UVUV) {
        config->info.fb.size[1].width  = buffer->width  / 2;
        config->info.fb.size[1].height = buffer->height / 2;
        config->info.fb.size[2].width  = buffer->width  / 2;
        config->info.fb.size[2].height = buffer->height / 2;

        config->info.fb.align[0] = 4;
        config->info.fb.align[1] = 4;
    } else {
        config->info.fb.size[1].width  = buffer->width;
        config->info.fb.size[1].height = buffer->height;
        config->info.fb.size[2].width  = buffer->width;
        config->info.fb.size[2].height = buffer->height;
    }

    config->info.mode   = LAYER_MODE_BUFFER;
    config->info.zorder = 0;

    struct disp_rect crop, window;
    __recalculate_position(context, &crop, &window);

    config->info.fb.crop.x = (long long)(((long long)(crop.x)) << 32);
    config->info.fb.crop.y = (long long)(((long long)(crop.y)) << 32);
    config->info.fb.crop.width  = (unsigned long long)crop.width  << 32;
    config->info.fb.crop.height = (unsigned long long)crop.height << 32;

    config->info.alpha_mode        = 1;
    config->info.alpha_value       = 0xff;
    config->info.screen_win.x      = window.x;
    config->info.screen_win.y      = window.y;
    config->info.screen_win.width  = window.width;
    config->info.screen_win.height = window.height;

#if (DE_VERSION == 30)
    config->info.fb.color_space = DISP_BT709;
    config->info.fb.eotf = DISP_EOTF_GAMMA22;
#endif

    return 0;
}

int
hwc_writeback_buffer_clear(SUNXI_hwcdev_context_t *context) {

    struct hwc_capture_context *capture = context->capture;

    if (wbcontext.writeback_to == -1
            || !capture->state)
        return 0;

    if (capture->current.fencefd) {
        close(capture->current.fencefd);
        capture->current.fencefd = -1;
    }
    return 0;
}


