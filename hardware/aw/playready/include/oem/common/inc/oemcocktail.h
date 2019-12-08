/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/


#ifndef __OEMCOCKTAIL_H__
#define __OEMCOCKTAIL_H__

#include <oemcocktailtypes.h>
#include <drmmanagertypes.h>
#include <oemaescommon.h>

ENTER_PK_NAMESPACE;

DRM_API DRM_RESULT DRM_CALL OEM_Cocktail_Init(
    __out_ecount( 1 )                OEM_COCKTAIL_CONTEXT *f_pCocktailContext,
    __in                             DRM_DWORD             f_cbContKey,
    __in_bcount( f_cbContKey ) const DRM_BYTE             *f_pbContKey );

DRM_API DRM_RESULT DRM_CALL OEM_Cocktail_InitDecrypt(
    __inout_ecount( 1 )                                                       OEM_COCKTAIL_CONTEXT *f_pCocktailContext,
    __in_bcount_opt( DRM_MIN( DRM_CPHR_CB_FOR_INITDECRYPT, f_cbData ) ) const DRM_BYTE             *f_pbLast15,
    __in                                                                      DRM_DWORD             f_cbData );

DRM_API DRM_RESULT DRM_CALL OEM_Cocktail_Decrypt(
    __inout_ecount( 1 )        OEM_COCKTAIL_CONTEXT *f_pCocktailContext,
    __in                       DRM_DWORD             f_cbData,
    __inout_bcount( f_cbData ) DRM_BYTE             *f_pbData );

DRM_API DRM_RESULT DRM_CALL OEM_Cocktail_Encrypt(
    __inout_ecount( 1 )        OEM_COCKTAIL_CONTEXT *f_pCocktailContext,
    __in                       DRM_DWORD             f_cbData,
    __inout_bcount( f_cbData ) DRM_BYTE             *f_pbData );

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL OEM_Cocktail_CalculateChecksum(
    __in_bcount( f_cbContentKey )       const DRM_BYTE                         *f_pbContentKey,
    __in                                const DRM_DWORD                         f_cbContentKey,
    __out_bcount_opt( *f_pcbChecksum )        DRM_BYTE                         *f_pbChecksum,
    __inout                                   DRM_DWORD                        *f_pcbChecksum ) DRM_NO_INLINE_ATTRIBUTE;

DRM_API DRM_RESULT DRM_CALL Drm_Cocktail_InitDecrypt(
    __inout_ecount( 1 )                                                    DRM_DECRYPT_CONTEXT *f_pDecryptContext,
    __in_bcount_opt( DRM_MIN(DRM_CPHR_CB_FOR_INITDECRYPT,f_cbData) ) const DRM_BYTE            *f_pbLast15,
    __in                                                                   DRM_DWORD            f_cbData );

DRM_API DRM_RESULT DRM_CALL Drm_Cocktail_Decrypt(
    __in_ecount( 1 )           DRM_DECRYPT_CONTEXT          *f_pDecryptContext,
    __inout_opt                DRM_AES_COUNTER_MODE_CONTEXT *f_pCtrContext,
    __inout_bcount( f_cbData ) DRM_BYTE                     *f_pbData,
    __in                       DRM_DWORD                     f_cbData );

EXIT_PK_NAMESPACE;

#endif /* __OEMCOCKTAIL_H__ */

