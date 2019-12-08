/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#ifndef __DRMBCERTCONTSTANTS_H__
#define __DRMBCERTCONTSTANTS_H__

#include <drmtypes.h>
#include <oemsha256.h>
#include <oemeccp256.h>

ENTER_PK_NAMESPACE;

#define DRM_BCERT_MAX_URL_LENGTH                  4096        /* bytes */
#define DRM_BCERT_MAX_EXTDATA_RECORDS             1
#define DRM_BINARY_DEVICE_CERT_MAX_KEYUSAGES      3
#define DRM_BCERT_MAX_MANUFACTURER_STRING_LENGTH  128         /* bytes */
#define DRM_BCERT_CHAIN_HEADER_TAG                0x43484149  /* "CHAI" */

#define DRM_BCERT_CLIENT_ID_LENGTH   16  /* 128 bits */
#define DRM_BCERT_CERT_ID_LENGTH     16  /* 128 bits */
#define DRM_BCERT_ACCOUNT_ID_LENGTH  16  /* 128 bits */
#define DRM_BCERT_METERING_ID_LENGTH 16  /* 128 bits */
#define DRM_BCERT_SERVICE_ID_LENGTH  16  /* 128 bits */

/*
** Minimal length of a certificate chain in bytes - 20
*/
#define DRM_MIN_CERT_CHAIN_LEN \
    ( sizeof( DRM_DWORD )           /* Header Constant */ \
    + sizeof( DRM_DWORD )           /* Version         */ \
    + sizeof( DRM_DWORD )           /* Length          */ \
    + sizeof( DRM_DWORD )           /* Flags           */ \
    + sizeof( DRM_DWORD ) )         /* No of certificate entries: zero for minimal cert chain */

#define DRM_BCERT_HEADER_DATA_SIZE_OFFSET  \
    ( sizeof( DRM_DWORD )           /* Header tag          */ \
    + sizeof( DRM_DWORD ) )         /* Version             */

#define DRM_BCERT_HEADER_SIGNED_DATA_SIZE_OFFSET  \
    ( sizeof( DRM_DWORD )           /* Header tag          */ \
    + sizeof( DRM_DWORD )           /* Version             */ \
    + sizeof( DRM_DWORD ) )         /* Certificate size    */

/*
** Defines for verification of object lengths in object headers
*/
#define DRM_BCERT_HEADER_LEN \
    ( sizeof( DRM_DWORD )           /* Header tag          */ \
    + sizeof( DRM_DWORD )           /* Version             */ \
    + sizeof( DRM_DWORD )           /* Certificate size    */ \
    + sizeof( DRM_DWORD ) )         /* Size of signed data */

#define DRM_BCERT_OBJECT_HEADER_LEN \
    ( sizeof( DRM_WORD )            /* Flags  */ \
    + sizeof( DRM_WORD )            /* Type   */ \
    + sizeof( DRM_DWORD ) )         /* Length */


#define DRM_BCERT_SIGNATURE_OBJECT_LEN \
    ( DRM_BCERT_OBJECT_HEADER_LEN   /* Object header             */ \
    + sizeof( DRM_DWORD )           /* Signature type and length */ \
    + sizeof( SIGNATURE_P256 )      /* Signature data            */ \
    + sizeof( DRM_DWORD )           /* Issuer key length         */ \
    + sizeof( PUBKEY_P256 ) )       /* Issuer key data           */

#define DRM_BCERT_BASIC_INFO_OBJECT_LEN \
    ( DRM_BCERT_OBJECT_HEADER_LEN   /* Object header     */ \
    + sizeof( DRM_ID )              /* Certificate ID    */ \
    + sizeof( DRM_DWORD )           /* Security level    */ \
    + sizeof( DRM_DWORD )           /* Certificate Flags */ \
    + sizeof( DRM_DWORD )           /* Type              */ \
    + SHA256_DIGEST_SIZE_IN_BYTES   /* Digest Value      */ \
    + sizeof( DRM_DWORD )           /* Expiration Date   */ \
    + sizeof( DRM_ID ) )            /* Client ID         */

#define DRM_BCERT_DEVICE_INFO_OBJECT_LEN \
    ( DRM_BCERT_OBJECT_HEADER_LEN   /* Object header           */ \
    + sizeof( DRM_DWORD )           /* Max License Size        */ \
    + sizeof( DRM_DWORD )           /* Max Header Size         */ \
    + sizeof( DRM_DWORD ) )         /* Max License Chain Depth */

#define DRM_BCERT_FEATURES_OBJECT_MIN_LEN \
    ( DRM_BCERT_OBJECT_HEADER_LEN   /* Object header                                     */ \
    + sizeof( DRM_DWORD ) )         /* Number of features: zero for minimal feature info */

#define DRM_BCERT_KEY_OBJECT_MIN_LEN \
    ( DRM_BCERT_OBJECT_HEADER_LEN   /* Object header     */ \
    + sizeof( DRM_WORD )            /* Type              */ \
    + sizeof( DRM_WORD )            /* Length            */ \
    + sizeof( DRM_DWORD )           /* Flags             */ \
    + ECC_P256_POINT_SIZE_IN_BYTES  /* Value             */ \
    + sizeof( DRM_DWORD )           /* Number of Entries */ \
    + sizeof( DRM_DWORD ) )         /* One entry         */


#define DRM_BCERT_EXTDATA_SIG_LEN \
    ( DRM_BCERT_OBJECT_HEADER_LEN   /* Object header     */ \
    + sizeof( DRM_WORD )            /* Signature type    */ \
    + sizeof( SIGNATURE_P256 ) )    /* Signatrue length  */


/*
** Minimal length of a certificate:
** Cert header, Basic Info, Feature Info with zero features,
** Key Info and Signature Info.
*/
#define DRM_MIN_CERTIFICATE_LEN \
    ( DRM_BCERT_HEADER_LEN               \
    + DRM_BCERT_BASIC_INFO_OBJECT_LEN    \
    + DRM_BCERT_FEATURES_OBJECT_MIN_LEN  \
    + DRM_BCERT_KEY_OBJECT_MIN_LEN       \
    + DRM_BCERT_SIGNATURE_OBJECT_LEN )

#define DRM_BCERT_MAX_CERTS_PER_CHAIN   6

#define DRM_BCERT_OBJFLAG_EMPTY             0x0000
#define DRM_BCERT_OBJFLAG_MUST_UNDERSTAND   0x0001
#define DRM_BCERT_OBJFLAG_CONTAINER_OBJ     0x0002


/* ------------------------------------------------------------------------- */
/*  BASIC INFO  */

#define DRM_BCERT_SECURITYLEVEL_150         150  /* for testing */
#define DRM_BCERT_SECURITYLEVEL_2000        2000
#define DRM_BCERT_SECURITYLEVEL_3000        3000 /* Refer to PlayReady compliance rules (CRs) */

#define DRM_BCERT_FLAGS_EMPTY               0x00000000 /* default */
#define DRM_BCERT_FLAGS_EXTDATA_PRESENT     0x00000001 /* presence of extended data */

#define DRM_BCERT_CERTTYPE_UNKNOWN          0x00000000
#define DRM_BCERT_CERTTYPE_PC               0x00000001
#define DRM_BCERT_CERTTYPE_DEVICE           0x00000002
#define DRM_BCERT_CERTTYPE_DOMAIN           0x00000003
#define DRM_BCERT_CERTTYPE_ISSUER           0x00000004
#define DRM_BCERT_CERTTYPE_CRL_SIGNER       0x00000005
#define DRM_BCERT_CERTTYPE_SERVICE          0x00000006
#define DRM_BCERT_CERTTYPE_SILVERLIGHT      0x00000007
#define DRM_BCERT_CERTTYPE_APPLICATION      0x00000008
#define DRM_BCERT_CERTTYPE_METERING         0x00000009
#define DRM_BCERT_CERTTYPE_KEYFILESIGNER    0x0000000a
#define DRM_BCERT_CERTTYPE_SERVER           0x0000000b
#define DRM_BCERT_CERTTYPE_LICENSESIGNER    0x0000000c

#define DRM_BCERT_CERTTYPE_MAX_VALUE        0x0000000c

#define DRM_BCERT_DEFAULT_EXPIRATION_DATE   0xFFFFFFFF

/* ------------------------------------------------------------------------- */
/*  SILVERLIGHT INFO  */


/*  Platform Identifiers for the SilverLight INFO structure, v2/v3 (not v4) */
#define DRM_BCERT_SILVERLIGHT_PLATFORM_WINDOWS_X86  0
#define DRM_BCERT_SILVERLIGHT_PLATFORM_WINDOWS_X64  1
#define DRM_BCERT_SILVERLIGHT_PLATFORM_OSX_PPC      2
#define DRM_BCERT_SILVERLIGHT_PLATFORM_OSX_X86      3
#define DRM_BCERT_SILVERLIGHT_PLATFORM_OSX_X64      4
#define DRM_BCERT_SILVERLIGHT_PLATFORM_MAX          4

/* ------------------------------------------------------------------------- */
/*  DEVICE INFO  */


#define DRM_BCERT_MAX_LICENSE_SIZE         10240  /* 10K */
#define DRM_BCERT_MAX_HEADER_SIZE          15360  /* 15K */
#define DRM_BCERT_MAX_LICENSE_CHAIN_DEPTH      2

/* ------------------------------------------------------------------------- */
/*  SECURITY VERSION */

#define DRM_BCERT_SECURITY_VERSION_UNSPECIFIED           (DRM_MAX_UNSIGNED_TYPE(DRM_DWORD))
#define DRM_BCERT_SECURITY_VERSION_PLATFORM_UNSPECIFIED  (DRM_MAX_UNSIGNED_TYPE(DRM_DWORD))
#define DRM_BCERT_SECURITY_VERSION_PLATFORM_OEM          DRM_BCERT_SECURITY_VERSION_PLATFORM_UNSPECIFIED

#define DRM_BCERT_SECURITY_VERSION_PLATFORM_WINDOWS      0x0
#define DRM_BCERT_SECURITY_VERSION_PLATFORM_OSX          0x1
#define DRM_BCERT_SECURITY_VERSION_PLATFORM_WINDOWS_ARM  0x2

#define DRM_BCERT_SECURITY_VERSION_PLATFORM_WM_7         0x5

#define DRM_BCERT_SECURITY_VERSION_PLATFORM_IOS_ARM      0x6
#define DRM_BCERT_SECURITY_VERSION_PLATFORM_XBOX_PPC     0x7
#define DRM_BCERT_SECURITY_VERSION_PLATFORM_WP8_ARM      0x8
#define DRM_BCERT_SECURITY_VERSION_PLATFORM_WP8_X86      0x9
#define DRM_BCERT_SECURITY_VERSION_PLATFORM_XBOX_ONE     0xa
#define DRM_BCERT_SECURITY_VERSION_PLATFORM_ANDROID_ARM  0xb
#define DRM_BCERT_SECURITY_VERSION_PLATFORM_WP8_1_ARM    0xc
#define DRM_BCERT_SECURITY_VERSION_PLATFORM_WP8_1_X86    0xd

/* ------------------------------------------------------------------------- */
/*  FEATURE INFO  */

#define DRM_BCERT_MAX_FEATURES 32    /* array size with room to grow */

#define DRM_BCERT_FEATURE_TRANSMITTER             0x00000001
#define DRM_BCERT_FEATURE_RECEIVER                0x00000002
#define DRM_BCERT_FEATURE_SHARED_CERTIFICATE      0x00000003
#define DRM_BCERT_FEATURE_SECURE_CLOCK            0x00000004
#define DRM_BCERT_FEATURE_ANTIROLLBACK_CLOCK      0x00000005
/*
** The next three feature flags should not be explicitly set in the certs
** because for any cert their values are already known
** (i.e. every device must support metering, license sync and symmetric optimization)
** These features will stay deprecated as long as they are not optional.
*/
#define DRM_BCERT_FEATURE_RESERVED_METERING       0x00000006
#define DRM_BCERT_FEATURE_RESERVED_LICSYNC        0x00000007
#define DRM_BCERT_FEATURE_RESERVED_SYMOPT         0x00000008
#define DRM_BCERT_FEATURE_SUPPORTS_CRLS           0x00000009

/*
** Server features
*/
#define DRM_BCERT_FEATURE_SERVER_BASIC_EDITION    0x0000000A
#define DRM_BCERT_FEATURE_SERVER_STANDARD_EDITION 0x0000000B
#define DRM_BCERT_FEATURE_SERVER_PREMIUM_EDITION  0x0000000C

/* Client supports different keys for different streams in the content even without key rotation, e.g. for Audio and Video */
#define DRM_BCERT_FEATURE_SUPPORTS_PR3_FEATURES   0x0000000D

#define DRM_BCERT_FEATURE_DEPRECATED_SECURE_STOP  0x0000000E

#define DRM_BCERT_FEATURE_MAX_VALUE               0x0000000E


#define BCERT_FEATURE_BIT(dwFeature)     (1 << ((dwFeature)-1))
#define DRM_BCERT_IS_FEATURE_SUPPORTED(dwSupportedFeatures, dwFeature)    ( 0 != ( BCERT_FEATURE_BIT( dwFeature ) & (dwSupportedFeatures) ) )

/* ------------------------------------------------------------------------- */
/*  KEY INFO  */


/* List of supported key types. (currently just one) */
#define DRM_BCERT_KEYTYPE_ECC256   0x0001

#define DRM_BCERT_KEYUSAGE_UNKNOWN                                0x00000000
#define DRM_BCERT_KEYUSAGE_SIGN                                   0x00000001
#define DRM_BCERT_KEYUSAGE_ENCRYPT_KEY                            0x00000002
#define DRM_BCERT_KEYUSAGE_SIGN_CRL                               0x00000003
#define DRM_BCERT_KEYUSAGE_ISSUER_ALL                             0x00000004
#define DRM_BCERT_KEYUSAGE_ISSUER_INDIV                           0x00000005
#define DRM_BCERT_KEYUSAGE_ISSUER_DEVICE                          0x00000006
#define DRM_BCERT_KEYUSAGE_ISSUER_LINK                            0x00000007
#define DRM_BCERT_KEYUSAGE_ISSUER_DOMAIN                          0x00000008
#define DRM_BCERT_KEYUSAGE_ISSUER_SILVERLIGHT                     0x00000009
#define DRM_BCERT_KEYUSAGE_ISSUER_APPLICATION                     0x0000000a
#define DRM_BCERT_KEYUSAGE_ISSUER_CRL                             0x0000000b
#define DRM_BCERT_KEYUSAGE_ISSUER_METERING                        0x0000000c
#define DRM_BCERT_KEYUSAGE_ISSUER_SIGN_KEYFILE                    0x0000000d
#define DRM_BCERT_KEYUSAGE_SIGN_KEYFILE                           0x0000000e
#define DRM_BCERT_KEYUSAGE_ISSUER_SERVER                          0x0000000f
#define DRM_BCERT_KEYUSAGE_ENCRYPTKEY_SAMPLE_PROTECTION_RC4       0x00000010
#define DRM_BCERT_KEYUSAGE_RESERVED2                              0x00000011
#define DRM_BCERT_KEYUSAGE_ISSUER_SIGN_LICENSE                    0x00000012
#define DRM_BCERT_KEYUSAGE_SIGN_LICENSE                           0x00000013
#define DRM_BCERT_KEYUSAGE_SIGN_RESPONSE                          0x00000014
#define DRM_BCERT_KEYUSAGE_PRND_ENCRYPT_KEY                       0x00000015
#define DRM_BCERT_KEYUSAGE_ENCRYPTKEY_SAMPLE_PROTECTION_AES128CTR 0x00000016

#define DRM_BCERT_KEYUSAGE_MAX_VALUE                              0x00000016

/*
** DRM_BCERT_MAX_KEY_USAGES has to be larger than DRM_BCERT_KEYUSAGE_MAX_VALUE.
** DRM_BCERT_MAX_KEY_USAGES should not exceed 32 since a bitmap based on DRM_DWORD
** is currently used to represent key usage set.
*/
#define DRM_BCERT_MAX_KEY_USAGES                23

#define BCERT_KEYUSAGE_BIT(dwKeyUsage)          (1 << ((dwKeyUsage)-1))
#define DRM_BCERT_IS_KEYUSAGE_SUPPORTED(dwSupportedKeyUsages, dwFeature) ( 0 != ( BCERT_KEYUSAGE_BIT( dwFeature ) & (dwSupportedKeyUsages) ) )

#define BCERT_KEYUSAGE_PARENT_ISSUERS_MASK                          \
    ( BCERT_KEYUSAGE_BIT( DRM_BCERT_KEYUSAGE_ISSUER_INDIV )         \
    | BCERT_KEYUSAGE_BIT( DRM_BCERT_KEYUSAGE_ISSUER_DEVICE )        \
    | BCERT_KEYUSAGE_BIT( DRM_BCERT_KEYUSAGE_ISSUER_LINK )          \
    | BCERT_KEYUSAGE_BIT( DRM_BCERT_KEYUSAGE_ISSUER_DOMAIN )        \
    | BCERT_KEYUSAGE_BIT( DRM_BCERT_KEYUSAGE_ISSUER_SILVERLIGHT )   \
    | BCERT_KEYUSAGE_BIT( DRM_BCERT_KEYUSAGE_ISSUER_APPLICATION )   \
    | BCERT_KEYUSAGE_BIT( DRM_BCERT_KEYUSAGE_ISSUER_CRL )           \
    | BCERT_KEYUSAGE_BIT( DRM_BCERT_KEYUSAGE_ISSUER_SIGN_KEYFILE )  \
    | BCERT_KEYUSAGE_BIT( DRM_BCERT_KEYUSAGE_ISSUER_METERING )      \
    | BCERT_KEYUSAGE_BIT( DRM_BCERT_KEYUSAGE_ISSUER_SIGN_LICENSE ) )


/*
** The following byte value should be equal to the largest public key length supported
*/
#define DRM_BCERT_MAX_PUBKEY_VALUE_LENGTH     sizeof( PUBKEY_P256 )

#define DRM_BCERT_PUBKEY    PUBKEY_P256


/* ------------------------------------------------------------------------- */
/*  SIGNATURE INFO  */


#define DRM_BCERT_SIGNATURE_TYPE_P256         0x0001    /* ECDSA-P-256-SHA-256 */

/*
** The following byte value should be equal to the largest signature length supported
*/
#define DRM_BCERT_MAX_SIGNATURE_DATA_LENGTH   sizeof( SIGNATURE_P256 )

EXIT_PK_NAMESPACE;

#endif /* __DRMBCERTCONSTANTS_H__ */
