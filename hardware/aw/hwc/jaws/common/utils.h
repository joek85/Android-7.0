#ifndef HWC_UTILS_H
#define HWC_UTILS_H
#include "../hwc.h"
#include <hardware/hwcomposer.h>
#define DISP_P2P_PROPERTY "persist.hwc.p2p"
#define SYSTEM_RSL_PROPERTY "ro.hwc.sysrsl"
#define DISPLAY_RSL_FILENAME "/mnt/Reserve0/disp_rsl.fex"
#define DISPLAY_MARGIN_FILENAME "/mnt/Reserve0/disp_margin.fex"
#if(BUSINESS_PLATFORM == PLATFORM_CMCCWASU)
	#define MARGIN_DEFAULT_PERCENT_WIDTH 100
	#define MARGIN_DEFAULT_PERCENT_HEIGHT 100
#else
	#define MARGIN_DEFAULT_PERCENT_WIDTH 100
	#define MARGIN_DEFAULT_PERCENT_HEIGHT 100
#endif

int is3DMode(int mode);
int getInfoOfMode(const tv_para_t *tvParaList, int number, int mode, int info);
device_info_t* hwcFindDevice(int hwDispChn);
int getHwDispId(int disp);
void setAllDeviceMargin(int hpercent, int vpercent);
int deviceGetOutputType(int disp);
int deviceGetOutputMode(int disp);
int deviceGetMarginW();
int deviceGetMarginH();
int getValueFromProperty(char const* propName);
int isDisplayP2P();
void invalidate(int times);
void *vsyncThreadWrapper(void *priv);
void calculateFactor(device_info_t *psDeviceInfo, float *xWidthFactor, float *xHighetfactor);
bool hwcIsBlended(hwc_layer_1_t *psLayer);
bool hwcIsPremult(hwc_layer_1_t *psLayer);
bool hwcIsPlaneAlpha(hwc_layer_1_t *psLayer);
bool checkVideoFormat(int format);
int hwcRegionIntersect(hwc_rect_t *rect0, hwc_rect_t *rect1);
void resetLayerType(hwc_display_contents_1_t* displays);
void calculateFactor(device_info_t *psDeviceInfo, float *xWidthFactor, float *xHightfactor);
bool isLayerProtected(int usage);
bool isLayerUsageSW(int usage);
bool isLayerBlended(hwc_layer_1_t* ly);
bool isLayerPremult(hwc_layer_1_t* ly);
bool isLayerScaled(device_info_t* deviceInfo, hwc_layer_1_t* ly);

//about rect operation
//return true if two region is intersected.
bool twoRegionIntersect(hwc_rect_t *rect0, hwc_rect_t *rect1);
//return true if the first region(rect0) is in rect1
bool inRegion(hwc_rect_t *rect0, hwc_rect_t* rect1);
//merge the first region to the second region
int regionMerge(hwc_rect_t *rectFrom, hwc_rect_t* rectTo, int boundWidth, int boundHeight);

bool isValidVideoFormat(int format);
int ionGetAddr(int shareFd);
device_info_t* getDeviceInfoByChn(int chn);
int calcPointByPercent(const unsigned char percent, const int middlePoint, const int srcPoint);
bool checkScaleFormat(int format);
bool reCountPercent(device_info_t *deviceInfo);
int getStringsFromFile(char const * fileName, char *values, unsigned int num);
int getDispMarginFromFile(unsigned char *percentWidth, unsigned char *percentHeight);
int getDispModeFromFile(int type);

#endif
