/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#ifndef __DRMLICGEN_H_
#define __DRMLICGEN_H_ 1

#include <drmmanagertypes.h>
#include <drmxmrformatbuilder.h>
#include <drmlicgentypes.h>
#include <drmmodulesupport.h>

ENTER_PK_NAMESPACE;

typedef struct __tagDRM_LOCAL_LICENSE_SESSION_CONTEXT
{
    DRM_APP_CONTEXT                     *poAppContext;

    OEM_CRYPTO_HANDLE                    handleCICK;
    DRM_VOID                            *pOpaqueHandleEncryption;

    DRM_TEE_BYTE_BLOB                    oCEKB;

    DRM_KID                              kid;
    DRM_LID                              lid;

    DRM_DWORD                            cbXMRLicense;
    DRM_BYTE                            *pbXMRLicense;

    DRM_DWORD                            dwRefCount;
    DRM_BOOL                             fCannotPersist;
    DRM_LOCAL_LICENSE_TYPE               eLicenseType;
} DRM_LOCAL_LICENSE_SESSION_CONTEXT;

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL DRM_LOCALLICENSE_CleanCache(
    __inout                             DRM_APP_CONTEXT                     *f_poAppContext ) DRM_NO_INLINE_ATTRIBUTE;

EXIT_PK_NAMESPACE;

#endif /* __DRMLICGEN_H_ */

