/*
* Copyright (c) 2008-2016 Allwinner Technology Co. Ltd.
* All rights reserved.
*
* File : sbm.h
* Description :
* History :
*   Author  : xyliu <xyliu@allwinnertech.com>
*   Date    : 2016/04/13
*   Comment :
*
*
*/

#ifndef SBM_H
#define SBM_H

#include "sbmInterface.h"
#include "log.h"

#ifdef __cplusplus
extern "C" {
#endif

//SbmInterface* GetSbmInterfaceStream();

SbmInterface* GetSbmInterface(int nType);

#ifdef __cplusplus
}
#endif
#endif

