/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#ifndef _OEMTEE_H_
#define _OEMTEE_H_ 1

#include <oemteetypes.h>
#include <oemeccp256.h>
#include <oemaes.h>
#include <oemsha256types.h>
#include <drmteetypes.h>
#include <drmteeproxystubcommon.h>

ENTER_PK_NAMESPACE;

PREFAST_PUSH_DISABLE_EXPLAINED(__WARNING_NONCONST_PARAM_25004, "Out params can't be const")
PREFAST_PUSH_DISABLE_EXPLAINED(__WARNING_NONCONST_BUFFER_PARAM_25033, "Out params can't be const")

/*
** Memory operations.
*/

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL OEM_TEE_BASE_AllocTEEContext(
    __inout                                         OEM_TEE_CONTEXT            *f_pContext ) DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_API_VOID DRM_VOID DRM_CALL OEM_TEE_BASE_FreeTEEContext(
    __inout_opt                                     OEM_TEE_CONTEXT            *f_pContextAllowNULL ) DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL OEM_TEE_BASE_GetVersion(
    __inout                                           OEM_TEE_CONTEXT          *f_pContext,
    __out                                             DRM_DWORD                *f_pdwVersion ) DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL OEM_TEE_BASE_GetVersionInformation(
    __inout                                           OEM_TEE_CONTEXT          *f_pContext,
    __out                                             DRM_DWORD                *f_pchManufacturerName,
    __deref_out_ecount( *f_pchManufacturerName )      DRM_WCHAR               **f_ppwszManufacturerName,
    __out                                             DRM_DWORD                *f_pchModelName,
    __deref_out_ecount( *f_pchModelName )             DRM_WCHAR               **f_ppwszModelName,
    __out                                             DRM_DWORD                *f_pchModelNumber,
    __deref_out_ecount( *f_pchModelNumber )           DRM_WCHAR               **f_ppwszModelNumber,
    __out_ecount( DRM_TEE_METHOD_FUNCTION_MAP_COUNT ) DRM_DWORD                *f_pdwFunctionMap,
    __out                                             DRM_DWORD                *f_pcbProperties,
    __deref_out_bcount( *f_pcbProperties )            DRM_BYTE                **f_ppbProperties ) DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL OEM_TEE_BASE_SecureMemAlloc(
    __inout_opt                                     OEM_TEE_CONTEXT            *f_pContextAllowNULL,
    __in                                            DRM_DWORD                   f_cbSize,
    __out                                           DRM_VOID                  **f_ppv ) DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_API_VOID DRM_VOID DRM_CALL OEM_TEE_BASE_SecureMemFree(
    __inout_opt                                     OEM_TEE_CONTEXT            *f_pContextAllowNULL,
    __inout                                         DRM_VOID                  **f_ppv ) DRM_NO_INLINE_ATTRIBUTE;

PREFAST_PUSH_DISABLE_EXPLAINED(__WARNING_COUNT_REQUIRED_FOR_VOIDPTR_BUFFER_25120, "OEM_TEE_MEMORY_HANDLE is OEM-defined and cannot be SAL annotated for size.");
DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL OEM_TEE_BASE_SecureMemHandleAlloc(
    __inout                                         OEM_TEE_CONTEXT            *f_pContext,
    __in                                            DRM_DWORD                   f_dwDecryptionMode,
    __in                                            DRM_DWORD                   f_cbSize,
    __out                                           DRM_DWORD                  *f_pcbMemHandle,
    __out                                           OEM_TEE_MEMORY_HANDLE      *f_pMemHandle ) DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_API_VOID DRM_VOID DRM_CALL OEM_TEE_BASE_SecureMemHandleFree(
    __inout_opt                                     OEM_TEE_CONTEXT            *f_pContextAllowNULL,
    __inout                                         OEM_TEE_MEMORY_HANDLE      *f_pMemHandle ) DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_API DRM_DWORD DRM_CALL OEM_TEE_BASE_SecureMemHandleGetSize(
    __inout                                         OEM_TEE_CONTEXT            *f_pContext,
    __in                                      const OEM_TEE_MEMORY_HANDLE       f_hMem ) DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL OEM_TEE_BASE_SecureMemHandleWrite(
    __inout                                         OEM_TEE_CONTEXT            *f_pContext,
    __in                                            DRM_DWORD                   f_ibWrite,
    __in                                            DRM_DWORD                   f_cbWrite,
    __in_bcount( f_cbWrite )                  const DRM_BYTE                   *f_pbWrite,
    __inout                                         OEM_TEE_MEMORY_HANDLE       f_hMem ) DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL OEM_TEE_BASE_SecureMemHandleRead(
    __inout                                         OEM_TEE_CONTEXT            *f_pContext,
    __in                                            DRM_DWORD                   f_ibRead,
    __in                                            DRM_DWORD                   f_cbRead,
    __out_bcount( f_cbRead )                        DRM_BYTE                   *f_pbRead,
    __in                                      const OEM_TEE_MEMORY_HANDLE       f_hMem ) DRM_NO_INLINE_ATTRIBUTE;
PREFAST_POP;  /* __WARNING_COUNT_REQUIRED_FOR_VOIDPTR_BUFFER_25120 */

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL OEM_TEE_BASE_AllocKeyAES128(
    __inout_opt                                     OEM_TEE_CONTEXT            *f_pContextAllowNULL,
    __out                                           OEM_TEE_KEY                *f_pKey ) DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL OEM_TEE_BASE_AllocKeyECC256(
    __inout                                         OEM_TEE_CONTEXT            *f_pContext,
    __out                                           OEM_TEE_KEY                *f_pKey ) DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_API_VOID DRM_VOID DRM_CALL OEM_TEE_BASE_FreeKeyAES128(
    __inout_opt                                     OEM_TEE_CONTEXT            *f_pContextAllowNULL,
    __inout                                         OEM_TEE_KEY                *f_pKey ) DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_API_VOID DRM_VOID DRM_CALL OEM_TEE_BASE_FreeKeyECC256(
    __inout                                         OEM_TEE_CONTEXT            *f_pContext,
    __inout                                         OEM_TEE_KEY                *f_pKey ) DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL OEM_TEE_BASE_CopyKeyAES128(
    __inout                                         OEM_TEE_CONTEXT            *f_pContext,
    __inout                                         OEM_TEE_KEY                *f_pPreallocatedDestKey,
    __in                                      const OEM_TEE_KEY                *f_pSourceKey ) DRM_NO_INLINE_ATTRIBUTE;

/*
** Basic key operations
*/
DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL OEM_TEE_BASE_GetCTKID(
    __inout_opt                                     OEM_TEE_CONTEXT            *f_pContextAllowNULL,
    __out                                           DRM_DWORD                  *f_pdwidCTK ) DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL OEM_TEE_BASE_GetTKByID(
    __inout_opt                                     OEM_TEE_CONTEXT            *f_pContextAllowNULL,
    __in                                            DRM_DWORD                   f_dwid,
    __inout                                         OEM_TEE_KEY                *f_pTK ) DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL OEM_TEE_BASE_DeriveKey(
    __inout_opt                                     OEM_TEE_CONTEXT            *f_pContextAllowNULL,
    __in                                      const OEM_TEE_KEY                *f_pTKDeriver,
    __in                                      const DRM_ID                     *f_pidLabel,
    __in_opt                                  const DRM_ID                     *f_pidContext,
    __inout                                         OEM_TEE_KEY                *f_pTKDerived ) DRM_NO_INLINE_ATTRIBUTE;

/*
** Signing and encryption operations
*/
DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL OEM_TEE_BASE_VerifyOMAC1Signature(
    __inout_opt                                     OEM_TEE_CONTEXT            *f_pContextAllowNULL,
    __in                                      const OEM_TEE_KEY                *f_pKey,
    __in                                            DRM_DWORD                   f_cbData,
    __in_bcount( f_cbData )                   const DRM_BYTE                   *f_pbData,
    __in                                            DRM_DWORD                   f_cbSignature,
    __in_bcount( f_cbSignature )              const DRM_BYTE                   *f_pbSignature) DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL OEM_TEE_BASE_SignWithOMAC1(
    __inout_opt                                     OEM_TEE_CONTEXT            *f_pContextAllowNULL,
    __in                                      const OEM_TEE_KEY                *f_pKey,
    __in                                            DRM_DWORD                   f_cbDataToSign,
    __in_bcount( f_cbDataToSign )             const DRM_BYTE                   *f_pbDataToSign,
    __out_bcount( DRM_AES128OMAC1_SIZE_IN_BYTES )   DRM_BYTE                   *f_pbSignature ) DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL OEM_TEE_BASE_SignHashWithDeviceSigningKey(
    __inout                                             OEM_TEE_CONTEXT        *f_pContext,
    __in                                          const OEM_TEE_KEY            *f_pSigningKey,
    __in_bcount( ECC_P256_INTEGER_SIZE_IN_BYTES ) const DRM_BYTE                f_rgbHashToSign[ECC_P256_INTEGER_SIZE_IN_BYTES],
    __out                                               SIGNATURE_P256         *f_pSignature ) DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL OEM_TEE_BASE_ECDSA_P256_Verify(
    __inout_opt                                     OEM_TEE_CONTEXT            *f_pContextAllowNULL,
    __in                                            DRM_DWORD                   f_cbDataToVerify,
    __in_bcount( f_cbDataToVerify )           const DRM_BYTE                   *f_pbDataToVerify,
    __in                                      const PUBKEY_P256                *f_pPubKey,
    __in                                      const SIGNATURE_P256             *f_pSignature ) DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL OEM_TEE_BASE_ECC256_Encrypt_AES128Keys(
    __inout                                         OEM_TEE_CONTEXT            *f_pContext,
    __in                                      const PUBKEY_P256                *f_pPubkey,
    __in_opt                                  const OEM_TEE_KEY                *f_pKey1,
    __in                                      const OEM_TEE_KEY                *f_pKey2,
    __out                                           CIPHERTEXT_P256            *f_pCiphertext ) DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL OEM_TEE_BASE_ECC256_Decrypt_AES128Keys(
    __inout                                         OEM_TEE_CONTEXT            *f_pContext,
    __in                                      const OEM_TEE_KEY                *f_pPrivKey,
    __in                                      const CIPHERTEXT_P256            *f_pEncryptedKeys,
    __inout_opt                                     OEM_TEE_KEY                *f_pKey1,
    __inout                                         OEM_TEE_KEY                *f_pKey2 ) DRM_NO_INLINE_ATTRIBUTE;

DRM_API DRM_RESULT DRM_CALL OEM_TEE_BASE_AES128CBC_EncryptData(
    __inout_opt                                     OEM_TEE_CONTEXT            *f_pContextAllowNULL,
    __in                                      const OEM_TEE_KEY                *f_pKey,
    __in_bcount( DRM_AES_BLOCKLEN )           const DRM_BYTE                    f_rgbIV[ DRM_AES_BLOCKLEN ],
    __in                                            DRM_DWORD                   f_cbData,
    __inout_bcount( f_cbData )                      DRM_BYTE                   *f_pbData ) DRM_NO_INLINE_ATTRIBUTE;

DRM_API DRM_RESULT DRM_CALL OEM_TEE_BASE_AES128CBC_DecryptData(
    __inout_opt                                     OEM_TEE_CONTEXT            *f_pContextAllowNULL,
    __in                                      const OEM_TEE_KEY                *f_pKey,
    __in_bcount( DRM_AES_BLOCKLEN )           const DRM_BYTE                    f_rgbIV[ DRM_AES_BLOCKLEN ],
    __in                                            DRM_DWORD                   f_cbData,
    __inout_bcount( f_cbData )                      DRM_BYTE                   *f_pbData ) DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL OEM_TEE_BASE_AES128ECB_EncryptData(
    __inout                                         OEM_TEE_CONTEXT            *f_pContext,
    __in                                      const OEM_TEE_KEY                *f_pKey,
    __in                                            DRM_DWORD                   f_cbData,
    __inout_bcount( f_cbData )                      DRM_BYTE                   *f_pbData ) DRM_NO_INLINE_ATTRIBUTE;

/*
** Random Number/Key Generation operations
*/
DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL OEM_TEE_BASE_GenerateRandomBytes(
    __inout_opt                                     OEM_TEE_CONTEXT            *f_pContextAllowNULL,
    __in                                            DRM_DWORD                   f_cb,
    __out_bcount( f_cb )                            DRM_BYTE                   *f_pb ) DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL OEM_TEE_BASE_GenerateRandomAES128Key(
    __inout                                         OEM_TEE_CONTEXT            *f_pContext,
    __inout                                         OEM_TEE_KEY                *f_pKey ) DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL OEM_TEE_BASE_GenerateRandomAES128KeyPair(
    __inout                                         OEM_TEE_CONTEXT            *f_pContext,
    __inout                                         OEM_TEE_KEY                *f_pKey1,
    __inout                                         OEM_TEE_KEY                *f_pKey2 ) DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL OEM_TEE_BASE_GenerateRandomAES128KeyPairAndAES128ECBEncrypt(
    __inout                                         OEM_TEE_CONTEXT            *f_pContext,
    __in                                      const OEM_TEE_KEY                *f_pEncryptingKey,
    __inout                                         OEM_TEE_KEY                *f_pKey1,
    __inout                                         OEM_TEE_KEY                *f_pKey2,
    __out_bcount( DRM_AES_KEYSIZE_128_X2 )          DRM_BYTE                   *f_pEncryptedKeys ) DRM_NO_INLINE_ATTRIBUTE;

/*
** Device Info operations specific to this device instance
*/
DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL OEM_TEE_BASE_GetDeviceUniqueID(
    __inout                                         OEM_TEE_CONTEXT            *f_pContext,
    __out                                           DRM_ID                     *f_pID ) DRM_NO_INLINE_ATTRIBUTE;

/*
** AES128CTR Playback operations
*/
DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL OEM_TEE_AES128CTR_BuildExternalPolicyCryptoInfo(
    __inout                                         OEM_TEE_CONTEXT            *f_pContext,
    __in_opt                                  const OEM_TEE_KEY                *f_pContentKey,
    __in_opt                                  const OEM_TEE_KEY                *f_pSampleProtectionKey,
    __in                                            DRM_DWORD                   f_cbOEMData,
    __in_bcount_opt( f_cbOEMData )            const DRM_BYTE                   *f_pbOEMData,
    __in                                      const DRM_TEE_POLICY_INFO        *f_pPolicy,
    __out                                           DRM_DWORD                  *f_pcbInfo,
    __deref_out_bcount( *f_pcbInfo )                DRM_BYTE                  **f_ppbInfo ) DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_API_VOID DRM_VOID DRM_CALL OEM_TEE_AES128CTR_RemapRequestedDecryptionModeToSupportedDecryptionMode(
    __inout                                         OEM_TEE_CONTEXT            *f_pContext,
    __inout                                         DRM_DWORD                  *f_pdwDecryptionMode ) DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL OEM_TEE_AES128CTR_DecryptContentKeysWithLicenseKey(
    __inout                                         OEM_TEE_CONTEXT            *f_pContext,
    __in                                      const OEM_TEE_KEY                *f_pRootCK,
    __in                                            DRM_DWORD                   f_cbCiphertext,
    __in_bcount( f_cbCiphertext )             const DRM_BYTE                   *f_pbCiphertext,
    __inout                                         OEM_TEE_KEY                *f_pCI,
    __inout                                         OEM_TEE_KEY                *f_pCK ) DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL OEM_TEE_AES128CTR_VerifyLicenseChecksum(
    __inout                                         OEM_TEE_CONTEXT            *f_pContext,
    __in                                      const OEM_TEE_KEY                *f_pCK,
    __in_bcount( sizeof(DRM_ID) )             const DRM_BYTE                   *f_pbKID,
    __in_bcount( f_cbChecksum )               const DRM_BYTE                   *f_pbChecksum,
    __in                                            DRM_DWORD                   f_cbChecksum ) DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL OEM_TEE_AES128CTR_DecryptContentIntoClear(
    __inout                                         OEM_TEE_CONTEXT            *f_pContext,
    __in                                      const OEM_TEE_KEY                *f_pCK,
    __in                                            DRM_DWORD                   f_cEncryptedRegionMappings,
    __in_ecount( f_cEncryptedRegionMappings ) const DRM_DWORD                  *f_pdwEncryptedRegionMappings,
    __in                                            DRM_UINT64                  f_ui64InitializationVector,
    __in                                            DRM_DWORD                   f_cbEncryptedToClear,
    __inout_bcount( f_cbEncryptedToClear )          DRM_BYTE                   *f_pbEncryptedToClear ) DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL OEM_TEE_AES128CTR_DecryptContentIntoHandle(
    __inout                                         OEM_TEE_CONTEXT            *f_pContext,
    __in                                      const OEM_TEE_KEY                *f_pCK,
    __in                                            DRM_DWORD                   f_cEncryptedRegionMappings,
    __in_ecount( f_cEncryptedRegionMappings ) const DRM_DWORD                  *f_pdwEncryptedRegionMappings,
    __in                                            DRM_UINT64                  f_ui64InitializationVector,
    __in                                            DRM_DWORD                   f_cbEncrypted,
    __in_bcount( f_cbEncrypted )              const DRM_BYTE                   *f_pbEncrypted,
    __inout                                         OEM_TEE_MEMORY_HANDLE       f_hClear ) DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL OEM_TEE_AES128CTR_UnshuffleScalableContentKeys(
    __inout                                        OEM_TEE_CONTEXT             *f_pContext,
    __inout                                        OEM_TEE_KEY                 *f_pCI,
    __inout                                        OEM_TEE_KEY                 *f_pCK ) DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL OEM_TEE_AES128CTR_ShuffleScalableContentKeys(
    __inout                                        OEM_TEE_CONTEXT             *f_pContext,
    __inout                                        OEM_TEE_KEY                 *f_pCI,
    __inout                                        OEM_TEE_KEY                 *f_pCK ) DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL OEM_TEE_AES128CTR_CalculateContentKeyPrimeWithAES128Key(
    __inout                                        OEM_TEE_CONTEXT             *f_pContext,
    __in                                     const OEM_TEE_KEY                 *f_pContentKey,
    __in_bcount( DRM_AES_KEYSIZE_128 )       const DRM_BYTE                    *f_pbPrime,
    __inout                                        OEM_TEE_KEY                 *f_pContentKeyPrime ) DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL OEM_TEE_AES128CTR_DeriveScalableKeyWithAES128Key(
    __inout                                        OEM_TEE_CONTEXT             *f_pContext,
    __in                                     const OEM_TEE_KEY                 *f_pEncKey,
    __in_bcount( DRM_AES_KEYSIZE_128 )       const DRM_BYTE                    *f_pbKey,
    __inout                                        OEM_TEE_KEY                 *f_pDerivedScalableKey ) DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL OEM_TEE_AES128CTR_InitUplinkXKey(
    __inout                                        OEM_TEE_CONTEXT             *f_pContext,
    __inout                                        OEM_TEE_KEY                 *f_pUplinkXKey ) DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL OEM_TEE_AES128CTR_UpdateUplinkXKey(
    __inout                                        OEM_TEE_CONTEXT             *f_pContext,
    __in                                     const OEM_TEE_KEY                 *f_pLocationKey,
    __inout                                        OEM_TEE_KEY                 *f_pUplinkXKey ) DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL OEM_TEE_AES128CTR_DecryptContentKeysWithDerivedKeys(
    __inout                                        OEM_TEE_CONTEXT             *f_pContext,
    __in                                     const OEM_TEE_KEY                 *f_pSecondaryKey,
    __in                                     const OEM_TEE_KEY                 *f_pUplinkxKey,
    __in_bcount( DRM_AES_KEYSIZE_128_X2 )    const DRM_BYTE                    *f_pbCiphertext,
    __inout                                        OEM_TEE_KEY                 *f_pCI,
    __inout                                        OEM_TEE_KEY                 *f_pCK ) DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL OEM_TEE_AES128CTR_EnforcePolicy(
    __inout                                        OEM_TEE_CONTEXT             *f_pContext,
    __in                                     const DRM_TEE_POLICY_INFO         *f_pPolicy ) DRM_NO_INLINE_ATTRIBUTE;

/*
** Clock operations
*/
DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL OEM_TEE_CLOCK_GetSecurelySetSystemTime(
    __inout                                         OEM_TEE_CONTEXT            *f_pContext,
    __out                                           DRMFILETIME                *f_pftSystemTime ) DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL OEM_TEE_CLOCK_GetSystemTime(
    __inout                                         OEM_TEE_CONTEXT            *f_pContext,
    __out                                           DRMFILETIME                *f_pftSystemTime ) DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL OEM_TEE_CLOCK_SetSecureSystemTime(
    __inout                                         OEM_TEE_CONTEXT            *f_pContext,
    __in                                      const DRMFILETIME                *f_pftSystemTime ) DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_API DRM_BOOL DRM_CALL OEM_TEE_CLOCK_SecureClockNeedsReSync(
    __inout                                        OEM_TEE_CONTEXT             *f_pContext) DRM_NO_INLINE_ATTRIBUTE;

/*
** Domain operations
*/
DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL OEM_TEE_DOM_DecryptDomainKeyWithSessionKeys(
    __inout                                             OEM_TEE_CONTEXT       *f_pContext,
    __in                                          const OEM_TEE_KEY           *f_pSessionKey1,
    __in                                          const OEM_TEE_KEY           *f_pSessionKey2,
    __in_bcount( ECC_P256_PRIVKEY_SIZE_IN_BYTES ) const DRM_BYTE              *f_pbEncryptedDomainKey,
    __inout                                             OEM_TEE_KEY           *f_pDomainPrivKey ) DRM_NO_INLINE_ATTRIBUTE;

/*
** H.264 Operations
*/
DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL OEM_TEE_H264_PreProcessEncryptedData(
    __inout                                              OEM_TEE_CONTEXT       *f_pContext,
    __in                                           const OEM_TEE_KEY           *f_pCK,
    __in                                                 DRM_DWORD              f_cbOEMKeyInfo,
    __in_bcount( f_cbOEMKeyInfo )                  const DRM_BYTE              *f_pbOEMKeyInfo,
    __inout                                              DRM_UINT64            *f_pui64InitializationVector,
    __in                                                 DRM_DWORD              f_cEncryptedRegionMappings,
    __in_ecount( f_cEncryptedRegionMappings )      const DRM_DWORD             *f_pdwEncryptedRegionMappings,
    __out                                                DRM_DWORD             *f_pcbOpaqueFrameData,
    __deref_out_bcount( *f_pcbOpaqueFrameData )          DRM_BYTE             **f_ppbOpaqueFrameData,
    __in                                                 DRM_DWORD              f_cbEncryptedTranscryptedFullFrame,
    __inout_bcount( f_cbEncryptedTranscryptedFullFrame ) DRM_BYTE              *f_pbEncryptedTranscryptedFullFrame ) DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL OEM_TEE_H264_DecryptContentFragmentIntoClear(
    __inout                                         OEM_TEE_CONTEXT            *f_pContext,
    __in                                      const OEM_TEE_KEY                *f_pCK,
    __in                                            DRM_UINT64                  f_ui64InitializationVector,
    __in                                            DRM_UINT64                  f_ui64Offset,
    __in                                            DRM_BYTE                    f_bOffset,
    __in                                            DRM_DWORD                   f_cbFragmentEncryptedToClear,
    __inout_bcount( f_cbFragmentEncryptedToClear )  DRM_BYTE                   *f_pbFragmentEncryptedToClear ) DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_API_VOID DRM_VOID DRM_CALL OEM_TEE_H264_ZERO_MEMORY(
    __inout_opt                                          OEM_TEE_CONTEXT       *f_pContextAllowNULL,
    __in                                                 DRM_DWORD              f_cbEncryptedTranscryptedFullFrame,
    __inout_bcount( f_cbEncryptedTranscryptedFullFrame ) DRM_BYTE              *f_pbEncryptedTranscryptedFullFrame ) DRM_NO_INLINE_ATTRIBUTE;

/*
** Key wrapping and unwraping operations
*/
DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL OEM_TEE_BASE_WrapAES128KeyForPersistedStorage(
    __inout                                         OEM_TEE_CONTEXT            *f_pContext,
    __in                                      const OEM_TEE_KEY                *f_pWrappingKey,
    __in                                      const OEM_TEE_KEY                *f_pKeyToBeWrapped,
    __inout                                         DRM_DWORD                  *f_pibWrappedKeyBuffer,
    __inout_opt                                     DRM_BYTE                   *f_pbWrappedKeyBuffer ) DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL OEM_TEE_BASE_WrapECC256KeyForPersistedStorage(
    __inout                                         OEM_TEE_CONTEXT            *f_pContext,
    __in                                      const OEM_TEE_KEY                *f_pWrappingKey,
    __in                                      const OEM_TEE_KEY                *f_pKeyToBeWrapped,
    __inout                                         DRM_DWORD                  *f_pibWrappedKeyBuffer,
    __inout_opt                                     DRM_BYTE                   *f_pbWrappedKeyBuffer ) DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL OEM_TEE_BASE_WrapAES128KeyForTransientStorage(
    __inout                                         OEM_TEE_CONTEXT            *f_pContext,
    __in                                      const OEM_TEE_KEY                *f_pWrappingKey,
    __in                                      const OEM_TEE_KEY                *f_pKeyToBeWrapped,
    __inout                                         DRM_DWORD                  *f_pibWrappedKeyBuffer,
    __inout_opt                                     DRM_BYTE                   *f_pbWrappedKeyBuffer ) DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL OEM_TEE_BASE_UnwrapAES128KeyFromPersistedStorage(
    __inout                                         OEM_TEE_CONTEXT            *f_pContext,
    __in                                      const OEM_TEE_KEY                *f_pWrappingKey,
    __inout                                         DRM_DWORD                  *f_pcbWrappedKeyBuffer,
    __inout                                         DRM_DWORD                  *f_pibWrappedKeyBuffer,
    __in                                      const DRM_BYTE                   *f_pbWrappedKeyBuffer,
    __inout                                         OEM_TEE_KEY                *f_pPreallocatedUnwrappedKey ) DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL OEM_TEE_BASE_UnwrapECC256KeyFromPersistedStorage(
    __inout                                         OEM_TEE_CONTEXT            *f_pContext,
    __in                                      const OEM_TEE_KEY                *f_pWrappingKey,
    __inout                                         DRM_DWORD                  *f_pcbWrappedKeyBuffer,
    __inout                                         DRM_DWORD                  *f_pibWrappedKeyBuffer,
    __in                                      const DRM_BYTE                   *f_pbWrappedKeyBuffer,
    __inout                                         OEM_TEE_KEY                *f_pPreallocatedUnwrappedKey ) DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL OEM_TEE_BASE_UnwrapAES128KeyFromTransientStorage(
    __inout_opt                                     OEM_TEE_CONTEXT            *f_pContextAllowNULL,
    __in                                      const OEM_TEE_KEY                *f_pWrappingKey,
    __inout                                         DRM_DWORD                  *f_pcbWrappedKeyBuffer,
    __inout                                         DRM_DWORD                  *f_pibWrappedKeyBuffer,
    __in                                      const DRM_BYTE                   *f_pbWrappedKeyBuffer,
    __inout                                         OEM_TEE_KEY                *f_pPreallocatedUnwrappedKey ) DRM_NO_INLINE_ATTRIBUTE;

/*
** License Generation operations
*/
DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL OEM_TEE_LICGEN_EncryptContentOutOfClear(
    __inout                                            OEM_TEE_CONTEXT         *f_pContext,
    __in                                         const OEM_TEE_KEY             *f_pCK,
    __in                                               DRM_DWORD                f_cEncryptedRegionMappings,
    __in_ecount( f_cEncryptedRegionMappings )    const DRM_DWORD               *f_pdwEncryptedRegionMappings,
    __in                                               DRM_DWORD                f_cbClearToEncrypted,
    __inout_bcount( f_cbClearToEncrypted )             DRM_BYTE                *f_pbClearToEncrypted,
    __out                                              DRM_UINT64              *f_pui64InitializationVector ) DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL OEM_TEE_LICGEN_EncryptContentOutOfHandle(
    __inout                                            OEM_TEE_CONTEXT         *f_pContext,
    __in                                         const OEM_TEE_KEY             *f_pCK,
    __in                                               DRM_DWORD                f_cEncryptedRegionMappings,
    __in_ecount( f_cEncryptedRegionMappings )    const DRM_DWORD               *f_pdwEncryptedRegionMappings,
    __in                                         const OEM_TEE_MEMORY_HANDLE    f_hClear,
    __out                                              DRM_DWORD               *f_pcbEncrypted,
    __deref_out_bcount( *f_pcbEncrypted )              DRM_BYTE               **f_ppbEncrypted,
    __out                                              DRM_UINT64              *f_pui64InitializationVector ) DRM_NO_INLINE_ATTRIBUTE;

/*
** All types of provisioning operations
*/
DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL OEM_TEE_PROV_GenerateRandomECC256KeyPair(
    __inout                                         OEM_TEE_CONTEXT            *f_pContext,
    __inout                                         OEM_TEE_KEY                *f_pKey,
    __out                                           PUBKEY_P256                *f_pPubKey ) DRM_NO_INLINE_ATTRIBUTE;

/*
** Local Provisioning operations
*/
DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL OEM_TEE_LPROV_ECDSA_Sign(
    __inout_opt                                     OEM_TEE_CONTEXT            *f_pContextAllowNULL,
    __in                                            DRM_DWORD                   f_cbToSign,
    __in_bcount( f_cbToSign )                 const DRM_BYTE                   *f_pbToSign,
    __in                                      const OEM_TEE_KEY                *f_pECC256SigningKey,
    __out                                           SIGNATURE_P256             *f_pSignature ) DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL OEM_TEE_LPROV_UnprotectModelPrivateSigningKey(
    __inout                                                                      OEM_TEE_CONTEXT   *f_pContext,
    __in                                                                         DRM_DWORD          f_cbOEMProtectedModelCertPrivateKey,
    __in_bcount( f_cbOEMProtectedModelCertPrivateKey )                     const DRM_BYTE          *f_pbOEMProtectedModelCertPrivateKey,
    __out                                                                        DRM_DWORD         *f_pcbOEMDefaultProtectedModelCertPrivateKey,
    __deref_out_bcount_opt( *f_pcbOEMDefaultProtectedModelCertPrivateKey )       DRM_BYTE         **f_ppbOEMDefaultProtectedModelCertPrivateKey,
    __inout                                                                      OEM_TEE_KEY       *f_pModelCertPrivateKey ) DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL OEM_TEE_LPROV_UnprotectModelCertificate(
    __inout                                            OEM_TEE_CONTEXT         *f_pContext,
    __in                                               DRM_DWORD                f_cbInputModelCertificate,
    __in_bcount( f_cbInputModelCertificate )     const DRM_BYTE                *f_pbInputModelCertificate,
    __out                                              DRM_DWORD               *f_pcbModelCertificate,
    __deref_out_bcount( *f_pcbModelCertificate )       DRM_BYTE               **f_ppbModelCertificate ) DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL OEM_TEE_LPROV_GetDeviceModelInfo(
    __inout                                         OEM_TEE_CONTEXT            *f_pContext,
    __inout                                         DRM_DWORD                  *f_pcchModelInfo,
    __out_ecount_opt( *f_pcchModelInfo )            DRM_WCHAR                  *f_pwchModelInfo ) DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL OEM_TEE_LPROV_GetModelSecurityVersion(
    __inout                                         OEM_TEE_CONTEXT            *f_pContext,
    __out                                           DRM_DWORD                  *f_pdwSecurityVersion,
    __out                                           DRM_DWORD                  *f_pdwPlatformID ) DRM_NO_INLINE_ATTRIBUTE;

/*
** PRND TX Operations
*/
DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL OEM_TEE_PRND_TX_CheckReceiverSecurityLevel(
    __inout                                         OEM_TEE_CONTEXT            *f_pContext,
    __in                                            DRM_DWORD                   f_dwReceiverSecurityLevel ) DRM_NO_INLINE_ATTRIBUTE;

/*
** Remote provisioning operations
*/
DRM_NO_INLINE DRM_API DRM_BOOL DRM_CALL OEM_TEE_RPROV_IsBootstrapNecessary() DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL OEM_TEE_RPROV_ProcessResponse(
    __inout                                       OEM_TEE_CONTEXT           *f_pOemTeeCtx,
    __in_bcount( f_cbResponse )             const DRM_BYTE                  *f_pbResponse,
    __in                                          DRM_DWORD                  f_cbResponse,
    __in_ecount( RPROV_KEYPAIR_COUNT )      const PUBKEY_P256               *f_poPubkeys,
    __deref_out_bcount( *f_pcbCert )              DRM_BYTE                 **f_ppbCert,
    __out_ecount( 1 )                             DRM_DWORD                 *f_pcbCert ) DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL OEM_TEE_RPROV_WrapProvisioningRequest(
    __inout                                       OEM_TEE_CONTEXT           *f_pOemTeeCtx,
    __deref_inout_bcount( *f_pcbOutputMessage )   DRM_BYTE                 **f_ppbOutputMessage,
    __inout_ecount( 1 )                           DRM_DWORD                 *f_pcbOutputMessage ) DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL OEM_TEE_RPROV_GenerateBootstrapChallenge(
    __inout                                         OEM_TEE_CONTEXT            *f_pContext,
    __inout                                         DRM_DWORD                  *f_pcbRProvContext,
    __deref_inout_bcount( *f_pcbRProvContext )      DRM_BYTE                  **f_ppbRProvContext,
    __out                                           DRM_DWORD                  *f_pdwType,
    __out                                           DRM_DWORD                  *f_pdwStep,
    __out                                           DRM_DWORD                  *f_pdwAdditionalInfoNeeded,
    __out                                           DRM_DWORD                  *f_pcbChallenge,
    __deref_out_bcount( *f_pcbChallenge )           DRM_BYTE                  **f_ppbChallenge ) DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL OEM_TEE_RPROV_ProcessBootstrapResponse(
    __inout                                         OEM_TEE_CONTEXT            *f_pContext,
    __inout                                         DRM_DWORD                  *f_pcbRProvContext,
    __deref_inout_bcount( *f_pcbRProvContext )      DRM_BYTE                  **f_ppbRProvContext,
    __in                                            DRM_DWORD                   f_cbResponse,
    __in_bcount_opt( f_cbResponse )           const DRM_BYTE                   *f_pbResponse,
    __inout                                         OEM_TEE_KEY                *f_pKSession ) DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL OEM_TEE_RPROV_GetSecureMediaPathCapabilities(
    __inout                                        OEM_TEE_CONTEXT             *f_pContext,
    __out                                          DRM_DWORD                   *f_pcbSMPC,
    __deref_out_bcount( *f_pcbSMPC )               DRM_BYTE                   **f_ppbSMPC ) DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_API DRM_DWORD DRM_CALL OEM_TEE_RPROV_GetLeafCertificateFeatures(
    __inout                                        OEM_TEE_CONTEXT             *f_pContext) DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL OEM_TEE_RPROV_VerifyNonce(
    __inout                                         OEM_TEE_CONTEXT            *f_pContext,
    __in                                            DRM_DWORD                   f_cbNonce,
    __in_bcount( f_cbNonce )                  const DRM_BYTE                   *f_pbNonce) DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL OEM_TEE_RPROV_GenerateNonce(
    __inout                                         OEM_TEE_CONTEXT            *f_pContext,
    __in                                            DRM_DWORD                   f_cbNonce,
    __out_bcount( f_cbNonce )                       DRM_BYTE                   *f_pbNonce ) DRM_NO_INLINE_ATTRIBUTE;

/*
** Sample Protection operations
*/
DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL OEM_TEE_SAMPLEPROT_ApplySampleProtection(
    __inout                                         OEM_TEE_CONTEXT            *f_pContext,
    __in                                      const OEM_TEE_KEY                *f_pSPK,
    __in                                            DRM_UINT64                  f_ui64InitializationVector,
    __in                                            DRM_DWORD                   f_cbClearToSampleProtected,
    __inout_bcount( f_cbClearToSampleProtected )    DRM_BYTE                   *f_pbClearToSampleProtected ) DRM_NO_INLINE_ATTRIBUTE;

/*
** Secure Stop operations
*/
DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL OEM_TEE_SECURESTOP_GetGenerationID(
    __inout                                         OEM_TEE_CONTEXT            *f_pContext,
    __in                                      const DRM_ID                     *f_pidEnvironment,
    __out                                           DRM_DWORD                  *f_pdwGenerationID ) DRM_NO_INLINE_ATTRIBUTE;

/*
** SHA256 functions
*/
DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL OEM_TEE_BASE_SHA256_Init(
    __inout_opt                                     OEM_TEE_CONTEXT            *f_pContextAllowNULL,
    __out_ecount( 1 )                               DRM_SHA256_CONTEXT         *f_pShaContext ) DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL OEM_TEE_BASE_SHA256_Update(
    __inout_opt                                     OEM_TEE_CONTEXT            *f_pContextAllowNULL,
    __inout_ecount( 1 )                             DRM_SHA256_CONTEXT         *f_pShaContext,
    __in                                            DRM_DWORD                   f_cbBuffer,
    __in_ecount( f_cbBuffer )                 const DRM_BYTE                    f_rgbBuffer[] ) DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL OEM_TEE_BASE_SHA256_Finalize(
    __inout_opt                                     OEM_TEE_CONTEXT            *f_pContextAllowNULL,
    __inout_ecount( 1 )                             DRM_SHA256_CONTEXT         *f_pShaContext,
    __out_ecount( 1 )                               DRM_SHA256_Digest          *f_pDigest ) DRM_NO_INLINE_ATTRIBUTE;

/*
** Critical Section functions
*/
DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL OEM_TEE_BASE_CRITSEC_Initialize() DRM_NO_INLINE_ATTRIBUTE;
DRM_NO_INLINE DRM_API_VOID DRM_VOID DRM_CALL OEM_TEE_BASE_CRITSEC_Uninitialize() DRM_NO_INLINE_ATTRIBUTE;
DRM_NO_INLINE DRM_API_VOID DRM_VOID DRM_CALL OEM_TEE_BASE_CRITSEC_Enter() DRM_NO_INLINE_ATTRIBUTE;
DRM_NO_INLINE DRM_API_VOID DRM_VOID DRM_CALL OEM_TEE_BASE_CRITSEC_Leave() DRM_NO_INLINE_ATTRIBUTE;

PREFAST_POP /* __WARNING_NONCONST_BUFFER_PARAM_25033 */
PREFAST_POP /* __WARNING_NONCONST_PARAM_25004 */

EXIT_PK_NAMESPACE;

#endif /* _OEMTEE_H_ */

