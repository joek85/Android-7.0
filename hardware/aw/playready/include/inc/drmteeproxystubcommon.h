/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#ifndef _DRMTEEPROXYSTUBCOMMON_H_
#define _DRMTEEPROXYSTUBCOMMON_H_ 1

#include <drmtypes.h>

ENTER_PK_NAMESPACE;

#define DRM_TEE_PROXY_MESSAGE_RESPONSE_OVERHEAD 1024
#define DRM_TEE_PROXY_MAX_MESSAGE_SIZE_DEFAULT  (12 * 1024)

/*
** IMPORTANT IMPORTANT IMPORTANT IMPORTANT IMPORTANT IMPORTANT IMPORTANT IMPORTANT
**
** All values in this enum MUST be referenced by OEM_TEE_BASE_GetVersionInformation.
** When this enum changes, the following MUST be updated:
**  DRM_METHOD_ID_DRM_TEE_Count
**  OEM_TEE_BASE_GetVersionInformation
**  Any test cases that do verification of this data.
** This enum MUST not be changed without a firmware update to match.
**
** IMPORTANT IMPORTANT IMPORTANT IMPORTANT IMPORTANT IMPORTANT IMPORTANT IMPORTANT
*/
typedef enum _tagDRM_METHOD_ID_DRM_TEE
{
    /*
    ** The following two enum values DRM_METHOD_ID_DRM_TEE_BASE_AllocTEEContext
    ** and DRM_METHOD_ID_DRM_TEE_BASE_GetSerializationRequirements
    ** are fixed values and must not change.
    */
    DRM_METHOD_ID_DRM_TEE_BASE_AllocTEEContext                                  = 0,

    /*
    ** The following TEE methods do not cross the TEE boundary
    **    DRM_METHOD_ID_DRM_TEE_BASE_MemAlloc,
    **    DRM_METHOD_ID_DRM_TEE_BASE_MemFree,
    **    DRM_METHOD_ID_DRM_TEE_BASE_AllocBlob,
    **    DRM_METHOD_ID_DRM_TEE_BASE_FreeBlob,
    **    DRM_METHOD_ID_DRM_TEE_BASE_TransferBlobOwnership
    */

    /* Value to indicate the start of the IDs with OEM function map values */
    DRM_METHOD_ID_START_OF_OEM_FUNCTION_MAPPED_IDS                              = 1,

    DRM_METHOD_ID_DRM_TEE_BASE_FreeTEEContext                                   = 1,
    DRM_METHOD_ID_DRM_TEE_BASE_SignDataWithSecureStoreKey                       = 2,
    DRM_METHOD_ID_DRM_TEE_BASE_CheckDeviceKeys                                  = 3,
    DRM_METHOD_ID_DRM_TEE_BASE_GetDebugInformation                              = 4,
    DRM_METHOD_ID_DRM_TEE_BASE_GenerateNonce                                    = 5,
    DRM_METHOD_ID_DRM_TEE_BASE_GetSystemTime                                    = 6,
    DRM_METHOD_ID_DRM_TEE_LPROV_GenerateDeviceKeys                              = 7,
    DRM_METHOD_ID_DRM_TEE_RPROV_GenerateBootstrapChallenge                      = 8,
    DRM_METHOD_ID_DRM_TEE_RPROV_ProcessBootstrapResponse                        = 9,
    DRM_METHOD_ID_DRM_TEE_RPROV_GenerateProvisioningRequest                     = 10,
    DRM_METHOD_ID_DRM_TEE_RPROV_ProcessProvisioningResponse                     = 11,
    DRM_METHOD_ID_DRM_TEE_LICPREP_PackageKey                                    = 12,
    DRM_METHOD_ID_DRM_TEE_SAMPLEPROT_PrepareSampleProtectionKey                 = 13,
    DRM_METHOD_ID_DRM_TEE_AES128CTR_PreparePolicyInfo                           = 14,
    DRM_METHOD_ID_DRM_TEE_AES128CTR_PrepareToDecrypt                            = 15,
    DRM_METHOD_ID_DRM_TEE_AES128CTR_CreateOEMBlobFromCDKB                       = 16,
    DRM_METHOD_ID_DRM_TEE_AES128CTR_DecryptContent                              = 17,
    DRM_METHOD_ID_DRM_TEE_SIGN_SignHash                                         = 18,
    DRM_METHOD_ID_DRM_TEE_DOM_PackageKeys                                       = 19,
    DRM_METHOD_ID_DRM_TEE_PRNDRX_ProcessRegistrationResponseMessage             = 20,
    DRM_METHOD_ID_DRM_TEE_PRNDRX_GenerateProximityResponseNonce                 = 21,
    DRM_METHOD_ID_DRM_TEE_PRNDRX_CompleteLicenseRequestMessage                  = 22,
    DRM_METHOD_ID_DRM_TEE_PRNDRX_ProcessLicenseTransmitMessage                  = 23,
    DRM_METHOD_ID_DRM_TEE_REVOCATION_IngestRevocationInfo                       = 24,
    DRM_METHOD_ID_DRM_TEE_LICGEN_CompleteLicense                                = 25,
    DRM_METHOD_ID_DRM_TEE_LICGEN_AES128CTR_EncryptContent                       = 26,
    DRM_METHOD_ID_DRM_TEE_PRNDTX_ProcessRegistrationRequestMessage              = 27,
    DRM_METHOD_ID_DRM_TEE_PRNDTX_CompleteRegistrationResponseMessage            = 28,
    DRM_METHOD_ID_DRM_TEE_PRNDTX_GenerateProximityDetectionChallengeNonce       = 29,
    DRM_METHOD_ID_DRM_TEE_PRNDTX_VerifyProximityDetectionResponseNonce          = 30,
    DRM_METHOD_ID_DRM_TEE_PRNDTX_ProcessLicenseRequestMessage                   = 31,
    DRM_METHOD_ID_DRM_TEE_PRNDTX_CompleteLicenseTransmitMessage                 = 32,
    DRM_METHOD_ID_DRM_TEE_PRNDTX_RebindLicenseToReceiver                        = 33,
    DRM_METHOD_ID_DRM_TEE_H264_PreProcessEncryptedData                          = 34,
    DRM_METHOD_ID_DRM_TEE_SECURESTOP_GetGenerationID                            = 35,
    DRM_METHOD_ID_DRM_TEE_Count                                                 = 36,
} DRM_METHOD_ID_DRM_TEE;

#define DRM_TEE_PROXY_METHOD_ID(m) DRM_METHOD_ID_##m

/* Only certain methods support structured serialization. */
#define DRM_METHOD_ID_DRM_TEE_SUPPORTS_STRUCTURE_SERIALIZATION( eID )   \
    ( ( (eID) == DRM_METHOD_ID_DRM_TEE_H264_PreProcessEncryptedData )   \
   || ( (eID) == DRM_METHOD_ID_DRM_TEE_AES128CTR_DecryptContent     ) )

/*
** All TEE APIs should generate a request/response message with a size no bigger than
** OEM_TEE_PROXY_MAX_PRITEE_MESSAGE_SIZE (defined in oemteeproxy.h).  The only exceptions
** are the methods below until either TEE shared memory support is added
** or structured serialization is only allowed for these functions.
*/
#define DRM_TEE_PROXY_METHOD_IS_MESSAGE_SIZE_EXCEPTIONS(eMethodID)                  \
     ( ( (eMethodID) == DRM_METHOD_ID_DRM_TEE_H264_PreProcessEncryptedData    )     \
    || ( (eMethodID) == DRM_METHOD_ID_DRM_TEE_AES128CTR_DecryptContent        )     \
    || ( (eMethodID) == DRM_METHOD_ID_DRM_TEE_LICGEN_AES128CTR_EncryptContent ) )

#define DRM_TEE_PROXY_PARAMETER_TYPE__INVALID           ( ( DRM_DWORD ) -1 )
#define DRM_TEE_PROXY_PARAMETER_TYPE__DRM_DWORD         ( ( DRM_DWORD ) 0  )
#define DRM_TEE_PROXY_PARAMETER_TYPE__DRM_QWORD         ( ( DRM_DWORD ) 1  )
#define DRM_TEE_PROXY_PARAMETER_TYPE__DRM_ID            ( ( DRM_DWORD ) 2  )
#define DRM_TEE_PROXY_PARAMETER_TYPE__DRM_TEE_BYTE_BLOB ( ( DRM_DWORD ) 3  )
#define DRM_TEE_PROXY_PARAMETER_TYPE__DRM_TEE_DWORDLIST ( ( DRM_DWORD ) 4  )

typedef DRM_DWORD DRM_TEE_PROXY_PARAMETER_TYPE;

/*
** The function map does not include the fixed method ID value for DRM_TEE_BASE_AllocTEEContext.
*/
#define DRM_TEE_METHOD_FUNCTION_MAP_COUNT ((DRM_DWORD)((DRM_METHOD_ID_DRM_TEE_Count - DRM_METHOD_ID_START_OF_OEM_FUNCTION_MAPPED_IDS) * 2 ))

#define DRM_TEE_PROXY_SHOULD_USE_STRUCT_PARAMS( dwFunctionMapOEMValue )    ( (DRM_DWORD)( (dwFunctionMapOEMValue) & (DRM_DWORD)0x80000000 ) != 0 )
#define DRM_TEE_PROXY_GET_STRUCT_PARAMS_METHOD_ID( dwFunctionMapOEMValue ) ( (DRM_DWORD)( (dwFunctionMapOEMValue) & (DRM_DWORD)0x00FFFFFF ) )

#define DRM_TEE_IS_8_BYTE_ALIGNED(x) (0 == ( (DRM_DWORD_PTR)(x) & (DRM_DWORD_PTR)7))
#define DRM_TEE_IS_4_BYTE_ALIGNED(x) (0 == ( (DRM_DWORD_PTR)(x) & (DRM_DWORD_PTR)3))

#define Chk8ByteAligned(expr) ChkBOOL( DRM_TEE_IS_8_BYTE_ALIGNED(expr), DRM_E_TEE_PROXY_INVALID_ALIGNMENT )
#define Chk4ByteAligned(expr) ChkBOOL( DRM_TEE_IS_4_BYTE_ALIGNED(expr), DRM_E_TEE_PROXY_INVALID_ALIGNMENT )

EXIT_PK_NAMESPACE;

#endif /* _DRMTEEPROXYSTUBCOMMON_H_ */

