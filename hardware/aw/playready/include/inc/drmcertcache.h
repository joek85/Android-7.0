/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#ifndef __DRM_CERT_CACHE__
#define __DRM_CERT_CACHE__

#include <drmsecurecoretypes.h>

ENTER_PK_NAMESPACE;

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL DRM_CERTCACHE_AddCachedCertificate(
    __inout                     DRM_SECURECORE_CONTEXT  *f_pSecureCoreCtx,
    __in                        DRM_SECSTORE_CONTEXT    *f_pcontextSSTRevocation,
    __in                        DRM_DST                 *f_pDatastore,
    __in_ecount(f_cbData) const DRM_BYTE                *f_pbData,
    __in                        DRM_DWORD                f_cbData ) DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_API DRM_BOOL DRM_CALL DRM_CERTCACHE_VerifyCachedCertificate(
    __inout                     DRM_SECURECORE_CONTEXT  *f_pSecureCoreCtx,
    __in                        DRM_SECSTORE_CONTEXT    *f_pcontextSST,
    __in                        DRM_DST                 *f_pDatastore,
    __in_ecount(f_cbData) const DRM_BYTE                *f_pbData,
    __in                        DRM_DWORD                f_cbData ) DRM_NO_INLINE_ATTRIBUTE;

EXIT_PK_NAMESPACE;

#endif /* __DRM_CERT_CACHE__ */

