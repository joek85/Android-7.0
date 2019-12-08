/*
 * Copyright (C) 2007-2014 The Android Open Source Project
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
#ifndef __HWCOMPOSER_PRIV_H__
#define __HWCOMPOSER_PRIV_H__

#include <hardware/hardware.h>
#include <hardware/hwcomposer.h>

#include <hardware/hal_public.h>
#include <hardware/sunxi_metadata_def.h>
#include "sunxi_display2.h"
#include "dev_composer2.h"
//#include <fb.h>

#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <cutils/log.h>
#include <cutils/atomic.h>
#include <sys/socket.h>
#include <linux/netlink.h>
#include <poll.h>
#include <cutils/properties.h>
#include <hardware_legacy/uevent.h>
#include <sys/resource.h>
#include <EGL/egl.h>
#include <linux/ion.h>
#include <ion/ion.h>
#include <sys/ioctl.h>
#include <math.h>
#include <sys/mman.h>
#include <utils/Trace.h>
#include <pthread.h>
#include <edid_parse/parse_edid_sunxi.h>
#include <sync/sync.h>

#define HAL_PIXEL_FORMAT_BGRX_8888  0x1ff
#define ION_IOC_SUNXI_PHYS_ADDR     7
#define ALIGN(x,a)  (((x) + (a) - 1L) & ~((a) - 1L))
#define HW_ALIGN    64
#define YV12_ALIGN 16
#define VE_YV12_ALIGN 16
#define DISPLAY_MAX_LAYER_NUM 16
#define HAS_HDCP 1
//#define SYSRSL_SWITCH_ON_EX

#define NUMLAYEROFCHANNEL 4
#define NUMCHANNELOFVIDEO 1
#define NUMCHANNELOFDSP   4
// tmp open 2 channel
#define PVRSRV_MEM_CONTIGUOUS               (1U<<15)

#define ENHANCE_MODE_PROPERTY "persist.sys.disp_enhance_mode"
#define ENHANCE_BRIGHT_PROPERTY "persist.sys.enhance_bright"
#define ENHANCE_CONTRAST_PROPERTY "persist.sys.enhance_contrast"
#define ENHANCE_DENOISE_PROPERTY "persist.sys.enhance_denoise"
#define ENHANCE_DETAIL_PROPERTY "persist.sys.enhance_detail"
#define ENHANCE_SATURATION_PROPERTY "persist.sys.enhance_saturation"
#define ENHANCE_MODE_ATTR "/sys/class/disp/disp/attr/enhance_mode"
#define ENHANCE_BRIGHT_ATTR "/sys/class/disp/disp/attr/enhance_bright"
#define ENHANCE_CONTRAST_ATTR "/sys/class/disp/disp/attr/enhance_contrast"
#define ENHANCE_DENOISE_ATTR "/sys/class/disp/disp/attr/enhance_denoise"
#define ENHANCE_DETAIL_ATTR "/sys/class/disp/disp/attr/enhance_detail"
#define ENHANCE_EDGE_ATTR "/sys/class/disp/disp/attr/enhance_edge"
#define ENHANCE_SATURATION_ATTR "/sys/class/disp/disp/attr/enhance_saturation"

enum {
    HW_CHANNEL_PRIMARY = 0,
    HW_CHANNEL_EXTERNAL,
    NUMBEROFDISPLAY,
    F_HWDISP_ID,
    EVALID_HWDISP_ID,
};

typedef enum {
    ASSIGN_INIT = 0,
    ASSIGN_GPU = 1,
    ASSIGN_OVERLAY = 2,
    ASSIGN_NEEDREASSIGNED = 3,
    ASSIGN_FAILED = 4,
    ASSIGN_CURSOR_OVERLAY = 5
} HwcAssignStatus;


typedef enum {
#define AssignDUETO(x) x,
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
    AssignDUETO(D_STOP_HWC)
    AssignDUETO(D_NO_PIPE)
    AssignDUETO(D_MEM_LIMIT)
#undef AssignDUETO
    DEFAULT,

} AssignDUETO_T;

enum {
    FPS_SHOW = 1,
    LAYER_DUMP = 2,
    VSYNC_TRIM_DEBUG = 4,
    SHOW_ALL = 7
};

enum {
    DISP_NONE_POLICY = 0,
    DISP_SINGLE_POLICY,
    DISP_DUAL_POLICY,
    DISP_ADAPT_POLICY,
    DISP_POLICY_NUM,
};

enum {
    TV_CAPABILITY_SDR   = 0x00000001,
    TV_CAPABILITY_WCG   = 0x00000002,
    TV_CAPABILITY_HDR   = 0x00000004,
    TV_CAPABILITY_OTHER = 0x00000008,
};

enum {
	HW_INPUT_SDR = 0,
	HW_INPUT_WCG,
	HW_INPUT_HDR,
	HW_INPUT_OTHER,
};

enum {
	IOMEM_TYPE_IOMMU = 0xAF10,
	IOMEM_TYPE_CMA = 0xFA01,
};

typedef struct {
    int speed; // ddr freq
    int limit;
}mem_speed_limit_t;

typedef struct bandwidth_record {
    int max;
    int available;
    int video;
    int fb;
    int total;
}bandwidth_t;

typedef struct {

    int                     outaquirefencefd;
    int                     width;
    int                     Wstride;
    int                     height;
    disp_pixel_format       format;
    unsigned int            phys_addr;

}WriteBack_t;

typedef struct ion_list {
    int                 has_ref;
    unsigned int        frame_index;
    int                 fd;
    ion_user_handle_t   handle;
    struct ion_list     *prev;
    struct ion_list     *next;
}ion_list_t;

#if (DE_VERSION == 30)
#define COMMIT_LAYER_CMD DISP_LAYER_SET_CONFIG2
typedef struct disp_layer_config2 disp_layer_t;
typedef struct disp_layer_info2 disp_layer_info_t;
#else
#define COMMIT_LAYER_CMD DISP_LAYER_SET_CONFIG
typedef struct disp_layer_config disp_layer_t;
typedef struct disp_layer_info disp_layer_info_t;
#endif

typedef struct {
    int               deviceid;
    int               layer_num;
    disp_layer_t      *hwclayer;
    int               *fencefd;
    bool              needWB;
    unsigned int      ehancemode;
    unsigned int      androidfrmnum;
    WriteBack_t       *WriteBackdata;
} setup_dispc_data_t;

typedef struct layer_info {
    HwcAssignStatus assigned;
    signed char     hwchannel;
    signed char     virchannel;
    signed char     HwzOrder;
    signed char     OrigOrder;
    bool            is3D;
    bool            is_dim_layer;
    AssignDUETO_T   info;
    hwc_layer_1_t   *psLayer;
    int             bandwidth;
}layer_info_t;

typedef struct ChannelInfo {
    bool            hasBlend;
    bool            hasVideo;
    bool            isFB;
    float           WTScaleFactor;
    float           HTScaleFactor;
    int             iCHFormat;
    unsigned char   planeAlpha;
    int             HwLayerCnt;//0~n ,0 is the first,  current will used
    layer_info_t   *HwLayer[4];
} ChannelInfo_t;

typedef struct {
    int                 VirtualToHWDisplay;
    bool                setblank;
    bool                VsyncEnable; //fixme: should check after exchangeDispChannel
    bool                issecure;
    unsigned int        ehancemode;

    int                 HwChannelNum;
    int                 LayerNumofCH;
    int                 VideoCHNum;

    int                 InitDisplayWidth;
    int                 InitDisplayHeight;
    int                 VarDisplayWidth;
    int                 VarDisplayHeight;
    uint64_t            mytimestamp;
    unsigned int        DiplayDPI_X;
    unsigned int        DiplayDPI_Y;
    unsigned int        DisplayVsyncP;
    int                 DisplayFps;
    unsigned int        power_mode;

    unsigned char       SetPersentWidth;
    unsigned char       SetPersentHeight;
    unsigned char       PersentWidth;
    unsigned char       PersentHeight;

    int                 DisplayType;
    int                 DisplayMode;
    __display_3d_mode   Current3DMode;
    __display_3d_mode   set3DMode;
    int                 videoCropHeightFor3D; /* be used only for 3D mode */
    int                 setDispMode; //for saving displayMode when 3d display
    int                 output_format; /* output format(such as yuv420) */
    int                 output_bits; /* output bits, such as 8bit */
    int                 setOutputformat; /* for get output pixel format */

    long long           fps;
    long long           de_clk;
    int                 screenRadio;
    bandwidth_t         bandwidth;

    int                 dataspace_mode; /* dataspace mode of televition, for setting */
	int                 actual_dataspce_mode;
	int                 hw_input;
	unsigned int        tv_capabilities;
	int                 enhance_mode;

#ifdef HWC_WRITEBACK_ENABLE
    int                 opened_for_wb;
#endif
}DisplayInfo;

typedef struct {
    bool                UsedFB;
    unsigned char       FBhasVideo;
    signed char         HwCHUsedCnt;//0~n ,0 is the first,  current is used
    signed char         VideoCHCnt;//0~n, 0 is the first,  current is used
    unsigned char       HaveVideo;
    unsigned char       haveAlpha;
    float               WidthScaleFactor;
    float               HighetScaleFactor;

    layer_info         *psAllLayer;
    unsigned int allocNumLayer;
    int                 numberofLayer;
    const DisplayInfo  *psDisplayInfo;
    ChannelInfo_t       ChannelInfo[4];  //zOrder 0~3

}HwcDisContext_t;

typedef struct {
    hwc_procs_t const*  psHwcProcs;
    pthread_t           sVsyncThread;
    int                 DisplayFd;
    int                 FBFd;
    int                 IonFd;
    int                 dvfsfd;

    unsigned int        HWCFramecount;

    bool                CanForceGPUCom;
    bool                ForceGPUComp;
    bool                DetectError;
    char                hwcdebug;

    unsigned int        uiBeginFrame;
    double              fBeginTime;
    int                 exchangeDispChannel;
    unsigned int        blankHwdisp; // msb is 1 if need close, the rest bits is hwDisp
    pthread_mutex_t     lock; //for exchangeDispChannel

    int                 NumberofDisp;
    DisplayInfo         *SunxiDisplay;
    HwcDisContext_t     *DisContext;
    setup_dispc_data_t  PrivateData[2];
    ion_list            *IonHandleHead;
    ion_list            *ionCurValidNode; // point the that has not ref of buf
    int                 ionListCurNum;
    int                 ionListTotalNum;

    unsigned int        iomem_type;

    int                 ban4k;
    int                 dispPolicy;
    int                 InitDisplaySize[2][2];

#ifdef HWC_WRITEBACK_ENABLE
	struct hwc_capture_context *capture;
#endif
}SUNXI_hwcdev_context_t;

typedef struct {
    int             type;// bit3:cvbs, bit2:ypbpr, bit1:vga, bit0:hdmi
    int             mode;
    int             width;
    int             height;
    int             refreshRate;
    int             support;
}tv_para_t;

typedef enum {
    WIDTH=2,
    HEIGHT,
    REFRESHRAE,

}MODEINFO;

typedef enum {
    FIND_HWDISPNUM=0,
    FIND_HWTYPE,
    NULL_DISPLAY,
    SET_DISP,
    FREE_DISP,

}ManageDisp;

typedef struct {
    ion_user_handle_t handle;
    unsigned int phys_addr;
    unsigned int size;
}sunxi_phys_data;

typedef struct {
    long    start;
    long    end;
}sunxi_cache_range;

#define ION_IOC_SUNXI_FLUSH_RANGE  5

enum {
	HWC_MIN_VSYNC_SAMPLES_REQ = 6,
	HWC_MAX_VSYNC_SAMPLES = 8,
	HWC_MIN_TS_ABS_DIF = 100000, // 0.1 ms
};

enum {
	HWC_VSYNC_ENABLE_CHANGED = 0x00000001,
	HWC_VSYNC_PERIOD_CHANGED = 0x00000002,
};

typedef struct {
	uint64_t period_base;
	uint64_t period_averge;
	unsigned int sample_num;
	unsigned int head_id;// the id of oldest one
	unsigned int tail_id; // the id of new one
	uint64_t ts_slot[HWC_MAX_VSYNC_SAMPLES];
	nsecs_t period_delta[HWC_MAX_VSYNC_SAMPLES];
	unsigned int hwDisp;
	unsigned int reset_flag;
	unsigned int count;
} hwc_vsync_modle_t;

extern SUNXI_hwcdev_context_t gSunxiHwcDevice;

extern int hwcdev_reset_device(SUNXI_hwcdev_context_t *psDevice, size_t disp);
extern HwcAssignStatus hwc_try_assign_layer(HwcDisContext_t *ctx, size_t  singcout,int zOrder);
extern SUNXI_hwcdev_context_t* hwc_create_device(void);
extern disp_tv_mode get_suitable_hdmi_mode(int i);
extern void *VsyncThreadWrapper(void *priv);
extern int hwc_setup_layer(HwcDisContext_t *ctx);
extern int hwc_reset_disp(SUNXI_hwcdev_context_t *ctx);
extern int _hwc_device_set_3d_mode(int disp,
    __display_3d_mode mode, int videoCropHeight);
extern int _hwc_device_set_backlight(int disp, int on_off,bool half);
extern int hwc_region_intersect(hwc_rect_t *rect0, hwc_rect_t *rect1);
extern int hwc_destroy_device(void);
SUNXI_hwcdev_context_t* hwc_create_device(void);
extern int get_info_mode(int mode,MODEINFO info);
extern int getVgaInfo(int mode,MODEINFO info);
int getMemLimit();
int getValueFromProperty(char const* propName);
int isDisplayP2P(void);
int getTvMode4SysResolution(void);
int getDispPolicy(void);
int getDispModeFromFile(int type);
int saveDispModeToFile(int type, int mode);
int readStringFromAttrFile(char const *fileName,
    char *values, const int len);
int writeStringToAttrFile(char const *fileName,
    char const *values, const int len);
int getDispMarginFromFile(int type, unsigned char * percentWidth,unsigned char * percentHeight);
int saveDispMarginToFile(int type, unsigned char percentWidth,unsigned char percentHeight);
int saveDataSpaceMode(unsigned int hwDisp, int mode);
int saveDispDeviceConfig(unsigned int hwDisp, struct disp_device_config *config);
int getSavedDataSpaceMode(unsigned int hwDisp);
int saveOutputPixelFormat(unsigned int hwDisp, int mode);
int getSavedOutputPixelFormat(unsigned int hwDisp);
unsigned int isHdmiHpd(int disp);
unsigned int isCvbsHpd(int disp);
extern unsigned int get_ion_addr(int fd);
extern int blank_disp(unsigned int hwDisp);
extern int hwcOutputSwitch(DisplayInfo * psDisplayInfo,int type,int mode);
int setDispDeviceConfigs(int hwDisp, void *configs);
int getDispDeviceConfigs(int hwDisp, void *configs);
extern int hwcOutputExchange();
int hwcCleanHwDisp(unsigned int hwDisp);
extern int getHwDispByType(int type);
int get_de_freq_and_fps(DisplayInfo *psDisplayInfo);
extern int resetDispMode(int disp, int type,int mode);
extern int check4KBan(void);
unsigned int getIomenType(unsigned int defval);
int _hwc_device_set_output_mode(int disp, int out_type, int out_mode);
int IonHandleDecRef(char dec_all);
bool sunxi_prepare(hwc_display_contents_1_t **displays ,size_t NumofDisp);
bool sunxi_set(hwc_display_contents_1_t** displays, size_t numDisplays,
    int *releasefencefd, int *retirefencefd);
int is_skip_gpu_buf(buffer_handle_t handle);
int initDisplayDevice(int disp,int hwDisp,disp_output * dispOutput);
void checkinTvConfig(DisplayInfo *psDisplayInfo);

int _hwc_device_set_margin(int disp,int hpercent,int vpercent);
int _hwc_set_persent(int disp,int para0, int para1);
int _hwc_device_is_support_hdmi_mode(int disp,int mode);
int _hwc_device_get_output_type(int disp);
int _hwc_device_get_output_mode(int disp);
int _hwc_device_set_screenradio(int disp, int screen_radio);
int _hwc_device_set_dataspace_mode(int disp, int mode);
int _hwc_device_get_dataspace_mode(int disp);
int _hwc_device_get_cur_dataspace_mode(int disp);
int _hwc_device_set_output_format(int disp, int format);
int _hwc_device_get_output_format(int disp);

int is3DMode(__display_3d_mode mode);

void hwc_set_vsync_need_reset(unsigned int flag);

#ifdef HWC_WRITEBACK_ENABLE
#include <libcapture/hwc_capture.h>

int hwc_writeback_init(SUNXI_hwcdev_context_t *context);
int hwc_writeback_prepare(const SUNXI_hwcdev_context_t *context,
		hwc_display_contents_1_t **contents, size_t dispcnt,
		hwc_display_contents_1_t **fixup_contents, size_t *fixup_dispcnt);
int hwc_writeback_buffer_setup(const SUNXI_hwcdev_context_t *context, setup_dispc_data_t *out);
int hwc_writeback_buffer_clear(SUNXI_hwcdev_context_t *context);
int hwc_writeback_is_enable(void);
size_t hwc_writeback_get_display_id(void);
#endif

int checkEnhanceMode(DisplayInfo *psDisplayInfo);
int _hwc_device_set_enhance_mode(int disp, int mode);
int _hwc_device_get_enhance_mode(int disp);
int _hwc_device_set_enhance_bright(int disp, int value);
int _hwc_device_get_enhance_bright(int disp);
int _hwc_device_set_enhance_contrast(int disp, int value);
int _hwc_device_get_enhance_contrast(int disp);
int _hwc_device_set_enhance_saturation(int disp, int value);
int _hwc_device_get_enhance_saturation(int disp);
int _hwc_device_set_enhance_denoise(int disp, int value);
int _hwc_device_get_enhance_denoise(int disp);
int _hwc_device_set_enhance_detail(int disp, int value);
int _hwc_device_get_enhance_detail(int disp);
int _hwc_device_set_enhance_edge(int disp, int value);
int _hwc_device_get_enhance_edge(int disp);

#endif
