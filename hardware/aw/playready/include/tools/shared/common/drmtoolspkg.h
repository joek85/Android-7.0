/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#ifndef __DRM_TOOLS_PKG_H__
#define __DRM_TOOLS_PKG_H__ 1

ENTER_PK_NAMESPACE;

DRM_API DRM_RESULT DRM_CALL DRMTOOLS_BuildContentHeader_Ex(
    __in_opt       DRM_CRYPTO_CONTEXT      *f_poCrypto,
    __in_opt const DRM_CONST_STRING        *f_pdstrContentID,
    __in     const DRM_CONST_STRING        *f_pdstrKeySeed,
    __in     const DRM_CONST_STRING        *f_pdstrKeyID,
    __in_opt const DRM_CONST_STRING        *f_pdstrLAURL,
    __in_opt const DRM_CONST_STRING        *f_pdstrLUIURL,
    __in_opt const DRM_CONST_STRING        *f_pdstrDSID,
    __in_opt const DRM_CONST_STRING        *f_pdstrCustomData,
    __in_opt const DRM_CONST_STRING        *f_pdstrDecryptorSetup,
    __in           DRM_CONTENT_SET_PROPERTY f_eHeaderType,
    __in           DRM_SUPPORTED_CIPHERS    f_eCipherType,
    __deref_inout_bcount_opt( *f_pcbHeader )
                   DRM_BYTE               **f_ppbHeader,
    __inout        DRM_DWORD               *f_pcbHeader,
    __inout        DRM_STRING              *f_pdstrContentKey );

DRM_API DRM_RESULT DRM_CALL DRMTOOLS_BuildContentHeader(
    __in       DRM_CRYPTO_CONTEXT      *f_poCrypto,
    __in const DRM_CONST_STRING        *f_pdstrFileInit,
    __in const DRM_CONST_STRING        *f_pdstrContentID,
    __inout    DRM_CONST_STRING        *f_pdstrKeyID,
    __in       DRM_CONTENT_SET_PROPERTY f_eHeaderType,
    __in       DRM_SUPPORTED_CIPHERS    f_eCipherType,
    __deref_inout_bcount_opt( *f_pcbHeader )
               DRM_BYTE               **f_ppbHeader,
    __inout    DRM_DWORD               *f_pcbHeader,
    __inout    DRM_STRING              *f_pdstrContentKey,
    __out      DRM_DWORD               *f_pcbChunk );

DRM_API DRM_RESULT DRM_CALL DRMTOOLS_WrapV4XMLIntoPlayReadyObject(
    __inout_ecount(*pcbHeader) DRM_BYTE  **ppbHeader,
    __inout                    DRM_DWORD  *pcbHeader );

EXIT_PK_NAMESPACE;

#endif /* __DRM_TOOLS_PKG_H__ */
