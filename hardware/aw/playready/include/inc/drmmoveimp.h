/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#ifndef __DRM_MOVE_IMP_H
#define __DRM_MOVE_IMP_H

#include <drmmovestore.h>
#include <drmsecurecoretypes.h>

ENTER_PK_NAMESPACE;

/*
** ---------------------------------------
** Move implementation API functions
** ---------------------------------------
*/
DRM_API DRM_RESULT DRM_CALL DRM_MOV_GenerateMoveChallenge(
    __inout                                   DRM_SECURECORE_CONTEXT    *f_pSecureCoreCtx,
    __in                                const DRM_MOVESTORE_CONTEXT     *f_poMoveStoreContext,
    __in_bcount( f_cbDeviceCert )       const DRM_BYTE                  *f_pbDeviceCert,
    __in                                      DRM_DWORD                  f_cbDeviceCert,
    __in_ecount(f_cLicensesToMove)      const DRM_LID                   *f_prgLIDsToMove,
    __in                                      DRM_DWORD                  f_cLicensesToMove,
    __out                                     DRM_ID                    *f_pidNonce,
    __out_bcount_opt( *f_pcbChallenge )       DRM_BYTE                  *f_pbChallenge,
    __inout                                   DRM_DWORD                 *f_pcbChallenge,
    __in_ecount_opt( 1 )                const PUBKEY_P256               *f_poECC256SignPubKey ) DRM_NO_INLINE_ATTRIBUTE;

DRM_API DRM_RESULT DRM_CALL DRM_MOV_ProcessMoveResponse(
    __inout                        DRM_SECURECORE_CONTEXT  *f_pSecureCoreCtx,
    __in                           DRM_MOVESTORE_CONTEXT   *f_poMoveStoreContext,
    __inout_bcount( f_cbResponse ) DRM_BYTE                *f_pbResponse,
    __in                           DRM_DWORD                f_cbResponse,
    __in                     const DRM_ID                  *f_pNonce,
    __out                          DRM_RESULT              *f_pResult ) DRM_NO_INLINE_ATTRIBUTE;

DRM_API DRM_RESULT DRM_CALL DRM_MOV_GenerateRefreshChallenge(
    __inout                             DRM_SECURECORE_CONTEXT  *f_pSecureCoreCtx,
    __in_bcount( f_cbDeviceCert ) const DRM_BYTE                *f_pbDeviceCert,
    __in                                DRM_DWORD                f_cbDeviceCert,
    __in                                DRM_DWORD                f_dwIndexSerialNumber,
    __in_ecount(f_cPagesToRefresh)const DRM_DWORD               *f_pdwPagesToRefresh,
    __in                                DRM_DWORD                f_cPagesToRefresh,
    __out                               DRM_ID                  *f_pidNonce,
    __out_bcount_opt( *f_pcbChallenge ) DRM_BYTE                *f_pbChallenge,
    __inout                             DRM_DWORD               *f_pcbChallenge,
    __in_ecount_opt( 1 )          const PUBKEY_P256             *f_poECC256SignPubKey ) DRM_NO_INLINE_ATTRIBUTE;

DRM_API DRM_RESULT DRM_CALL DRM_MOV_ProcessRefreshResponse(
    __inout                        DRM_SECURECORE_CONTEXT   *f_pSecureCoreCtx,
    __in                           DRM_MOVESTORE_CONTEXT    *f_poMoveStoreContext,
    __inout_bcount( f_cbResponse ) DRM_BYTE                 *f_pbResponse,
    __in                           DRM_DWORD                 f_cbResponse,
    __in                     const DRM_ID                   *f_pidNonce,
    __out                          DRM_RESULT               *f_pResult ) DRM_NO_INLINE_ATTRIBUTE;

DRM_API DRM_RESULT DRM_CALL DRM_MOV_GenerateCopyChallenge(
    __inout                                   DRM_SECURECORE_CONTEXT    *f_pSecureCoreCtx,
    __in                                const DRM_MOVESTORE_CONTEXT     *f_poMoveStoreContext,
    __in_bcount( f_cbDeviceCert )       const DRM_BYTE                  *f_pbDeviceCert,
    __in                                      DRM_DWORD                  f_cbDeviceCert,
    __in_ecount(f_cLicensesToCopy)      const DRM_LID                   *f_prgLIDsToCopy,
    __in_ecount(f_cLicensesToCopy)      const DRM_DWORD                 *f_rgdwOriginalCopyCount,
    __in                                      DRM_DWORD                  f_cLicensesToCopy,
    __out                                     DRM_ID                    *f_pidNonce,
    __out_bcount_opt( *f_pcbChallenge )       DRM_BYTE                  *f_pbChallenge,
    __inout                                   DRM_DWORD                 *f_pcbChallenge,
    __in_ecount_opt( 1 )                const PUBKEY_P256               *f_poECC256SignPubKey ) DRM_NO_INLINE_ATTRIBUTE;

DRM_API DRM_RESULT DRM_CALL DRM_MOV_ProcessCopyResponse(
    __inout                        DRM_SECURECORE_CONTEXT   *f_pSecureCoreCtx,
    __in                           DRM_MOVESTORE_CONTEXT    *f_poMoveStoreContext,
    __inout_bcount( f_cbResponse ) DRM_BYTE                 *f_pbResponse,
    __in                           DRM_DWORD                 f_cbResponse,
    __in                     const DRM_ID                   *f_pNonce,
    __out                          DRM_RESULT               *f_pResult ) DRM_NO_INLINE_ATTRIBUTE;

EXIT_PK_NAMESPACE;

#endif /* __DRM_MOVE_IMP_H */
