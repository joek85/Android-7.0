/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#ifndef __DRMAESKEYWRAP_H__
#define __DRMAESKEYWRAP_H__

#include <oemaeskeywraptypes.h>
#include <oembroker.h>

ENTER_PK_NAMESPACE;

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL Oem_AesKeyWrap_WrapKeyAES128(
    __in                         const OEM_AES_KEY_CONTEXT       *f_pKey,
    __in_ecount( 1 )             const OEM_UNWRAPPED_KEY_AES_128 *f_pPlaintext,
    __out_ecount( 1 )                  OEM_WRAPPED_KEY_AES_128   *f_pCiphertext ) DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL Oem_AesKeyWrap_WrapKeyECC256(
    __in                         const OEM_AES_KEY_CONTEXT       *f_pKey,
    __in_ecount( 1 )             const OEM_UNWRAPPED_KEY_ECC_256 *f_pPlaintext,
    __out_ecount( 1 )                  OEM_WRAPPED_KEY_ECC_256   *f_pCiphertext ) DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL Oem_AesKeyWrap_UnwrapKeyAES128(
    __in                          const OEM_AES_KEY_CONTEXT       *f_pKey,
    __in_ecount( 1 )              const OEM_WRAPPED_KEY_AES_128   *f_pCiphertext,
    __out_ecount( 1 )                   OEM_UNWRAPPED_KEY_AES_128 *f_pPlaintext ) DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL Oem_AesKeyWrap_UnwrapKeyECC256(
    __in                          const OEM_AES_KEY_CONTEXT       *f_pKey,
    __in_ecount( 1 )              const OEM_WRAPPED_KEY_ECC_256   *f_pCiphertext,
    __out_ecount( 1 )                   OEM_UNWRAPPED_KEY_ECC_256 *f_pPlaintext ) DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL Oem_AesKeyWrap_WrapKeyAES128_Only(
    __in                         const OEM_AES_KEY_CONTEXT            *f_pKey,
    __in_ecount( 1 )             const OEM_UNWRAPPED_KEY_AES_128_ONLY *f_pPlaintext,
    __out_ecount( 1 )                  OEM_WRAPPED_KEY_AES_128_ONLY   *f_pCiphertext ) DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL Oem_AesKeyWrap_UnwrapKeyAES128_Only(
    __in                          const OEM_AES_KEY_CONTEXT            *f_pKey,
    __in_ecount( 1 )              const OEM_WRAPPED_KEY_AES_128_ONLY   *f_pCiphertext,
    __out_ecount( 1 )                   OEM_UNWRAPPED_KEY_AES_128_ONLY *f_pPlaintext ) DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL Oem_AesKeyWrap_UnwrapKeyECC256_Only(
    __in                          const OEM_AES_KEY_CONTEXT            *f_pKey,
    __in_ecount( 1 )              const OEM_WRAPPED_KEY_ECC_256_ONLY   *f_pCiphertext,
    __out_ecount( 1 )                   OEM_UNWRAPPED_KEY_ECC_256_ONLY *f_pPlaintext ) DRM_NO_INLINE_ATTRIBUTE;

EXIT_PK_NAMESPACE;

#endif /* __DRMAESKEYWRAP_H__ */
