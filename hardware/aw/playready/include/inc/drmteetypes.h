/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#ifndef _DRMTEETYPES_H_
#define _DRMTEETYPES_H_ 1

#include <oemteetypes.h>
#include <drmteeproxystubcommon.h>

ENTER_PK_NAMESPACE;

/*
** The possible blob allocation behaviors for DRM_TEE_BASE_AllocBlob.
**
** DRM_TEE_BLOB_ALLOC_BEHAVIOR_INVALID: Invalid behavior.
** DRM_TEE_BLOB_ALLOC_BEHAVIOR_NEW:     Allocate a new blob.
**                                      It will NOT be zero-freed.
** DRM_TEE_BLOB_ALLOC_BEHAVIOR_COPY:    Allocate a new blob and
**                                      copy the given data into it.
**                                      The pointer to the data being
**                                      copied must point to a buffer
**                                      at least as large as the requested
**                                      size.
** DRM_TEE_BLOB_ALLOC_BEHAVIOR_WEAKREF: Take a weak reference to memory
**                                      allocated by the caller.
**                                      DRM_TEE_BASE_FreeBlob will
**                                      NOT free this memory.
*/
typedef enum
{
    DRM_TEE_BLOB_ALLOC_BEHAVIOR_INVALID = 0,
    DRM_TEE_BLOB_ALLOC_BEHAVIOR_NEW     = 1,
    DRM_TEE_BLOB_ALLOC_BEHAVIOR_COPY    = 2,
    DRM_TEE_BLOB_ALLOC_BEHAVIOR_WEAKREF = 3,
} DRM_TEE_BLOB_ALLOC_BEHAVIOR;

/*
** The possible types of TEE byte blobs.
**
** DRM_TEE_BYTE_BLOB_TYPE_INVALID:      Invalid blob type.
** DRM_TEE_BYTE_BLOB_TYPE_WEAK_REF:     Memory allocated by calling code
**                                      outside any TEE function.
**                                      Note: This type may be returned by
**                                      DRM_TEE_BASE_AllocBlob depending on
**                                      parameters passed to that function.
** DRM_TEE_BYTE_BLOB_TYPE_USER_MODE:    Memory allocated in user mode.
**                                      There are no associated TEE resources.
**                                      Note: Encrypted data which is not
**                                      of a different type uses this value.
**                                      Note: This type may be returned by
**                                      DRM_TEE_BASE_AllocBlob depending on
**                                      parameters passed to that function.
** DRM_TEE_BYTE_BLOB_TYPE_CCD:          Memory that represents a CCD and may
**                                      or may not have associated TEE resources.
** DRM_TEE_BYTE_BLOB_TYPE_SECURED_MODE: Memory allocated inside the TEE for its
**                                      own purposes.  This blob type never
**                                      leaves the TEE (and this is verified by
**                                      DRM_TEE_PROXYSTUB_XB_AddEntryByteBlob).
*/
typedef enum
{
    DRM_TEE_BYTE_BLOB_TYPE_INVALID      = 0,
    DRM_TEE_BYTE_BLOB_TYPE_WEAK_REF     = 1,
    DRM_TEE_BYTE_BLOB_TYPE_USER_MODE    = 2,
    DRM_TEE_BYTE_BLOB_TYPE_CCD          = 3,
    DRM_TEE_BYTE_BLOB_TYPE_SECURED_MODE = 4,
} DRM_TEE_BYTE_BLOB_TYPE;

/*
** eType:       The blob type.  Should be DRM_TEE_BYTE_BLOB_TYPE_USER_MODE when caller-allocated.
**              When callee-allocated, will be set by the callee and
**              should only be used by DRM_TEE_BASE_FreeBlob.
** dwSubType:   The blob sub-type.  Should be 0 when caller-allocated.
**              When callee-allocated, will be set by the callee and
**              should only be used by DRM_TEE_BASE_FreeBlob by other
**              code inside the PlayReady PK itself.
**              (For example, will be set to be equal to the mode
**              in DRM_TEE_AES128CTR_PrepareToDecrypt.)
** cb:          Number of bytes.
** pb:          The bytes.
*/
typedef struct __tagDRM_TEE_BYTE_BLOB
{
    DRM_TEE_BYTE_BLOB_TYPE   eType;
    DRM_DWORD                dwSubType;
    DRM_DWORD                cb;
    _Field_size_bytes_full_opt_( cb )  const DRM_BYTE                *pb;
} DRM_TEE_BYTE_BLOB;

/* Note: This MUST be equivalent to:  memset( &blob, 0, sizeof(blob) ); */
#define DRM_TEE_BYTE_BLOB_EMPTY {DRM_TEE_BYTE_BLOB_TYPE_INVALID,0,0,NULL}

#define IsBlobAssigned(pblob)   ( ( (pblob)->pb != NULL ) && ( (pblob)->cb != 0 ) )
#define IsBlobConsistent(pblob) ( ( (pblob) == NULL ) || ( ( (pblob)->pb == NULL ) == ( (pblob)->cb == 0 ) ) )
#define IsBlobEmpty(pblob)      ( ( (pblob)->pb == NULL ) && ( (pblob)->cb == 0 ) )


/*
** The struct DRM_TEE_PROXY_SERIALIZATION_REQUIREMENTS is used to determine the
** serialization options for an OEM TEE implementation.  This structure defines
** the maximum TEE message size and header/footer sizes for request/response
** messages.
**
** cbMaxSerializedMessage:  Defines the maximum length (in bytes) of a PRITEE
**                          serialized function call request/response message.
** cbInputHeaderSize:       The size in bytes to prepend to the PRITEE method
**                          invocation request (input) messages.  This data
**                          will be uninitialized and will be used only by the
**                          OEM to add header data to the serialized TEE
**                          messages.
**                          Note, this value MUST be a multiple of 8 bytes to
**                          avoid alignment issues.
** cbInputFooterSize:       The size in bytes to append to the PRITEE method
**                          invocation request (input) message.  This data
**                          will be uninitialized and will be used only by the
**                          OEM to add footer data to the serialized TEE
**                          messages.
**                          Note, this value MUST be a multiple of 8 bytes to
**                          avoid alignment issues.
** cbOutputHeaderSize:      The size in bytes to prepend to the PRITEE method
**                          invocation response (output) message.  This data
**                          will be uninitialized and will be used only by the
**                          OEM to add header data to the serialized TEE
**                          messages.
**                          Note, this value MUST be a multiple of 8 bytes to
**                          avoid alignment issues.
** cbOutputFooterSize:      The size in bytes to append to the PRITEE method
**                          invocation response output message.  This data
**                          will be uninitialized and will be used only by the
**                          OEM to add footer data to the serialized TEE
**                          messages.
**                          Note, this value MUST be a multiple of 8 bytes to
**                          avoid alignment issues.
*/
typedef struct __tagDRM_TEE_PROXY_SERIALIZATION_REQUIREMENTS
{
    DRM_DWORD cbMaxSerializationMessage;
    DRM_DWORD cbInputHeaderSize;
    DRM_DWORD cbInputFooterSize;
    DRM_DWORD cbOutputHeaderSize;
    DRM_DWORD cbOutputFooterSize;
} DRM_TEE_PROXY_SERIALIZATION_REQUIREMENTS;

#define DRM_TEE_PROXY_SERIALIZATION_REQUIREMENTS_EMPTY {0,0,0,0,0}

/* All values in this structure are returned from OEM_TEE_BASE_GetVersion and OEM_TEE_BASE_GetVersionInformation */
typedef struct __tagDRM_TEE_PROXY_CONTEXT_OEM_VERSION_INFO
{
    DRM_DWORD                                    dwOEMVersion;
    DRM_WCHAR                                   *pwszOEMManufacturerName;
    DRM_DWORD                                    cchOEMManufacturerName;
    DRM_WCHAR                                   *pwszOEMModelName;
    DRM_DWORD                                    cchOEMModelName;
    DRM_WCHAR                                   *pwszOEMModelNumber;
    DRM_DWORD                                    cchOEMModelNumber;
    DRM_DWORD                                    rgdwFunctionMap[ DRM_TEE_METHOD_FUNCTION_MAP_COUNT ];
    DRM_TEE_BYTE_BLOB                            oSystemProperties;
} DRM_TEE_PROXY_CONTEXT_OEM_VERSION_INFO;

#define DRM_TEE_PROXY_CONTEXT_OEM_VERSION_INFO_EMPTY { 0, NULL, 0, NULL, 0, NULL, 0, {0}, DRM_TEE_BYTE_BLOB_EMPTY }

typedef struct __tagDRM_TEE_PROXY_CONTEXT
{
    DRM_VOID                                    *pvUserCtx;
    DRM_BYTE                                    *pbTeeCtx;
    DRM_DWORD                                    cbTeeCtx;
    DRM_ID                                       idUnique;
    DRM_DWORD                                    dwPKMajorVersion;
    DRM_DWORD                                    dwPKMinorVersion;
    DRM_DWORD                                    dwPKBuildVersion;
    DRM_DWORD                                    dwPKQFEVersion;
    DRM_TEE_PROXY_CONTEXT_OEM_VERSION_INFO       oOEMVersionInfo;
    DRM_TEE_PROXY_SERIALIZATION_REQUIREMENTS     oSerializationRequirements;
} DRM_TEE_PROXY_CONTEXT;

#define DRM_TEE_PROXY_CONTEXT_EMPTY { NULL, NULL, 0, {0}, 0, 0, 0, 0, DRM_TEE_PROXY_CONTEXT_OEM_VERSION_INFO_EMPTY, DRM_TEE_PROXY_SERIALIZATION_REQUIREMENTS_EMPTY }

typedef struct __tagDRM_TEE_CONTEXT
{
    DRM_ID                                  idSession;
    OEM_TEE_CONTEXT                         oContext;
} DRM_TEE_CONTEXT;

#define DRM_TEE_CONTEXT_EMPTY {{0}, OEM_TEE_CONTEXT_EMPTY}

/*
** The possible types of XBinary key blobs. Note that these types are part
** of the XBinary format itself so they cannot be changed otherwise existing
** key blobs will be invalidated.
**
** DRM_TEE_XB_KB_TYPE_PPKB:  Public/Private (auxiliary) key blob
** DRM_TEE_XB_KB_TYPE_LKB:   License (auxiliary) key blob
** DRM_TEE_XB_KB_TYPE_CDKB:  Content decryption key blob
** DRM_TEE_XB_KB_TYPE_DKB:   Domain key blob
** DRM_TEE_XB_KB_TYPE_MRKB:  Message receiver key blob
** DRM_TEE_XB_KB_TYPE_MTKB:  Message transmitter key blob
** DRM_TEE_XB_KB_TYPE_RKB:   Revocation key blob
** DRM_TEE_XB_KB_TYPE_CEKB:  Content encryption key blob
** DRM_TEE_XB_KB_TYPE_TPKB:  Temporary provisioning key blob
** DRM_TEE_XB_KB_TYPE_SPKB:  Sample protection key blob
** DRM_TEE_XB_KB_TYPE_RPCKB: Remote provisioning context key blob
*/
typedef enum
{
    DRM_TEE_XB_KB_TYPE_PPKB  = 0x1,
    DRM_TEE_XB_KB_TYPE_LKB   = 0x2,
    DRM_TEE_XB_KB_TYPE_CDKB  = 0x3,
    DRM_TEE_XB_KB_TYPE_DKB   = 0x4,
    DRM_TEE_XB_KB_TYPE_MRKB  = 0x5,
    DRM_TEE_XB_KB_TYPE_MTKB  = 0x6,
    DRM_TEE_XB_KB_TYPE_RKB   = 0x7,
    DRM_TEE_XB_KB_TYPE_CEKB  = 0x8,
    DRM_TEE_XB_KB_TYPE_TPKB  = 0x9,
    DRM_TEE_XB_KB_TYPE_SPKB  = 0xa,
    DRM_TEE_XB_KB_TYPE_NKB   = 0xb,
    DRM_TEE_XB_KB_TYPE_RPCKB = 0xc,
} DRM_TEE_XB_KB_TYPE;

/*
** cdwData: Number of DWORD values in pdwData
** pdwData: An array of DWORD values.
*/
typedef struct __tagDRM_TEE_DWORDLIST
{
                                           DRM_DWORD  cdwData;
    _Field_size_full_opt_(cdwData)   const DRM_DWORD *pdwData;
} DRM_TEE_DWORDLIST;

/* Note: This MUST be equivalent to:  memset( &blob, 0, sizeof(blob) ); */
#define DRM_TEE_DWORDLIST_EMPTY {0,NULL}

#define DRM_TEE_DWORDLIST_TRANSFER_OWNERSHIP_TO_PTR(pcdwTarget, ppdwTarget, pdwlSource)  \
    do {                                                        \
        DRMASSERT( (pcdwTarget) != NULL );                      \
        DRMASSERT( (ppdwTarget) != NULL );                      \
        DRMASSERT( (pdwlSource) != NULL );                      \
        *(pcdwTarget) = (pdwlSource)->cdwData;                  \
        *(ppdwTarget) = ( DRM_DWORD * )(pdwlSource)->pdwData;   \
        *(( DRM_DWORD ** )&(pdwlSource)->pdwData) = NULL;       \
        (pdwlSource)->cdwData = 0;                              \
    } while( FALSE )

#define DRM_TEE_DWORDLIST_TRANSFER_OWNERSHIP(pdwlTarget, pdwlSource)                                                                                                        \
    DRMASSERT( (pdwlTarget) != NULL );                                                                                                                                      \
    DRMASSERT( (pdwlSource) != NULL );                                                                                                                                      \
    DRM_TEE_DWORDLIST_TRANSFER_OWNERSHIP_TO_PTR(&(pdwlTarget)->cdwData, ( DRM_DWORD ** )&(pdwlTarget)->pdwData, pdwlSource)

#define IsDwordListAssigned(pdwl)   ( ( (pdwl)->pdwData != NULL ) && ( (pdwl)->cdwData != 0 ) )
#define IsDwordListConsistent(pdwl) ( ( (pdwl)          == NULL ) || ( ( (pdwl)->pdwData == NULL ) == ( (pdwl)->cdwData == 0 ) ) )
#define IsDwordListEmpty(pdwl)      ( ( (pdwl)->pdwData == NULL ) && ( (pdwl)->cdwData == 0 ) )

/*
** The set of operations that DRM_TEE_LICGEN_CompleteLicense can perform.
**
** DRM_TEE_LICGEN_OP_INVALID:           Invalid operation.
** DRM_TEE_LICGEN_OP_GEN_REMOTE_SIMPLE: Given an unsigned simple license with empty
**                                      key material, generate new license keys,
**                                      encrypt them with a given remote client's
**                                      certificate's public encryption key
**                                      (i.e. bind them to the remote certificate),
**                                      and complete and sign the license.
**
** DRM_TEE_LICGEN_OP_GEN_REMOTE_ROOT:   Given an unsigned root license with empty
**                                      key material, generate new license keys,
**                                      encrypt them with a given remote client's
**                                      certificate's public encryption key
**                                      (i.e. bind them to the remote certificate),
**                                      and complete and sign the license.
**
** DRM_TEE_LICGEN_OP_GEN_LOCAL_SIMPLE:  Given an unsigned simple license with empty
**                                      key material, generate new license keys,
**                                      create a new LKB containing new license keys.
**                                      (this LKB can be used to write license into disk)
**
** DRM_TEE_LICGEN_OP_GEN_LOCAL_ROOT:    Given an unsigned simple license with empty
**                                      key material, generate new license keys,
**                                      create a new LKB containing new license keys.
**                                      (this LKB can be used to write license into disk)
**
** DRM_TEE_LICGEN_OP_GEN_LEAF:          Given an unsigned leaf license with empty
**                                      key material, generate new license keys,
**                                      encrypt them with a given root license's
**                                      content key (i.e. bind them to that license),
**                                      and complete and sign the license.
*/
typedef enum
{
    DRM_TEE_LICGEN_OP_INVALID           = 0,
    DRM_TEE_LICGEN_OP_GEN_REMOTE_SIMPLE = 1,
    DRM_TEE_LICGEN_OP_GEN_REMOTE_ROOT   = 2,
    DRM_TEE_LICGEN_OP_GEN_LOCAL_SIMPLE  = 3,
    DRM_TEE_LICGEN_OP_GEN_LOCAL_ROOT    = 4,
    DRM_TEE_LICGEN_OP_GEN_LEAF          = 5,
} DRM_TEE_LICGEN_OP;

/*
** The possible types of play enablers. These values form a bit field
** to enumerate all of the play enablers present in a license.
**
** DRM_TEE_PLAY_ENABLER_TYPE_UNKNOWN_OUTPUT:             Permit passing uncompressed decrypted video to
**                                                       an unknown output
** DRM_TEE_PLAY_ENABLER_TYPE_UNKNOWN_OUTPUT_CONSTRAINED: Permit passing constrained resolution video to
**                                                       an unknown output
** DRM_TEE_PLAY_ENABLER_TYPE_DTCP:                       Permit passing decrypted content to DTCP
** DRM_TEE_PLAY_ENABLER_TYPE_HELIX:                      Permit passing decrypted content to Helix
** DRM_TEE_PLAY_ENABLER_TYPE_HDCP_MIRACAST:              Permit passing decrypted compressed content to
**                                                       a licensed implementation of HDCP over Miracast
** DRM_TEE_PLAY_ENABLER_TYPE_HDCP_WIVU:                  Permit passing decrypted compressed content to
**                                                       a licensed implementation of HDCP over WiVu
** DRM_TEE_PLAY_ENABLER_TYPE_AIRPLAY:                    Permit passing decrypted compressed content to
**                                                       a licensed implementation of AirPlay
**
*/
typedef enum
{
    DRM_TEE_PLAY_ENABLER_TYPE_UNKNOWN_OUTPUT             = 0x01,
    DRM_TEE_PLAY_ENABLER_TYPE_UNKNOWN_OUTPUT_CONSTRAINED = 0x02,
    DRM_TEE_PLAY_ENABLER_TYPE_DTCP                       = 0x04,
    DRM_TEE_PLAY_ENABLER_TYPE_HELIX                      = 0x08,
    DRM_TEE_PLAY_ENABLER_TYPE_HDCP_MIRACAST              = 0x10,
    DRM_TEE_PLAY_ENABLER_TYPE_HDCP_WIVU                  = 0x20,
    DRM_TEE_PLAY_ENABLER_TYPE_AIRPLAY                    = 0x40,
} DRM_TEE_PLAY_ENABLER_TYPE;

/*
** The possible properties returned as part of the System Properties from DRM_TEE_BASE_AllocTEEContext.
** Refer to that function for more information.  These values form a bit field.
**
** DRM_TEE_PROPERTY_SUPPORTS_HEVC_HW_DECODING:
**     This TEE supports HEVC HW Decoding.
**
** DRM_TEE_PROPERTY_SUPPORTS_REMOTE_PROVISIONING:
**     This TEE supports remove provisioning.
**
** DRM_TEE_PROPERTY_SUPPORTS_PRE_PROCESS_ENCRYPTED_DATA:
**     This TEE supports calling DRM_TEE_H264_PreProcessEncryptedData.
**
** DRM_TEE_PROPERTY_REQUIRES_PRE_PROCESS_ENCRYPTED_DATA_WITH_FULL_FRAMES:
**     This TEE requires a call to DRM_TEE_H264_PreProcessEncryptedData with complete frames.
**     This property REQUIRES that DRM_TEE_PROPERTY_SUPPORTS_PRE_PROCESS_ENCRYPTED_DATA is also set.
**
** DRM_TEE_PROPERTY_REQUIRES_SAMPLE_PROTECTION:
**     This TEE requires sample protection for decrypting SL2000 and below audio content.
**
** DRM_TEE_PROPERTY_SUPPORTS_SECURE_CLOCK:
**     This TEE supports enforcement of time-based restrictions in licenses using a secure clock.
**
** DRM_TEE_PROPERTY_SUPPORTS_SECURE_STOP:
**     This TEE supports enforcement of the secure stop license restriction.
**
** DRM_TEE_PROPERTY_SUPPORTS_SECURE_HDCP_TYPE_1:
**     This TEE supports enforcement of HDCP Type 1.
**
** DRM_TEE_PROPERTY_REQUIRES_PREPARE_POLICY_INFO:
**     This TEE requires a call to prepare policy info.
*/
typedef enum
{
    DRM_TEE_PROPERTY_SUPPORTS_HEVC_HW_DECODING                              = 0,
    DRM_TEE_PROPERTY_SUPPORTS_REMOTE_PROVISIONING                           = 1,
    DRM_TEE_PROPERTY_SUPPORTS_PRE_PROCESS_ENCRYPTED_DATA                    = 2,
    DRM_TEE_PROPERTY_REQUIRES_PRE_PROCESS_ENCRYPTED_DATA_WITH_FULL_FRAMES   = 3,
    DRM_TEE_PROPERTY_REQUIRES_SAMPLE_PROTECTION                             = 4,
    DRM_TEE_PROPERTY_SUPPORTS_SECURE_CLOCK                                  = 5,
    DRM_TEE_PROPERTY_SUPPORTS_SECURE_STOP                                   = 6,
    DRM_TEE_PROPERTY_SUPPORTS_SECURE_HDCP_TYPE_1                            = 7,
    DRM_TEE_PROPERTY_REQUIRES_PREPARE_POLICY_INFO                           = 8,

    /*
    ** Note: when adding entries to this enum, this value MUST be updated.
    ** In addition, older TEEs are assumed to NOT have new properties set.
    ** So if an older TEE supports FOO and a newer TEE does not,
    ** then the new property MUST be named something like
    ** DRM_TEE_PROPERTY_DOES_NOT_SUPPORT_FOO (i.e. as a negative)
    ** so that only newer TEEs will ever have the property set.
    ** The property array is always initialized to zero for all extra bits
    ** in the last byte inside the TEE, so this makes behavior consistent
    ** regardless of whether a new bit is added into an existing byte or
    ** causes another byte to be required.
    */
    DRM_TEE_PROPERTY_MAX                                                    = 8,
} DRM_TEE_PROPERTY;

#define DRM_TEE_PROPERTY_ARRAY_SIZE     ( (DRM_DWORD)( ( DRM_TEE_PROPERTY_MAX >> 3 ) + 1 ) )

#define DRM_TEE_PROPERTY_BIT_HELPER( _bit )     ( 1 << ( ( ( ~( (DRM_DWORD)( _bit ) ) ) & 7 ) ) )

/*
** If the TEE is older than the insecure world and had fewer enum values defined,
** the property array may not have enough entries and we don't want a read-buffer-overrun.
** As mentioned in comments for DRM_TEE_PROPERTY_MAX, we assume that a property
** from an older TEE is NOT set by treating this case as FALSE (i.e. not set).
*/
#define DRM_TEE_PROPERTY_IS_SET( _cb, _pb, _e )             \
    ( ( ( _cb ) > ( ( (DRM_DWORD)( _e ) ) >> 3 ) )          \
   && ( ( _pb )[ ( ( (DRM_DWORD)( _e ) ) >> 3 ) ] & DRM_TEE_PROPERTY_BIT_HELPER( _e ) ) )

#define DRM_TEE_PROPERTY_IS_SET_IN_TEE_PROXY_CONTEXT( _pctx, _e )     \
    DRM_TEE_PROPERTY_IS_SET( (_pctx)->oOEMVersionInfo.oSystemProperties.cb, (_pctx)->oOEMVersionInfo.oSystemProperties.pb, (_e) )

#if DRM_DBG
#define DRM_TEE_PROPERTY_SET_VALUE( _cb, _pb, _e )                  \
    do                                                              \
    {                                                               \
        DRM_DWORD    __cb = ( _cb );                                \
        DRM_BYTE    *__pb = ( _pb );                                \
        DRM_DWORD    __ib = (DRM_DWORD)( _e );                      \
        DRMASSERT( __cb > __ib >> 3 );                              \
        __pb[ __ib >> 3 ] |= DRM_TEE_PROPERTY_BIT_HELPER( __ib );   \
    } while( FALSE )
#else   /* DRM_DBG */
#define DRM_TEE_PROPERTY_SET_VALUE( _cb, _pb, _e )                  \
    do                                                              \
    {                                                               \
        DRM_BYTE    *__pb = ( _pb );                                \
        DRM_DWORD    __ib = (DRM_DWORD)( _e );                      \
        __pb[ __ib >> 3 ] |= DRM_TEE_PROPERTY_BIT_HELPER( __ib );   \
    } while( FALSE )
#endif  /* DRM_DBG */

typedef struct __tagDRM_TEE_POLICY_RESTRICTION
{
    DRM_WORD   wRestrictionCategory;
    DRM_ID     idRestrictionType;
    DRM_DWORD  cbRestrictionConfiguration;
    DRM_BYTE  *pbRestrictionConfiguration;
} DRM_TEE_POLICY_RESTRICTION;

typedef enum __tagDRM_TEE_POLICY_OUTPUT_PROTECTION_LEVEL_INDEX
{
    OUTPUT_PROTECTION__COMPRESSED_DIGITAL_VIDEO_INDEX    = 0,
    OUTPUT_PROTECTION__UNCOMPRESSED_DIGITAL_VIDEO_INDEX  = 1,
    OUTPUT_PROTECTION__ANALOG_VIDEO_INDEX                = 2,
    OUTPUT_PROTECTION__COMPRESSED_DIGITAL_AUDIO_INDEX    = 3,
    OUTPUT_PROTECTION__UNCOMPRESSED_DIGITAL_AUDIO_INDEX  = 4,
    OUTPUT_PROTECTION__COUNT                             = 5,
} DRM_TEE_POLICY_OUTPUT_PROTECTION_LEVEL_INDEX;

typedef struct __tagDRM_TEE_POLICY_INFO
{
    DRM_KID                     oKID;
    DRM_LID                     oLID;
    DRM_DWORD                   dwDecryptionMode;
    DRM_WORD                    wSecurityLevel;
    DRM_DWORD                   dwBeginDate;
    DRM_DWORD                   dwEndDate;
    DRM_BOOL                    fRealTimeExpirationPresent;
    DRM_WORD                    rgwOutputProtectionLevels[OUTPUT_PROTECTION__COUNT];
    DRM_DWORD                   dwPlayEnablers;
    DRM_DWORD                   cRestrictions;
    DRM_TEE_POLICY_RESTRICTION *pRestrictions;
} DRM_TEE_POLICY_INFO;

#define DRM_MAX_DOMAIN_JOIN_KEYS ((DRM_DWORD)(100))

#define DRM_TEE_OLDEST_TK_ID ((DRM_DWORD)(0))

EXIT_PK_NAMESPACE;

#endif /* _DRMTEETYPES_H_ */

