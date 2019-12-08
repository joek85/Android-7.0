/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#ifndef __DRM_XMR_CONSTANTS_H__
#define __DRM_XMR_CONSTANTS_H__

ENTER_PK_NAMESPACE;

/*
** constants, enums and data types
*/
#define XMR_MAGIC_CONSTANT        0x584D5200 /* 'XMR\0' */

#define XMR_UNLIMITED             DRM_MAX_UNSIGNED_TYPE( DRM_DWORD )

#define XMR_AES_OMAC_SIGNATURE_LENGTH     16

#define XMR_SOURCEID_MAXCOUNT   100

#define DRM_XMR_MAX_UNPACK_ALLOC_FACTOR 7

#define XMR_RID_OFFSET           ( sizeof( DRM_DWORD ) * 2 )
#define XMR_HEADER_LENGTH        ( XMR_RID_OFFSET + sizeof( DRM_ID ) )
#define XMR_BASE_OBJECT_LENGTH   ( sizeof( DRM_WORD ) * 2  + sizeof( DRM_DWORD ) )
#define XMR_MAXIMUM_OBJECT_DEPTH  5

enum  XMR_SOURCEID
{
    XMR_SOURCEID_NONE          = 0,
    XMR_SOURCEID_MACROVISION   = 1,
    XMR_SOURCEID_CGMSA         = 2,
    XMR_SOURCEID_WSS           = 3,
    XMR_SOURCEID_DIGITAL_CABLE = 4,
    XMR_SOURCEID_ATSC          = 5,
    XMR_SOURCEID_PDRM          = 260,
    XMR_SOURCEID_LEGACY_DVR    = 261,
    XMR_SOURCEID_V1            = 262,
};

enum XMR_VERSION
{
    XMR_VERSION_INVALID  = 0x0000,
    XMR_VERSION_1        = 0x0001,
    XMR_VERSION_2        = 0x0002,
    XMR_VERSION_3        = 0x0003,

    XMR_VERSION_MAX      = XMR_VERSION_3,
};

enum XMR_OBJECT_FLAGS
{
    XMR_FLAGS_NONE                  = 0x0000,
    XMR_FLAGS_MUST_UNDERSTAND       = 0x0001,
    XMR_FLAGS_CONTAINER             = 0x0002,
    XMR_FLAGS_ALLOW_EXTERNAL_PARSE  = 0x0004,
    XMR_FLAGS_BEST_EFFORT           = 0x0008,
    XMR_FLAGS_HAS_SECURE_STATE      = 0x0010
};

enum XMR_SETTINGS_FLAGS
{
    XMR_SETTINGS_FLAG_CANNOT_PERSIST    = 0x0001
};

/*
** Symmetric encryption types used for content encryption
*/
enum XMR_SYMMETRIC_ENCRYPTION_TYPE
{
    XMR_SYMMETRIC_ENCRYPTION_TYPE_INVALID      = 0x0000,
    XMR_SYMMETRIC_ENCRYPTION_TYPE_AES_128_CTR  = 0x0001,
    XMR_SYMMETRIC_ENCRYPTION_TYPE_RC4_CIPHER   = 0x0002,
    XMR_SYMMETRIC_ENCRYPTION_TYPE_AES_128_ECB  = 0x0003,
    XMR_SYMMETRIC_ENCRYPTION_TYPE_COCKTAIL     = 0x0004
};

/*
** Asymmetric encryption types used for encrypting the content key
*/
enum XMR_ASYMMETRIC_ENCRYPTION_TYPE
{
    XMR_ASYMMETRIC_ENCRYPTION_TYPE_INVALID          = 0x0000,
    XMR_ASYMMETRIC_ENCRYPTION_TYPE_RSA_1024         = 0x0001,
    XMR_ASYMMETRIC_ENCRYPTION_TYPE_CHAINED_LICENSE  = 0x0002,
    XMR_ASYMMETRIC_ENCRYPTION_TYPE_ECC_256          = 0x0003,
    XMR_ASYMMETRIC_ENCRYPTION_TYPE_ECC_256_WITH_KZ  = 0x0004,
    XMR_ASYMMETRIC_ENCRYPTION_TYPE_TEE_TRANSIENT    = 0x0005
};

/*
** Symmetric encryption types used for encrypting the content key (optimized)
*/
enum XMR_SYMMETRIC_KEY_ENCRYPTION_TYPE
{
    XMR_SYMMETRIC_KEY_ENCRYPTION_TYPE_INVALID          = 0x0000,
    XMR_SYMMETRIC_KEY_ENCRYPTION_TYPE_AES_128_ECB      = 0x0001,
    XMR_SYMMETRIC_KEY_ENCRYPTION_TYPE_AES_128_ECB_SLK  = 0x0002
};

enum XMR_ECC_CURVE_TYPE
{
    XMR_ECC_CURVE_TYPE_INVALID                = 0x0000,
    XMR_ECC_CURVE_TYPE_P256                   = 0x0001
};

enum XMR_SIGNATURE_TYPE
{
    XMR_SIGNATURE_TYPE_INVALID                = 0x0000,
    XMR_SIGNATURE_TYPE_AES_128_OMAC           = 0x0001,
    XMR_SIGNATURE_TYPE_SHA_256_HMAC           = 0x0002
};

enum XMR_GLOBAL_RIGHTS_SETTINGS
{
    XMR_RIGHTS_CANNOT_PERSIST                 = 0x001,
    XMR_RIGHTS_ALLOW_BACKUP_RESTORE           = 0x004,
    XMR_RIGHTS_COLLABORATIVE_PLAY             = 0x008,
    XMR_RIGHTS_BASE_LICENSE                   = 0x010,
                                             /* 0x020 was previously used by THUMBNAIL right in Vista/Polaris and shouldn't be used */
    XMR_RIGHTS_CANNOT_BIND_LICENSE            = 0x040,
    XMR_RIGHTS_TEMP_STORE_ONLY                = 0x080,
};

enum XMR_EXTENSIBLE_RESTRICTON_STATE
{
    XMR_EXTENSIBLE_RESTRICTON_STATE_COUNT      = 0x02,
    XMR_EXTENSIBLE_RESTRICTON_STATE_DATE       = 0x03,
    XMR_EXTENSIBLE_RESTRICTON_STATE_BYTEARRAY  = 0x04,
};

enum XMR_EMBEDDING_BEHAVIOR
{
    XMR_EMBEDDING_BEHAVIOR_INVALID = 0x00,
    XMR_EMBEDDING_BEHAVIOR_IGNORE  = 0x01,
    XMR_EMBEDDING_BEHAVIOR_COPY    = 0x02,
    XMR_EMBEDDING_BEHAVIOR_MOVE    = 0x03
};

enum XMR_UPLINK_CHECKSUM_TYPE
{
    XMR_UPLINK_CHECKSUM_TYPE_XMRV1      = 0x00,
    XMR_UPLINK_CHECKSUM_TYPE_AESOMAC1   = 0x01,
};

enum XMR_OBJECT_TYPE
{
    XMR_OBJECT_TYPE_INVALID                                            = 0x0000,
    XMR_OBJECT_TYPE_OUTER_CONTAINER                                    = 0x0001,
    XMR_OBJECT_TYPE_GLOBAL_POLICY_CONTAINER                            = 0x0002,
    XMR_OBJECT_TYPE_MINIMUM_ENVIRONMENT_OBJECT                         = 0x0003,
    XMR_OBJECT_TYPE_PLAYBACK_POLICY_CONTAINER                          = 0x0004,
    XMR_OBJECT_TYPE_OUTPUT_PROTECTION_OBJECT                           = 0x0005,
    XMR_OBJECT_TYPE_UPLINK_KID_OBJECT                                  = 0x0006,
    XMR_OBJECT_TYPE_EXPLICIT_ANALOG_VIDEO_OUTPUT_PROTECTION_CONTAINER  = 0x0007,
    XMR_OBJECT_TYPE_ANALOG_VIDEO_OUTPUT_CONFIGURATION_OBJECT           = 0x0008,
    XMR_OBJECT_TYPE_KEY_MATERIAL_CONTAINER                             = 0x0009,
    XMR_OBJECT_TYPE_CONTENT_KEY_OBJECT                                 = 0x000A,
    XMR_OBJECT_TYPE_SIGNATURE_OBJECT                                   = 0x000B,
    XMR_OBJECT_TYPE_SERIAL_NUMBER_OBJECT                               = 0x000C,
    XMR_OBJECT_TYPE_SETTINGS_OBJECT                                    = 0x000D,
    XMR_OBJECT_TYPE_COPY_POLICY_CONTAINER                              = 0x000E,
    XMR_OBJECT_TYPE_ALLOW_PLAYLISTBURN_POLICY_CONTAINER                = 0x000F,
    XMR_OBJECT_TYPE_INCLUSION_LIST_OBJECT                              = 0x0010,
    XMR_OBJECT_TYPE_PRIORITY_OBJECT                                    = 0x0011,
    XMR_OBJECT_TYPE_EXPIRATION_OBJECT                                  = 0x0012,
    XMR_OBJECT_TYPE_ISSUEDATE_OBJECT                                   = 0x0013,
    XMR_OBJECT_TYPE_EXPIRATION_AFTER_FIRSTUSE_OBJECT                   = 0x0014,
    XMR_OBJECT_TYPE_EXPIRATION_AFTER_FIRSTSTORE_OBJECT                 = 0x0015,
    XMR_OBJECT_TYPE_METERING_OBJECT                                    = 0x0016,
    XMR_OBJECT_TYPE_PLAYCOUNT_OBJECT                                   = 0x0017,
    XMR_OBJECT_TYPE_GRACE_PERIOD_OBJECT                                = 0x001A,
    XMR_OBJECT_TYPE_COPYCOUNT_OBJECT                                   = 0x001B,
    XMR_OBJECT_TYPE_COPY_PROTECTION_OBJECT                             = 0x001C,
    XMR_OBJECT_TYPE_PLAYLISTBURN_COUNT_OBJECT                          = 0x001F,
    XMR_OBJECT_TYPE_REVOCATION_INFORMATION_VERSION_OBJECT              = 0x0020,
    XMR_OBJECT_TYPE_RSA_DEVICE_KEY_OBJECT                              = 0x0021,
    XMR_OBJECT_TYPE_SOURCEID_OBJECT                                    = 0x0022,
    XMR_OBJECT_TYPE_REVOCATION_CONTAINER                               = 0x0025,
    XMR_OBJECT_TYPE_RSA_LICENSE_GRANTER_KEY_OBJECT                     = 0x0026,
    XMR_OBJECT_TYPE_USERID_OBJECT                                      = 0x0027,
    XMR_OBJECT_TYPE_RESTRICTED_SOURCEID_OBJECT                         = 0x0028,
    XMR_OBJECT_TYPE_DOMAIN_ID_OBJECT                                   = 0x0029,
    XMR_OBJECT_TYPE_ECC_DEVICE_KEY_OBJECT                              = 0x002A,
    XMR_OBJECT_TYPE_GENERATION_NUMBER_OBJECT                           = 0x002B,
    XMR_OBJECT_TYPE_POLICY_METADATA_OBJECT                             = 0x002C,
    XMR_OBJECT_TYPE_OPTIMIZED_CONTENT_KEY_OBJECT                       = 0x002D,
    XMR_OBJECT_TYPE_EXPLICIT_DIGITAL_AUDIO_OUTPUT_PROTECTION_CONTAINER = 0x002E,
    XMR_OBJECT_TYPE_RINGTONE_POLICY_CONTAINER                          = 0x002F,
    XMR_OBJECT_TYPE_EXPIRATION_AFTER_FIRSTPLAY_OBJECT                  = 0x0030,
    XMR_OBJECT_TYPE_DIGITAL_AUDIO_OUTPUT_CONFIGURATION_OBJECT          = 0x0031,
    XMR_OBJECT_TYPE_REVOCATION_INFORMATION_VERSION_2_OBJECT            = 0x0032,
    XMR_OBJECT_TYPE_EMBEDDING_BEHAVIOR_OBJECT                          = 0x0033,
    XMR_OBJECT_TYPE_SECURITY_LEVEL                                     = 0x0034,
    XMR_OBJECT_TYPE_COPY_TO_PC_CONTAINER                               = 0x0035,
    XMR_OBJECT_TYPE_PLAY_ENABLER_CONTAINER                             = 0x0036,
    XMR_OBJECT_TYPE_MOVE_ENABLER_OBJECT                                = 0x0037,
    XMR_OBJECT_TYPE_COPY_ENABLER_CONTAINER                             = 0x0038,
    XMR_OBJECT_TYPE_PLAY_ENABLER_OBJECT                                = 0x0039,
    XMR_OBJECT_TYPE_COPY_ENABLER_OBJECT                                = 0x003A,
    XMR_OBJECT_TYPE_UPLINK_KID_2_OBJECT                                = 0x003B,
    XMR_OBJECT_TYPE_COPY_POLICY_2_CONTAINER                            = 0x003C,
    XMR_OBJECT_TYPE_COPYCOUNT_2_OBJECT                                 = 0x003D,
    XMR_OBJECT_TYPE_RINGTONE_ENABLER_OBJECT                            = 0x003E,
    XMR_OBJECT_TYPE_EXECUTE_POLICY_CONTAINER                           = 0x003F,
    XMR_OBJECT_TYPE_EXECUTE_POLICY_OBJECT                              = 0x0040,
    XMR_OBJECT_TYPE_READ_POLICY_CONTAINER                              = 0x0041,
    XMR_OBJECT_TYPE_EXTENSIBLE_POLICY_RESERVED_42                      = 0x0042,
    XMR_OBJECT_TYPE_EXTENSIBLE_POLICY_RESERVED_43                      = 0x0043,
    XMR_OBJECT_TYPE_EXTENSIBLE_POLICY_RESERVED_44                      = 0x0044,
    XMR_OBJECT_TYPE_EXTENSIBLE_POLICY_RESERVED_45                      = 0x0045,
    XMR_OBJECT_TYPE_EXTENSIBLE_POLICY_RESERVED_46                      = 0x0046,
    XMR_OBJECT_TYPE_EXTENSIBLE_POLICY_RESERVED_47                      = 0x0047,
    XMR_OBJECT_TYPE_EXTENSIBLE_POLICY_RESERVED_48                      = 0x0048,
    XMR_OBJECT_TYPE_EXTENSIBLE_POLICY_RESERVED_49                      = 0x0049,
    XMR_OBJECT_TYPE_EXTENSIBLE_POLICY_RESERVED_4A                      = 0x004A,
    XMR_OBJECT_TYPE_EXTENSIBLE_POLICY_RESERVED_4B                      = 0x004B,
    XMR_OBJECT_TYPE_EXTENSIBLE_POLICY_RESERVED_4C                      = 0x004C,
    XMR_OBJECT_TYPE_EXTENSIBLE_POLICY_RESERVED_4D                      = 0x004D,
    XMR_OBJECT_TYPE_EXTENSIBLE_POLICY_RESERVED_4E                      = 0x004E,
    XMR_OBJECT_TYPE_EXTENSIBLE_POLICY_RESERVED_4F                      = 0x004F,
    XMR_OBJECT_TYPE_REMOVAL_DATE_OBJECT                                = 0x0050,
    XMR_OBJECT_TYPE_AUX_KEY_OBJECT                                     = 0x0051,
    XMR_OBJECT_TYPE_UPLINKX_OBJECT                                     = 0x0052,
    XMR_OBJECT_TYPE_INVALID_RESERVED_53                                = 0x0053,    /* Deprecated: 0x0053 was documented in PlayReady 2.0 CHM for Content Boundary Identification Object */
    XMR_OBJECT_TYPE_APPLICATION_ID_LIST                                = 0x0054,
    XMR_OBJECT_TYPE_REAL_TIME_EXPIRATION                               = 0x0055,
    XMR_OBJECT_TYPE_ND_TX_AUTH_CONTAINER                               = 0x0056,
    XMR_OBJECT_TYPE_ND_TX_AUTH_OBJECT                                  = 0x0057,
    XMR_OBJECT_TYPE_EXPLICIT_DIGITAL_VIDEO_OUTPUT_PROTECTION_CONTAINER = 0x0058,
    XMR_OBJECT_TYPE_DIGITAL_VIDEO_OUTPUT_CONFIGURATION_OBJECT          = 0x0059,
    XMR_OBJECT_TYPE_SECURE_STOP_OBJECT                                 = 0x005A,
    XMR_OBJECT_TYPE_MAXIMUM_DEFINED                                    = 0x005A

    /*
    ** When adding a new object but NOT increasing XMR_OBJECT_TYPE_MAXIMUM_DEFINED (and thus NOT updating the various tables),
    **  please include the following information in comments so that when XMR_OBJECT_TYPE_MAXIMUM_DEFINED is increasing
    **  in a later release, the appropriate table information can be added without having to dig through specs.
    ** 1. To update drmxmr.c:
    **  fIsContainer:                 TRUE or FALSE
    **  wParent:                      The parent object for this object
    **  XMR_OBJECT_FLAGS:             A bitmask of XMR_OBJECT_FLAGS or XMR_FLAGS_NONE
    **                                (Note: must match fIsContainer for whether XMR_FLAGS_CONTAINER is set or not)
    ** 2. To update drmxmrbuilder.c:
    **  XMR_BUILDER_INTERNAL_FLAGS:   A bitmask of XMR_BUILDER_INTERNAL_FLAGS or XMR_BUILDER_INTERNAL_FLAG_NO_FLAGS
    **
    ** When increasing XMR_OBJECT_TYPE_MAXIMUM_DEFINED, remove the comments at the same time as you use them to update the various tables.
    */
};

EXIT_PK_NAMESPACE;

#endif  /* __DRM_XMR_CONSTANTS_H__ */

