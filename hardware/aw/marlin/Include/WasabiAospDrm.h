/*****************************************************************
|
|   Wasabi - Wasabi AOSP DRM functions
|
|   $Id: WasabiAospDrm.h 10528 2014-03-14 22:13:18Z ehodzic $
|   Original author: Edin Hodzic (dino@concisoft.com)
|
|   This software is provided to you pursuant to your agreement 
|   with Intertrust Technologies Corporation ("Intertrust").
|   This software may be used only in accordance with the terms 
|   of the agreement.
|
|   Copyright (c) 2014 by Intertrust. All rights reserved. 
|
****************************************************************/

#ifndef _WASABI_AOSP_DRM_H_
#define _WASABI_AOSP_DRM_H_

/*----------------------------------------------------------------------
|   includes
+---------------------------------------------------------------------*/
#include <binder/Parcel.h>
#include <utils/Errors.h>

/*----------------------------------------------------------------------
|   namespace
+---------------------------------------------------------------------*/
namespace android {

/*----------------------------------------------------------------------
|   WSB_Drm_Function
+---------------------------------------------------------------------*/
// Keep WSB_DRM_FUN_* values in sync with Drm.java
enum WSB_Drm_Function {
    WSB_DRM_FUN_PROCESS_TOKEN = 1,
    WSB_DRM_FUN_CHECK_LICENSE = 2,
    WSB_DRM_FUN_IS_PERSONALIZED = 3,
    WSB_DRM_FUN_PERSONALIZE = 4
};

/*----------------------------------------------------------------------
|   functions
+---------------------------------------------------------------------*/
status_t WSB_Drm_Invoke(const Parcel &request, Parcel *reply);

}; // namespace

#endif // _WASABI_AOSP_DRM_H_
