#ifndef EAGLE_COMPONENT_H
#define EAGLE_COMPONENT_H
//the content of the structure must keep the same as the linux/driver/video/sunxi/disp2/disp/dev_composer.c
#include "sunxi_display2.h"
typedef struct writeBack{
    int                     outAquireFenceFd;
    int                     width;
    int                     wstride;
    int                     height;
    disp_pixel_format       format;
    unsigned int            physAddr;
}writeBack_t;

typedef struct setup_dispc_data{
    int                 layerNum[2];
    disp_layer_config   layerInfo[2][16];

    int*                aquireFenceFd;
    int                 aquireFenceCnt;
    int*                returnFenceFd;
    bool                needWB[2];
    unsigned int        enhanceMode[2];
    unsigned int        androidFrmNum;
    writeBack_t         *writeBackData;
}setup_dispc_data_t;
#endif