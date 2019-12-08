#ifndef JAWS_PRIVATE_H
#define JAWS_PRIVATE_H
#include "drv_display.h"
#define HDMI_USED 1
#define CVBS_USED 0
#define LCD_USED  0
#define EDP_USED  2
#define VGA_USED  0
#define NUMBEROFDISPLAY 3
#define HAL_PIXEL_FORMAT_AW_NV12    0x101
#define HAL_PIXEL_FORMAT_BGRX_8888  0x1ff

#define ION_IOC_SUNXI_PHYS_ADDR     7
#define HW_ALIGN	32
#define YV12_ALIGN 16
// PHY_OFFSET is 0x20000000 at A80 platform
#define PHY_OFFSET 0x20000000

static float mFeClk = 297000000;
static unsigned int mDeFps = 60;
static int mG2dFd;

//define the struct of kernel composer, keep this as same with linux3.4/drivers/video/sunxi/disp/dev_composer.c
typedef struct setup_dispc_data{
	int layer_num[3];
	disp_layer_info layer_info[3][4];
	void* hConfigData;
}setup_dispc_data_t;

//define the struct of composer module
#define LAYER_NUM 4
#define PIPE_NUM 2
#define CHANNEL_NUM 3
#define UI_FE_NUM 1
#define VE_FE_NUM 1		//define the Fe number which can be used by video
#define FE_NUM 2		//define the total Fe number
typedef struct layer{
	int zOrder;
	int pipeId;
	int assigned;     //value: NO_ASSIGNED, ASSIGNED_OVERLAY, ASSIGNED_GPU, ASSIGNED_FBTARGET0
	hwc_layer_1_t* hwc_layer;
	int flag;  //value: define in hwc.h
}layer_t;

typedef struct disp_channel{
	layer_t layers[LAYER_NUM];
	int pipeUsed;   //define the pipe number which has been used.
	int uiFeUsed;	//define the Fe numbber which has been used by ui;
	int veFeUsed;
	int allFeUsed;
}disp_channel_t;
typedef struct de_module{
	disp_channel_t dispChn[CHANNEL_NUM];
}de_module_t;

//private data of hwc
typedef struct sun9i_hwc_private{
	int 				g2dFd;
	float				fe_clk;
	unsigned int		de_fps;
}sun9i_hwc_private_t;

#endif
