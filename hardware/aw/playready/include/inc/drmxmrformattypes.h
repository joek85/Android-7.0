/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#ifndef __DRM_XMR_FORMAT_TYPES_H__
#define __DRM_XMR_FORMAT_TYPES_H__

#include <drmxb.h>
#include <drmxmrconstants.h>
#include <drmxmrformat_generated.h>
#include <oemaescommon.h>

ENTER_PK_NAMESPACE;

#define DRM_XMRFORMAT_MAX_UNPACK_ALLOC_FACTOR  7
#define DRM_XMRFORAT_EXPIRATION_MIN_BEGIN_DATE 0

typedef struct __tagDRM_XMRFORMAT_AUX_KEY_ENTRY
{
    DRM_DWORD dwLocation;
    DRM_BYTE  rgbAuxKey[DRM_AES_KEYSIZE_128];
} DRM_XMRFORMAT_AUX_KEY_ENTRY;

typedef struct __tagDRM_XMRFORMAT_EXPLICIT_OUTPUT_PROTECTION_BASE
{
    DRM_BOOL                                     fValid;
    DRM_XMRFORMAT_EXPLICIT_OUTPUT_PROTECTION    *pOPL;
    DRM_DWORD                                    cEntries;
} DRM_XMRFORMAT_EXPLICIT_OUTPUT_PROTECTION_BASE;

typedef DRM_XMRFORMAT_WORD  DRM_XMRFORMAT_RIGHTS;
typedef DRM_XMRFORMAT_DWORD DRM_XMRFORMAT_EXPIRATION_AFTER_FIRSTUSE;
typedef DRM_XMRFORMAT_DWORD DRM_XMRFORMAT_SOURCEID;
typedef DRM_XMRFORMAT_DWORD DRM_XMRFORMAT_ISSUEDATE;
typedef DRM_XMRFORMAT_DWORD DRM_XMRFORMAT_REVOCATION_INFORMATION_VERSION;
typedef DRM_XMRFORMAT_WORD  DRM_XMRFORMAT_EMBEDDING_BEHAVIOR;

/*
** License evaluation macros
*/
#define XMRFORMAT_BASE_OBJECT_LENGTH   ( sizeof( DRM_WORD ) * 2  + sizeof( DRM_DWORD ) )

#define XMRFORMAT_GET_LICENSE_LENGTH_WITHOUT_SIGNATURE( plicenseXMR )                  \
    ( XBBA_TO_CB( ( plicenseXMR )->xbbaRawData )                                       \
      - XMRFORMAT_BASE_OBJECT_LENGTH                                                   \
      - sizeof( DRM_WORD ) * 2                                                         \
      - XBBA_TO_CB( ( plicenseXMR )->OuterContainer.Signature.xbbaSignature ) )        \

#define XMRFORMAT_IS_RID_VALID( plicenseXMR )                                          \
    (  ( plicenseXMR )->HeaderData.fValid )                                            \

#define XMRFORMAT_IS_METERING_VALID( plicenseXMRFORMAT )                               \
    ( ( plicenseXMRFORMAT )->OuterContainer.fValid                                     \
   && ( plicenseXMRFORMAT )->OuterContainer.GlobalPolicyContainer.fValid               \
   && ( plicenseXMRFORMAT )->OuterContainer.GlobalPolicyContainer.Metering.fValid )

#define XMRFORMAT_IS_SECURESTOP_VALID( plicenseXMRFORMAT )                             \
    ( ( plicenseXMRFORMAT )->OuterContainer.fValid                                     \
   && ( plicenseXMRFORMAT )->OuterContainer.GlobalPolicyContainer.fValid               \
   && ( plicenseXMRFORMAT )->OuterContainer.GlobalPolicyContainer.SecureStop.fValid )

#define XMRFORMAT_IS_KEY_MATERIAL_VALID( plicenseXMRFORMAT )                           \
    ( ( plicenseXMRFORMAT )->OuterContainer.fValid                                     \
   && ( plicenseXMRFORMAT )->OuterContainer.KeyMaterialContainer.fValid )

#define XMRFORMAT_IS_UPLINK_KID_VALID( plicenseXMRFORMAT )                             \
    ( ( plicenseXMRFORMAT )->OuterContainer.fValid                                     \
   && ( plicenseXMRFORMAT )->OuterContainer.KeyMaterialContainer.fValid                \
   && ( plicenseXMRFORMAT )->OuterContainer.KeyMaterialContainer.UplinkKID.fValid )    \

#define XMRFORMAT_IS_SOURCEID_VALID( plicenseXMRFORMAT )                               \
    ( ( plicenseXMRFORMAT )->OuterContainer.fValid                                     \
   && ( plicenseXMRFORMAT )->OuterContainer.GlobalPolicyContainer.fValid               \
   && ( plicenseXMRFORMAT )->OuterContainer.GlobalPolicyContainer.SourceID.fValid )

#define XMRFORMAT_IS_EXPIRATION_AFTER_FIRST_USE_VALID( plicenseXMR )                   \
    ( ( plicenseXMR )->OuterContainer.fValid                                           \
   && ( plicenseXMR )->OuterContainer.GlobalPolicyContainer.fValid                     \
   && ( plicenseXMR )->OuterContainer.GlobalPolicyContainer.ExpirationAfterUse.fValid )

#define XMRFORMAT_IS_RESTRICTED_SOURCEID_VALID( plicenseXMRFORMAT )                    \
    ( ( plicenseXMRFORMAT )->OuterContainer.fValid                                     \
    && ( plicenseXMRFORMAT )->OuterContainer.GlobalPolicyContainer.fValid              \
    && ( plicenseXMRFORMAT )->OuterContainer.GlobalPolicyContainer.RestrictedSourceID.fValid )

#define XMRFORMAT_IS_REVOCATION_INFORMATION_VERSION_VALID( plicenseXMRFORMAT )         \
    ( ( plicenseXMRFORMAT )->OuterContainer.fValid                                     \
   && ( plicenseXMRFORMAT )->OuterContainer.GlobalPolicyContainer.fValid               \
   && ( plicenseXMRFORMAT )->OuterContainer.GlobalPolicyContainer.RevocationInfoVersion.fValid )

#define XMRFORMAT_IS_GLOBAL_POLICIES_VALID( plicenseXMRFORMAT )                        \
    ( ( plicenseXMRFORMAT )->OuterContainer.fValid                                     \
   && ( plicenseXMRFORMAT )->OuterContainer.GlobalPolicyContainer.fValid )

#define XMRFORMAT_IS_EXPIRATION_VALID( plicenseXMRFORMAT )                             \
    ( ( plicenseXMRFORMAT )->OuterContainer.fValid                                     \
   && ( plicenseXMRFORMAT )->OuterContainer.GlobalPolicyContainer.fValid               \
   && ( plicenseXMRFORMAT )->OuterContainer.GlobalPolicyContainer.Expiration.fValid )

#define XMRFORMAT_IS_ISSUEDATE_VALID( plicenseXMRFORMAT )                              \
    ( ( plicenseXMRFORMAT )->OuterContainer.fValid                                     \
   && ( plicenseXMRFORMAT )->OuterContainer.GlobalPolicyContainer.fValid               \
   && ( plicenseXMRFORMAT )->OuterContainer.GlobalPolicyContainer.IssueDate.fValid )

#define XMRFORMAT_IS_GRACE_PERIOD_VALID( plicenseXMRFORMAT )                           \
    ( ( plicenseXMRFORMAT )->OuterContainer.fValid                                     \
   && ( plicenseXMRFORMAT )->OuterContainer.GlobalPolicyContainer.fValid               \
   && ( plicenseXMRFORMAT )->OuterContainer.GlobalPolicyContainer.GracePeriod.fValid )

#define XMRFORMAT_IS_SECURITY_LEVEL_VALID( plicenseXMRFORMAT )                         \
    ( ( plicenseXMRFORMAT )->OuterContainer.fValid                                     \
   && ( plicenseXMRFORMAT )->OuterContainer.GlobalPolicyContainer.fValid               \
   && ( plicenseXMRFORMAT )->OuterContainer.GlobalPolicyContainer.SecurityLevel.fValid )

#define XMRFORMAT_IS_OPL_VALID( plicenseXMRFORMAT )                                    \
    ( (plicenseXMRFORMAT)->OuterContainer.fValid                                       \
   && (plicenseXMRFORMAT)->OuterContainer.PlaybackPolicyContainer.fValid               \
   && (plicenseXMRFORMAT)->OuterContainer.PlaybackPolicyContainer.MinimumOutputProtectionLevel.fValid )

#define XMRFORMAT_IS_EXPLICITANALOGVIDEOPROTECTION_VALID( plicenseXMRFORMAT )          \
    ( (plicenseXMRFORMAT)->OuterContainer.fValid                                       \
   && (plicenseXMRFORMAT)->OuterContainer.PlaybackPolicyContainer.fValid               \
   && (plicenseXMRFORMAT)->OuterContainer.PlaybackPolicyContainer.ExplicitAnalogVideoProtection.fValid )

#define XMRFORMAT_IS_EXPLICITDIGITALAUDIOPROTECTION_VALID( plicenseXMRFORMAT )         \
    ( (plicenseXMRFORMAT)->OuterContainer.fValid                                       \
   && (plicenseXMRFORMAT)->OuterContainer.PlaybackPolicyContainer.fValid               \
   && (plicenseXMRFORMAT)->OuterContainer.PlaybackPolicyContainer.ExplicitDigitalAudioProtection.fValid )

#define XMRFORMAT_IS_EXPLICITDIGITALVIDEOPROTECTION_VALID( plicenseXMRFORMAT )         \
    ( (plicenseXMRFORMAT)->OuterContainer.fValid                                       \
   && (plicenseXMRFORMAT)->OuterContainer.PlaybackPolicyContainer.fValid               \
   && (plicenseXMRFORMAT)->OuterContainer.PlaybackPolicyContainer.ExplicitDigitalVideoProtection.fValid )

#define XMRFORMAT_IS_POLICYMETADATA_VALID( plicenseXMRFORMAT )                         \
    ( ( plicenseXMRFORMAT )->OuterContainer.fValid                                     \
   && ( plicenseXMRFORMAT )->OuterContainer.GlobalPolicyContainer.fValid               \
   && ( plicenseXMRFORMAT )->OuterContainer.GlobalPolicyContainer.PolicyMetadata.fValid )

#define XMRFORMAT_IS_PLAY_VALID( plicenseXMRFORMAT )                                   \
    ( ( plicenseXMRFORMAT )->OuterContainer.fValid                                     \
   && ( plicenseXMRFORMAT )->OuterContainer.PlaybackPolicyContainer.fValid )

#define XMRFORMAT_IS_COPY_VALID( plicenseXMRFORMAT )                                   \
    ( ( plicenseXMRFORMAT )->OuterContainer.fValid                                     \
   && ( plicenseXMRFORMAT )->OuterContainer.CopyPolicyContainer.fValid )

#define XMRFORMAT_IS_COPYCOUNT_VALID( plicenseXMRFORMAT )                              \
    ( ( plicenseXMRFORMAT )->OuterContainer.fValid                                     \
   && ( plicenseXMRFORMAT )->OuterContainer.CopyPolicyContainer.fValid                 \
   && ( plicenseXMRFORMAT )->OuterContainer.CopyPolicyContainer.CopyCount.fValid )

#define XMRFORMAT_IS_MOVEENABLER_VALID( plicenseXMRFORMAT )                            \
    ( ( plicenseXMRFORMAT )->OuterContainer.fValid                                     \
   && ( plicenseXMRFORMAT )->OuterContainer.CopyPolicyContainer.fValid                 \
   && ( plicenseXMRFORMAT )->OuterContainer.CopyPolicyContainer.MoveEnabler.fValid )

#define XMRFORMAT_IS_SIGNATURE_VALID( plicenseXMRFORMAT )                              \
    ( ( plicenseXMRFORMAT )->OuterContainer.fValid                                     \
   && ( plicenseXMRFORMAT )->OuterContainer.Signature.fValid )

#define XMRFORMAT_IS_CONTENT_KEY_VALID( plicenseXMRFORMAT )                            \
    ( ( plicenseXMRFORMAT )->OuterContainer.fValid                                     \
   && ( plicenseXMRFORMAT )->OuterContainer.KeyMaterialContainer.fValid                \
   && ( plicenseXMRFORMAT )->OuterContainer.KeyMaterialContainer.ContentKey.fValid )

#define XMRFORMAT_IS_OPTIMIZED_CONTENT_KEY_VALID( plicenseXMRFORMAT )                  \
    ( ( plicenseXMRFORMAT )->OuterContainer.fValid                                     \
   && ( plicenseXMRFORMAT )->OuterContainer.KeyMaterialContainer.fValid                \
   && ( plicenseXMRFORMAT )->OuterContainer.KeyMaterialContainer.OptimizedContentKey.fValid )

#define XMRFORMAT_IS_DEVICE_KEY_VALID( plicenseXMRFORMAT )                             \
    ( (plicenseXMRFORMAT)->OuterContainer.fValid                                       \
   && (plicenseXMRFORMAT)->OuterContainer.KeyMaterialContainer.fValid                  \
   && (plicenseXMRFORMAT)->OuterContainer.KeyMaterialContainer.DeviceKey.fValid )

#define XMRFORMAT_IS_RIGHTS_VALID( plicenseXMRFORMAT )                                 \
    ( ( plicenseXMRFORMAT )->OuterContainer.fValid                                     \
   && ( plicenseXMRFORMAT )->OuterContainer.GlobalPolicyContainer.fValid               \
   && ( plicenseXMRFORMAT )->OuterContainer.GlobalPolicyContainer.Rights.fValid )

#define XMRFORMAT_IS_CANNOT_PERSIST_LICENSE( plicenseXMRFORMAT )                       \
    ( XMRFORMAT_IS_RIGHTS_VALID( ( plicenseXMRFORMAT ) )                              \
   && ( XMR_RIGHTS_CANNOT_PERSIST == ( ( plicenseXMRFORMAT )->OuterContainer.GlobalPolicyContainer.Rights.wValue & XMR_RIGHTS_CANNOT_PERSIST ) ) )

#define XMRFORMAT_IS_CANNOT_BIND_LICENSE( plicenseXMRFORMAT )                          \
    ( XMRFORMAT_IS_RIGHTS_VALID( ( plicenseXMRFORMAT ) )                              \
   && ( XMR_RIGHTS_CANNOT_BIND_LICENSE == ( ( plicenseXMRFORMAT )->OuterContainer.GlobalPolicyContainer.Rights.wValue & XMR_RIGHTS_CANNOT_BIND_LICENSE ) ) )

#define XMRFORMAT_IS_TEMP_STORE_ONLY( plicenseXMRFORMAT )                              \
    ( XMRFORMAT_IS_RIGHTS_VALID( ( plicenseXMRFORMAT ) )                              \
   && ( XMR_RIGHTS_TEMP_STORE_ONLY == ( ( plicenseXMRFORMAT )->OuterContainer.GlobalPolicyContainer.Rights.wValue & XMR_RIGHTS_TEMP_STORE_ONLY ) ) )

#define XMRFORMAT_IS_COLLABORATIVE_PLAY( plicenseXMRFORMAT )                           \
    ( XMRFORMAT_IS_RIGHTS_VALID( ( plicenseXMRFORMAT ) )                              \
   && ( XMR_RIGHTS_COLLABORATIVE_PLAY == ( ( plicenseXMRFORMAT )->OuterContainer.GlobalPolicyContainer.Rights.wValue & XMR_RIGHTS_COLLABORATIVE_PLAY ) ) )

#define XMRFORMAT_IS_DOMAIN_ID_VALID( plicenseXMRFORMAT )                              \
    ( (plicenseXMRFORMAT)->OuterContainer.fValid                                       \
   && (plicenseXMRFORMAT)->OuterContainer.GlobalPolicyContainer.fValid                 \
   && (plicenseXMRFORMAT)->OuterContainer.GlobalPolicyContainer.DomainID.fValid )

#define XMRFORMAT_IS_SERIAL_NUMBER_RESTRICTION_VALID( plicenseXMRFORMAT )              \
    ( (plicenseXMRFORMAT)->OuterContainer.fValid                                       \
   && (plicenseXMRFORMAT)->OuterContainer.GlobalPolicyContainer.fValid                 \
   && (plicenseXMRFORMAT)->OuterContainer.GlobalPolicyContainer.SerialNumber.fValid )

#define XMRFORMAT_IS_EMBEDDING_BEHAVIOR_VALID( plicenseXMRFORMAT )                     \
    ( (plicenseXMRFORMAT)->OuterContainer.fValid                                       \
   && (plicenseXMRFORMAT)->OuterContainer.GlobalPolicyContainer.fValid                 \
   && (plicenseXMRFORMAT)->OuterContainer.GlobalPolicyContainer.EmbeddingBehavior.fValid )

#define XMRFORMAT_IS_UNKNOWN_CONTAINER_VALID( plicenseXMRFORMAT )                      \
    ( (plicenseXMRFORMAT)->OuterContainer.fValid                                       \
   && (plicenseXMRFORMAT)->OuterContainer.UnknownContainer.fValid )

#define XMRFORMAT_IS_REMOVAL_DATE_VALID( plicenseXMRFORMAT )                           \
    ( ( plicenseXMRFORMAT )->OuterContainer.fValid                                     \
   && ( plicenseXMRFORMAT )->OuterContainer.GlobalPolicyContainer.fValid               \
   && ( plicenseXMRFORMAT )->OuterContainer.GlobalPolicyContainer.RemovalDate.fValid )

/*
** Check if license should be disabled on clock rollback
*/
#define XMRFORMAT_HAS_TIME_BASED_RESTRICTIONS( plicenseXMRFORMAT )                                                  \
    ( ( plicenseXMRFORMAT )->OuterContainer.fValid                                                                  \
   && ( plicenseXMRFORMAT )->OuterContainer.GlobalPolicyContainer.fValid                                            \
   && ( ( plicenseXMRFORMAT )->OuterContainer.GlobalPolicyContainer.Expiration.fValid                               \
     || ( plicenseXMRFORMAT )->OuterContainer.GlobalPolicyContainer.ExpirationAfterUse.fValid ) )                   \

#define XMRFORMAT_IS_EXPIRATION_DATE_VALID( plicenseXMRFORMAT )                                                     \
    ( ( plicenseXMRFORMAT )->OuterContainer.GlobalPolicyContainer.Expiration.dwBeginDate != 0                       \
   || ( plicenseXMRFORMAT )->OuterContainer.GlobalPolicyContainer.Expiration.dwEndDate   != XMR_UNLIMITED )         \

#define XMRFORMAT_IS_AUX_KEY_VALID( plicenseXMRFORMAT )                                                             \
    ( ( plicenseXMRFORMAT )->OuterContainer.fValid                                                                  \
   && ( plicenseXMRFORMAT )->OuterContainer.KeyMaterialContainer.fValid                                             \
   && ( plicenseXMRFORMAT )->OuterContainer.KeyMaterialContainer.AuxKeys.fValid )                                   \

#define XMRFORMAT_IS_UPLINKX_VALID( plicenseXMRFORMAT )                                                             \
    ( ( plicenseXMRFORMAT )->OuterContainer.fValid                                                                  \
   && ( plicenseXMRFORMAT )->OuterContainer.KeyMaterialContainer.fValid                                             \
   && ( plicenseXMRFORMAT )->OuterContainer.KeyMaterialContainer.UplinkX.fValid )                                   \

#define XMRFORMAT_IS_LEAF_LICENSE( plicenseXMRFORMAT )                                                              \
    ( XMRFORMAT_IS_UPLINK_KID_VALID( plicenseXMRFORMAT )                                                            \
   || XMRFORMAT_IS_UPLINKX_VALID( plicenseXMRFORMAT ) )

#define XMRFORMAT_IS_REAL_TIME_EXPIRATION_VALID( plicenseXMRFORMAT )                                                \
    ( ( plicenseXMRFORMAT )->OuterContainer.fValid                                                                  \
   && ( plicenseXMRFORMAT )->OuterContainer.GlobalPolicyContainer.fValid                                            \
   && ( plicenseXMRFORMAT )->OuterContainer.GlobalPolicyContainer.RealTimeExpiration.fValid )

EXIT_PK_NAMESPACE;

#endif  /* __DRM_XMR_FORMAT_TYPES_H__ */
