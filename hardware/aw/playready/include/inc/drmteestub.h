/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#ifndef _DRMTEESTUB_H_
#define _DRMTEESTUB_H_ 1

#include <drmteetypes.h>
#include <oemteetypes.h>

ENTER_PK_NAMESPACE;

/* Secure world TEE stub API */

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL DRM_TEE_STUB_HandleMethodRequest(
    __in                                                DRM_DWORD           f_cbRequestMessage,
    __in_bcount( f_cbRequestMessage )             const DRM_BYTE           *f_pbRequestMessage,
    __inout_opt                                         DRM_DWORD          *f_pcbResponseMessage,
    __inout_bcount_opt( *f_pcbResponseMessage )         DRM_BYTE           *f_pbResponseMessage ) DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL DRM_TEE_STUB_XB_TEECtxToBytes(
    __inout                                             DRM_TEE_CONTEXT    *f_pTEEContext,
    __out                                               DRM_DWORD          *f_pcbTEECtx,
    __deref_out_bcount( *f_pcbTEECtx )                  DRM_BYTE          **f_ppbTEECtx ) DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL DRM_TEE_STUB_XB_TEECtxFromBytes(
    __in                                                DRM_DWORD           f_cbTEECtx,
    __in_bcount( f_cbTEECtx )                     const DRM_BYTE           *f_pbTEECtx,
    __inout                                             DRM_TEE_CONTEXT    *f_pTEEContext ) DRM_NO_INLINE_ATTRIBUTE;

EXIT_PK_NAMESPACE;

#endif /* _DRMTEESTUB_H_ */

