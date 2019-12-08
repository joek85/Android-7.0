#ifndef __HWCOMPOSER_PRIV_H__
#define __HWCOMPOSER_PRIV_H__

#include <hardware/hardware.h>
#include <hardware/hwcomposer.h>

#include <hardware/hal_public.h>
#include "sunxi_display2.h"
#include <fb.h>

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

#define HAL_PIXEL_FORMAT_BGRX_8888  0x1ff
#define ION_IOC_SUNXI_PHYS_ADDR     7
#define ALIGN(x,a)	(((x) + (a) - 1L) & ~((a) - 1L))
#define HW_ALIGN	32
#define YV12_ALIGN 16
#define DISPLAY_MAX_LAYER_NUM 16
#define NUMBEROFDISPLAY 2
#define HOLD_BUFS_UNTIL_NOT_DISPLAYED
#define HAS_HDCP 1
//#define HWC_1_3
//#define WB

#define NUMLAYEROFCHANNEL 4
#define NUMCHANNELOFVIDEO 1
#define NUMCHANNELOFDSP   4
// tmp open 2 channel
#define PVRSRV_MEM_CONTIGUOUS				(1U<<15)

#define HW_CHANNEL_PRIMARY 0
#define HW_CHANNEL_EXTERNAL 1
#define HDMI_USED HW_CHANNEL_PRIMARY
#define CVBS_USED HW_CHANNEL_EXTERNAL

typedef enum
{
    ASSIGN_INIT = 0,
    ASSIGN_GPU = 1,
	ASSIGN_OVERLAY = 2,

	ASSIGN_NEEDREASSIGNED = 3,
    ASSIGN_FAILED = 4
} HwcAssignStatus;


typedef enum
{
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
#undef AssignDUETO
    DEFAULT,

} AssignDUETO_T;

enum
{
	FPS_SHOW = 1,
	LAYER_DUMP = 2,
	SHOW_ALL = 3
};

enum {
    DISP_NONE_POLICY = 0,
    DISP_SINGLE_POLICY,
    DISP_DUAL_POLICY,
    DISP_ADAPT_POLICY,
    DISP_POLICY_NUM,
};

typedef struct {

    int                     outaquirefencefd;
    int                     width;
    int                     Wstride;
    int                     height;
    disp_pixel_format       format;
    unsigned int            phys_addr;

}WriteBack_t;

typedef struct ion_list {
    unsigned int        frame_index;
    int                 fd;
    struct ion_handle   *handle;
    struct ion_list     *prev;
    struct ion_list     *next;
}ion_list_t;



typedef struct
{
    int                 layer_num[2];
    disp_layer_config   layer_info[2][16];

    int*                aquireFenceFd;
    int                 aquireFenceCnt;
    int*                returnfenceFd;
    bool                needWB[2];
    unsigned int        ehancemode[2];
    unsigned int        androidfrmnum;
    WriteBack_t         *WriteBackdata;
}setup_dispc_data_t;

typedef struct layer_info {
    HwcAssignStatus assigned;
    signed char     hwchannel;
    signed char     virchannel;
    signed char     HwzOrder;
    signed char     OrigOrder;
    bool            is3D;
    AssignDUETO_T   info;
    hwc_layer_1_t   *psLayer;
}layer_info_t;

typedef struct ChannelInfo{
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

typedef struct{
    int                 VirtualToHWDisplay;
    bool                setblank;
    bool                VsyncEnable; //fixme: should check after exchangeDispChannel
    bool                issecure;
    unsigned int        ehancemode;

    int                 HwChannelNum;
    int                 LayerNumofCH;
    int                 VideoCHNum;

    int                 InitDisplayWidth;
    int    	            InitDisplayHeight;
	int    	            VarDisplayWidth;
	int    	            VarDisplayHeight;
    uint64_t            mytimestamp;
    unsigned int        DiplayDPI_X;
    unsigned int        DiplayDPI_Y;
    unsigned int        DisplayVsyncP;
    int                 DisplayFps;

    unsigned char       SetPersentWidth;
    unsigned char       SetPersentHeight;
    unsigned char       PersentWidth;
    unsigned char       PersentHeight;

    int                 DisplayType;
    int                 DisplayMode;
    __display_3d_mode   Current3DMode;
    int                 setDispMode; //for saving displayMode when 3d display

    long long           de_clk;
    int                 screenRadio;
}DisplayInfo;

typedef struct{
    bool                UsedFB;
    unsigned char       FBhasVideo;
    signed char         HwCHUsedCnt;//0~n ,0 is the first,  current is used
    signed char         VideoCHCnt;//0~n, 0 is the first,  current is used
    unsigned char       HaveVideo;
    unsigned char       haveAlpha;
    float               WidthScaleFactor;
    float               HighetScaleFactor;

    layer_info         *psAllLayer;
    int                 numberofLayer;
    const DisplayInfo  *psDisplayInfo;
    ChannelInfo_t       ChannelInfo[4];  //zOrder 0~3

}HwcDisContext_t;

typedef struct
{
    /*
    usually:  display 1: LCD
              display 2:HDMI   fixed
              We assume that all display could hot_plug,but there is only one PrimaryDisplay,0 is the PrimaryDisplay.
    */
	hwc_procs_t	const*  psHwcProcs;
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

	unsigned int		uiBeginFrame;
	double				fBeginTime;
    int                 exchangeDispChannel;
    pthread_mutex_t     lock; //for exchangeDispChannel

    int                 NumberofDisp;
    DisplayInfo         *SunxiDisplay;
    HwcDisContext_t     *DisContext;
    setup_dispc_data_t* pvPrivateData;
    ion_list            *IonHandleHead;

    int                 ban4k;
    int                 dispPolicy;
}SUNXI_hwcdev_context_t;

typedef struct
{
    int             type;// bit3:cvbs, bit2:ypbpr, bit1:vga, bit0:hdmi
    disp_tv_mode    mode;
    int             width;
    int             height;
	int             refreshRate;
    char            support;
}tv_para_t;

typedef enum
{
    WIDTH=2,
    HEIGHT,
    REFRESHRAE,

}MODEINFO;

typedef
 enum{
    FIND_HWDISPNUM=0,
    FIND_HWTYPE,
    NULL_DISPLAY,
    SET_DISP,
    FREE_DISP,

}ManageDisp;


typedef struct {
    void *handle;
    unsigned int phys_addr;
    unsigned int size;
}sunxi_phys_data;

typedef struct {
	long 	start;
	long 	end;
}sunxi_cache_range;

#define ION_IOC_SUNXI_FLUSH_RANGE  5


extern SUNXI_hwcdev_context_t gSunxiHwcDevice;

extern int hwcdev_reset_device(SUNXI_hwcdev_context_t *psDevice, size_t disp);
extern HwcAssignStatus hwc_try_assign_layer(HwcDisContext_t *ctx, size_t  singcout,int zOrder);
extern SUNXI_hwcdev_context_t* hwc_create_device(void);
extern int  _hwcdev_layer_config_3d(const DisplayInfo  *PsDisplayInfo, disp_layer_info *layer_info);
extern disp_tv_mode get_suitable_hdmi_mode(int i);
extern void *VsyncThreadWrapper(void *priv);
extern int hwc_setup_layer(HwcDisContext_t *ctx);
extern int hwc_reset_disp(SUNXI_hwcdev_context_t *ctx);
extern int _hwc_device_set_3d_mode(int disp, __display_3d_mode mode);
extern int _hwc_device_set_backlight(int disp, int on_off,bool half);
extern int _hwc_device_set_enhancemode(int disp, bool on_off,bool half);
extern int hwc_region_intersect(hwc_rect_t *rect0, hwc_rect_t *rect1);
extern int hwc_destroy_device(void);
SUNXI_hwcdev_context_t* hwc_create_device(void);
extern int get_info_mode(int mode,MODEINFO info);
int getValueFromProperty(char const* propName);
int isDisplayP2P(void);
int getTvMode4SysResolution(void);
int getDispPolicy(void);
int getDispModeFromFile(int type);
int getDispMarginFromFile(unsigned char * percentWidth,unsigned char * percentHeight);
unsigned int isHdmiHpd(int disp);
disp_tv_mode checkout_mode(int select,bool reset);
extern unsigned int get_ion_addr(int fd);
extern int hwc_hotplug_switch(int DisplayNum, bool plug);
extern int hwcOutputSwitch(DisplayInfo * psDisplayInfo,int type,int mode);
extern int hwcOutputExchange();
extern int getHwDispByType(int type);
extern int check4KBan(void);
extern DisplayInfo* hwc_manage_display(DisplayInfo *HWDisplayInfo[], int DispInfo,ManageDisp mode);
int _hwc_device_set_output_mode(int disp, int out_type, int out_mode);
int IonHandleDecRef(void);
bool sunxi_prepare(hwc_display_contents_1_t **displays ,size_t NumofDisp);
bool sunxi_set(hwc_display_contents_1_t** displays, size_t numDisplays, int *releasefencefd, int *retirefencefd);
int initDisplayDevice(int disp,int hwDisp,disp_output * dispOutput);

int aw_get_hdmi_setting(int *HdmiMode);
int _hwc_device_set_margin(int disp,int hpercent,int vpercent);
int  _hwc_set_persent(int disp,int para0, int para1);
int _hwc_device_is_support_hdmi_mode(int disp,int mode);
int _hwc_device_get_output_type(int disp);
int _hwc_device_get_output_mode(int disp);
int _hwc_device_set_screenradio(int disp, int screen_radio);
int is3DMode(__display_3d_mode mode);

#endif
