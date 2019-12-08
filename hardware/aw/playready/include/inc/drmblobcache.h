/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#ifndef __DRMBLOBCACHE_H__
#define __DRMBLOBCACHE_H__

#include <drmsecurecoretypes.h>
#include <drmsecurestoretypes.h>
#include <drmdatastoretypes.h>

ENTER_PK_NAMESPACE;

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL DRM_BLOBCACHE_Verify(
    __inout_opt                         DRM_SECURECORE_CONTEXT  *f_pSecureCoreCtx,
    __in_opt                            DRM_SECSTORE_CONTEXT    *f_poSecStoreContext,
    __in_opt                            DRM_DST                 *f_poDatastoreHDS,
    __in_ecount( f_cbBlob1 )      const DRM_BYTE                *f_pbBlob1,
    __in                                DRM_DWORD                f_cbBlob1,
    __in_ecount_opt( f_cbBlob2 )  const DRM_BYTE                *f_pbBlob2,
    __in                                DRM_DWORD                f_cbBlob2,
    __out                               DRM_BOOL                *f_pfResult ) DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL DRM_BLOBCACHE_Update(
    __inout                             DRM_SECURECORE_CONTEXT  *f_pSecureCoreCtx,
    __in                                DRM_SECSTORE_CONTEXT    *f_poSecStoreContext,
    __in                                DRM_DST                 *f_poDatastoreHDS,
    __in_ecount( f_cbBlob1 )      const DRM_BYTE                *f_pbBlob1,
    __in                                DRM_DWORD                f_cbBlob1,
    __in_ecount_opt( f_cbBlob2 )  const DRM_BYTE                *f_pbBlob2,
    __in                                DRM_DWORD                f_cbBlob2 ) DRM_NO_INLINE_ATTRIBUTE;

EXIT_PK_NAMESPACE;

#endif /* __DRMBLOBCACHE_H__ */

