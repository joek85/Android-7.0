/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#ifndef __DRMBCERTFORMATPARSERCACHE_H__
#define __DRMBCERTFORMATPARSERCACHE_H__

#include <drmtypes.h>
#include <drmbcertcachetypes.h>
#include <drmsecurecoretypes.h>

ENTER_PK_NAMESPACE;

PREFAST_PUSH_DISABLE_EXPLAINED(__WARNING_NONCONST_PARAM_25004,"Ignore non const parameter for DRM_SECURECORE_CONTEXT." )
DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL DRM_BCERTFORMAT_LoadPropertiesCache(
    __inout                         DRM_SECURECORE_CONTEXT               *f_pSecureCoreCtx,
    __in_bcount(f_cbCertData) const DRM_BYTE                             *f_pbCertData,
    __in                      const DRM_DWORD                             f_cbCertData,
    __inout                         DRM_BINARY_DEVICE_CERT_CACHED_VALUES *f_pCachedValues ) DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_API_VOID DRM_VOID DRM_CALL DRM_BCERTFORMAT_CleanupPropertiesCache(
    __inout                         DRM_SECURECORE_CONTEXT               *f_pSecureCoreCtx,
    __inout                         DRM_BINARY_DEVICE_CERT_CACHED_VALUES *f_pCachedValues ) DRM_NO_INLINE_ATTRIBUTE;
PREFAST_POP /* __WARNING_NONCONST_PARAM_25004 */

EXIT_PK_NAMESPACE;

#endif /* __DRMBCERTFORMATPARSERCACHE_H__ */

