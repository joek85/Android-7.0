/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/


#ifndef __OEMSHA1_H__
#define __OEMSHA1_H__

#include <drmtypes.h>

ENTER_PK_NAMESPACE;

#ifndef DRM_SHA1_DIGEST_LEN
#define DRM_SHA1_DIGEST_LEN 20    /* 5 32-bit numbers which is 20 BYTES */
#endif  /* DRM_SHA1_DIGEST_LEN */

#define DRM_SHA1_BLOCK_SIZE 64    /* Sha operates on 64 BYTE blocks */

#define DRM_SHA1_B64ENC_DIGEST_LEN 28 /* CCH_BASE64_EQUIV( DRM_SHA1_DIGEST_LEN ) */


/**********************************************************************
** Struct:      DRM_SHA1_CONTEXT
**
** Synopsis:    Maintains the running state of a sha operations
**
** Members:        [dwHighByteCount] -- High order of the byte count
**                [dwLowByteCount] -- Low order of the byte count
**                [bTempBuffer] -- Maximum temp buffer we may need
**                [ABCDE] -- current SHA state
**
***********************************************************************/
typedef struct __tagDRM_SHA1_CONTEXT
{
    DRM_DWORD dwHighByteCount;
    DRM_DWORD dwLowByteCount;
    DRM_BYTE  bTempBuffer[ DRM_SHA1_BLOCK_SIZE ];
    DRM_DWORD ABCDE[5];
} DRM_SHA1_CONTEXT;

typedef struct __tagDRM_SHA1_HMAC_CONTEXT
{
    DRM_BOOL         fInitialized;
DRM_OBFUS_FILL_BYTES(4)
    DRM_SHA1_CONTEXT shaContext;
    DRM_BYTE         shaDigest[DRM_SHA1_DIGEST_LEN];
    DRM_BYTE         rgbBuffer[DRM_SHA1_BLOCK_SIZE];
} DRM_SHA1_HMAC_CONTEXT;

/*********************************************************************
**
**  Function:  DRM_SHA1_Init
**
**  Synopsis:  Start a clean new instance of Sha
**
**  Arguments:
**     [pShaContext] -- Pointer to context to hold current state of sha
**
**  Returns:  None
**
*********************************************************************/

DRM_API DRM_RESULT DRM_CALL DRM_SHA1_Init(
    __inout DRM_SHA1_CONTEXT* pShaContext );

/*********************************************************************
**
**  Function:  DRM_SHA1_Update
**
**  Synopsis:
**
**  Arguments:
**     [pbData] -- array of bytes to add to the sha computation
**     [cbData] -- length of pbData in bytes
**     [pShaContext] -- running sha context
**
**  Returns:
**
*********************************************************************/

DRM_API DRM_RESULT DRM_CALL DRM_SHA1_Update(
    __in_bcount( cbData ) const DRM_BYTE         *pbData,
    __in                        DRM_DWORD         cbData,
    __inout                     DRM_SHA1_CONTEXT *pShaContext );


DRM_API DRM_RESULT DRM_CALL DRM_SHA1_UpdateOffset(
    __in_bcount( ibData + cbDataRemaining ) const DRM_BYTE         *pbData,
    __in                                          DRM_DWORD         ibData,
    __in                                          DRM_DWORD         cbDataRemaining,
    __inout                                       DRM_SHA1_CONTEXT *pShaContext );

/*********************************************************************
**
**  Function:  DRM_SHA1_Finalize
**
**  Synopsis:
**
**  Arguments:
**     [pShaContext] -- current running sha context
**     [ShaDigest] -- resultant sha digest from sha operation
**
**  Returns:  None
**
*********************************************************************/

DRM_API DRM_RESULT DRM_CALL DRM_SHA1_Finalize(
    __in                                DRM_SHA1_CONTEXT    *pShaContext,
    __out_bcount( DRM_SHA1_DIGEST_LEN ) DRM_BYTE             rgbDigest[ DRM_SHA1_DIGEST_LEN ] );

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL DRM_SHA1_HMAC_Init(
    __inout_ecount( 1 )          DRM_SHA1_HMAC_CONTEXT         *f_pContextHMAC,
    __in_bcount( f_cbKey ) const DRM_BYTE                      *f_pbKey,
    __in                         DRM_DWORD                      f_cbKey ) DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL DRM_SHA1_HMAC_Update(
    __inout_ecount( 1 )           DRM_SHA1_HMAC_CONTEXT  *f_pContextHMAC,
    __in_bcount( f_cbData ) const DRM_BYTE               *f_pbData,
    __in                          DRM_DWORD               f_cbData ) DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL DRM_SHA1_HMAC_Finalize(
    __inout_ecount( 1 )           DRM_SHA1_HMAC_CONTEXT  *f_pContextHMAC,
    __out_bcount( f_cbKeyedHash ) DRM_BYTE               *f_pbKeyedHash,
    __in                          DRM_DWORD               f_cbKeyedHash ) DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL DRM_SHA1_HMAC_FinalizeOffset(
    __inout_ecount( 1 )                                      DRM_SHA1_HMAC_CONTEXT  *f_pContextHMAC,
    __out_bcount( f_cbKeyedHashRemaining + f_ibKeyedHash )   DRM_BYTE               *f_pbKeyedHash,
    __in                                                     DRM_DWORD               f_ibKeyedHash,
    __in                                                     DRM_DWORD               f_cbKeyedHashRemaining ) DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL DRM_SHA1_HMAC_CreateMAC(
    __in_bcount( f_cbKey )                       const DRM_BYTE                      *f_pbKey,
    __in                                               DRM_DWORD                      f_cbKey,
    __in_bcount( f_cbDataRemaining + f_ibData )  const DRM_BYTE                      *f_pbData,
    __in                                               DRM_DWORD                      f_ibData,
    __in                                               DRM_DWORD                      f_cbDataRemaining,
    __out_bcount( f_cbMACRemaining + f_ibMAC )         DRM_BYTE                      *f_pbMAC,
    __in                                               DRM_DWORD                      f_ibMAC,
    __in                                               DRM_DWORD                      f_cbMACRemaining ) DRM_NO_INLINE_ATTRIBUTE;

DRM_API DRM_RESULT DRM_CALL DRM_SHA1_HMAC_VerifyMAC(
    __in_bcount( f_cbKey )                       const DRM_BYTE                      *f_pbKey,
    __in                                               DRM_DWORD                      f_cbKey,
    __in_bcount( f_cbDataRemaining + f_ibData )  const DRM_BYTE                      *f_pbData,
    __in                                               DRM_DWORD                      f_ibData,
    __in                                               DRM_DWORD                      f_cbDataRemaining,
    __in_bcount( f_cbMACRemaining + f_ibMAC )    const DRM_BYTE                      *f_pbMAC,
    __in                                               DRM_DWORD                      f_ibMAC,
    __in                                               DRM_DWORD                      f_cbMACRemaining ) DRM_NO_INLINE_ATTRIBUTE;

EXIT_PK_NAMESPACE;

#endif /* __OEMSHA1_H__ */

