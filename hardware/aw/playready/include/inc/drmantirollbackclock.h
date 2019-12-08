/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#ifndef __DRMANTIROLLBACKCLOCK_H__
#define __DRMANTIROLLBACKCLOCK_H__

#include <drmliceval.h>
#include <drmlicstore.h>

ENTER_PK_NAMESPACE;

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL DRM_ARCLK_LEVL_OverrideMachineDateTimeWithSavedValue(
    __inout       DRM_LICEVAL_CONTEXT *f_pContextLicEval,
    __inout       DRM_UINT64          *f_pui64MachineDateTime ) DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL DRM_ARCLK_LEVL_GetMachineDatetime(
    __inout       DRM_LICEVAL_CONTEXT *f_pContextLicEval,
    __inout       DRM_UINT64          *f_pui64MachineDateTime ) DRM_NO_INLINE_ATTRIBUTE;

DRM_API DRM_RESULT DRM_CALL DRM_ARCLK_ASD_CheckRollback(
    __inout       DRM_LICEVAL_CONTEXT *f_pContextLicEval );

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL DRM_ARCLK_LEVL_DisableOnRollback(
    __inout       DRM_LICEVAL_CONTEXT *f_pContextLicEval,
    __in    const DRM_XMRFORMAT       *f_pXMR,
    __out         DRM_BOOL            *f_pfDisabled ) DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL DRM_ARCLK_LEVL_SaveLicenseStoreTime(
    __inout       DRM_LICEVAL_CONTEXT *f_pContextLicEval,
    __in    const DRM_XMRFORMAT       *f_pXMR ) DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL DRM_ARCLK_LEVL_ReportClockRollback(
    __inout       DRM_LICEVAL_CONTEXT *f_pContextLicEval,
    __in    const DRM_XMRFORMAT       *f_pXMR ) DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL DRM_ARCLK_DOMST_GetDomainSavedDateTime(
    __in_bcount( DRM_SHA1_DIGEST_LEN ) const DRM_BYTE                 f_rgbPasswordSST[ DRM_SHA1_DIGEST_LEN ],
    __inout_opt                              DRM_VOID                *f_pOEMContext,
    __in                                     DRM_DOMAINSTORE_CONTEXT *f_poDomainStoreContext,
    __in                               const DRM_GUID                *f_poAccountID,
    __out_opt                                DRM_UINT64              *f_puiDomainSavedDateTime ) DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL DRM_ARCLK_DOMST_ClockRollbackDetected(
    __in    DRM_DOMAINSTORE_CONTEXT *f_poDomainStoreContext,
    __inout DRM_SECURECORE_CONTEXT  *f_pSecureCoreCtx,
    __in    DRM_UINT64               f_ui64GlobalSavedTime ) DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL DRM_ARCLK_DOMST_CreateDomainStorePassword(
    __in                                const DRM_GUID                *f_poAccountID,
    __inout                                   DRM_SECURECORE_CONTEXT  *f_pSecureCoreCtx,
    __out_bcount( DRM_SHA1_DIGEST_LEN )       DRM_BYTE                 f_rgbPasswordSST[ DRM_SHA1_DIGEST_LEN ] ) DRM_NO_INLINE_ATTRIBUTE;

DRM_API DRM_RESULT DRM_CALL DRM_ARCLK_LIC_CheckClockRollback(
    __inout                 DRM_LICEVAL_CONTEXT         *f_pLicEval,
    __in                    DRM_LICSTORE_CONTEXT        *f_pLicStoreXMR,
    __in                    DRM_LICSTOREENUM_CONTEXT    *f_pLicEnum,
    __in_ecount(f_cbBuffer) DRM_BYTE                    *f_pbBuffer,
    __in                    DRM_DWORD                    f_cbBuffer,
    __in                    DRM_DST                     *f_pDatastore,
    __in                    DRM_STACK_ALLOCATOR_CONTEXT *f_poStack );

DRM_API DRM_RESULT DRM_CALL DRM_ARCLK_LIC_ResetClockRollback(
    __in DRM_LICEVAL_CONTEXT       *f_pLicEval );

EXIT_PK_NAMESPACE;

#endif /*__DRMANTIROLLBACKCLOCK_H__ */

