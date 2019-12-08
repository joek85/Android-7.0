/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#ifndef __OEMSHA256_H__
#define __OEMSHA256_H__ 1

#include <oemsha256types.h>

ENTER_PK_NAMESPACE;

DRM_API DRM_RESULT DRM_CALL DRM_SHA256_Init(
    __out_ecount( 1 ) DRM_SHA256_CONTEXT *f_pShaContext );

DRM_API DRM_RESULT DRM_CALL DRM_SHA256_Update(
    __inout_ecount( 1 )             DRM_SHA256_CONTEXT *f_pShaContext,
    __in_ecount( f_cbBuffer ) const DRM_BYTE            f_rgbBuffer[],
    __in                            DRM_DWORD           f_cbBuffer );

DRM_API DRM_RESULT DRM_CALL DRM_SHA256_UpdateOffset(
    __inout_ecount( 1 )                                         DRM_SHA256_CONTEXT *f_pShaContext,
    __in_ecount( f_cbBufferRemaining + f_ibBufferOffset ) const DRM_BYTE            f_rgbBuffer[],
    __in                                                        DRM_DWORD           f_cbBufferRemaining,
    __in                                                        DRM_DWORD           f_ibBufferOffset );

DRM_API DRM_RESULT DRM_CALL DRM_SHA256_Finalize(
    __inout_ecount( 1 ) DRM_SHA256_CONTEXT *f_pContext,
    __out_ecount( 1 )   DRM_SHA256_Digest  *f_pDigest );

/* Equivalent to DRM_SHA256_Finalize, but the final hash is only DRM_SHA1_DIGEST_LEN (20) instead of SHA256_DIGEST_SIZE_IN_BYTES (32) */
DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL DRM_SHA256_Finalize_With_SHA_1_Size(
    __inout_ecount( 1 )                 DRM_SHA256_CONTEXT  *f_pContext,
    __out_bcount( DRM_SHA1_DIGEST_LEN ) DRM_BYTE             f_rgbDigest[DRM_SHA1_DIGEST_LEN] ) DRM_NO_INLINE_ATTRIBUTE;


DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL DRM_SHA256_HMAC_Init(
    __inout_ecount( 1 )          DRM_SHA256_HMAC_CONTEXT       *f_pContextHMAC,
    __in_bcount( f_cbKey ) const DRM_BYTE                      *f_pbKey,
    __in                         DRM_DWORD                      f_cbKey ) DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL DRM_SHA256_HMAC_Update(
    __inout_ecount( 1 )           DRM_SHA256_HMAC_CONTEXT  *f_pContextHMAC,
    __in_bcount( f_cbData ) const DRM_BYTE                 *f_pbData,
    __in                          DRM_DWORD                 f_cbData ) DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL DRM_SHA256_HMAC_Finalize(
    __inout_ecount( 1 )           DRM_SHA256_HMAC_CONTEXT  *f_pContextHMAC,
    __out_bcount( f_cbKeyedHash ) DRM_BYTE                 *f_pbKeyedHash,
    __in                          DRM_DWORD                 f_cbKeyedHash ) DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL DRM_SHA256_HMAC_FinalizeOffset(
    __inout_ecount( 1 )                                     DRM_SHA256_HMAC_CONTEXT  *f_pContextHMAC,
    __out_bcount( f_cbKeyedHashRemaining + f_ibKeyedHash )  DRM_BYTE                 *f_pbKeyedHash,
    __in                                                    DRM_DWORD                 f_ibKeyedHash,
    __in                                                    DRM_DWORD                 f_cbKeyedHashRemaining ) DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL DRM_SHA256_HMAC_CreateMAC(
    __in_bcount( f_cbKey )                       const DRM_BYTE                      *f_pbKey,
    __in                                               DRM_DWORD                      f_cbKey,
    __in_bcount( f_cbDataRemaining + f_ibData )  const DRM_BYTE                      *f_pbData,
    __in                                               DRM_DWORD                      f_ibData,
    __in                                               DRM_DWORD                      f_cbDataRemaining,
    __out_bcount( f_cbMACRemaining + f_ibMAC )         DRM_BYTE                      *f_pbMAC,
    __in                                               DRM_DWORD                      f_ibMAC,
    __in                                               DRM_DWORD                      f_cbMACRemaining ) DRM_NO_INLINE_ATTRIBUTE;

DRM_API DRM_RESULT DRM_CALL DRM_SHA256_HMAC_VerifyMAC(
    __in_bcount( f_cbKey )                       const DRM_BYTE                      *f_pbKey,
    __in                                               DRM_DWORD                      f_cbKey,
    __in_bcount( f_cbDataRemaining + f_ibData )  const DRM_BYTE                      *f_pbData,
    __in                                               DRM_DWORD                      f_ibData,
    __in                                               DRM_DWORD                      f_cbDataRemaining,
    __in_bcount( f_cbMACRemaining + f_ibMAC )    const DRM_BYTE                      *f_pbMAC,
    __in                                               DRM_DWORD                      f_ibMAC,
    __in                                               DRM_DWORD                      f_cbMACRemaining ) DRM_NO_INLINE_ATTRIBUTE;

EXIT_PK_NAMESPACE;

#endif /* #ifndef __OEMSHA256_H__ */

