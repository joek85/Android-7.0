/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#ifndef _DRMSECURECOREFUNCDEFS_H_
#define _DRMSECURECOREFUNCDEFS_H_ 1

#include <drmsecurecoreconstants.h>
#include <oemciphertypes.h>
#include <drmnoncestore.h>
#include <drmrevocationstore.h>
#include <drmsecurecoretypes.h>
#include <drmlicgen.h>
#include <drmprndprotocoltypes.h>

PREFAST_PUSH_DISABLE_EXPLAINED(__WARNING_NONCONST_PARAM_25004, "Prefast Noise: DRM_SECURECORE_CONTEXT* should not be const.");

#define SECURE_CORE_FUNCTION_LIST                                           \
    ACTIVITY(BASE_FreeBlob)                                                 \
    ACTIVITY(BuildLicense)                                                  \
    ACTIVITY(CertCachingInitialize)                                         \
    ACTIVITY(CleanLocalLicenseContext)                                      \
    ACTIVITY(CleanupCache)                                                  \
    ACTIVITY(CleanupPublicKeyContext)                                       \
    ACTIVITY(CloseDecryptContext)                                           \
    ACTIVITY(CompleteCaching)                                               \
    ACTIVITY(CreateBlobCachePassword)                                       \
    ACTIVITY(CreateGlobalStorePassword)                                     \
    ACTIVITY(CreateLicenseStateStorePassword)                               \
    ACTIVITY(CreateMeterStorePassword)                                      \
    ACTIVITY(CreateOEMBlobFromCDKB)                                         \
    ACTIVITY(CreateRevocationStorePassword)                                 \
    ACTIVITY(CreateSecureStopStorePassword)                                 \
    ACTIVITY(DecryptContent)                                                \
    ACTIVITY(DecryptContentLegacy)                                          \
    ACTIVITY(DeleteDomainLKBs)                                              \
    ACTIVITY(DuplicateDecryptContext)                                       \
    ACTIVITY(DuplicatePublicKeyContext)                                     \
    ACTIVITY(EncryptOpaque)                                                 \
    ACTIVITY(EscrowRevocationInfo)                                          \
    ACTIVITY(FindAndCleanLicenseKeyCache)                                   \
    ACTIVITY(FreeDecryptedContent)                                          \
    ACTIVITY(GenerateNonce)                                                 \
    ACTIVITY(GetCertificate)                                                \
    ACTIVITY(GetCertificateWeakRef)                                         \
    ACTIVITY(GetDeviceKeyEncryptPublicKeyWeakRef)                           \
    ACTIVITY(GetDeviceKeyPrndEncryptPublicKeyWeakRef)                       \
    ACTIVITY(GetDeviceKeySignPublicKeyWeakRef)                              \
    ACTIVITY(GetOpaqueKeyFileContextWeakRef)                                \
    ACTIVITY(GetError)                                                      \
    ACTIVITY(SetError)                                                      \
    ACTIVITY(GetRKBWeakRef)                                                 \
    ACTIVITY(GetSystemTime)                                                 \
    ACTIVITY(GetVersionInformation)                                         \
    ACTIVITY(GetIsRunningInHWDRM)                                           \
    ACTIVITY(HasTeeData)                                                    \
    ACTIVITY(InitLocalLicenseContext)                                       \
    ACTIVITY(IsSystemPropertySet)                                           \
    ACTIVITY(PrepareDomainKeysForStorage)                                   \
    ACTIVITY(PreparePolicyInfo)                                             \
    ACTIVITY(PrepareSampleProtectionKey)                                    \
    ACTIVITY(PrepareToDecrypt)                                              \
    ACTIVITY(PRND_RX_CompleteLicenseRequestMessage)                         \
    ACTIVITY(PRND_RX_CompleteRegistrationRequestMessage)                    \
    ACTIVITY(PRND_RX_EndSession)                                            \
    ACTIVITY(PRND_RX_GenerateProximityResponseNonce)                        \
    ACTIVITY(PRND_RX_HandleRenewal)                                         \
    ACTIVITY(PRND_RX_ProcessLicenseTransmitMessage)                         \
    ACTIVITY(PRND_RX_ProcessRegistrationResponseMessage)                    \
    ACTIVITY(PRND_TransferPublicKeyContext)                                 \
    ACTIVITY(PRND_TX_CheckClockDrift)                                       \
    ACTIVITY(PRND_TX_CompleteLicenseTransmitMessage)                        \
    ACTIVITY(PRND_TX_CompleteRegistrationResponseMessage)                   \
    ACTIVITY(PRND_TX_CompleteTransmitterAuthentication)                     \
    ACTIVITY(PRND_TX_EndSession)                                            \
    ACTIVITY(PRND_TX_GenerateProximityDetectionChallengeNonce)              \
    ACTIVITY(PRND_TX_MarkDeviceAsValid)                                     \
    ACTIVITY(PRND_TX_ProcessLicenseRequestMessage)                          \
    ACTIVITY(PRND_TX_ProcessRegistrationRequestMessage)                     \
    ACTIVITY(PRND_Tx_RebindLicenseKeysToNewECCKey)                          \
    ACTIVITY(PRND_TX_SignData)                                              \
    ACTIVITY(PRND_TX_ValidateDevice)                                        \
    ACTIVITY(PRND_TX_VerifyProximityDetectionChallengeNonce)                \
    ACTIVITY(PRND_TX_VerifyRegistrationRequest)                             \
    ACTIVITY(ProcessLicenseForStorage)                                      \
    ACTIVITY(DeleteLicenseLKB)                                              \
    ACTIVITY(ReleaseLicense)                                                \
    ACTIVITY(SetStorePasswordCallback)                                      \
    ACTIVITY(SignRMPChallenge)                                              \
    ACTIVITY(TransferPublicKeyContext)                                      \
    ACTIVITY(TX_SafeReleaseKey)                                             \
    ACTIVITY(UnwrapPublicKeyHandle)                                         \
    ACTIVITY(ValidateMachineID)                                             \
    
#define ACTIVITY_INITIALIZE_SC_FUNCTION_POINTERS(FUNCNAME, POINTER, IMPL)                                                                                          \
    POINTER->pfnDRM_SECURECORE_##FUNCNAME                                                  = DRM_SECURECORE_##IMPL##_##FUNCNAME;                                   \

ENTER_PK_NAMESPACE;

typedef DRM_API DRM_RESULT( DRM_CALL *LPFN_DRM_SECURECORE_PreInitialize )(
    __in_opt                            DRM_VOID                 *f_pOEMContext,
    __out                               DRM_VOID                **f_ppvPreInitializeContext,
    __out                               DRM_ID                   *f_pCurrentAppId,
    __out                               DRM_ID                   *f_pUniqueId );

typedef DRM_API DRM_RESULT (DRM_CALL *LPFN_DRM_SECURECORE_Initialize)(
    __inout                             DRM_SECURECORE_CONTEXT   *f_pSecureCoreCtx,
    __in_opt                            DRM_VOID                 *f_pOEMContext,
    __in                                DRM_DWORD                 f_cchVersion,
    __in_ecount(f_cchVersion)           DRM_CHAR                 *f_pszVersion,
    __in                                DRM_DST                  *f_pHDS,
    __in_opt                      const DRM_CONST_STRING         *f_pdstrDeviceStoreName,
    __in                                DRM_DWORD                 f_cbWorkingBuffer,
    __inout_bcount(f_cbWorkingBuffer)   DRM_BYTE                 *f_pbWorkingBuffer,
    __inout_opt                         DRM_VOID                **f_ppvPreInitializeContext,
    __in_opt                            DRM_ID                   *f_pCurrentAppId );

#define DRM_SC_DEF_FOR_FUNCPTR 1

#include <drmsecurecore_template.h>

#undef DRM_SC_DEF_FOR_FUNCPTR

#define ACTIVITY(FUNCNAME)\
    LPFN_DRM_SECURECORE_##FUNCNAME                                               pfnDRM_SECURECORE_##FUNCNAME;

typedef struct __tagDRM_SECURECORE_FUNCTIONPTRS
{
    SECURE_CORE_FUNCTION_LIST

    /* Special functions - not defined for all SecureCores */
    LPFN_DRM_SECURECORE_PreInitialize                                               pfnDRM_SECURECORE_PreInitialize;
    LPFN_DRM_SECURECORE_PreUninitialize                                             pfnDRM_SECURECORE_PreUninitialize;
    LPFN_DRM_SECURECORE_Initialize                                                  pfnDRM_SECURECORE_Initialize;
    LPFN_DRM_SECURECORE_Uninitialize                                                pfnDRM_SECURECORE_Uninitialize;
} DRM_SECURECORE_FUNCTION_POINTERS;
#undef ACTIVITY

typedef struct __tagDRM_SECURECORE_DECRYPT_FUNCTIONPTRS
{
    LPFN_DRM_SECURECORE_CloseDecryptContext                                         pfnDRM_SECURECORE_CloseDecryptContext;
    LPFN_DRM_SECURECORE_DecryptContent                                              pfnDRM_SECURECORE_DecryptContent;
    LPFN_DRM_SECURECORE_DecryptContentLegacy                                        pfnDRM_SECURECORE_DecryptContentLegacy;
    LPFN_DRM_SECURECORE_DuplicateDecryptContext                                     pfnDRM_SECURECORE_DuplicateDecryptContext;
    LPFN_DRM_SECURECORE_FreeDecryptedContent                                        pfnDRM_SECURECORE_FreeDecryptedContent;
} DRM_SECURECORE_DECRYPT_FUNCTION_POINTERS;

EXIT_PK_NAMESPACE;

PREFAST_POP; /* __WARNING_NONCONST_PARAM_25004 */

#endif /* _DRMSECURECOREFUNCDEF_H_ */

