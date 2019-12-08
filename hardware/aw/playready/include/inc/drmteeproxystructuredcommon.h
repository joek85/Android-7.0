/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <drmteetypes.h>
#include <oemteetypes.h>
#include <drmteeproxystructureddefines.h>

#ifndef __DRMTEEPROXYSTRUCTUREDCOMMON_H__
#define __DRMTEEPROXYSTRUCTUREDCOMMON_H__ 1

ENTER_PK_NAMESPACE;

#define DRM_TEE_PROXY_ROUND_TO_XBYTE_BOUNDARY(align,ib)     \
    do {                                                    \
        DRM_DWORD     _ib   = (ib);                         \
        DRM_DWORD     _mask = (align) - 1;                  \
        DRMASSERT( DRM_IS_INTEGER_POWER_OF_TW0( align ) );  \
        ChkDR( DRM_DWordAddSame( &_ib, _mask ) );           \
        (ib) = ( _ib & ( ~_mask ) );                        \
    } while( FALSE )

#define DRM_TEE_PROXY_ROUND_TO_8BYTE_BOUNDARY(ib) DRM_TEE_PROXY_ROUND_TO_XBYTE_BOUNDARY(8,ib)
#define DRM_TEE_PROXY_ROUND_TO_4BYTE_BOUNDARY(ib) DRM_TEE_PROXY_ROUND_TO_XBYTE_BOUNDARY(4,ib)

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL DRM_TEE_PROXY_StartResponseSerialization(
    __in                                                DRM_DWORD                    f_cbResponse,
    __inout_bcount( f_cbResponse )                      DRM_BYTE                    *f_pbResponse,
    __in                                                DRM_DWORD                    f_cOutputParams,
    __in                                                DRM_DWORD                    f_dwOEMFunctionMapID ) DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL DRM_TEE_PROXY_FinishResponseSerialization(
    __in                                                DRM_DWORD                    f_cbResponse,
    __inout_bcount( f_cbResponse )                      DRM_BYTE                    *f_pbResponse,
    __in                                                DRM_RESULT                   f_drResult ) DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL DRM_TEE_PROXY_GetMethodID(
    __in                                                DRM_DWORD                    f_cbRequest,
    __in_bcount( f_cbRequest )                    const DRM_BYTE                    *f_pbRequest,
    __out_ecount( 1 )                                   DRM_DWORD                   *f_pdwOemMethodID ) DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL DRM_TEE_PROXY_AllocateResponseData(
    __in                                                DRM_DWORD                    f_cbResponse,
    __inout_bcount_part( f_cbResponse, f_cbResponse)    DRM_BYTE                    *f_pbResponse,
    __in                                                DRM_DWORD                    f_cbData,
    __deref_out_bcount_opt( f_cbData )                  DRM_BYTE                   **f_ppbData ) DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL DRM_TEE_PROXY_GetTeeContextArgByIndex(
    __in                                                DRM_DWORD                    f_dwArgIndex,
    __in                                                DRM_DWORD                    f_cbRequest,
    __in_bcount( f_cbRequest )                    const DRM_BYTE                    *f_pbRequest,
    __out                                               DRM_DWORD                   *f_pcbTeeCtx,
    __deref_out_bcount( *f_pcbTeeCtx )                  DRM_BYTE                   **f_ppbTeeCtx ) DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL DRM_TEE_PROXY_AllocateTeeContextArgByIndex(
    __in                                                DRM_DWORD                    f_dwArgIndex,
    __in                                                DRM_DWORD                    f_cbResponse,
    __inout_bcount_part( f_cbResponse, f_cbResponse)    DRM_BYTE                    *f_pbResponse,
    __in                                                DRM_DWORD                    f_cbTeeCtx,
    __deref_out_bcount_opt( f_cbTeeCtx )                DRM_BYTE                   **f_ppbTeeCtx ) DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL DRM_TEE_PROXY_SetTeeContextArgByIndex(
    __in                                                DRM_DWORD                    f_dwArgIndex,
    __in                                                DRM_DWORD                    f_cbResponse,
    __inout_bcount( f_cbResponse )                      DRM_BYTE                    *f_pbResponse,
    __in                                                DRM_DWORD                    f_cbTeeCtx,
    __in_bcount_opt( f_cbTeeCtx )                 const DRM_BYTE                    *f_pbTeeCtx ) DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL DRM_TEE_PROXY_GetBlobArgByIndex(
    __in                                                DRM_DWORD                    f_dwArgIndex,
    __in                                                DRM_DWORD                    f_cbRequest,
    __in_bcount( f_cbRequest )                    const DRM_BYTE                    *f_pbRequest,
    __out_opt                                           DRM_DWORD                   *f_pdwType,
    __out_opt                                           DRM_DWORD                   *f_pdwSubType,
    __out                                               DRM_DWORD                   *f_pcbBlob,
    __deref_out_bcount_opt( *f_pcbBlob )                DRM_BYTE                   **f_ppbBlob ) DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL DRM_TEE_PROXY_AllocateBlobArgByIndex(
    __in                                                DRM_DWORD                    f_dwArgIndex,
    __in                                                DRM_DWORD                    f_cbResponse,
    __inout_bcount_part( f_cbResponse, f_cbResponse)    DRM_BYTE                    *f_pbResponse,
    __in                                                DRM_DWORD                    f_dwType,
    __in                                                DRM_DWORD                    f_dwSubType,
    __in                                                DRM_DWORD                    f_cbBlob,
    __deref_out_bcount_opt( f_cbBlob )                  DRM_BYTE                   **f_ppbBlob ) DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL DRM_TEE_PROXY_SetBlobArgByIndex(
    __in                                                DRM_DWORD                    f_dwArgIndex,
    __in                                                DRM_DWORD                    f_cbResponse,
    __inout_bcount( f_cbResponse )                      DRM_BYTE                    *f_pbResponse,
    __in                                                DRM_DWORD                    f_dwType,
    __in                                                DRM_DWORD                    f_dwSubType,
    __in                                                DRM_DWORD                    f_cbBlob,
    __in_bcount_opt( f_cbBlob )                   const DRM_BYTE                    *f_pbBlob ) DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL DRM_TEE_PROXY_GetIDArgByIndex(
    __in                                                DRM_DWORD                    f_dwArgIndex,
    __in                                                DRM_DWORD                    f_cbRequest,
    __in_bcount( f_cbRequest )                    const DRM_BYTE                    *f_pbRequest,
    __out_ecount( 1 )                                   DRM_ID                      *f_pidArgValue ) DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL DRM_TEE_PROXY_SetIDArgByIndex(
    __in                                                DRM_DWORD                    f_dwArgIndex,
    __in                                                DRM_DWORD                    f_cbResponse,
    __inout_bcount( f_cbResponse )                      DRM_BYTE                    *f_pbResponse,
    __in                                                DRM_ID                       f_idArgValue ) DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL DRM_TEE_PROXY_GetDwordArgByIndex(
    __in                                                DRM_DWORD                    f_dwArgIndex,
    __in                                                DRM_DWORD                    f_cbRequest,
    __in_bcount( f_cbRequest )                    const DRM_BYTE                    *f_pbRequest,
    __out_ecount( 1 )                                   DRM_DWORD                   *f_pdwArgValue ) DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL DRM_TEE_PROXY_SetDwordArgByIndex(
    __in                                                DRM_DWORD                    f_dwArgIndex,
    __in                                                DRM_DWORD                    f_cbResponse,
    __inout_bcount( f_cbResponse )                      DRM_BYTE                    *f_pbResponse,
    __in                                                DRM_DWORD                    f_dwArgValue ) DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL DRM_TEE_PROXY_GetQwordArgByIndex(
    __in                                                DRM_DWORD                    f_dwArgIndex,
    __in                                                DRM_DWORD                    f_cbRequest,
    __in_bcount( f_cbRequest )                    const DRM_BYTE                    *f_pbRequest,
    __out_ecount( 1 )                                   DRM_UINT64                  *f_pqwArgValue ) DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL DRM_TEE_PROXY_SetQwordArgByIndex(
    __in                                                DRM_DWORD                    f_dwArgIndex,
    __in                                                DRM_DWORD                    f_cbResponse,
    __inout_bcount( f_cbResponse )                      DRM_BYTE                    *f_pbResponse,
    __in                                                DRM_UINT64                   f_qwArgValue ) DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL DRM_TEE_PROXY_GetDwordListArgByIndex(
    __in                                                DRM_DWORD                    f_dwArgIndex,
    __in                                                DRM_DWORD                    f_cbRequest,
    __in_bcount( f_cbRequest )                    const DRM_BYTE                    *f_pbRequest,
    __out                                               DRM_DWORD                   *f_pcdwDWL,
    __deref_out_ecount( *f_pcdwDWL )                    DRM_DWORD                  **f_ppdwDWL ) DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL DRM_TEE_PROXY_AllocateDwordListArgByIndex(
    __in                                                DRM_DWORD                    f_dwArgIndex,
    __in                                                DRM_DWORD                    f_cbResponse,
    __inout_bcount_part( f_cbResponse, f_cbResponse)    DRM_BYTE                    *f_pbResponse,
    __out_opt                                           DRM_DWORD                   *f_pibParam,
    __in                                                DRM_DWORD                    f_cdwDWL,
    __deref_out_ecount_opt( f_cdwDWL )                  DRM_DWORD                  **f_ppdwDWL ) DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL DRM_TEE_PROXY_SetDwordListArgByIndex(
    __in                                                DRM_DWORD                    f_dwArgIndex,
    __in                                                DRM_DWORD                    f_cbResponse,
    __inout_bcount( f_cbResponse )                      DRM_BYTE                    *f_pbResponse,
    __in                                                DRM_DWORD                    f_cdwDWL,
    __in_ecount_opt( f_cdwDWL )                   const DRM_DWORD                   *f_pdwDWL ) DRM_NO_INLINE_ATTRIBUTE;

EXIT_PK_NAMESPACE;

#endif /* __DRMTEEPROXYSTRUCTUREDCOMMON_H__ */
