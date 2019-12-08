/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/


#ifndef __DRMREVOCATION_H__
#define __DRMREVOCATION_H__

#include <drmliceval.h>
#include <drmrevocationstore.h>
#include <drmrevocationtypes.h>
#include <oemparsers.h>
#include <drmmodulesupport.h>

ENTER_PK_NAMESPACE;

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL DRM_RVK_ProcessRevocationInfoXML(
    __in                                      DRM_REVOCATIONSTORE_CONTEXT *f_pContextRev,
    __in_ecount(f_cchRevocationInfoXML) const DRM_CHAR                    *f_pchRevocationInfoXML,
    __in                                      DRM_DWORD                    f_cchRevocationInfoXML,
    __in_ecount(f_cbRevocationBuffer)         DRM_BYTE                    *f_pbRevocationBuffer,
    __in                                      DRM_DWORD                    f_cbRevocationBuffer,
    __out_opt                                 DRM_BOOL                    *f_pfUpdated ) DRM_NO_INLINE_ATTRIBUTE;

DRM_API DRM_RESULT DRM_CALL DRM_RVK_UpdateRevocationVersionsCache(
    __inout                            DRM_REVOCATIONSTORE_CONTEXT *f_pContextRev,
    __inout                            DRM_REVLIST_ID_VALUES       *f_pRevListIDs,
    __inout_opt                        DRM_REVLIST_ID_VALUES       *f_pRevListIDsCopy,
    __out_bcount(f_cbRevocationBuffer) DRM_BYTE                    *f_pbRevocationBuffer,
    __in                               DRM_DWORD                    f_cbRevocationBuffer,
    __out_opt                          DRM_BOOL                    *f_pfUpdated ) DRM_NO_INLINE_ATTRIBUTE;

DRM_API DRM_RESULT DRM_CALL DRM_RVK_VerifyRevInfoSafe(
    __in                               DRM_REVOCATIONSTORE_CONTEXT *f_pContextRev,
    __in const                         DRM_GUID                    *f_pguidRevocationType,
    __inout_bcount(f_cbRevocationInfo) DRM_BYTE                    *f_pbRevocationInfo,
    __in                               DRM_DWORD                    f_cbRevocationInfo,
    __in_bcount(f_cbRevocationBuffer)  const DRM_BYTE              *f_pbRevocationBuffer,
    __in                               DRM_DWORD                    f_cbRevocationBuffer,
    __inout_opt                        DRM_REVLIST_ID_VALUES       *f_pRevList,
    __out                              DRM_RLVI                    *f_pnewRLVI,
    __out_opt                          DRM_BOOL                    *f_pfRIV2,
    __out                              DRM_BOOL                    *f_pfVerified ) DRM_NO_INLINE_ATTRIBUTE;

DRM_API DRM_RESULT DRM_RVK_StoreRevocationLists(
    __in                              DRM_REVOCATIONSTORE_CONTEXT *f_pContextRev,
    __in                              DRM_DWORD                    f_cRevocationLists,
    __in_ecount(f_cRevocationLists)   DRM_RVK_LIST                *f_pRevocationLists,
    __in_ecount(f_cbRevocationBuffer) DRM_BYTE                    *f_pbRevocationBuffer,
    __in                              DRM_DWORD                    f_cbRevocationBuffer,
    __inout_ecount(1)                 DRM_REVLIST_ID_VALUES       *f_pRevList );

DRM_API DRM_RESULT DRM_CALL DRM_RVK_VerifyRevocationInfo(
    __inout                    DRM_SECURECORE_CONTEXT *f_pSecureCoreCtx,
    __in_bcount( f_cbRevInfo ) DRM_BYTE               *f_pbRevInfo,
    __in                       DRM_DWORD               f_cbRevInfo,
    __out                      DRM_RLVI               *f_pRLVI,
    __in_opt                   DRM_SECSTORE_CONTEXT   *f_pcontextSST,
    __in_opt                   DRM_DST                *f_pDatastore ) DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_API DRM_BOOL DRM_RVK_IsWrappedCRL(
    __in const DRM_CONST_STRING *f_pdstrRevocationList );

DRM_API DRM_RESULT DRM_CALL DRM_RVK_VerifySigningCert(
    __in                          const DRM_ANSI_CONST_STRING       *f_pdastrResponse,
    __in                          const DRM_SUBSTRING               *f_pdasstrResponse,
    __in                          const DRM_ANSI_CONST_STRING       *f_pdastrCertNodePath,
    __in_bcount( f_cbBcertBuffer )      DRM_BYTE                    *f_pbBCertBuffer,
    __in                                DRM_DWORD                    f_cbBcertBuffer,
    __in_bcount( f_cbRevocationBuffer ) DRM_BYTE                    *f_pbRevocationBuffer,
    __in                                DRM_DWORD                    f_cbRevocationBuffer,
    __inout                             DRM_SECURECORE_CONTEXT      *f_pSecureCoreCtx,
    __in                                DRM_SECSTORE_CONTEXT        *f_pSecStoreGlobalContext,
    __in                                DRM_DST                     *f_pDatastore,
    __in                                DRM_REVOCATIONSTORE_CONTEXT *f_pRevStoreContext );

#define DRM_APP_REVOCATION_VERSION_NONE    0xFFFFFFFF

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL DRM_RVK_GetCRL(
    __in                                DRM_REVOCATIONSTORE_CONTEXT  *f_pContextRev,
    __in                          const DRM_GUID                     *f_pguidRevocationType,
    __out_bcount_opt( *f_pcbCRL )       DRM_BYTE                     *f_pbCRL,
    __inout                             DRM_DWORD                    *f_pcbCRL,
    __out_opt                           DRM_DWORD                    *f_pdwCRLVersion ) DRM_NO_INLINE_ATTRIBUTE;

DRM_API DRM_RESULT DRM_CALL DRM_RVK_SetCRL(
    __in                                      DRM_REVOCATIONSTORE_CONTEXT  *f_pContextRev,
    __in                                const DRM_GUID                     *f_pguidRevocationType,
    __in_bcount( f_cbCRL )                    DRM_BYTE                     *f_pbCRL,
    __in                                      DRM_DWORD                     f_cbCRL,
    __in_bcount( f_cbRevocationBuffer )       DRM_BYTE                     *f_pbRevocationBuffer,
    __in                                      DRM_DWORD                     f_cbRevocationBuffer,
    __out                                     DRM_BOOL                     *f_pfUpdated ) DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL DRM_RVK_CheckRevInfoForExpiration(
    __in                                DRM_REVOCATIONSTORE_CONTEXT  *f_pContextRev,
    __in                          const DRM_GUID                     *f_pguidRevocationType,
    __in_opt                      const DRM_ID                       *f_pidSession,
    __out_bcount_opt( *f_pcbRevInfo )   DRM_BYTE                     *f_pbRevInfo,
    __inout                             DRM_DWORD                    *f_pcbRevInfo ) DRM_NO_INLINE_ATTRIBUTE;


EXIT_PK_NAMESPACE;

#endif /* __DRMREVOCATION_H__ */

