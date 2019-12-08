/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#ifndef __DRMSECURECLOCKCONSTANTS_H__
#define __DRMSECURECLOCKCONSTANTS_H__

#include <drmtypes.h>
#include <oemcryptoctx.h>

ENTER_PK_NAMESPACE;

    extern DRM_GLOBAL_CONST  DRM_CONST_STRING       g_dstrTagError;
    extern DRM_GLOBAL_CONST  DRM_CONST_STRING       g_dstrExprVarInGracePeriod;
    extern DRM_GLOBAL_CONST  DRM_CONST_STRING       g_dstrExprVarGpStartTime;
    extern DRM_GLOBAL_CONST  DRM_CONST_STRING       g_dstrStatus;
    extern DRM_GLOBAL_CONST  DRM_CONST_STRING       g_dstrTagGmtTime;
    extern DRM_GLOBAL_CONST  DRM_CONST_STRING       g_dstrTagRefreshDate;
    extern DRM_GLOBAL_CONST  DRM_CONST_STRING       g_dstrTagDRMReqRoot;

    extern DRM_GLOBAL_CONST  DRM_CONST_STRING       g_dstrDefaultPlayReadySecureClockServerUrl;

EXIT_PK_NAMESPACE;

#endif /* __DRMSECURECLOCKCONSTANTS_H__ */

