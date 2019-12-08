/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#ifndef __DRMNDCERTCONSTANTS_H__
#define __DRMNDCERTCONSTANTS_H__

#include <drmtypes.h>

ENTER_PK_NAMESPACE;

    extern DRM_GLOBAL_CONST  DRM_CONST_STRING       g_dstrTagModulus;
    extern DRM_GLOBAL_CONST  DRM_CONST_STRING       g_dstrTagExponent;
    extern DRM_GLOBAL_CONST  DRM_CONST_STRING       g_dstrTagPrime0;
    extern DRM_GLOBAL_CONST  DRM_CONST_STRING       g_dstrTagPrime1;
    extern DRM_GLOBAL_CONST  DRM_CONST_STRING       g_dstrTagCRTExponent0;
    extern DRM_GLOBAL_CONST  DRM_CONST_STRING       g_dstrTagCRTExponent1;
    extern DRM_GLOBAL_CONST  DRM_CONST_STRING       g_dstrTagIQMP;
    extern DRM_GLOBAL_CONST  DRM_CONST_STRING       g_dstrWMDRMCertExponent;

    extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING  g_dastrTagUnsignedTemplate;
    extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING  g_dastrSecurityVersion;

EXIT_PK_NAMESPACE;

#endif /* __DRMNDCERTCONSTANTS_H__ */

