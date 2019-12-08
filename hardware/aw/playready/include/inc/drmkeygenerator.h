/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#ifndef __DRMKEYGEN_H__
#define __DRMKEYGEN_H__

#include <drmnamespace.h>
#include <oemciphertypes.h>

ENTER_PK_NAMESPACE;

#define DRM_KEYSEED_BASE64_LEN  40
#define DRM_KEYSEED_BINARY_LEN  CB_BASE64_DECODE(DRM_KEYSEED_BASE64_LEN)

/*
**
** NOTE:  All Keygen functions include a NULL termination character
** in the character count and the string on function return
**
*/

DRM_API DRM_RESULT DRM_CALL DRM_KG_GenerateKeyseed(
    __in_opt                           DRM_VOID        *f_pOEMContext,
    __out_ecount_opt( *f_pcchKeyseed ) DRM_WCHAR *f_pwszKeyseed,
    __inout                            DRM_DWORD *f_pcchKeyseed );

DRM_API DRM_RESULT DRM_CALL DRM_KG_GenerateKeyID(
    __in_opt                         DRM_VOID  *f_pOEMContext,
    __out_ecount_opt( *f_pcchKeyID ) DRM_WCHAR *f_pwszKeyID,           /* base64 encoded string */
    __inout                          DRM_DWORD *f_pcchKeyID );


DRM_API DRM_RESULT DRM_CALL DRM_KG_GenerateContentKeyB64(
    __in_ecount( f_cchKeySeed )            const DRM_WCHAR             *f_pwszKeySeed,          /* base64 encoded string */
    __in                                         DRM_DWORD              f_cchKeySeed,
    __in_ecount( f_cchKeyID )              const DRM_WCHAR             *f_pwszKeyID,            /* base64 encoded string */
    __in                                         DRM_DWORD              f_cchKeyID,
    __in                                         DRM_SUPPORTED_CIPHERS  f_eCipherType,
    __in                                         DRM_DWORD              f_cbContentKey,
    __out_ecount_opt(*f_pcchContentKey)          DRM_WCHAR             *f_pwszContentKey,
    __in                                         DRM_DWORD             *f_pcchContentKey,
    __out_bcount_opt( f_cbScratchKeySeed )       DRM_BYTE              *f_pbScratchKeySeed,
    __in                                         DRM_DWORD              f_cbScratchKeySeed );



DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL DRM_KG_GenerateContentKeyBinary(
    __in_bcount( f_cbKeySeed )             const DRM_BYTE               *f_pbKeySeed,
    __in                                         DRM_DWORD               f_cbKeySeed,
    __in                                   const DRM_KID                *f_pKID,
    __in                                         DRM_SUPPORTED_CIPHERS   f_eCipherType,
    __out_bcount(f_cbContentKey)                 DRM_BYTE               *f_pbContentKey,
    __in                                         DRM_DWORD               f_cbContentKey,
    __out_bcount_opt( f_cbScratchKeySeed )       DRM_BYTE               *f_pbScratchKeySeed,
    __in                                         DRM_DWORD               f_cbScratchKeySeed ) DRM_NO_INLINE_ATTRIBUTE;





EXIT_PK_NAMESPACE;

#endif /*__DRMKEYGEN_H__ */

