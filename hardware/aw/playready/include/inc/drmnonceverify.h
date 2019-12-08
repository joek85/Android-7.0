/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#ifndef _DRMNONCEVERIFY_H_
#define _DRMNONCEVERIFY_H_ 1

#include <drmtypes.h>

ENTER_PK_NAMESPACE;

DRM_API DRM_RESULT DRM_CALL DRM_NONCE_VerifyNonce(
    __in const DRM_ID                 *f_poNonce,
    __in const DRM_LID                *f_poLID,
    __in       DRM_DWORD               f_cLicenses ) DRM_NO_INLINE_ATTRIBUTE;

EXIT_PK_NAMESPACE;

#endif /* _DRMNONCEVERIFY_H_ */