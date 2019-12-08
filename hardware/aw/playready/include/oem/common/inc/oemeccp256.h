/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/
#ifndef __OEMECCP256_H__
#define __OEMECCP256_H__

#include <drmerr.h>
#include <ecurve.h>
#include <oemcryptoctx.h>

ENTER_PK_NAMESPACE;

/*
** P256 integer size defines
*/
#define ECC_P256_INTEGER_SIZE_IN_BITS      256
#define ECC_P256_INTEGER_SIZE_IN_BYTES     32
#define ECC_P256_INTEGER_SIZE_IN_DIGITS    ( DRM_BITS_TO_DIGITS( ECC_P256_INTEGER_SIZE_IN_BITS ) )
#define ECC_P256_INTEGER_SIZE_IN_ECAFFINES ( ECC_P256_INTEGER_SIZE_IN_BYTES / sizeof( digit_t ) )
#define ECC_P256_PRIVKEY_SIZE_IN_BITS      ( ECC_P256_INTEGER_SIZE_IN_BITS ) /* privkeys are 256bit integers */
#define ECC_P256_PRIVKEY_SIZE_IN_BYTES     ( ECC_P256_INTEGER_SIZE_IN_BYTES )

/*
** P256 point size defines
*/
#define ECC_P256_POINT_SIZE_IN_BITS        ( 2 * ECC_P256_INTEGER_SIZE_IN_BITS )
#define ECC_P256_POINT_SIZE_IN_BYTES       ( 2 * ECC_P256_INTEGER_SIZE_IN_BYTES )
#define ECC_P256_POINT_SIZE_IN_DIGITS      ( DRM_BITS_TO_DIGITS( ECC_P256_POINT_SIZE_IN_BITS ) )
#define ECC_P256_POINT_SIZE_IN_ECAFFINES   ( 2 * ( ECC_P256_INTEGER_SIZE_IN_BYTES / sizeof( digit_t ) ) )
#define ECC_P256_PUBKEY_SIZE_IN_BITS       ( ECC_P256_POINT_SIZE_IN_BITS ) /* pubkeys are EC Points */
#define ECC_P256_PUBKEY_SIZE_IN_BYTES      ( ECC_P256_POINT_SIZE_IN_BYTES ) /* pubkeys are EC Points */


/*
** P256 integer size defines
*/
#define ECC_P256_CIPHERTEXT_SIZE_IN_BYTES  ( 2 * ECC_P256_POINT_SIZE_IN_BYTES )
#define ECC_P256_PLAINTEXT_SIZE_IN_BYTES   ( ECC_P256_INTEGER_SIZE_IN_BYTES )
#define ECDSA_P256_SIGNATURE_SIZE_IN_BYTES ( ECC_P256_POINT_SIZE_IN_BYTES )

#define ECC_POINT_ADDITION    ( ( DRM_LONG ) 1 )
#define ECC_POINT_SUBTRACTION ( ( DRM_LONG ) -1 )

/*
** Supplied temps needed for certain P256 EC functions. Note these are upper bounds,
** some operations may not require as large of temps, this is a potential optimization.
*/
#define ECC_P256_LNGRED2          ( ( ECC_P256_INTEGER_SIZE_IN_DIGITS + 1 ) / 2 )
#define ECC_P256_NDIGTEMPS        ( ( 19 * ECC_P256_INTEGER_SIZE_IN_DIGITS ) + 6 )
#define ECC_P256_VERIFY_TEMPS     ( ECC_P256_NDIGTEMPS )
#define ECC_P256_SIGN_TEMPS       ( ECC_P256_NDIGTEMPS )
#define ECC_P256_ENCRYPT_TEMPS    ( ECC_P256_NDIGTEMPS )
#define ECC_P256_POINTMUL_TEMPS   ( ECC_P256_NDIGTEMPS )
#define ECC_P256_POINTADD_TEMPS   ( ECC_P256_NDIGTEMPS )
#define ECC_P256_INTEGERADD_TEMPS ( ECC_P256_NDIGTEMPS )
#define ECC_P256_DECRYPT_TEMPS    ( ECC_P256_NDIGTEMPS )
#define ECC_P256_GENKEYPAIR_TEMPS ( ECC_P256_NDIGTEMPS )

/*
** Struct Defines for ECC P256 and the AES XMR struct that will be encrypted and decrypted by it.
*/
typedef struct __tagCIPHERTEXT_P256_2POINTS
{
    DRM_BYTE m_rgbC1[ ECC_P256_POINT_SIZE_IN_BYTES ];
    DRM_BYTE m_rgbC2[ ECC_P256_POINT_SIZE_IN_BYTES ];
} CIPHERTEXT_P256_2POINTS;

typedef struct __tagPUBKEY_P256
{
    DRM_BYTE m_rgbPubkey[ ECC_P256_POINT_SIZE_IN_BYTES ];
} PUBKEY_P256;

typedef struct __tagPRIVKEY_P256
{
    DRM_BYTE m_rgbPrivkey[ ECC_P256_INTEGER_SIZE_IN_BYTES ];
} PRIVKEY_P256;

typedef struct __tagCIPHERTEXT_P256
{
    DRM_BYTE m_rgbCiphertext [ ECC_P256_CIPHERTEXT_SIZE_IN_BYTES ];
} CIPHERTEXT_P256;

typedef struct __tagSIGNATURE_P256
{
    DRM_BYTE m_rgbSignature[ ECDSA_P256_SIGNATURE_SIZE_IN_BYTES ];
} SIGNATURE_P256;

typedef struct __tagPLAINTEXT_P256
{
    /*
    ** HMAC key goes in 1st half, content key goes in 2nd half.
    */
    DRM_BYTE m_rgbPlaintext[ ECC_P256_PLAINTEXT_SIZE_IN_BYTES ];
} PLAINTEXT_P256;

/*
** Function Protos
*/
DRM_API DRM_RESULT DRM_CALL OEM_ECC_GenerateHMACKey_P256(
    __inout PLAINTEXT_P256   *f_pKeys,
    __inout struct bigctx_t  *f_pBigCtx,
    __in    DRM_DWORD         f_cbBigCtx );

DRM_API DRM_RESULT DRM_CALL OEM_ECC_CanMapToPoint_P256(
    __in_ecount( ECC_P256_INTEGER_SIZE_IN_DIGITS ) const  digit_t   f_rgdNumber[],
    __inout                                        struct bigctx_t *f_pBigCtx );

DRM_API DRM_RESULT DRM_CALL OEM_ECC_MapX2PointP256(
    __in_ecount(ECC_P256_INTEGER_SIZE_IN_DIGITS)   const digit_t          *f_pX,
    __out_ecount(ECC_P256_INTEGER_SIZE_IN_DIGITS)        digit_t          *f_pY,
    __inout_opt                                          digit_t           f_rgdSuppliedTemps[],
    __inout                                       struct bigctx_t         *f_pBigCtx );

DRM_API_VOID DRM_VOID DRM_CALL OEM_ECC_ZeroPublicKey_P256(
    __inout        PUBKEY_P256     *f_pPubkey );

DRM_API_VOID DRM_VOID DRM_CALL OEM_ECC_ZeroPrivateKey_P256(
    __inout        PRIVKEY_P256    *f_pPrivkey );

DRM_API DRM_RESULT DRM_CALL OEM_ECC_Decrypt_P256(
    __in    const  PRIVKEY_P256    *f_pPrivkey,
    __in    const  CIPHERTEXT_P256 *f_pCiphertext,
    __out          PLAINTEXT_P256  *f_pPlaintext,
    __inout struct bigctx_t        *f_pBigCtx,
    __in           DRM_DWORD        f_cbBigCtx );


DRM_API DRM_RESULT DRM_CALL OEM_ECC_Encrypt_P256(
    __in    const  PUBKEY_P256      *f_pPubkey,
    __in    const  PLAINTEXT_P256   *f_pPlaintext,
    __out          CIPHERTEXT_P256  *f_pCiphertext,
    __inout struct bigctx_t         *f_pBigCtx,
    __in           DRM_DWORD         f_cbBigCtx );

DRM_API DRM_RESULT DRM_CALL OEM_ECC_GenKeyPair_P256(
    __out          PUBKEY_P256  *f_pPubKey,
    __out          PRIVKEY_P256 *f_pPrivKey,
    __inout struct bigctx_t     *f_pBigCtx,
    __in           DRM_DWORD     f_cbBigCtx );

DRM_API DRM_RESULT DRM_CALL OEM_ECC_GenKeyPairRestrictedPriv_P256(
    __out          PUBKEY_P256  *f_pPubKey,
    __out          PRIVKEY_P256 *f_pPrivKey,
    __inout struct bigctx_t     *f_pBigCtx,
    __in           DRM_DWORD     f_cbBigCtx );

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL OEM_ECDSA_Sign_P256(
    __in_ecount( f_cbMessageLen ) const  DRM_BYTE         f_rgbMessage[],
    __in                          const  DRM_DWORD        f_cbMessageLen,
    __in                          const  PRIVKEY_P256    *f_pPrivkey,
    __out                                SIGNATURE_P256  *f_pSignature,
    __inout                       struct bigctx_t        *f_pBigCtx,
    __in                                 DRM_DWORD        f_cbBigCtx ) DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL OEM_ECDSA_SignHash_P256(
    __in_ecount( f_cbMessageLen ) const  DRM_BYTE         f_rgbMessage[],
    __in                          const  DRM_DWORD        f_cbMessageLen,
    __in                          const  PRIVKEY_P256    *f_pPrivkey,
    __out                                SIGNATURE_P256  *f_pSignature,
    __inout                       struct bigctx_t        *f_pBigCtx,
    __in                                 DRM_DWORD        f_cbBigCtx ) DRM_NO_INLINE_ATTRIBUTE;

DRM_API DRM_RESULT DRM_CALL OEM_ECDSA_Verify_P256(
    __in_ecount( f_cbMessageLen )              const  DRM_BYTE         f_rgbMessage[],
    __in                                       const  DRM_DWORD        f_cbMessageLen,
    __in                                       const  PUBKEY_P256     *f_pPubkey,
    __in                                       const  SIGNATURE_P256  *f_pSignature,
    __inout                                    struct bigctx_t        *f_pBigCtx,
    __in                                              DRM_DWORD        f_cbBigCtx ) DRM_NO_INLINE_ATTRIBUTE;

DRM_API DRM_RESULT DRM_CALL Convert_BigEndianBytesToDigits(
  __in_ecount( ( f_cBits + 7 ) / 8 )              const DRM_BYTE  f_rgbInBytes[],
  __out_ecount( DRM_BITS_TO_DIGITS( f_cBits ) )         digit_t   f_rgdOutDigits[],
  __in                                            const DRM_DWORD f_cBits );

DRM_API DRM_RESULT DRM_CALL Convert_DigitsToBigEndianBytes(
  __in_ecount( DRM_BITS_TO_DIGITS( f_cBits ) )      const digit_t   f_rgdInDigits[],
  __out_ecount( ( f_cBits + 7 ) / 8 )                     DRM_BYTE  f_rgbOutBytes[],
  __in                                              const DRM_DWORD f_cBits );

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL Convert_P256_PointToPlaintext(
    __in_ecount( ECC_P256_POINT_SIZE_IN_ECAFFINES ) const  digit_t          f_rgptPoint[],
    __in_ecount( 1 )                                const  ecurve_t        *f_pEcurve,
    __out_ecount( 1 )                                      PLAINTEXT_P256  *f_pPlaintext ) DRM_NO_INLINE_ATTRIBUTE;


DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL Convert_P256_BigEndianBytesToPoint(
    __in_ecount( ECC_P256_POINT_SIZE_IN_BYTES )              const  DRM_BYTE   f_rgbInBytes[],
    __in_ecount( 1 )                                         const  ecurve_t  *f_pEcurve,
    __out_ecount( ECC_P256_POINT_SIZE_IN_ECAFFINES )                digit_t    f_rgptOutPoint[],
    __inout                                                  struct bigctx_t  *f_pBigCtx ) DRM_NO_INLINE_ATTRIBUTE;

DRM_API DRM_RESULT DRM_CALL Convert_P256_BigEndianBytesToDigits(
    __in_ecount( ECC_P256_INTEGER_SIZE_IN_BYTES )              const DRM_BYTE f_rgdInBytes[],
    __out_ecount( ECC_P256_INTEGER_SIZE_IN_DIGITS )                  digit_t  f_rgbOutDigits[] );

DRM_API DRM_RESULT DRM_CALL Convert_P256_BigEndianBytesToDigitsModOrder(
    __in_ecount( ECC_P256_INTEGER_SIZE_IN_BYTES )   const DRM_BYTE  f_rgbBytes[],
    __in_ecount( 1 )                                const ecurve_t *f_pEcurve,
    __out_ecount( ECC_P256_INTEGER_SIZE_IN_DIGITS )       digit_t   f_rgdDigits[] );

DRM_API DRM_RESULT DRM_CALL Convert_P256_ModularIntToDigitsModOrder(
    __in_ecount( ECC_P256_INTEGER_SIZE_IN_ECAFFINES ) const  digit_t   f_rgecInModularInt[],
    __in_ecount( 1 )                                  const  ecurve_t *f_pEcurve,
    __out_ecount( ECC_P256_INTEGER_SIZE_IN_DIGITS )          digit_t   f_rgbOutDigits[] );

DRM_API DRM_RESULT DRM_CALL Convert_P256_DigitsToBigEndianBytes(
    __in_ecount( ECC_P256_INTEGER_SIZE_IN_DIGITS )              const digit_t  f_rgdInDigits[],
    __out_ecount( ECC_P256_INTEGER_SIZE_IN_BYTES )                    DRM_BYTE f_rgbOutBytes[] );

#define OEM_ECC_ZeroPrivateKey( pprivkey, cbprivkey ) do {                                  \
        if( cbprivkey == sizeof( PRIVKEY ) )                                                \
        {                                                                                   \
            OEM_ECC_ZeroPrivateKey_P160( ( PRIVKEY* )pprivkey );                            \
        }                                                                                   \
        else                                                                                \
        {                                                                                   \
            OEM_ECC_ZeroPrivateKey_P256( ( PRIVKEY_P256* )pprivkey );                       \
        }                                                                                   \
    } while( FALSE )

DRM_EXPORT_VAR extern DRM_GLOBAL_CONST ecurve_t g_Ecurve;

DRM_NO_INLINE DRM_API_VOID DRM_VOID DRM_CALL DRM_RSA_PKInitSize(
    __inout DRM_VOID *f_pContext,
    __in    DRM_DWORD f_dwSize ) DRM_NO_INLINE_ATTRIBUTE;

#define OEM_ECC_PKInit( pContext, pOEMContext  ) OEM_ECC_InitializeBignumStackOEMCtx( pContext, pOEMContext )

/* Zeroes out bignum stack, not thread safe */
DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL OEM_ECC_InitializeBignumStack(
    __inout DRM_VOID* f_pContext ) DRM_NO_INLINE_ATTRIBUTE;

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL OEM_ECC_InitializeBignumStackOEMCtx(
    __inout DRM_VOID* f_pContext,
    __in_opt DRM_VOID* f_pOEMContext ) DRM_NO_INLINE_ATTRIBUTE;

typedef struct __DRMBIGNUM_CONTEXT_STRUCT
{
    DRM_STACK_ALLOCATOR_CONTEXT oHeap;
    union
    {
        DRM_VOID *pOEMContext;
        DRM_VOID *pOEMTEEContext;
    } oContext;
    DRM_BYTE  rgbHeap[ 1 ]; /* DRM_PKCRYPTO_CONTEXT_BUFFER_INTERNAL_SIZE for full RSA operation */
} DRMBIGNUM_CONTEXT_STRUCT;

EXIT_PK_NAMESPACE;

#endif /* __OEMECCP256_H__ */

