
#include <stdarg.h>
#include "hwc.h"
#include "hwcomposerInterface.h"

int hwc_getOutputFormat(int display, va_list args)
{
    int *type = va_arg(args, int *);
    int *mode = va_arg(args, int *);

    *type = _hwc_device_get_output_type(display);
    *mode = _hwc_device_get_output_mode(display);
    return 0;
}

int hwc_setOutputFormat(int display, va_list args)
{
    int type = va_arg(args, int);
    int mode = va_arg(args, int);
    ALOGD("hwc_setOutputFormat: disp %d type %d mode %d", display, type, mode);
    return _hwc_device_set_output_mode(display, type, mode);
}

int hwc_getEnhanceMode(int display, va_list args)
{
    int *mode = va_arg(args, int *);
    *mode = _hwc_device_get_enhance_mode(display);
    return 0;
}

int hwc_setEnhanceMode(int display, va_list args)
{
    int mode = va_arg(args, int);
    ALOGD("hwc_setEnhanceMode: disp %d enhanceMode %d",
        display, mode);
    return _hwc_device_set_enhance_mode(display, mode);
}

int hwc_getEnhanceComponent(int display, va_list args)
{
    int component_type = va_arg(args, int);
    int *value = va_arg(args, int *);

    switch ((enhance_component_type_t)component_type) {
    case ENHANCE_EDGE:
       *value = _hwc_device_get_enhance_edge(display);
       return 0;
    case ENHANCE_DETAIL:
        *value = _hwc_device_get_enhance_detail(display);
        return 0;
    case ENHANCE_BRIGHT:
        *value = _hwc_device_get_enhance_bright(display);
        return 0;
    case ENHANCE_DENOISE:
        *value = _hwc_device_get_enhance_denoise(display);
        return 0;
    case ENHANCE_CONTRAST:
        *value = _hwc_device_get_enhance_contrast(display);
        return 0;
    case ENHANCE_SATURATION:
        *value = _hwc_device_get_enhance_saturation(display);
        return 0;
    default:
        ALOGD("To get unknown component_type(%d)", component_type);
        return -1;
    }
}

int hwc_setEnhanceComponent(int display, va_list args)
{
    int component_type = va_arg(args, int);
    int value = va_arg(args, int);

    switch ((enhance_component_type_t)component_type) {
    case ENHANCE_EDGE:
        value = _hwc_device_set_enhance_edge(display, value);
        return value;
    case ENHANCE_DETAIL:
        value = _hwc_device_set_enhance_detail(display, value);
        return value;
    case ENHANCE_BRIGHT:
        value = _hwc_device_set_enhance_bright(display, value);
        return value;
    case ENHANCE_DENOISE:
        value = _hwc_device_set_enhance_denoise(display, value);
        return value;
    case ENHANCE_CONTRAST:
        value = _hwc_device_set_enhance_contrast(display, value);
        return value;
    case ENHANCE_SATURATION:
        value = _hwc_device_set_enhance_saturation(display, value);
        return value;
    default:
        ALOGD("To set unknown component_type(%d)", component_type);
        return -1;
    }
}

int hwc_setScreenOverscan(int /* display */, va_list args)
{
    int *value_ptr = va_arg(args, int *);
    _hwc_device_set_margin(HWC_DISPLAY_PRIMARY,  *value_ptr, *(value_ptr + 1));
    _hwc_device_set_margin(HWC_DISPLAY_EXTERNAL, *value_ptr, *(value_ptr + 1));
    return 0;
}

extern SUNXI_hwcdev_context_t gSunxiHwcDevice;
int hwc_getScreenOverscan(int display, va_list args)
{
    int *value_ptr = va_arg(args, int *);

    if (display < 0 || display > 2)
        return -1;

    *(value_ptr + 0) = gSunxiHwcDevice.SunxiDisplay[display].SetPersentWidth;
    *(value_ptr + 1) = gSunxiHwcDevice.SunxiDisplay[display].SetPersentHeight;
    *(value_ptr + 2) = 0;
    *(value_ptr + 3) = 0;
    return 0;
}

int hwc_get3DLayerMode(int /* display */, va_list args)
{
    int *mode = va_arg(args, int *);
    *mode = DISPLAY_2D_ORIGINAL;
    return 0;
}

int hwc_set3DLayerMode(int display, va_list args)
{
    int mode = va_arg(args, int);
    return _hwc_device_set_3d_mode(display, (__display_3d_mode)mode);
}

int hwc_setScreenRadio(int display, va_list args)
{
    int radio = va_arg(args, int);
    return _hwc_device_set_screenradio(display, radio);
}

int hwc_perform(int display, int cmd, ...)
{
    int ret = 0;
    va_list args;
    va_start(args, cmd);

    switch (cmd) {
    case HWC_GET_OUTPUT_FORMAT:
        ret = hwc_getOutputFormat(display, args);
        break;
    case HWC_SET_OUTPUT_FORMAT:
        ret = hwc_setOutputFormat(display, args);
        break;
    case HWC_SET_OVERSCAN:
        ret = hwc_setScreenOverscan(display, args);
        break;
    case HWC_GET_OVERSCAN:
        ret = hwc_getScreenOverscan(display, args);
        break;
    case HWC_GET_3D_LAYER_MODE:
        ret = hwc_get3DLayerMode(display, args);
        break;
    case HWC_SET_3D_LAYER_MODE:
        ret = hwc_set3DLayerMode(display, args);
        break;
    case HWC_SET_SCREEN_RADIO:
        ret = hwc_setScreenRadio(display, args);
        break;
    case HWC_GET_ENHANCE_MODE:
        ret = hwc_getEnhanceMode(display, args);
        break;
    case HWC_SET_ENHANCE_MODE:
        ret = hwc_setEnhanceMode(display, args);
        break;
    case HWC_GET_ENHANCE_COMPONENT:
        ret = hwc_getEnhanceComponent(display, args);
        break;
    case HWC_SET_ENHANCE_COMPONENT:
        ret = hwc_setEnhanceComponent(display, args);
        break;
    default:
        ret = -1;
    }
    return ret;
}
