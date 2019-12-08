#include "utils.h"

int is3DMode(int mode){
    switch(mode){
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

int getInfoOfMode(const tv_para_t* tvParaList, int number, int mode, int info){
    ALOGD("tvParaList size is %d", number);
    for(int i = 0; i < number; i++){
        if(tvParaList[i].mode == mode){

            return *(((int *)(tvParaList+i))+info);
        }
    }
    return INVALID_VALUE;
}

device_info_t* hwcFindDevice(int hwDispChn){
    device_info_t* psDeviceInfo = NULL;
    device_info_t* tmpDeviceInfo = NULL;
    hwc_context_t *glbCtx = &gHwcContext;
    int find = 0;
    int disp = 0;
    for(disp = 0; disp < glbCtx->displayNum; disp++){
        psDeviceInfo = glbCtx->sunxiDevice + disp;
        if(psDeviceInfo->disp.id == hwDispChn){
            ALOGD("1.hwcFindDevice %d", disp);
            return psDeviceInfo;
        }
    }
    //if can not find , return a unused device.
    for(disp = 0; disp < glbCtx->displayNum; disp++){
        psDeviceInfo = glbCtx->sunxiDevice + disp;
        if(psDeviceInfo->disp.id == INVALID_VALUE){
            ALOGD("2.hwcFindDevice %d", disp);
            return psDeviceInfo;
        }
    }
    return NULL;
}

int getHwDispId(int disp){
    hwc_context_t *glbCtx = &gHwcContext;
    switch(disp){
    case HWC_DISPLAY_PRIMARY:
        return glbCtx->mainDispId;
    case HWC_DISPLAY_EXTERNAL:
        return glbCtx->secDispId;
    default:
        return INVALID_VALUE;
    }
}

void setAllDeviceMargin(int hpercent, int vpercent){
    hwc_context_t *glbCtx = &gHwcContext;
    device_info_t *psDeviceInfo = NULL;

    for(int i = 0; i < glbCtx->displayNum; i++)
    {
        psDeviceInfo = &glbCtx->sunxiDevice[i];
        psDeviceInfo->displayPercentWT = (hpercent < MARGIN_MIN_PERCENT) ? MARGIN_MIN_PERCENT :
            ((hpercent > MARGIN_MAX_PERCENT) ? MARGIN_MAX_PERCENT : hpercent);
        psDeviceInfo->displayPercentHT = (vpercent < MARGIN_MIN_PERCENT) ? MARGIN_MIN_PERCENT :
            ((vpercent > MARGIN_MAX_PERCENT) ? MARGIN_MAX_PERCENT : vpercent);
    }
}

int deviceGetOutputType(int disp)
{
    hwc_context_t *glbctx = &gHwcContext;
    device_info_t *psDeviceInfo = NULL;
    int hwDisp = INVALID_VALUE;
    for(int i = 0; i < glbctx->displayNum; i++)
    {
        psDeviceInfo = &glbctx->sunxiDevice[i];
        hwDisp = psDeviceInfo->disp.id;
        if(hwDisp == INVALID_VALUE){
            continue;
        }
        if((hwDisp == glbctx->mainDispId && disp == HWC_DISPLAY_PRIMARY)
            ||(hwDisp == glbctx->secDispId && disp == HWC_DISPLAY_EXTERNAL))
        {
            return psDeviceInfo->displayType;
        }
    }
    return 0;
}
int deviceGetOutputMode(int disp){
    hwc_context_t *glbCtx = &gHwcContext;
    switch(disp){
    case HWC_DISPLAY_PRIMARY:
        return glbCtx->mainDispMode;
    case HWC_DISPLAY_EXTERNAL:
        return glbCtx->secDispMode;
    default:
        return INVALID_VALUE;
    }
}
int deviceGetMarginW(){
    hwc_context_t *glbCtx = &gHwcContext;
    //As all the margin size of displays are the same , we can just returm
    //one of them.
    device_info_t *psDeviceInfo = &glbCtx->sunxiDevice[0];
    return (int)(psDeviceInfo->displayPercentWT);
}
int deviceGetMarginH(){
    hwc_context_t *glbCtx = &gHwcContext;
    //As all the margin size of displays are the same , we can just returm
    //one of them.
    device_info_t *psDeviceInfo = &glbCtx->sunxiDevice[0];
    return (int)(psDeviceInfo->displayPercentHT);
}
int getValueFromProperty(char const* propName){
    char property[PROPERTY_VALUE_MAX];
    int value = -1;
    if (property_get(propName, property, NULL) > 0)
    {
        value = atoi(property);
    }
    ALOGD("###%s: propName:%s,value=%d", __func__, propName, value);
    return value;
}

//check whether we config that display point to point
//-->Size of DisplayMode == Size of System Resolution
int isDisplayP2P(void)
{
    if(1 != getValueFromProperty(DISP_P2P_PROPERTY))
    {
        return 0; // not support display point2p
    }
    else
    {
        return 1;
    }
}
int repaint_request = 0;
unsigned int uiBeginFrame = 0;
double fBeginTime = 0.0;

void updateFps(hwc_context_t *context){
    //update fps
    timeval tv = { 0, 0 };
    gettimeofday(&tv, NULL);
    double fCurrentTime = tv.tv_sec + tv.tv_usec / 1.0e6;

    if(fCurrentTime - fBeginTime >= 1)
    {
        char property[PROPERTY_VALUE_MAX];
        int  showFpsSettings = 0;
        if (property_get("debug.hwc.showfps", property, NULL) >= 0)
        {
            showFpsSettings = atoi(property);
        }else{
            uiBeginFrame = context->hwcFrameCount;
            ALOGD("No hwc debug attribute node.");
            return;
        }
        if((showFpsSettings&FPS_SHOW) != (context->hwcDebug&FPS_SHOW))
        {
            ALOGD("###### %s hwc fps print ######",(showFpsSettings&1) != 0 ? "Enable":"Disable");
        }
        context->hwcDebug = showFpsSettings&SHOW_ALL;
        if(context->hwcDebug&1)
        {
            ALOGD(">>>fps %d\n", int((context->hwcFrameCount - uiBeginFrame) * 1.0f
                                      / (fCurrentTime - fBeginTime)));
        }
        uiBeginFrame = context->hwcFrameCount;
        fBeginTime = fCurrentTime;
    }

}



void invalidate(int times){
    repaint_request = times;
}


void *vsyncThreadWrapper(void *priv){
    setpriority(PRIO_PROCESS, 0, HAL_PRIORITY_URGENT_DISPLAY);
    hwc_context_t *context = &gHwcContext;
    struct sockaddr_nl snl;
    const int buffersize = 32*1024;
    int retval;
    int hotplug_sock;

    memset(&snl, 0x0, sizeof(snl));
    snl.nl_family = AF_NETLINK;
    snl.nl_pid = 0;
    snl.nl_groups = 0xffffffff;

    hotplug_sock = socket(PF_NETLINK, SOCK_DGRAM, NETLINK_KOBJECT_UEVENT);
    if (hotplug_sock == -1) {
        ALOGE("####socket is failed in %s error:%d %s###", __FUNCTION__, errno, strerror(errno));
        return NULL;
    }

    setsockopt(hotplug_sock, SOL_SOCKET, SO_RCVBUFFORCE, &buffersize, sizeof(buffersize));

    retval = bind(hotplug_sock, (struct sockaddr *)&snl, sizeof(struct sockaddr_nl));

    if (retval < 0) {
        ALOGE("####bind is failed in %s error:%d %s###", __FUNCTION__, errno, strerror(errno));
        close(hotplug_sock);
        return NULL;
    }
    while(1)
    {
        char buf[4096*2] = {0};
        struct pollfd fds;
        int err;
        //unsigned int cout;

        fds.fd = hotplug_sock;
        fds.events = POLLIN;
        fds.revents = 0;
        err = poll(&fds, 1, 1000);

        if(err > 0 && fds.revents == POLLIN)
        {
            int count = recv(hotplug_sock, &buf, sizeof(buf),0);
            if(count > 0)
            {
                int isVsync;

                isVsync = !strcmp(buf, "change@/devices/platform/disp");

                if(isVsync)
                {
                    uint64_t timestamp = 0;
                    int display_id = -1;
                    const char *s = buf;

                    if(!context->psHwcProcs || !context->psHwcProcs->vsync)
                    {
                       ALOGD("####unable to call Globctx->psHwcProcs->vsync, should not happened");
                       continue;
                    }

                    s += strlen(s) + 1;
                    while(s)
                    {
                        if (!strncmp(s, "VSYNC0=", strlen("VSYNC0=")))
                        {
                            timestamp = strtoull(s + strlen("VSYNC0="), NULL, 0);
                            ALOGV("#### %s display0 timestamp:%lld###", s,timestamp);
                            display_id = DISP_CHN_0;
                        }
                        else if (!strncmp(s, "VSYNC1=", strlen("VSYNC1=")))
                        {
                            timestamp = strtoull(s + strlen("VSYNC1="), NULL, 0);
                            ALOGV("#### %s display1 timestamp:%lld###", s,timestamp);
                            display_id = DISP_CHN_1;
                        }else if(!strncmp(s, "VSYNC2=", strlen("VSYNC2="))){
                            timestamp = strtoull(s + strlen("VSYNC1="), NULL, 0);
                            ALOGV("#### %s display2 timestamp:%lld###", s,timestamp);
                            display_id = DISP_CHN_2;
                        }

                        s += strlen(s) + 1;
                        if(s - buf >= count)
                        {
                            break;
                        }
                    }
                    if(display_id == context->mainDispId)
                    {
                        device_info_t *psDeviceInfo = NULL;
                        for(int i = 0; i < context->displayNum; i++)
                        {
                            if(display_id == context->sunxiDevice[i].disp.id)
                            {
                                psDeviceInfo = &(context->sunxiDevice[i]);
                                break;
                            }
                        }

                        if(psDeviceInfo != NULL)
                        {
                            display_id = 0;
                            context->psHwcProcs->vsync(context->psHwcProcs, display_id, timestamp);
                        }
                    }
                }
            }
            context->forceGPUComp = 0;
        }
        updateFps(context);
    }

    return NULL;
}

void calculateFactor(device_info_t *psDeviceInfo, float *xWidthFactor, float *xHightfactor){
    hwc_context_t *glbCtx = &gHwcContext;
    float widthFactor = (float)psDeviceInfo->displayPercentW/ 100;
    float hightfactor = (float)psDeviceInfo->displayPercentH/ 100;
    if(psDeviceInfo->disp.id == glbCtx->mainDispId){
        if(glbCtx->mainDispWidth && glbCtx->mainDispHeight){
            widthFactor = (float)psDeviceInfo->varDisplayWidth / glbCtx->mainDispWidth * psDeviceInfo->displayPercentW / 100;
            hightfactor = (float)psDeviceInfo->varDisplayHeight / glbCtx->mainDispHeight * psDeviceInfo->displayPercentH / 100;
        }
        *xWidthFactor = widthFactor;
        *xHightfactor = hightfactor;
    }else if(psDeviceInfo->disp.id == glbCtx->secDispId){
        *xWidthFactor = 1.0f;
        *xHightfactor = 1.0f;
    }
}
bool hwcIsBlended(hwc_layer_1_t *psLayer){
    return (psLayer->blending != HWC_BLENDING_NONE);
}
bool hwcIsPremult(hwc_layer_1_t *psLayer){
    return (psLayer->blending == HWC_BLENDING_PREMULT);
}
bool hwcIsPlaneAlpha(hwc_layer_1_t *psLayer){
    return (psLayer->planeAlpha != 0xff);
}
bool checkVideoFormat(int format){
    switch(format){
    case HAL_PIXEL_FORMAT_YV12:
    case HAL_PIXEL_FORMAT_YCrCb_420_SP:
    case HAL_PIXEL_FORMAT_AW_NV12:
        return 1;
    default:
        return 0;
    }
}
int hwcRegionIntersect(hwc_rect_t *rect0, hwc_rect_t *rect1){
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

    if(mid_diff_x < (sum_width/2) && mid_diff_y < (sum_height/2)){
        return 1;//return 1 is intersect
    }
    return 0;
}

void resetLayerType(hwc_display_contents_1_t* displays){
    hwc_display_contents_1_t *list = displays;
    if (list && list->numHwLayers > 1)
    {
        unsigned int j = 0;
        for(j = 0; j < list->numHwLayers; j++)
        {
            if(list->hwLayers[j].compositionType != HWC_FRAMEBUFFER_TARGET)
            {
               list->hwLayers[j].compositionType = HWC_FRAMEBUFFER;
            }
        }
    }
}

bool isLayerProtected(int usage){
    return (usage & GRALLOC_USAGE_PROTECTED);
}

bool isLayerUsageSW(int usage){
    return usage & (GRALLOC_USAGE_SW_READ_OFTEN |
                    GRALLOC_USAGE_SW_WRITE_OFTEN);
}

bool isLayerBlended(hwc_layer_1_t* psLayer){
	return (psLayer->blending != HWC_BLENDING_NONE);
}

bool isLayerPremult(hwc_layer_1_t* psLayer){
    return (psLayer->blending == HWC_BLENDING_PREMULT);
}

bool isLayerScaled(device_info_t* deviceInfo, hwc_layer_1_t* ly){
    float xWidthFactor = 1;
    float xHightFactor = 1;

    calculateFactor(deviceInfo, &xWidthFactor, &xHightFactor);

    int w = ly->sourceCrop.right - ly->sourceCrop.left;
    int h = ly->sourceCrop.bottom - ly->sourceCrop.top;

    if (ly->transform & HWC_TRANSFORM_ROT_90)
    {
        int tmp = w;
        w = h;
        h = tmp;
    }

    return ((ly->displayFrame.right - ly->displayFrame.left) * xWidthFactor != w)
        || ((ly->displayFrame.bottom - ly->displayFrame.top) * xHightFactor != h);
}


//about rect operation
//return true if two region is intersected.
bool twoRegionIntersect(hwc_rect_t *rect0, hwc_rect_t *rect1){
    if(rect0->bottom < 0 || rect0->left < 0 || rect0->right < 0 || rect0->top < 0 ||
        rect1->bottom < 0 || rect1->left < 0 || rect1->right < 0 || rect1->top < 0){
        return false;
    }

    int midX0, midY0, midX1, midY1;
    int midDiffX, midDiffY;
    int sumWidth, sumHeight;

    midX0 = (rect0->right + rect0->left)/2;
    midY0 = (rect0->bottom + rect0->top)/2;
    midX1 = (rect1->right + rect1->left)/2;
    midY1 = (rect1->bottom + rect1->top)/2;

    midDiffX = (midX0 >= midX1)? (midX0 - midX1):(midX1 - midX0);
    midDiffY = (midY0 >= midY1)? (midY0 - midY1):(midY1 - midY0);

    sumWidth = (rect0->right - rect0->left) + (rect1->right - rect1->left);
    sumHeight = (rect0->bottom - rect0->top) + (rect1->bottom - rect1->top);

    if(midDiffX < (sumWidth/2) && midDiffY < (sumHeight/2))
    {
        return true;// is intersect
    }

    return false;

}
//return true if the first region(rect0) is in rect1
bool inRegion(hwc_rect_t *rectUp, hwc_rect_t* rectDw){
    return ((rectDw->left <= rectUp->left)&&(rectDw->right > rectUp->right) && (rectDw->top < rectUp->top)&&(rectDw->bottom > rectUp->bottom));
}
//merge the first region to the second region
int regionMerge(hwc_rect_t *rectFrom, hwc_rect_t* rectTo, int boundWidth, int boundHeight){
    if(rectTo->left == INVALID_VALUE){
        rectTo->left = rectFrom->left;
    }
    if(rectTo->right == INVALID_VALUE){
        rectTo->right = rectFrom->right;
    }
    if(rectTo->top == INVALID_VALUE){
        rectTo->top = rectFrom->top;
    }
    if(rectTo->bottom == INVALID_VALUE){
        rectTo->bottom = rectFrom->bottom;
    }

    if(rectFrom->left <= rectTo->left){
		rectTo->left = (rectFrom->left<0)?0:rectFrom->left;
	}
	if(rectFrom->right >= rectTo->right){
		rectTo->right = (rectFrom->right > boundWidth)?boundWidth:rectFrom->right;
	}
	if(rectFrom->top <= rectTo->top){
		rectTo->top = (rectFrom->top < 0)?0:rectFrom->top;
	}
	if(rectFrom->bottom >= rectTo->bottom){
		rectTo->bottom = (rectFrom->bottom > boundHeight)?boundHeight:rectFrom->bottom;
	}
    return 1;
}

bool isValidVideoFormat(int format){
	switch(format){
    case HAL_PIXEL_FORMAT_AW_YUV_PLANNER420:
    case HAL_PIXEL_FORMAT_AW_YVU_PLANNER420:
    case HAL_PIXEL_FORMAT_AW_NV12:
    case HAL_PIXEL_FORMAT_AW_NV21:
	case HAL_PIXEL_FORMAT_YV12:
	case HAL_PIXEL_FORMAT_YCrCb_420_SP:
        return 1;
    default:
        return 0;
    }
}

int ionGetAddr(int shareFd){
    hwc_context_t*glbCtx = &gHwcContext;
    struct ion_custom_data customData;
	sunxi_phys_data_t physData;
    ion_handle_data freeData;

    struct ion_fd_data data ;
    data.fd = shareFd;
    int ret = ioctl(glbCtx->ionFd, ION_IOC_IMPORT, &data);
    if (ret < 0)
    {
        ALOGE("#######ion_import  error#######");
        return 0;
    }
    customData.cmd = ION_IOC_SUNXI_PHYS_ADDR;
	physData.handle = (void*)data.handle;
	customData.arg = (unsigned long)&physData;
	ret = ioctl(glbCtx->ionFd, ION_IOC_CUSTOM,&customData);
	if(ret < 0){
        ALOGE("ION_IOC_CUSTOM(err=%d)",ret);
        return 0;
    }
    freeData.handle = data.handle;
    ret = ioctl(glbCtx->ionFd, ION_IOC_FREE, &freeData);
    if(ret < 0){
        ALOGE("ION_IOC_FREE(err=%d)",ret);
        return 0;
    }
    return physData.phys_addr;
}

device_info_t* getDeviceInfoByChn(int chn){
    hwc_context_t* context = &gHwcContext;
    int i;
    device_info_t *deviceInfo = NULL;
    for(i = 0; i < context->displayNum; i++){
        deviceInfo = &(context->sunxiDevice[i]);
        if(deviceInfo->disp.id == chn){
            return deviceInfo;
        }
    }
    return NULL;
}
int calcPointByPercent(const unsigned char percent, const int middlePoint, const int srcPoint){
    int condition = (srcPoint > middlePoint) ? 1 : 0;
    int length = condition ? (srcPoint - middlePoint) : (middlePoint - srcPoint);
    length = length * percent / 100;
    return condition ? (middlePoint + length) : (middlePoint - length);
}
bool checkScaleFormat(int format){
    switch(format){
    case HAL_PIXEL_FORMAT_YV12:
	case HAL_PIXEL_FORMAT_YCrCb_420_SP:
    case HAL_PIXEL_FORMAT_AW_NV12:
        return 1;
    default:
        return 0;
    }
}
bool reCountPercent(device_info_t *deviceInfo){
    if((deviceInfo->displayType == DISP_OUTPUT_TYPE_HDMI
        && deviceInfo->current3DMode == DISPLAY_2D_ORIGINAL)
        || deviceInfo->displayType == DISP_OUTPUT_TYPE_TV){
        deviceInfo->displayPercentW = deviceInfo->displayPercentWT;
        deviceInfo->displayPercentH = deviceInfo->displayPercentHT;
    }else{
        deviceInfo->displayPercentW = 100;
        deviceInfo->displayPercentH = 100;
    }
    return true;
}
int getStringsFromFile(char const * fileName, char *values, unsigned int num){
	FILE *fp;
	int i = 0;
	if(NULL ==(fp = fopen(fileName, "r"))){
		ALOGW("cannot open this file:%s\n", fileName);
		return -1;
	}
	while(!feof(fp) && (i < num - 1)){
		values[i] = fgetc(fp);
		i++;
	}
	values[i] = '\0';
	fclose(fp);
	return i;
}
#define ARRAYLENGTH 32
int getDispMarginFromFile(unsigned char *percentWidth, unsigned char *percentHeight){
    char valueString[ARRAYLENGTH] = {0};
    char datas[ARRAYLENGTH] = {0};
    int i = 0;
    int j = 0;
    int num = 0;
    int data[4] = {0};

    memset(valueString, 0, 32);
    if(getStringsFromFile(DISPLAY_MARGIN_FILENAME, valueString, ARRAYLENGTH) == -1){
        return -1;
    }
    for(i = 0; valueString[i] != '\0' && 4 > num; i++){
        if('\n' == valueString[i]){
            datas[j] = '\0';
            //ALOGD("datas = %s\n", datas);
            data[num] = (int)strtoul(datas, NULL, 16);
            num++;
            j = 0;
        }else{
            datas[j++] = valueString[i];
        }
    }
    if(2 > num){
        ALOGD("need 2 parameters only. num = %d.", num);
        return -1;
    }
    *percentWidth = (unsigned char)data[0];
    *percentHeight = (unsigned char)data[1];
    return 0;
}
int getDispModeFromFile(int type){
	char valueString[ARRAYLENGTH] = {0};
	char datas[ARRAYLENGTH] = {0};
	int i = 0;
	int j = 0;
	int data = 0;

	memset(valueString, 0, ARRAYLENGTH);
	if(getStringsFromFile(DISPLAY_RSL_FILENAME, valueString, ARRAYLENGTH) == -1){
		return -1;
	}
	for(i = 0; valueString[i] != '\0'; i++){
		if('\n' == valueString[i]){
			datas[j] = '\0';
			//ALOGD("datas = %s\n", datas);
			data = (int)strtoul(datas, NULL, 16);
			if(type == ((data >> 8) & 0xFF)){
				return (data & 0xFF);
			}
			j = 0;
		}
		else{
			datas[j++] = valueString[i];
		}
	}
	return -1;
}


