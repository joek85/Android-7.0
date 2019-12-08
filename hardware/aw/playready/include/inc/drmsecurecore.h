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
#include <drmsecurecorefuncptrdefs.h>
#include <oemciphertypes.h>
#include <drmnoncestore.h>
#include <drmrevocationstore.h>
#include <drmsecurecoretypes.h>
#include <drmprndprotocoltypes.h>
#include <drmlicgentypes.h>
#include <drmlicgen.h>

PREFAST_PUSH_DISABLE_EXPLAINED( __WARNING_NONCONST_PARAM_25004, "Prefast Noise: DRM_SECURECORE_CONTEXT* should not be const." );
PREFAST_PUSH_DISABLE_EXPLAINED( __WARNING_NONCONST_BUFFER_PARAM_25033, "Prefast Noise: DRM_SECURECORE_CONTEXT* should not be const." );

ENTER_PK_NAMESPACE;

typedef DRM_API DRM_RESULT (DRM_CALL *LPFN_DRM_SECURECORE_FUNCTBL_INITIALIZER)(
    __inout                             DRM_VOID                *f_pSecureCoreFuncTbl );

#define DRM_SC_DEF_FOR_EXTCALLERS   1

#include <drmsecurecore_template.h>

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL DRM_SECURECORE_FUNCTBL_Initializer(
    __inout                             DRM_VOID                *f_pSecureCoreFuncTbl );

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL DRM_SECURECORE_TEE_FUNCTBL_Initializer(
    __inout                             DRM_VOID                *f_pSecureCoreFuncTbl );

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL DRM_SECURECORE_NOTEE_FUNCTBL_Initializer(
    __inout                             DRM_VOID                *f_pSecureCoreFuncTbl );

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL DRM_SECURECORE_INT_FUNCTBL_Initializer(
    __inout                             DRM_VOID                *f_pSecureCoreFuncTbl );

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL DRM_SECURECORE_NOSCINIT_FUNCTBL_Initializer(
    __inout                             DRM_VOID                *f_pSecureCoreFuncTbl );

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL DRM_SECURECORE_PreInitialize(
    __in                                LPFN_DRM_SECURECORE_FUNCTBL_INITIALIZER   pfnSecureCoreFuncTblInit,
    __inout                             DRM_SECURECORE_CONTEXT                   *f_pSecureCoreCtx,
    __in_opt                            DRM_VOID                                 *f_pOEMContext,
    __out                               DRM_VOID                                **f_ppvPreInitializeContext,
    __out                               DRM_ID                                   *f_pCurrentAppId,
    __out                               DRM_ID                                   *f_pUniqueId ) DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL DRM_SECURECORE_Initialize(
    __in                                LPFN_DRM_SECURECORE_FUNCTBL_INITIALIZER    pfnSecureCoreFuncTblInit,
    __inout                             DRM_SECURECORE_CONTEXT                    *f_pSecureCoreCtx,
    __in_opt                            DRM_VOID                                  *f_pOEMContext,
    __in                                DRM_DWORD                                  f_cchVersion,
    __in_ecount(f_cchVersion)           DRM_CHAR                                  *f_pszVersion,
    __in                                DRM_DST                                   *f_pHDS,
    __in_opt                      const DRM_CONST_STRING                          *f_pdstrDeviceStoreName,
    __in                                DRM_DWORD                                  f_cbWorkingBuffer,
    __inout_bcount(f_cbWorkingBuffer)   DRM_BYTE                                  *f_pbWorkingBuffer,
    __inout_opt                         DRM_VOID                                 **f_ppvPreInitializeContext,
    __in_opt                            DRM_ID                                    *f_pCurrentAppId ) DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_API DRM_BOOL DRM_CALL DRM_SECURECORE_IsInternal() DRM_NO_INLINE_ATTRIBUTE;

/*
** Licenses and Domains use the exact same password except that the
** former uses the LID while the latter uses the Domain Account ID,
** so there's no reason to create a separate API for domains.
*/
#define DRM_SECURECORE_CreateDomainStorePassword DRM_SECURECORE_CreateLicenseStateStorePassword

#undef DRM_SC_DEF_FOR_EXTCALLERS

EXIT_PK_NAMESPACE;

PREFAST_POP; /* __WARNING_NONCONST_BUFFER_PARAM_25033 */
PREFAST_POP; /* __WARNING_NONCONST_PARAM_25004 */

#endif /* _DRMSECURECORE_H_ */

