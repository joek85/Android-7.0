/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#ifndef _DRMSECURECORE_H_
#define _DRMSECURECORE_H_ 1

#include <drmsecurecoreconstants.h>
#include <oemciphertypes.h>
#include <drmnoncestore.h>
#include <drmrevocationstore.h>
#include <drmprndprotocoltypes.h>
#include <drmlicgentypes.h>
#include <drmlicgen.h>

PREFAST_PUSH_DISABLE_EXPLAINED( __WARNING_NONCONST_PARAM_25004, "Prefast Noise: DRM_SECURECORE_CONTEXT* should not be const." );
PREFAST_PUSH_DISABLE_EXPLAINED( __WARNING_NONCONST_BUFFER_PARAM_25033, "Prefast Noise: DRM_SECURECORE_CONTEXT* should not be const." );

ENTER_PK_NAMESPACE;

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL DRM_SECURECORE_TEE_BASE_OpenConfigNamespace(
    __in    DRM_DST                     *f_pHDS,
    __out   DRM_DST_NAMESPACE_CONTEXT   *f_pNsCtx,
    __out   DRM_BOOL                    *f_pfNsOpen ) DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL DRM_SECURECORE_TEE_BASE_SlotWriteHelper(
    __in                       DRM_DST                     *f_pHDS,
    __in                       DRM_DST_NAMESPACE_CONTEXT   *f_pNsCtx,
    __in                       DRM_DST_SLOT_CONTEXT        *f_pSlot,
    __inout                    DRM_BOOL                    *f_pfSlotOpen,
    __in                       DRM_DWORD                    f_cbOld,
    __in                       DRM_DWORD                    f_cbNew,
    __in_ecount(f_cbNew) const DRM_BYTE                    *f_pbNew,
    __in                 const DRM_ID                      *f_pidSlot ) DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL DRM_SECURECORE_TEE_BASE_SlotDeleteHelper(
    __inout  const DRM_DST                   *f_pHDS,
    __in           DRM_DST_NAMESPACE_CONTEXT *f_pNsCtx,
    __in     const DRM_ID                    *f_pidSlot ) DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL DRM_SECURECORE_TEE_VerifyAndStoreCertAndPPKB(
    __inout                          DRM_TEE_PROXY_CONTEXT       *f_pTeeCtx,
    __in                             DRM_DST                     *f_pHDS,
    __in                             DRM_DWORD                    f_cbOldPPKB,
    __in                             DRM_DWORD                    f_cbNewPPKB,
    __in_bcount( f_cbNewPPKB ) const DRM_BYTE                    *f_pbNewPPKB,
    __in                             DRM_DWORD                    f_cbOldCert,
    __in                             DRM_DWORD                    f_cbNewCert,
    __in_bcount( f_cbNewCert ) const DRM_BYTE                    *f_pbNewCert,
    __inout_opt                      DRM_CRYPTO_CONTEXT          *f_pCryptoContext,
    __inout_opt                      DRM_DST_NAMESPACE_CONTEXT   *f_pNsCtx,
    __inout_opt                      DRM_DST_SLOT_CONTEXT        *f_pSlotPPKB,
    __inout_opt                      DRM_BOOL                    *f_pfSlotPPKBOpen,
    __inout_opt                      DRM_DST_SLOT_CONTEXT        *f_pSlotCert,
    __inout_opt                      DRM_BOOL                    *f_pfSlotCertOpen ) DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL DRM_SECURECORE_TEE_PreInitialize(
    __in_opt                            DRM_VOID                 *f_pOEMContext,
    __out                               DRM_VOID                **f_ppvPreInitializeContext,
    __out                               DRM_ID                   *f_pCurrentAppId,
    __out                               DRM_ID                   *f_pUniqueId ) DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL DRM_SECURECORE_TEE_Initialize(
    __inout                             DRM_SECURECORE_CONTEXT   *f_pSecureCoreCtx,
    __in_opt                            DRM_VOID                 *f_pOEMContext,
    __in                                DRM_DWORD                 f_cchVersion,
    __in_ecount(f_cchVersion)           DRM_CHAR                 *f_pszVersion,
    __in                                DRM_DST                  *f_pHDS,
    __in_opt                      const DRM_CONST_STRING         *f_pdstrDeviceStoreName,
    __in                                DRM_DWORD                 f_cbWorkingBuffer,
    __inout_bcount(f_cbWorkingBuffer)   DRM_BYTE                 *f_pbWorkingBuffer,
    __inout_opt                         DRM_VOID                **f_ppvPreInitializeContext,
    __in_opt                            DRM_ID                   *f_pCurrentAppId ) DRM_NO_INLINE_ATTRIBUTE;

#define DRM_SC_DEF_FOR_TEE 1

#include <drmsecurecore_template.h>

#undef DRM_SC_DEF_FOR_TEE

EXIT_PK_NAMESPACE;

PREFAST_POP; /* __WARNING_NONCONST_BUFFER_PARAM_25033 */
PREFAST_POP; /* __WARNING_NONCONST_PARAM_25004 */

#endif /* _DRMSECURECORE_H_ */

