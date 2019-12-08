/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#ifndef __DRMMODELREVOCATION_H__
#define __DRMMODELREVOCATION_H__

#include <drmtypes.h>
#include <drmbcrltypes.h>

ENTER_PK_NAMESPACE;

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL DRM_MODELREVOCATION_CheckIfPlayReadyModelRevoked(
    __in       DRM_VOID *f_pOEMContext,
    __in const DRM_BCRL *f_pPRMainCRL,
    __out      DRM_BOOL *f_pfStoreCRL ) DRM_NO_INLINE_ATTRIBUTE;

EXIT_PK_NAMESPACE;

#endif /*__DRMMODELREVOCATION_H__ */

