

#ifndef HWC_DISPLAY_H
#define HWC_DISPLAY_H

#include <hardware/hardware.h>
#include <hardware/hwcomposer.h>
#include <hardware/hal_public.h>

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

#define INVALID_VALUE -1
#define DISP_CHN_0  0
#define DISP_CHN_1  1
#define DISP_CHN_2  2
#define MARGIN_MIN_PERCENT 50
#define MARGIN_MAX_PERCENT 100

// defined at swextend/utils/libswconfig/swconfig.h
#define SW_JAWS 0x00
#define SW_EAGLE 0x02

#define SUPPORT_DISP_FRAMEWORK_1_0 1
#define SUPPORT_DISP_FRAMEWORK_2_0 0

#define ALIGN(x,a)	(((x) + (a) - 1L) & ~((a) - 1L))

/********************************* start define debug ***************************************/
enum{
	FPS_SHOW = 1,
	LAYER_DUMP = 2,
	SHOW_ALL = 3,
};
/********************************************************************************************/

/************************************ start define Display Device here. *************************8*/

typedef struct disp_channel_info{
	int 				id;
	void				*privateData;	//the display channel feature.
}disp_channel_info_t;

typedef struct device_info{
	disp_channel_info_t disp;
    bool                vsyncEnable;
	unsigned int    	varDisplayWidth;
	unsigned int    	varDisplayHeight;
    unsigned int        displayDPI_X;
    unsigned int        displayDPI_Y;
    unsigned int        displayVsyncP;
    unsigned char       displayPercentWT;
    unsigned char       displayPercentHT;
    unsigned char       displayPercentW;
    unsigned char       displayPercentH;
    int                 displayType;
    int                 displayMode;
    int                 current3DMode;
}device_info_t;

/********************************************************************************************/



/********************************* start define global context*******************************/

typedef struct hwc_context{
	//main display msg
	int 				mainDispId;
	int 				mainDispMode;
	int 				mainDispType;
	int					mainDispWidth;
	int 				mainDispHeight;
	//second display msg
	int 				secDispId;
	int 				secDispMode;
	int 				secDispType;
	int					secDispWidth;
	int					secDispHeight;
	//global operation fd
	hwc_procs_t const* 	psHwcProcs;
	int 				displayFd;
	int 				ionFd;

	//hardware
	void*				hwcPrivate;
	device_info_t*		sunxiDevice;		//the pointer of display devices
	void*               pvPrivateData;		//the private data of the platform.Used in Layer's composition
	int                 displayNum;			//the display channel num of the platform

	//others
	int 				hwcDebug;
	unsigned int        hwcFrameCount;
	bool 				forceGPUComp;
	unsigned int	    uiPrivateDataSize;
	int 				screenRadio;
}hwc_context_t;
/********************************************************************************************/

/* define the device operation */
typedef struct device_opr{
	int (*init)(hwc_context_t *context);
	int (*set3DMode)(int disp, int mode);
	int (*setBacklightMode)(int disp, int enable);
	int (*setEnhanceMode)(int disp, int enable);
	int (*setOutputMode)(int disp, int type, int mode);
	int (*setSaturation)(int disp, int saturation);
	int (*setHue)(int disp, int hue);
	int (*setBright)(int disp, int bright);
	int (*setContrast)(int disp, int contrast);
	int (*setMargin)(int disp, int hpercent, int vpercent);
	int (*isSupportHdmiMode)(int disp, int mode);
	int (*isSupport3DMode)(int disp);
	int (*getOutputType)(int disp);
	int (*getOutputMode)(int disp);
	int (*getSaturation)(int disp);
	int (*getHue)(int disp);
	int (*getBright)(int disp);
	int (*getContrast)(int disp);
	int (*getMarginW)(int disp);
	int (*getMarginH)(int disp);
	int (*vsyncEnable)(int disp, int enable);
}device_opr_t;


/********************************* start define hwc operation********************************/
typedef struct hwc_opr{
	void (*init)();
	void (*set)(hwc_display_contents_1_t** displays, size_t numDisplays,
		int *releasefencefd);
	bool (*isLayerAvailable)(hwc_layer_1_t *layer, int disp);
	bool (*updateFbTarget)(hwc_layer_1_t *layer, int zOrder, int disp);
	bool (*tryToAssignLayer)(hwc_layer_1_t *layer, int zOrder, int disp);
	void (*reset)(int disp);
	void (*postPrepare)(hwc_display_contents_1_t *display);
	void (*beforePrepare)();
}hwc_opr_t;

/********************************************************************************************/


/********************************* start define others **************************************/
typedef struct tv_para{
	int type;
	int mode;
	int width;
	int height;
	int refreshRate;
}tv_para_t;

//mode info
enum{
	WIDTH=2,
	HEIGHT,
	REFRESHRATE,
};

// defind para of manageDisp
enum{
	FIND_HWDISPLAYNUM=0,
	NULL_DISPLAY,
	SET_DISP,
	FREE_DISP,
};


//define about ion operation
#define ION_IOC_SUNXI_PHYS_ADDR     7
typedef struct sunxi_phy_data{
    void *handle;
    unsigned int phys_addr;
    unsigned int size;
}sunxi_phys_data_t;

//define global valiables
extern hwc_context_t gHwcContext;

#if(SW_CHIP_PLATFORM == SW_JAWS)
//A80 operation set.
extern device_opr_t sun9iDeviceOpr;
extern hwc_opr_t sun9iHwcOpr;
#elif(SW_CHIP_PLATFORM == SW_EAGLE)
//H8 operation set.
extern device_opr_t sun8iDeviceOpr;
extern hwc_opr_t sun8iHwcOpr;
#else
#error "please select a platform\n"
#endif
/********************************************************************************************/

/****** define something about composer ***************/
//define the flag of "try to assign"
enum{
	OVERLAY = 0,	//assign success, overlay
	NULL_BUF,		//assign fail, buffer of this layer is null
	CONTIG_MEM,		//assign fail, buffer of this layer is not continues memory
	VIDEO_PD,		//assign fail, video buffer of this layer is protected.
	CANNT_SCALE,	//assign fail, buffer of this format can not be scale.
	SKIP_LAYER,		//assign fail, surfaceflinger tell the hwc to skip this buffer.
	NO_FORMAT,		//assign fail, DE do not support this buffer format.
	BACKGROUND,		//assign fail, this is a background layer.
	TR_N_0,			//assign fail, this layer is rotated.
	ALPHA,			//assign fail, DE do not support blending of this buffer.
	X_FB,			//assign fail, this layer is intersect with FBTarget layer.
	SW_OFTEN,		//assign fail, buffer of this layer is written ofter.(not check video)
	SCALE_OUT,		//assign fail, this scaling is not supported by DE.
	STOP_HWC,		//assign fail, the hwc is stop.
	NO_PIPE,		//assign fail, this pipe is not enough
	NO_FE,
};
enum{
	NO_ASSIGNED,
	ASSIGNED_SUCCESS,
};
/****** end define composer **************************/

typedef enum{
	DISP_OUTPUT_TYPE_NONE   = 0,
	DISP_OUTPUT_TYPE_LCD    = 1,
	DISP_OUTPUT_TYPE_TV     = 2,
	DISP_OUTPUT_TYPE_HDMI   = 4,
	DISP_OUTPUT_TYPE_VGA    = 8,
}disp_output_type;

#endif
