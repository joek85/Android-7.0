#ifndef EAGLE_PRIVATE_H
#define EAGLE_PRIVATE_H

#include "sunxi_display2.h"
#include "hwc.h"
//ion list ,use for CMA
typedef struct ion_list {
    unsigned int        frameIndex;
    int                 fd;
    struct ion_handle   *handle;
    struct ion_list     *prev;
    struct ion_list     *next;
}ion_list_t;


//private data of display channel
typedef struct sun8i_disp_private{
	bool isSecure;
	unsigned int enhanceMode;
	int hwChannelNum;
	int layerNumOfCh;
	int videoChNum;
	uint64_t myTimeStamp;
}sun8i_disp_private_t;

//private data of hwc context
typedef struct sun8i_hwc_private{
	ion_list *ionHandleHead;
}sun8i_hwc_private_t;

//others
#define NUMLAYEROFCHANNEL 4
enum{
	ASSIGN_INIT = 0,
	ASSIGN_GPU,
	ASSIGN_OVERLAY,
	ASSIGN_NEEDREASSIGNED,
	ASSIGN_FAILED
};

enum{
    I_OVERLAY = 0,
    D_NULL_BUF,
    D_CONTIG_MEM,
	D_VIDEO_PD,
	D_CANNT_SCALE,
	D_SKIP_LAYER,
    D_NO_FORMAT,
    D_BACKGROUND,
    D_TR_N_0,
    D_ALPHA,
    D_X_FB,
    D_SW_OFTEN,
    D_SCALE_OUT,
    D_NO_PIPE,
    DEFAULT,
};

typedef struct layer_info {
    int             assigned;
    signed char     hwChannel;
    signed char     virChannel;
    signed char     hwzOrder;
    signed char     origOrder;
    bool            is3D;
    int             assignInfo;
    hwc_layer_1_t   *psLayer;
}layer_info_t;

typedef struct channel_info{
    bool            hasBlend;
    bool            hasVideo;
    bool            isFB;
    float           wtScaleFactor;
    float           htScaleFactor;
    int             iChnFormat;
    unsigned char   planeAlpha;
    int             hwLayerCnt;//0~n ,0 is the first,  current will used
    layer_info_t   *hwLayer[4];
} channel_info_t;

typedef struct hwc_disp_context{
    bool                usedFB;
    unsigned char       fbHasVideo;
    unsigned char       hwChnUsedCnt;//0~n ,0 is the first,  current is used
    unsigned char       videoChnCnt;//0~n, 0 is the first,  current is used
    unsigned char       haveVideo;
    float               widthScaleFactor;
    float               hightScaleFactor;

    layer_info         *psAllLayer;
    int                 numberOfLayer;
    const device_info_t *psDeviceInfo;
    channel_info_t      channelInfo[4];  //zOrder 0~3

}hwc_disp_context_t;

#define NUMLAYEROFCHANNEL 4
#define NUMCHANNELOFVIDEO 1
#define NUMCHANNELOFDSP   4
#define HW_ALIGN	32
#define YV12_ALIGN 32
#define PVRSRV_MEM_CONTIGUOUS				(1U<<15)

static tv_para_t sun8iTvPara[]={
    {8, DISP_TV_MOD_NTSC,             720,    480, 60},
    {8, DISP_TV_MOD_PAL,              720,    576, 60},
    {5, DISP_TV_MOD_480I,             720,    480, 60},
    {5, DISP_TV_MOD_576I,             720,    576, 60},
    {5, DISP_TV_MOD_480P,             720,    480, 60},
    {5, DISP_TV_MOD_576P,             720,    576, 60},
    {5, DISP_TV_MOD_720P_50HZ,        1280,   720, 50},
    {5, DISP_TV_MOD_720P_60HZ,        1280,   720, 60},
    {1, DISP_TV_MOD_1080P_24HZ,       1920,   1080, 24},
    {5, DISP_TV_MOD_1080P_50HZ,       1920,   1080, 50},
    {5, DISP_TV_MOD_1080P_60HZ,       1920,   1080, 60},
    {5, DISP_TV_MOD_1080I_50HZ,       1920,   1080, 50},
    {5, DISP_TV_MOD_1080I_60HZ,       1920,   1080, 60},
    {1, DISP_TV_MOD_1080P_24HZ_3D_FP, 1920,   1080, 24},
    {1, DISP_TV_MOD_720P_50HZ_3D_FP,  1280,   720, 50},
    {1, DISP_TV_MOD_720P_60HZ_3D_FP,  1280,   720, 60},
    {1, DISP_TV_MODE_NUM,  0,   0, 0},
};
#endif
