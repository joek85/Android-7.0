/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#if defined(DRM_SC_DEF_FOR_EXTCALLERS)
#define DRM_GET_FUNCNAME(FUNCNAME) DRM_SECURECORE_##FUNCNAME
#elif defined(DRM_SC_DEF_FOR_INT)
#define DRM_GET_FUNCNAME(FUNCNAME) DRM_SECURECORE_INT_##FUNCNAME
#elif defined(DRM_SC_DEF_FOR_TEE)
#define DRM_GET_FUNCNAME(FUNCNAME) DRM_SECURECORE_TEE_##FUNCNAME
#elif defined(DRM_SC_DEF_FOR_NOTEE)
#define DRM_GET_FUNCNAME(FUNCNAME) DRM_SECURECORE_NOTEE_##FUNCNAME
#else
#define DRM_GET_FUNCNAME(FUNCNAME)
#endif


#if defined(DRM_SC_DEF_FOR_FUNCPTR)
#define DRM_SIGNATURE_POSTFIX
#define DRM_GET_FUNCTION_SIGNATURE(FUNCNAME, APITYPE, RETTYPE)         typedef APITYPE RETTYPE (DRM_CALL *LPFN_DRM_SECURECORE_##FUNCNAME)
#define DRM_GET_FUNCTION_SIGNATURE_WOTYPE(FUNCNAME, APITYPE, RETTYPE)   typedef APITYPE RETTYPE (DRM_CALL *LPFN_DRM_SECURECORE_##FUNCNAME)
#else
#define DRM_SIGNATURE_POSTFIX DRM_NO_INLINE_ATTRIBUTE
#define DRM_GET_FUNCTION_SIGNATURE(FUNCNAME, APITYPE, RETTYPE)         DRM_NO_INLINE APITYPE RETTYPE DRM_CALL DRM_GET_FUNCNAME(FUNCNAME)
#define DRM_GET_FUNCTION_SIGNATURE_WOTYPE(FUNCNAME, APITYPE, RETTYPE)   DRM_NO_INLINE APITYPE RETTYPE DRM_CALL DRM_SECURECORE_##FUNCNAME
#endif

DRM_GET_FUNCTION_SIGNATURE( PreUninitialize, DRM_API_VOID, DRM_VOID )(
    __inout                             DRM_SECURECORE_CONTEXT   *f_pSecureCoreCtx,
    __inout                             DRM_VOID                **f_ppvPreInitializeContext ) DRM_SIGNATURE_POSTFIX;

DRM_GET_FUNCTION_SIGNATURE( Uninitialize, DRM_API_VOID, DRM_VOID)(
    __inout                             DRM_SECURECORE_CONTEXT   *f_pSecureCoreCtx ) DRM_SIGNATURE_POSTFIX;

DRM_GET_FUNCTION_SIGNATURE( GetCertificate, DRM_API, DRM_RESULT)(
    __inout                             DRM_SECURECORE_CONTEXT  *f_pSecureCoreCtx,
    __inout                             DRM_DWORD               *f_pcbCert,
    __out_bcount(*f_pcbCert)            DRM_BYTE                *f_pbCert ) DRM_SIGNATURE_POSTFIX;

DRM_GET_FUNCTION_SIGNATURE( GetCertificateWeakRef, DRM_API, DRM_RESULT)(
    __inout                         DRM_SECURECORE_CONTEXT   *f_pSecureCoreCtx,
    __in                            DRM_KF_CERT_TYPE          f_eCertType,
    __out_ecount(1)                 DRM_DWORD                *f_pcbCertData,
    __out_ecount(*f_pcbCertData)    DRM_BYTE                **f_ppbCertData ) DRM_SIGNATURE_POSTFIX;

DRM_GET_FUNCTION_SIGNATURE( IsSystemPropertySet, DRM_API, DRM_BOOL)(
    __inout                         DRM_SECURECORE_CONTEXT   *f_pSecureCoreCtx,
    __in                            DRM_TEE_PROPERTY          f_eProperty ) DRM_SIGNATURE_POSTFIX;

DRM_GET_FUNCTION_SIGNATURE( GetVersionInformation, DRM_API, DRM_RESULT)(
    __inout                                             DRM_SECURECORE_CONTEXT     *f_pSecureCoreCtx,
    __out_opt                                           DRM_DWORD                  *f_pdwPKMajorVersion,
    __out_opt                                           DRM_DWORD                  *f_pdwPKMinorVersion,
    __out_opt                                           DRM_DWORD                  *f_pdwPKBuildVersion,
    __out_opt                                           DRM_DWORD                  *f_pdwPKQFEVersion,
    __out_opt                                           DRM_DWORD                  *f_pdwOEMVersion,
    __out_ecount_opt(DRM_TEE_METHOD_FUNCTION_MAP_COUNT) DRM_DWORD                  *f_pdwFunctionMap ) DRM_SIGNATURE_POSTFIX;

DRM_GET_FUNCTION_SIGNATURE( GetIsRunningInHWDRM, DRM_API, DRM_BOOL )(
    __inout     DRM_SECURECORE_CONTEXT     *f_pSecureCoreCtx ) DRM_SIGNATURE_POSTFIX;

DRM_GET_FUNCTION_SIGNATURE( SetStorePasswordCallback, DRM_API_VOID, DRM_VOID )(
    __inout                             DRM_SECURECORE_CONTEXT                              *f_pSecureCoreCtx,
    __in_opt                            DRM_SECURECORE_TEE_DATA_STORE_PASSWORD_CALLBACK      f_pfnStorePasswordCallback ) DRM_SIGNATURE_POSTFIX;

DRM_GET_FUNCTION_SIGNATURE( HasTeeData, DRM_API, DRM_BOOL)(
    __inout                                             DRM_SECURECORE_CONTEXT     *f_pSecureCoreCtx ) DRM_SIGNATURE_POSTFIX;

DRM_GET_FUNCTION_SIGNATURE( GetDeviceKeySignPublicKeyWeakRef, DRM_API, DRM_RESULT)(
    __inout                                             DRM_SECURECORE_CONTEXT     *f_pSecureCoreCtx,
    __deref_inout                                       PUBKEY_P256               **f_ppoPublicKeyWeakRef ) DRM_SIGNATURE_POSTFIX;

DRM_GET_FUNCTION_SIGNATURE( GetDeviceKeyEncryptPublicKeyWeakRef, DRM_API, DRM_RESULT)(
    __inout                                             DRM_SECURECORE_CONTEXT     *f_pSecureCoreCtx,
    __deref_inout                                       PUBKEY_P256               **f_ppoPublicKeyWeakRef ) DRM_SIGNATURE_POSTFIX;

DRM_GET_FUNCTION_SIGNATURE( GetDeviceKeyPrndEncryptPublicKeyWeakRef, DRM_API, DRM_RESULT)(
    __inout                                             DRM_SECURECORE_CONTEXT     *f_pSecureCoreCtx,
    __deref_inout                                       PUBKEY_P256               **f_ppoPublicKeyWeakRef ) DRM_SIGNATURE_POSTFIX;

DRM_GET_FUNCTION_SIGNATURE( GetRKBWeakRef, DRM_API, DRM_RESULT)(
    __inout                                             DRM_SECURECORE_CONTEXT     *f_pSecureCoreCtx,
    __deref_inout                                       DRM_TEE_BYTE_BLOB         **f_ppoRKBWeakRef ) DRM_SIGNATURE_POSTFIX;

DRM_GET_FUNCTION_SIGNATURE( GetOpaqueKeyFileContextWeakRef, DRM_API, DRM_RESULT)(
    __inout                                             DRM_SECURECORE_CONTEXT     *f_pSecureCoreCtx,
    __deref_inout                                       DRM_VOID                  **f_ppoOpaqueKeyfileContextWeakRef ) DRM_SIGNATURE_POSTFIX;

DRM_GET_FUNCTION_SIGNATURE( GetError, DRM_API, DRM_RESULT)(
    __inout                                             DRM_SECURECORE_CONTEXT     *f_pSecureCoreCtx ) DRM_SIGNATURE_POSTFIX;

DRM_GET_FUNCTION_SIGNATURE( SetError, DRM_API, DRM_RESULT)(
    __inout                                             DRM_SECURECORE_CONTEXT     *f_pSecureCoreCtx,
    __in                                                DRM_RESULT                  f_drError ) DRM_SIGNATURE_POSTFIX;

DRM_GET_FUNCTION_SIGNATURE( CreateGlobalStorePassword, DRM_API, DRM_RESULT)(
    __inout                             DRM_SECURECORE_CONTEXT   *f_pSecureCoreCtx,
    __out_bcount( DRM_SHA1_DIGEST_LEN ) DRM_BYTE                 *f_pbPasswordSST ) DRM_SIGNATURE_POSTFIX;

DRM_GET_FUNCTION_SIGNATURE( CreateRevocationStorePassword, DRM_API, DRM_RESULT)(
    __inout                             DRM_SECURECORE_CONTEXT   *f_pSecureCoreCtx,
    __out_bcount( DRM_SHA1_DIGEST_LEN ) DRM_BYTE                 *f_pbPasswordSST ) DRM_SIGNATURE_POSTFIX;

DRM_GET_FUNCTION_SIGNATURE( CreateLicenseStateStorePassword, DRM_API, DRM_RESULT)(
    __inout                             DRM_SECURECORE_CONTEXT   *f_pSecureCoreCtx,
    __in                          const DRM_ID                   *f_pLID,
    __out_bcount( DRM_SHA1_DIGEST_LEN ) DRM_BYTE                 *f_pbPasswordSST ) DRM_SIGNATURE_POSTFIX;

DRM_GET_FUNCTION_SIGNATURE( CreateMeterStorePassword, DRM_API, DRM_RESULT)(
    __inout                                                                       DRM_SECURECORE_CONTEXT  *f_pSecureCoreCtx,
    __in_bcount( CCH_BASE64_EQUIV( sizeof(DRM_MID) ) * sizeof(DRM_WCHAR) )  const DRM_BYTE                *f_pbBase64MID,
    __out_bcount( DRM_SHA1_DIGEST_LEN )                                           DRM_BYTE                *f_pbPasswordSST ) DRM_SIGNATURE_POSTFIX;

DRM_GET_FUNCTION_SIGNATURE( CreateSecureStopStorePassword, DRM_API, DRM_RESULT)(
    __inout                             DRM_SECURECORE_CONTEXT   *f_pSecureCoreCtx,
    __out_bcount( DRM_SHA1_DIGEST_LEN ) DRM_BYTE                 *f_pbPasswordSST ) DRM_SIGNATURE_POSTFIX;

DRM_GET_FUNCTION_SIGNATURE( CreateBlobCachePassword, DRM_API, DRM_RESULT)(
    __inout                             DRM_SECURECORE_CONTEXT   *f_pSecureCoreCtx,
    __in                                       DRM_DWORD                 f_cbBlob1,
    __in_ecount( f_cbBlob1 )             const DRM_BYTE                 *f_pbBlob1,
    __out_bcount( DRM_SHA1_DIGEST_LEN )        DRM_BYTE                 *f_pbPasswordSST ) DRM_SIGNATURE_POSTFIX;

DRM_GET_FUNCTION_SIGNATURE( CleanupPublicKeyContext, DRM_API_VOID, DRM_VOID)(
    __inout                                    DRM_SECURECORE_CONTEXT   *f_pSecureCoreCtx,
    __inout_ecount(1)                          DRM_PUBLIC_KEY_CONTEXT   *f_pKeyContext ) DRM_SIGNATURE_POSTFIX;

DRM_GET_FUNCTION_SIGNATURE( TransferPublicKeyContext, DRM_API, DRM_RESULT)(
    __inout                             DRM_SECURECORE_CONTEXT   *f_pSecureCoreCtx,
    __out_ecount(1)                     DRM_PUBLIC_KEY_CONTEXT   *f_pDestinationKeyContext,
    __inout_ecount(1)                   DRM_PUBLIC_KEY_CONTEXT   *f_pSourceKeyContext ) DRM_SIGNATURE_POSTFIX;

DRM_GET_FUNCTION_SIGNATURE( DuplicatePublicKeyContext, DRM_API, DRM_RESULT)(
    __inout                             DRM_SECURECORE_CONTEXT   *f_pSecureCoreCtx,
    __out_ecount(1)                     DRM_PUBLIC_KEY_CONTEXT   *f_pDestinationKeyContext,
    __inout_ecount(1)                   DRM_PUBLIC_KEY_CONTEXT   *f_pSourceKeyContext ) DRM_SIGNATURE_POSTFIX;

DRM_GET_FUNCTION_SIGNATURE( SignRMPChallenge, DRM_API, DRM_RESULT)(
    __inout                             DRM_SECURECORE_CONTEXT   *f_pSecureCoreCtx,
    __in                                DRM_DWORD                  f_cbToSign,
    __in_bcount( f_cbToSign )     const DRM_BYTE                  *f_pbToSign,
    __in                          const PUBKEY_P256               *f_pPubKey,
    __out                               SIGNATURE_P256            *f_pSignature ) DRM_SIGNATURE_POSTFIX;

DRM_GET_FUNCTION_SIGNATURE( CertCachingInitialize, DRM_API, DRM_RESULT)(
    __inout                         DRM_SECURECORE_CONTEXT               *f_pSecureCoreCtx,
    __inout_ecount( 1 )             DRM_BINARY_DEVICE_CERT_CACHED_VALUES *f_pCache,
    __in_bcount(f_cbCertData) const DRM_BYTE                             *f_pbCertData,
    __in                      const DRM_DWORD                             f_cbCertData,
    __in                            DRM_DWORD                             f_dwChainLength,
    __in                            DRM_DWORD                             f_bOffset,
    __out_ecount( 1 )               OEM_CRYPTO_HANDLE                    *f_phCertChainPubKey ) DRM_SIGNATURE_POSTFIX;

DRM_GET_FUNCTION_SIGNATURE( UnwrapPublicKeyHandle, DRM_API, DRM_RESULT)(
    __inout                                 DRM_SECURECORE_CONTEXT *f_pSecureCoreCtx,
    __in                                    OEM_CRYPTO_HANDLE       f_hWrappingKey,
    __in_bcount( f_cbCertificate )    const DRM_BYTE               *f_pbCertificate,
    __in                                    DRM_DWORD               f_cbCertificate,
    __in                                    DRM_DWORD               f_iKeyInCert,
    __inout_ecount( 1 )                     DRM_PUBLIC_KEY_CONTEXT *f_pKeyContext ) DRM_SIGNATURE_POSTFIX;

DRM_GET_FUNCTION_SIGNATURE( CompleteCaching, DRM_API_VOID, DRM_VOID)(
    __inout                         DRM_SECURECORE_CONTEXT               *f_pSecureCoreCtx,
    __inout_ecount_opt( 1 )         DRM_BINARY_DEVICE_CERT_CACHED_VALUES *f_pCache,
    __in                            DRM_RESULT                            f_drCachingResult,
    __inout_ecount_opt( 1 )         OEM_CRYPTO_HANDLE                    *f_phCertChainPubKey ) DRM_SIGNATURE_POSTFIX;

DRM_GET_FUNCTION_SIGNATURE( CleanupCache, DRM_API_VOID, DRM_VOID)(
    __inout                         DRM_SECURECORE_CONTEXT               *f_pSecureCoreCtx,
    __inout_ecount_opt( 1 )         DRM_BINARY_DEVICE_CERT_CACHED_VALUES *f_pCache ) DRM_SIGNATURE_POSTFIX;

DRM_GET_FUNCTION_SIGNATURE( ProcessLicenseForStorage, DRM_API, DRM_RESULT)(
    __inout                                 DRM_SECURECORE_CONTEXT  *f_pSecureCoreCtx,
    __in_opt                                DRM_DOMAINSTORE_CONTEXT *f_poDomainStoreContext,
    __in                              const DRM_XMRFORMAT           *f_pXMRLicense,
    __in                                    DRM_DWORD                f_cbLicense,
    __in_bcount( f_cbLicense )        const DRM_BYTE                *f_pbLicense,
    __inout                                 DRM_DST                 *f_pHDS,
    __out_opt                               DRM_TEE_BYTE_BLOB       *f_pLKB ) DRM_SIGNATURE_POSTFIX;

DRM_GET_FUNCTION_SIGNATURE( DeleteLicenseLKB, DRM_API, DRM_RESULT )(
    __inout        DRM_SECURECORE_CONTEXT   *f_pSecureCoreCtx,
    __inout        DRM_DST                  *f_pHDS,
    __in     const DRM_ID                   *f_pLID ) DRM_SIGNATURE_POSTFIX;

DRM_GET_FUNCTION_SIGNATURE( GenerateNonce, DRM_API, DRM_RESULT)(
    __inout                                 DRM_SECURECORE_CONTEXT  *f_pSecureCoreCtx,
    __inout_opt                             DRM_NONCESTORE_CONTEXT  *f_poNonceStoreContext,
    __out                                   DRM_ID                  *f_pNonce ) DRM_SIGNATURE_POSTFIX;

DRM_GET_FUNCTION_SIGNATURE( GetSystemTime, DRM_API, DRM_RESULT)(
    __inout                                 DRM_SECURECORE_CONTEXT  *f_pSecureCoreCtx,
    __out                                   DRM_UINT64              *f_pui64SystemTime ) DRM_SIGNATURE_POSTFIX;

DRM_GET_FUNCTION_SIGNATURE( PreparePolicyInfo, DRM_API, DRM_RESULT)(
    __inout                                 DRM_SECURECORE_CONTEXT  *f_pSecureCoreCtx,
    __in                                    DRM_DWORD                f_dwChainLen,
    __in_ecount( f_dwChainLen )       const DRM_BINDING_INFO        *f_pBindingInfos,
    __in                                    DRM_DWORD                f_dwDecryptionMode,
    __out                                   DRM_TEE_BYTE_BLOB       *f_pOEMPolicyInfo ) DRM_SIGNATURE_POSTFIX;

DRM_GET_FUNCTION_SIGNATURE( PrepareToDecrypt, DRM_API, DRM_RESULT)(
    __inout                                 DRM_SECURECORE_CONTEXT  *f_pSecureCoreCtx,
    __in                                    DRM_DOMAINSTORE_CONTEXT *f_poDomainStoreContext,
    __in                                    DRM_DWORD                f_dwChainLen,
    __in_ecount( f_dwChainLen )       const DRM_BINDING_INFO        *f_pBindingInfos,
    __inout                                 DRM_TEE_BYTE_BLOB       *f_pLKB,
    __in_opt                                DRM_TEE_BYTE_BLOB       *f_pSPKB,
    __inout                                 DRM_DST                 *f_pHDS,
    __inout                                 DRM_DWORD               *f_pdwDecryptionMode,
    __in_opt                                DRM_VOID                *f_pvCipherUserContext,
    __out                                   DRM_CIPHER_CONTEXT      *f_pCipherContext ) DRM_SIGNATURE_POSTFIX;

DRM_GET_FUNCTION_SIGNATURE( CreateOEMBlobFromCDKB, DRM_API, DRM_RESULT)(
    __inout                                 DRM_SECURECORE_CONTEXT  *f_pSecureCoreCtx,
    __in                                    DRM_CIPHER_CONTEXT      *f_pCipherContext,
    __in                                    DRM_DWORD                f_cbOEMInitData,
    __in_bcount( f_cbOEMInitData )    const DRM_BYTE                *f_pbOEMInitData ) DRM_SIGNATURE_POSTFIX;

DRM_GET_FUNCTION_SIGNATURE( CloseDecryptContext, DRM_API_VOID, DRM_VOID)(
    __inout                                 DRM_CIPHER_CONTEXT      *f_pCipherContext ) DRM_SIGNATURE_POSTFIX;

DRM_GET_FUNCTION_SIGNATURE( DuplicateDecryptContext, DRM_API, DRM_RESULT)(
    __out                                   DRM_CIPHER_CONTEXT      *f_pDuplicateCipherContext,
    __in                                    DRM_CIPHER_CONTEXT      *f_pSourceCipherContext ) DRM_SIGNATURE_POSTFIX;

DRM_GET_FUNCTION_SIGNATURE( PrepareSampleProtectionKey, DRM_API, DRM_RESULT)(
    __inout                                         DRM_SECURECORE_CONTEXT   *f_pSecureCoreCtx,
    __in                                            DRM_DWORD                 f_cbCertificate,
    __in                                      const DRM_BYTE                 *f_pbCertificate,
    __out                                           DRM_TEE_BYTE_BLOB        *f_pSPKB,
    __out                                           CIPHERTEXT_P256          *f_pEncryptedKey ) DRM_SIGNATURE_POSTFIX;

DRM_GET_FUNCTION_SIGNATURE( DecryptContentLegacy, DRM_API, DRM_RESULT)(
    __in                                            DRM_CIPHER_CONTEXT              *f_pCipherContext,
    __inout_opt                                     DRM_AES_COUNTER_MODE_CONTEXT    *f_pCtrContext,
    __in                                            DRM_DWORD                        f_cbData,
    __inout_bcount( f_cbData )                      DRM_BYTE                        *f_pbData ) DRM_SIGNATURE_POSTFIX;

PREFAST_PUSH_DISABLE_EXPLAINED( __WARNING_NONCONST_BUFFER_PARAM_25033, "f_pbEncryptedContent is inout" )
DRM_GET_FUNCTION_SIGNATURE( DecryptContent, DRM_API, DRM_RESULT)(
    __in                                            DRM_CIPHER_CONTEXT       *f_pCipherContext,
    __in                                            DRM_DWORD                 f_cEncryptedRegionMappings,
    __in_ecount( f_cEncryptedRegionMappings ) const DRM_DWORD                *f_pdwEncryptedRegionMappings,
    __in                                            DRM_UINT64                f_ui64InitializationVector,
    __in                                            DRM_DWORD                 f_cbEncryptedContent,
    __inout_bcount( f_cbEncryptedContent )          DRM_BYTE                 *f_pbEncryptedContent,
    __out                                           DRM_DWORD                *f_pcbOpaqueClearContent,
    __deref_out_bcount( *f_pcbOpaqueClearContent )  DRM_BYTE                **f_ppbOpaqueClearContent ) DRM_SIGNATURE_POSTFIX;
PREFAST_POP /* __WARNING_NONCONST_BUFFER_PARAM_25033 */
DRM_GET_FUNCTION_SIGNATURE( FreeDecryptedContent, DRM_API_VOID, DRM_VOID)(
    __inout        DRM_CIPHER_CONTEXT  *f_pCipherContext,
    __in           DRM_DWORD            f_cbOpaqueClearContent,
    __inout_opt    DRM_BYTE            *f_pbOpaqueClearContent ) DRM_SIGNATURE_POSTFIX;

DRM_GET_FUNCTION_SIGNATURE( PrepareDomainKeysForStorage, DRM_API, DRM_RESULT)(
    __inout                                                                           DRM_SECURECORE_CONTEXT     *f_pSecureCoreCtx,
    __in                                                                              DRM_DWORD                   f_dwProtocolVersion,
    __in                                                                              DRM_DWORD                   f_cbDomainKeysData,
    __in_bcount( f_cbDomainKeysData )                                           const DRM_BYTE                   *f_pbDomainKeysData,
    __in                                                                              DRM_DWORD                   f_cbDomainCert,
    __in_bcount( f_cbDomainCert )                                               const DRM_BYTE                   *f_pbDomainCert,
    __out_ecount( 1 )                                                                 DRM_DWORD                  *f_pcPrivateDomainKeys,
    __out_ecount( 1 )                                                                 DRM_DWORD                  *f_pcbPrivateDomainKeys,
    __deref_out_ecount( (*f_pcPrivateDomainKeys)*(*f_pcbPrivateDomainKeys) )          DRM_BYTE                  **f_ppbPrivateDomainKeys,
    __out_ecount( *f_pcPrivateDomainKeys )                                            DRM_DWORD                 **f_ppdwRevisions ) DRM_SIGNATURE_POSTFIX;

DRM_GET_FUNCTION_SIGNATURE( DeleteDomainLKBs, DRM_API, DRM_RESULT)(
    __inout                                              DRM_SECURECORE_CONTEXT      *f_pSecureCoreCtx,
    __in                                                 DRM_DOMAINSTORE_CONTEXT     *f_poDomainStoreContext,
    __in                                           const DRM_GUID                    *f_poDomainAccountID ) DRM_SIGNATURE_POSTFIX;

DRM_GET_FUNCTION_SIGNATURE( BASE_FreeBlob, DRM_API_VOID, DRM_VOID)(
    __inout                                              DRM_SECURECORE_CONTEXT      *f_pSecureCoreCtx,
    __inout                                              DRM_TEE_BYTE_BLOB           *f_pBlob ) DRM_SIGNATURE_POSTFIX;

DRM_GET_FUNCTION_SIGNATURE( EscrowRevocationInfo, DRM_API, DRM_RESULT)(
    __inout                                             DRM_SECURECORE_CONTEXT       *f_pSecureCoreCtx,
    __in                                                DRM_REVOCATIONSTORE_CONTEXT  *f_pRevocationStoreContext,
    __in_opt                                      const DRM_ID                       *f_pidSession ) DRM_SIGNATURE_POSTFIX;

DRM_GET_FUNCTION_SIGNATURE( ReleaseLicense, DRM_API, DRM_RESULT)(
    __inout                                                DRM_SECURECORE_CONTEXT              *f_pSecureCoreCtx,
    __inout                                                DRM_LICENSE_HANDLE                  *f_phLicense )  DRM_SIGNATURE_POSTFIX;

DRM_GET_FUNCTION_SIGNATURE( BuildLicense, DRM_API, DRM_RESULT)(
    __inout                                                DRM_SECURECORE_CONTEXT              *f_pSecureCoreCtx,
    __inout                                                DRM_SECURECORE_CONTEXT              *f_pRootSecureCoreCtx,
    __in                                                   DRM_LOCAL_LICENSE_TYPE               f_eLicenseType,
    __in                                                   DRM_DWORD                            f_dwEncryptionMode,
    __inout                                                DRM_LOCAL_LICENSE_SESSION_CONTEXT   *f_poLocalLicenseSession,
    __in                                             const DRM_LOCAL_LICENSE_SESSION_CONTEXT   *f_phRootLicenseSession,
    __inout                                                DRM_LOCAL_LICENSE_CONTEXT           *f_poLocalLicenseContext,
    __in                                                   DRM_DWORD                            f_cbRevInfo,
    __in                                                   DRM_DWORD                            f_cbRevInfoAndRuntimeCRL,
    __in_bcount( f_cbRevInfoAndRuntimeCRL )          const DRM_BYTE                            *f_pbRevInfoAndRuntimeCRL,
    __in                                                   DRM_WORD                             f_wSecurityLevel,
    __in                                                   DRM_DWORD                            f_cbRemoteCert,
    __in_ecount_opt( f_cbRemoteCert )                const DRM_BYTE                            *f_pbRemoteCert,
    __in                                             const DRM_PUBLIC_KEY_CONTEXT              *f_pRemotePubKey,
    __in                                                   DRM_DWORD                            f_cbPartialLicense,
    __in_bcount( f_cbPartialLicense )                const DRM_BYTE                            *f_pbPartialLicense,
    __inout                                                DRM_DST                             *f_pHDS )  DRM_SIGNATURE_POSTFIX;

DRM_GET_FUNCTION_SIGNATURE( FindAndCleanLicenseKeyCache, DRM_API, DRM_RESULT)(
    __inout                                                DRM_SECURECORE_CONTEXT               *f_pSecureCoreCtx,
    __inout                                                DRM_LOCAL_LICENSE_CONTEXT            *f_poLocalLicenseContext,
    __in                                             const DRM_KID                              *f_pKid,
    __out_opt                                              DRM_LOCAL_LICENSE_KEY_CACHE         **f_ppoLicenseKeyCache ) DRM_SIGNATURE_POSTFIX;

DRM_GET_FUNCTION_SIGNATURE( InitLocalLicenseContext, DRM_API, DRM_RESULT)(
    __inout                                                DRM_SECURECORE_CONTEXT              *f_pSecureCoreCtx,
    __out                                                  DRM_LOCAL_LICENSE_CONTEXT           *f_ppLocalLicenseContext ) DRM_SIGNATURE_POSTFIX;

DRM_GET_FUNCTION_SIGNATURE( CleanLocalLicenseContext, DRM_API_VOID, DRM_VOID)(
    __inout_opt                                            DRM_SECURECORE_CONTEXT              *f_pSecureCoreCtx,
    __out_opt                                              DRM_LOCAL_LICENSE_CONTEXT           *f_pLocalLicenseContext ) DRM_SIGNATURE_POSTFIX;

DRM_GET_FUNCTION_SIGNATURE( EncryptOpaque, DRM_API, DRM_RESULT)(
    __inout                                                DRM_SECURECORE_CONTEXT              *f_pSecureCoreCtx,
    __in                                             const DRM_LICENSE_HANDLE                  *f_phLicense,
    __in                                                   DRM_DWORD                            f_cEncryptedRegionMappings,
    __in_ecount( f_cEncryptedRegionMappings )        const DRM_DWORD                           *f_pdwEncryptedRegionMappings,
    __in                                                   DRM_DWORD                            f_cbOpaqueClearContent,
    __in_bcount( f_cbOpaqueClearContent )            const DRM_BYTE                            *f_pbOpaqueClearContent,
    __out                                                  DRM_DWORD                           *f_pcbEncryptedContent,
    __deref_out_bcount( *f_pcbEncryptedContent )           DRM_BYTE                           **f_ppbEncryptedContent,
    __out                                                  DRM_UINT64                          *f_pqwIV ) DRM_SIGNATURE_POSTFIX;

DRM_GET_FUNCTION_SIGNATURE( PRND_TransferPublicKeyContext, DRM_API, DRM_RESULT)(
    __inout                                          DRM_SECURECORE_CONTEXT                  *f_pSecureCoreCtx,
    __out_ecount(1)                                  DRM_PUBLIC_KEY_CONTEXT                  *f_pDestinationKeyContext,
    __inout_ecount(1)                                DRM_PUBLIC_KEY_CONTEXT                  *f_pSourceKeyContext ) DRM_SIGNATURE_POSTFIX;

DRM_GET_FUNCTION_SIGNATURE( PRND_RX_HandleRenewal, DRM_API, DRM_RESULT)(
    __inout                                             DRM_SECURECORE_CONTEXT                  *f_pSecureCoreCtx,
    __inout                                             DRM_PRND_RECEIVER_SESSION_CONTEXT       *f_pSession ) DRM_SIGNATURE_POSTFIX;

DRM_GET_FUNCTION_SIGNATURE( PRND_RX_CompleteRegistrationRequestMessage, DRM_API, DRM_RESULT)(
    __inout                                             DRM_SECURECORE_CONTEXT                  *f_pSecureCoreCtx,
    __inout_bcount( f_cbData )                          DRM_BYTE                                *f_pbData,
    __in                                                DRM_DWORD                                f_cbData,
    __in                                                DRM_DWORD                                f_cbDataToSign,
    __in_ecount( 1 )                              const PUBKEY_P256                             *f_poRXPubKey,
    __in                                                DRM_DWORD                                f_ibSignature ) DRM_SIGNATURE_POSTFIX;

DRM_GET_FUNCTION_SIGNATURE( PRND_RX_ProcessRegistrationResponseMessage, DRM_API, DRM_RESULT)(
    __inout                                             DRM_SECURECORE_CONTEXT                  *f_pSecureCoreCtx,
    __inout                                             DRM_PRND_RECEIVER_SESSION_CONTEXT       *f_pSession,
    __in_bcount( f_cbData )                       const DRM_BYTE                                *f_pbData,
    __in                                                DRM_DWORD                                f_cbData,
    __in                                                DRM_DWORD                                f_cbDataToVerify,
    __in_bcount( f_cbMIMKEncrypted )              const DRM_BYTE                                *f_pbMIMKEncrypted,
    __in                                                DRM_DWORD                                f_cbMIMKEncrypted,
    __in_ecount( 1 )                              const PUBKEY_P256                             *f_poRXPubKey,
    __in                                                DRM_DWORD                                f_ibSignature ) DRM_SIGNATURE_POSTFIX;

DRM_GET_FUNCTION_SIGNATURE( PRND_RX_GenerateProximityResponseNonce, DRM_API, DRM_RESULT)(
    __inout                                             DRM_SECURECORE_CONTEXT                  *f_pSecureCoreCtx,
    __in                                          const DRM_PRND_RECEIVER_SESSION_CONTEXT       *f_pSession,
    __inout_ecount( 1 )                                 DRM_ID                                  *f_pNonce ) DRM_SIGNATURE_POSTFIX;

DRM_GET_FUNCTION_SIGNATURE( PRND_RX_CompleteLicenseRequestMessage, DRM_API, DRM_RESULT)(
    __inout                                             DRM_SECURECORE_CONTEXT                  *f_pSecureCoreCtx,
    __in                                          const DRM_PRND_RECEIVER_SESSION_CONTEXT       *f_pSession,
    __inout_bcount( f_cbData )                          DRM_BYTE                                *f_pbData,
    __in                                                DRM_DWORD                                f_cbData,
    __in                                                DRM_DWORD                                f_cbDataToSign,
    __in                                                DRM_DWORD                                f_ibSignature ) DRM_SIGNATURE_POSTFIX;

DRM_GET_FUNCTION_SIGNATURE( PRND_RX_ProcessLicenseTransmitMessage, DRM_API, DRM_RESULT)(
    __inout                                             DRM_SECURECORE_CONTEXT                  *f_pSecureCoreCtx,
    __in                                          const DRM_PRND_RECEIVER_SESSION_CONTEXT       *f_pSession,
    __in_bcount( f_cbData )                       const DRM_BYTE                                *f_pbData,
    __in                                                DRM_DWORD                                f_cbData,
    __in                                                DRM_DWORD                                f_cbDataToVerify,
    __in                                                DRM_DWORD                                f_ibSignature ) DRM_SIGNATURE_POSTFIX;

DRM_GET_FUNCTION_SIGNATURE( PRND_RX_EndSession, DRM_API, DRM_RESULT)(
    __inout                                             DRM_SECURECORE_CONTEXT                  *f_pSecureCoreCtx,
    __inout                                             DRM_PRND_RECEIVER_SESSION_CONTEXT       *f_pSession ) DRM_SIGNATURE_POSTFIX;

DRM_GET_FUNCTION_SIGNATURE( PRND_TX_ValidateDevice, DRM_API, DRM_RESULT)(
    __inout                                           DRM_SECURECORE_CONTEXT                  *f_pSecureCoreCtx,
    __in                                        const DRM_PRND_TRANSMITTER_SESSION_CONTEXT    *f_pSession,
    __in_bcount( f_cbData )                     const DRM_BYTE                                *f_pbData,
    __in                                              DRM_DWORD                                f_cbData,
    __in_ecount( DRM_AES_BLOCKLEN )             const DRM_BYTE                                 f_rgbSignature[DRM_AES_BLOCKLEN] )  DRM_SIGNATURE_POSTFIX;

DRM_GET_FUNCTION_SIGNATURE( PRND_TX_MarkDeviceAsValid, DRM_API, DRM_RESULT)(
    __inout                                           DRM_SECURECORE_CONTEXT                  *f_pSecureCoreCtx,
    __in                                        const DRM_PRND_TRANSMITTER_SESSION_CONTEXT    *f_pSession,
    __in_bcount( f_cbData )                     const DRM_BYTE                                *f_pbData,
    __in                                              DRM_DWORD                                f_cbData,
    __out_ecount( DRM_AES_BLOCKLEN )                  DRM_BYTE                                 f_rgbSignature[DRM_AES_BLOCKLEN] )  DRM_SIGNATURE_POSTFIX;

DRM_GET_FUNCTION_SIGNATURE( PRND_TX_SignData, DRM_API, DRM_RESULT)(
    __inout                                           DRM_SECURECORE_CONTEXT                  *f_pSecureCoreCtx,
    __in                                              DRM_VOID                                *f_pvCryptoHandle,
    __in_bcount( f_cbData )                     const DRM_BYTE                                *f_pbData,
    __in                                              DRM_DWORD                                f_cbData,
    __out_ecount( DRM_AES_BLOCKLEN )                  DRM_BYTE                                 f_rgbSignature[DRM_AES_BLOCKLEN] )  DRM_SIGNATURE_POSTFIX;

DRM_GET_FUNCTION_SIGNATURE( PRND_TX_CompleteTransmitterAuthentication, DRM_API, DRM_RESULT)(
    __inout                                                  DRM_SECURECORE_CONTEXT                  *f_pSecureCoreCtx,
    __in_bcount( f_cbData )                            const DRM_BYTE                                *f_pbData,
    __in                                                     DRM_DWORD                                f_cbData,
    __in_ecount( 1 )                                   const PUBKEY_P256                             *f_poECC256PubKey,
    __out_bcount( ECDSA_P256_SIGNATURE_SIZE_IN_BYTES )       DRM_BYTE                                f_rgbECC256Signature[ ECDSA_P256_SIGNATURE_SIZE_IN_BYTES ] ) DRM_SIGNATURE_POSTFIX;

DRM_GET_FUNCTION_SIGNATURE( PRND_TX_CheckClockDrift, DRM_API, DRM_RESULT)(
    __inout                                 DRM_PRND_TRANSMITTER_CONTEXT                     *f_poPrndTransmitterContext,
    __inout                                 DRM_PRND_TRANSMITTER_SESSION_CONTEXT             *f_poPrndTransmitterSessionContext,
    __in                                    DRM_UINT64                                        f_qwReceiverCurrentTime ) DRM_SIGNATURE_POSTFIX;

DRM_GET_FUNCTION_SIGNATURE( PRND_TX_VerifyRegistrationRequest, DRM_API, DRM_RESULT)(
    __inout                                           DRM_SECURECORE_CONTEXT                  *f_pSecureCoreCtx,
    __in_bcount( f_cbData )                     const DRM_BYTE                                *f_pbData,
    __in                                              DRM_DWORD                                f_cbData,
    __in_bcount( f_cbSignatureBuffer )          const DRM_BYTE                                *f_pbSignatureBuffer,
    __in                                              DRM_DWORD                                f_cbSignatureBuffer,
    __in                                        const DRM_XB_BYTEARRAY                        *f_pxbbaReceiverCertificate ) DRM_SIGNATURE_POSTFIX;

DRM_GET_FUNCTION_SIGNATURE( PRND_TX_GenerateProximityDetectionChallengeNonce, DRM_API, DRM_RESULT)(
    __inout                                           DRM_SECURECORE_CONTEXT                  *f_pSecureCoreCtx,
    __inout                                           DRM_PRND_TRANSMITTER_SESSION_CONTEXT    *f_pSession ) DRM_SIGNATURE_POSTFIX;

DRM_GET_FUNCTION_SIGNATURE( PRND_TX_VerifyProximityDetectionChallengeNonce, DRM_API, DRM_RESULT)(
    __inout                                           DRM_SECURECORE_CONTEXT                  *f_pSecureCoreCtx,
    __inout                                           DRM_PRND_TRANSMITTER_SESSION_CONTEXT    *f_pSession,
    __in_ecount( 1 )                            const DRM_ID                                  *f_pEncryptedNonce ) DRM_SIGNATURE_POSTFIX;

DRM_GET_FUNCTION_SIGNATURE( PRND_TX_ProcessRegistrationRequestMessage, DRM_API, DRM_RESULT)(
    __inout                                           DRM_SECURECORE_CONTEXT                  *f_pSecureCoreCtx,
    __in                                              DRM_REVOCATIONSTORE_CONTEXT             *f_pRevocationStoreContext,
    __in                                        const DRM_BYTE                                *f_pbRequestMessage,
    __in                                              DRM_DWORD                                f_cbRequestMessage,
    __in                                              DRM_BOOL                                 f_fRenewal,
    __in                                              DRM_DWORD                                f_dwRequestedSecurityLevel,
    __inout_ecount( 1 )                               CIPHERTEXT_P256                         *f_poEncryptedAESKeyPair,
    __inout                                           DRM_PRND_TRANSMITTER_SESSION_CONTEXT    *f_pSession ) DRM_SIGNATURE_POSTFIX;

DRM_GET_FUNCTION_SIGNATURE( PRND_TX_CompleteRegistrationResponseMessage, DRM_API, DRM_RESULT)(
    __inout                                           DRM_SECURECORE_CONTEXT                  *f_pSecureCoreCtx,
    __inout                                           DRM_PRND_TRANSMITTER_SESSION_CONTEXT    *f_pSession,
    __inout_bcount( f_cbData )                        DRM_BYTE                                *f_pbData,
    __in                                              DRM_DWORD                                f_cbData,
    __in                                              DRM_DWORD                                f_cbDataToSign,
    __in                                              DRM_DWORD                                f_ibSignature ) DRM_SIGNATURE_POSTFIX;

DRM_GET_FUNCTION_SIGNATURE( PRND_TX_ProcessLicenseRequestMessage, DRM_API, DRM_RESULT)(
    __inout                                           DRM_SECURECORE_CONTEXT                  *f_pSecureCoreCtx,
    __inout                                           DRM_PRND_TRANSMITTER_SESSION_CONTEXT    *f_pSession,
    __in_bcount( f_cbData )                     const DRM_BYTE                                *f_pbData,
    __in                                              DRM_DWORD                                f_cbData,
    __in                                              DRM_DWORD                                f_cbDataToVerify,
    __in                                              DRM_DWORD                                f_ibSignature ) DRM_SIGNATURE_POSTFIX;

DRM_GET_FUNCTION_SIGNATURE( PRND_TX_CompleteLicenseTransmitMessage, DRM_API, DRM_RESULT)(
    __inout                                           DRM_SECURECORE_CONTEXT                  *f_pSecureCoreCtx,
    __inout                                           DRM_PRND_TRANSMITTER_SESSION_CONTEXT    *f_pSession,
    __in_bcount( f_cbData )                     const DRM_BYTE                                *f_pbData,
    __in                                              DRM_DWORD                                f_cbData,
    __in                                              DRM_DWORD                                f_cbDataToSign,
    __in                                              DRM_DWORD                                f_ibSignature ) DRM_SIGNATURE_POSTFIX;

DRM_GET_FUNCTION_SIGNATURE( PRND_Tx_RebindLicenseKeysToNewECCKey, DRM_API, DRM_RESULT)(
    __inout                                           DRM_SECURECORE_CONTEXT                  *f_pSecureCoreCtx,
    __inout                                           DRM_PRND_TRANSMITTER_SESSION_CONTEXT    *f_pSession,
    __in_ecount( 1 )                            const DRM_BINDING_INFO                        *f_pRootmostLicenseBindingInfo,
    __in                                        const DRM_ID                                  *f_pKID,
    __in                                              DRM_DWORD                                f_cbLicense,
    __in_bcount( f_cbLicense )                  const DRM_BYTE                                *f_pbLicense,
    __in                                              DRM_XMRFORMAT                           *f_pXMRLicense,
    __inout                                           DRM_DST                                 *f_pHDS,
    __in                                              DRM_DWORD                                f_cbRevInfo,
    __in_ecount_opt( f_cbRevInfo )              const DRM_BYTE                                *f_pbRevInfo ) DRM_SIGNATURE_POSTFIX;

DRM_GET_FUNCTION_SIGNATURE( TX_SafeReleaseKey, DRM_API_VOID, DRM_VOID)(
    __inout                                           DRM_SECURECORE_CONTEXT                  *f_pSecureCoreCtx,
    __inout_ecount( 1 )                               OEM_CRYPTO_HANDLE                       *f_phKey ) DRM_SIGNATURE_POSTFIX;

DRM_GET_FUNCTION_SIGNATURE( PRND_TX_EndSession, DRM_API, DRM_RESULT)(
    __inout                                           DRM_SECURECORE_CONTEXT                  *f_pSecureCoreCtx,
    __inout                                           DRM_PRND_TRANSMITTER_SESSION_CONTEXT    *f_pSession ) DRM_SIGNATURE_POSTFIX;

DRM_GET_FUNCTION_SIGNATURE( ValidateMachineID, DRM_API, DRM_RESULT)(
    __in                                        const DRM_SECURECORE_CONTEXT                  *f_pSecureCoreCtx ) DRM_SIGNATURE_POSTFIX;


#if defined(DRM_SC_DEF_FOR_EXTCALLERS) || defined(DRM_SC_DEF_FOR_TEE)

DRM_GET_FUNCTION_SIGNATURE_WOTYPE( H264_PreProcessEncryptedData, DRM_API, DRM_RESULT)(
    __in                                                        DRM_CIPHER_CONTEXT       *f_pCipherContext,
    __inout                                                     DRM_UINT64               *f_pui64InitializationVector,
    __in                                                        DRM_DWORD                 f_cEncryptedRegionMappings,
    __in_ecount( f_cEncryptedRegionMappings )             const DRM_DWORD                *f_pdwEncryptedRegionMappings,
    __in                                                        DRM_DWORD                 f_cbEncryptedTranscryptedFullFrame,
    __inout_bcount_opt( f_cbEncryptedTranscryptedFullFrame )    DRM_BYTE                 *f_pbEncryptedTranscryptedFullFrame,
    __in                                                        DRM_DWORD                 f_cbEncryptedPartialFrame,
    __in_bcount( f_cbEncryptedPartialFrame )              const DRM_BYTE                 *f_pbEncryptedPartialFrame,
    __in                                                        DRM_DWORD                 f_cdwOffsetData,
    __in_ecount_opt( f_cdwOffsetData )                    const DRM_DWORD                *f_pdwOffsetData,
    __in                                                        DRM_DWORD                 f_cbOpaqueState,
    __in_bcount_opt( f_cbOpaqueState )                    const DRM_BYTE                 *f_pbOpaqueState,
    __out                                                       DRM_DWORD                *f_pcbOpaqueStateUpdated,
    __deref_out_bcount_opt( *f_pcbOpaqueStateUpdated )          DRM_BYTE                **f_ppbOpaqueStateUpdated,
    __out                                                       DRM_DWORD                *f_pcbSliceHeaders,
    __deref_out_bcount_opt( *f_pcbSliceHeaders )                DRM_BYTE                **f_ppbSliceHeaders,
    __out                                                       DRM_DWORD                *f_pcbOpaqueFrameData,
    __deref_out_bcount_opt( *f_pcbOpaqueFrameData )             DRM_BYTE                **f_ppbOpaqueFrameData ) DRM_SIGNATURE_POSTFIX;

DRM_GET_FUNCTION_SIGNATURE_WOTYPE( RPROV_PrepareProvisioningChallengeForNetwork, DRM_API, DRM_RESULT)(
    __in                                          DRM_DWORD                   f_cbChallenge,
    __in_bcount( f_cbChallenge )            const DRM_BYTE                   *f_pbChallenge,
    __in                                          DRM_DWORD                   f_cbServerBlob,
    __in_bcount_opt( f_cbServerBlob )       const DRM_BYTE                   *f_pbServerBlob,
    __out                                         DRM_DWORD                  *f_pcbNetworkChallenge,
    __deref_out_bcount( *f_pcbNetworkChallenge )  DRM_BYTE                  **f_ppbNetworkChallenge ) DRM_SIGNATURE_POSTFIX;

DRM_GET_FUNCTION_SIGNATURE_WOTYPE( RPROV_PrepareBootstrapProvisioningChallengeForNetwork, DRM_API, DRM_RESULT)(
    __in                                          DRM_DWORD                   f_cbChallenge,
    __in_bcount( f_cbChallenge )            const DRM_BYTE                   *f_pbChallenge,
    __in                                          DRM_DWORD                   f_cbServerBlob,
    __in_bcount_opt( f_cbServerBlob )       const DRM_BYTE                   *f_pbServerBlob,
    __out                                         DRM_DWORD                  *f_pcbNetworkChallenge,
    __deref_out_bcount( *f_pcbNetworkChallenge )  DRM_BYTE                  **f_ppbNetworkChallenge,
    __in                                          DRM_DWORD                   f_dwType,
    __in                                          DRM_DWORD                   f_dwStep ) DRM_SIGNATURE_POSTFIX;

DRM_GET_FUNCTION_SIGNATURE_WOTYPE( RPROV_ParseProvisioningResponseFromNetwork, DRM_API, DRM_RESULT)(
    __in                                        DRM_DWORD     f_cbNetworkResponse,
    __in_bcount( f_cbNetworkResponse )    const DRM_BYTE     *f_pbNetworkResponse,
    __out                                       DRM_DWORD    *f_pcbResponse,
    __deref_out_bcount( *f_pcbResponse )        DRM_BYTE    **f_ppbResponse,
    __out                                       DRM_DWORD    *f_pcbServerBlob,
    __deref_out_bcount( *f_pcbServerBlob )      DRM_BYTE    **f_ppbServerBlob ) DRM_SIGNATURE_POSTFIX;

DRM_GET_FUNCTION_SIGNATURE_WOTYPE( RPROV_GenerateBootstrapChallenge, DRM_API, DRM_RESULT)(
    __inout                                               DRM_SECURECORE_CONTEXT     *f_pSecureCoreCtx,
    __in                                                  DRM_DWORD                   f_cbRProvContext,
    __in_bcount_opt( f_cbRProvContext )             const DRM_BYTE                   *f_pbRProvContext,
    __out                                                 DRM_DWORD                  *f_pcbRProvContextUpdated,
    __deref_out_bcount( *f_pcbRProvContextUpdated )       DRM_BYTE                  **f_ppbRProvContextUpdated,
    __out                                                 DRM_DWORD                  *f_pcbChallenge,
    __deref_out_bcount( *f_pcbChallenge )                 DRM_BYTE                  **f_ppbChallenge,
    __out                                                 DRM_DWORD                  *f_pdwType,
    __out                                                 DRM_DWORD                  *f_pdwStep ) DRM_SIGNATURE_POSTFIX;

DRM_GET_FUNCTION_SIGNATURE_WOTYPE( RPROV_ProcessBootstrapResponse, DRM_API, DRM_RESULT)(
    __inout                                               DRM_SECURECORE_CONTEXT     *f_pSecureCoreCtx,
    __in                                                  DRM_DWORD                   f_cbResponse,
    __in_bcount_opt( f_cbResponse )                 const DRM_BYTE                   *f_pbResponse,
    __in                                                  DRM_DWORD                   f_cbRProvContext,
    __in_bcount_opt( f_cbRProvContext )             const DRM_BYTE                   *f_pbRProvContext,
    __out                                                 DRM_DWORD                  *f_pcbRProvContextUpdated,
    __deref_out_bcount( *f_pcbRProvContextUpdated )       DRM_BYTE                  **f_ppbRProvContextUpdated,
    __out                                                 DRM_DWORD                  *f_pcbTPKB,
    __deref_out_bcount( *f_pcbTPKB )                      DRM_BYTE                  **f_ppbTPKB ) DRM_SIGNATURE_POSTFIX;

DRM_GET_FUNCTION_SIGNATURE_WOTYPE( RPROV_GenerateProvisioningRequest, DRM_API, DRM_RESULT)(
    __inout                                               DRM_SECURECORE_CONTEXT     *f_pSecureCoreCtx,
    __in                                                  DRM_DWORD                   f_cbTPKB,
    __in_bcount( f_cbTPKB )                         const DRM_BYTE                   *f_pbTPKB,
    __in                                            const DRM_ID                     *f_pbApplicationID,
    __out                                                 DRM_DWORD                  *f_pcbChallenge,
    __deref_out_bcount( *f_pcbChallenge )                 DRM_BYTE                  **f_ppbChallenge ) DRM_SIGNATURE_POSTFIX;

DRM_GET_FUNCTION_SIGNATURE_WOTYPE( RPROV_ProcessProvisioningResponse, DRM_API, DRM_RESULT)(
    __inout                                               DRM_SECURECORE_CONTEXT     *f_pSecureCoreCtx,
    __in                                                  DRM_DST                    *f_pHDS,
    __in                                                  DRM_DWORD                   f_cbResponse,
    __in_bcount( f_cbResponse )                     const DRM_BYTE                   *f_pbResponse,
    __in                                                  DRM_DWORD                   f_cbTPKB,
    __in_bcount( f_cbTPKB )                         const DRM_BYTE                   *f_pbTPKB ) DRM_SIGNATURE_POSTFIX;

DRM_GET_FUNCTION_SIGNATURE_WOTYPE( SignHash, DRM_API, DRM_RESULT)(
    __inout                                              DRM_SECURECORE_CONTEXT      *f_pSecureCoreCtx,
    __in                                                 DRM_DWORD                    f_cbToSign,
    __in_bcount( f_cbToSign )                      const DRM_BYTE                    *f_pbToSign,
    __out                                                SIGNATURE_P256              *f_pSignature ) DRM_SIGNATURE_POSTFIX;

DRM_GET_FUNCTION_SIGNATURE_WOTYPE( GetSecureStopGenerationID, DRM_API, DRM_RESULT)(
    __inout                                             DRM_SECURECORE_CONTEXT       *f_pSecureCoreCtx,
    __in                                          const DRM_ID                       *f_pidEnvironment,
    __out                                               DRM_DWORD                    *f_pdwGenerationID ) DRM_SIGNATURE_POSTFIX;

#endif //defined(SC_DEF_FOR_EXTCALLERS) || defined(SC_DEF_FOR_TEE)

#undef DRM_GET_FUNCNAME
#undef DRM_SIGNATURE_POSTFIX
#undef DRM_GET_FUNCTION_SIGNATURE
#undef DRM_GET_FUNCTION_SIGNATURE_WOTYPE
