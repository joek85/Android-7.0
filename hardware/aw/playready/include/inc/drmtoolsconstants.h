/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#ifndef __DRMTOOLSCONSTANTS_H__
#define __DRMTOOLSCONSTANTS_H__

#include <drmtypes.h>
#include <drmprofiletypes.h>

ENTER_PK_NAMESPACE;

extern DRM_GLOBAL_CONST  DRM_BYTE          g_rgbFFFE[ 2 ];

/*
** cert tools
*/

extern DRM_GLOBAL_CONST  DRM_CONST_STRING  g_dstrTagRequestDAC;
extern DRM_GLOBAL_CONST  DRM_CONST_STRING  g_dstrTagDAC_WMDRM_MD;
extern DRM_GLOBAL_CONST  DRM_CONST_STRING  g_dstrTagDAC_WMDRM_ND;
extern DRM_GLOBAL_CONST  DRM_CONST_STRING  g_dstrTagManufName;
extern DRM_GLOBAL_CONST  DRM_CONST_STRING  g_dstrTagOEMPrivateKeys;
extern DRM_GLOBAL_CONST  DRM_CONST_STRING  g_dstrTagKeyValue;
extern DRM_GLOBAL_CONST  DRM_CONST_STRING  g_dstrTagRSAKeyValue;
extern DRM_GLOBAL_CONST  DRM_CONST_STRING  g_dstrTagSecurityLevel;
extern DRM_GLOBAL_CONST  DRM_CONST_STRING  g_dstrModel;
extern DRM_GLOBAL_CONST  DRM_CONST_STRING  g_dstrUnsignedtemplate;
extern DRM_GLOBAL_CONST  DRM_CONST_STRING  g_dstrTagDataSecVerNumber;
extern DRM_GLOBAL_CONST  DRM_CONST_STRING  g_dstrTagLicense;
extern DRM_GLOBAL_CONST  DRM_CONST_STRING  g_dstrTagName;

/*
** Xpaths
*/

extern DRM_GLOBAL_CONST  DRM_CONST_STRING  g_dstrXPathWMDRM_MDPubKey;
extern DRM_GLOBAL_CONST  DRM_CONST_STRING  g_dstrXPathWMDRMNDPubKeyModulus;
extern DRM_GLOBAL_CONST  DRM_CONST_STRING  g_dstrXPathWMDRMNDPubKeyExponent;
extern DRM_GLOBAL_CONST  DRM_CONST_STRING  g_dstrXPathPlayReadyNDPubKeyModulus;
extern DRM_GLOBAL_CONST  DRM_CONST_STRING  g_dstrXPathPlayReadyNDPubKeyExponent;
extern DRM_GLOBAL_CONST  DRM_CONST_STRING  g_dstrXPathWMDRMPrivKey;
extern DRM_GLOBAL_CONST  DRM_CONST_STRING  g_dstrXPathWMDRMPubKey;

/*
** generateDAC
*/

extern DRM_GLOBAL_CONST  DRM_CONST_STRING  g_dstrTagDataSecurityVersionValue;
extern DRM_GLOBAL_CONST  DRM_CONST_STRING  g_dstrAuthorizationIDDAC;
extern DRM_GLOBAL_CONST  DRM_CONST_STRING  g_dstrTagSecurityLevelValue;

/*
** XML utils
*/

extern DRM_GLOBAL_CONST  DRM_CONST_STRING  g_dstrTestTemplate;
extern DRM_GLOBAL_CONST  DRM_CONST_STRING  g_dstrXMLTree1;
extern DRM_GLOBAL_CONST  DRM_CONST_STRING  g_dstrXMLTree2;
extern DRM_GLOBAL_CONST  DRM_CONST_STRING  g_dstrXMLTree3;
extern DRM_GLOBAL_CONST  DRM_CONST_STRING  g_dstrXMLTree4;

/*
** package tools
*/

extern DRM_GLOBAL_CONST  DRM_CONST_STRING  g_dstrTagRoot;
extern DRM_GLOBAL_CONST  DRM_CONST_STRING  g_dstrTagKeySeed;
extern DRM_GLOBAL_CONST  DRM_CONST_STRING  g_dstrTagLicAcqInfo;
extern DRM_GLOBAL_CONST  DRM_CONST_STRING  g_dstrTagCustomData;
extern DRM_GLOBAL_CONST  DRM_CONST_STRING  g_dstrEncBlkSize;
extern DRM_GLOBAL_CONST  DRM_CONST_STRING  g_dstrCID;
extern DRM_GLOBAL_CONST  DRM_CONST_STRING  g_dstrTagPrivkey;
extern DRM_GLOBAL_CONST  DRM_CONST_STRING  g_dstrTagKeypair;

/*
** PlayReady Certificate Generator
*/

extern DRM_GLOBAL_CONST  DRM_CONST_STRING  g_dstrVersion;
extern DRM_GLOBAL_CONST  DRM_CONST_STRING  g_dstrCertType;
extern DRM_GLOBAL_CONST  DRM_CONST_STRING  g_dstrDevice;
extern DRM_GLOBAL_CONST  DRM_CONST_STRING  g_dstrPC;
extern DRM_GLOBAL_CONST  DRM_CONST_STRING  g_dstrExtendedData;
extern DRM_GLOBAL_CONST  DRM_CONST_STRING  g_dstrExtendedDataSigningKey;
extern DRM_GLOBAL_CONST  DRM_CONST_STRING  g_dstrExtendedDataSigningKeyPublicKey;
extern DRM_GLOBAL_CONST  DRM_CONST_STRING  g_dstrExtendedDataSigningKeyPrivateKey;
extern DRM_GLOBAL_CONST  DRM_CONST_STRING  g_dstrExtendedDataRecord;
extern DRM_GLOBAL_CONST  DRM_CONST_STRING  g_dstrDomain;
extern DRM_GLOBAL_CONST  DRM_CONST_STRING  g_dstrSilverLight;
extern DRM_GLOBAL_CONST  DRM_CONST_STRING  g_dstrApplication;
extern DRM_GLOBAL_CONST  DRM_CONST_STRING  g_dstrMeteringCertType;
extern DRM_GLOBAL_CONST  DRM_CONST_STRING  g_dstrServerCertType;
extern DRM_GLOBAL_CONST  DRM_CONST_STRING  g_dstrIssuer;
extern DRM_GLOBAL_CONST  DRM_CONST_STRING  g_dstrService;
extern DRM_GLOBAL_CONST  DRM_CONST_STRING  g_dstrCRLSigner;
extern DRM_GLOBAL_CONST  DRM_CONST_STRING  g_dstrKeyFileSigner;
extern DRM_GLOBAL_CONST  DRM_CONST_STRING  g_dstrLicenseSigner;
extern DRM_GLOBAL_CONST  DRM_CONST_STRING  g_dstrBasicInfo;
extern DRM_GLOBAL_CONST  DRM_CONST_STRING  g_dstrSecurityLevel;
extern DRM_GLOBAL_CONST  DRM_CONST_STRING  g_dstrClientID;
extern DRM_GLOBAL_CONST  DRM_CONST_STRING  g_dstrExpiration;
extern DRM_GLOBAL_CONST  DRM_CONST_STRING  g_dstrWarningDays;
extern DRM_GLOBAL_CONST  DRM_CONST_STRING  g_dstrServiceID;
extern DRM_GLOBAL_CONST  DRM_CONST_STRING  g_dstrAccountID;
extern DRM_GLOBAL_CONST  DRM_CONST_STRING  g_dstrRevision;
extern DRM_GLOBAL_CONST  DRM_CONST_STRING  g_dstrDomainUrl;
extern DRM_GLOBAL_CONST  DRM_CONST_STRING  g_dstrSecurityVersion;
extern DRM_GLOBAL_CONST  DRM_CONST_STRING  g_dstrSecurityVersionSeperator;
extern DRM_GLOBAL_CONST  DRM_CONST_STRING  g_dstrExtendedDataSigningKeyType;
extern DRM_GLOBAL_CONST  DRM_CONST_STRING  g_dstrExtendedDataSigningKeyPublicKeyValue;
extern DRM_GLOBAL_CONST  DRM_CONST_STRING  g_dstrExtendedDataSigningKeyPrivateKeyValue;
extern DRM_GLOBAL_CONST  DRM_CONST_STRING  g_dstrExtendedDataRecordType;
extern DRM_GLOBAL_CONST  DRM_CONST_STRING  g_dstrExtendedDataRecordFlags;
extern DRM_GLOBAL_CONST  DRM_CONST_STRING  g_dstrExtendedDataRecordMaxLength;
extern DRM_GLOBAL_CONST  DRM_CONST_STRING  g_dstrExtendedDataRecordValue;
extern DRM_GLOBAL_CONST  DRM_CONST_STRING  g_dstrTransmitter;
extern DRM_GLOBAL_CONST  DRM_CONST_STRING  g_dstrReceiver;
extern DRM_GLOBAL_CONST  DRM_CONST_STRING  g_dstrSharedCertificate;
extern DRM_GLOBAL_CONST  DRM_CONST_STRING  g_dstrSecureClock;
extern DRM_GLOBAL_CONST  DRM_CONST_STRING  g_dstrSampleProtectionRC4;
extern DRM_GLOBAL_CONST  DRM_CONST_STRING  g_dstrSampleProtectionAES128CTR;
extern DRM_GLOBAL_CONST  DRM_CONST_STRING  g_dstrRollbackClock;
extern DRM_GLOBAL_CONST  DRM_CONST_STRING  g_dstrMetering;
extern DRM_GLOBAL_CONST  DRM_CONST_STRING  g_dstrSymmOpt;
extern DRM_GLOBAL_CONST  DRM_CONST_STRING  g_dstrSupportCRL;
extern DRM_GLOBAL_CONST  DRM_CONST_STRING  g_dstrSupportPlayReady3Features;
extern DRM_GLOBAL_CONST  DRM_CONST_STRING  g_dstrServerBasicEdition;
extern DRM_GLOBAL_CONST  DRM_CONST_STRING  g_dstrServerStandardEdition;
extern DRM_GLOBAL_CONST  DRM_CONST_STRING  g_dstrServerPremiumEdition;
extern DRM_GLOBAL_CONST  DRM_CONST_STRING  g_dstrCertFeatures;
extern DRM_GLOBAL_CONST  DRM_CONST_STRING  g_dstrCertManufacturer;
extern DRM_GLOBAL_CONST  DRM_CONST_STRING  g_dstrManufacturerName;
extern DRM_GLOBAL_CONST  DRM_CONST_STRING  g_dstrMake;
extern DRM_GLOBAL_CONST  DRM_CONST_STRING  g_dstrModelName;
extern DRM_GLOBAL_CONST  DRM_CONST_STRING  g_dstrModelNumber;
extern DRM_GLOBAL_CONST  DRM_CONST_STRING  g_dstrLimits;
extern DRM_GLOBAL_CONST  DRM_CONST_STRING  g_dstrCertKey;
extern DRM_GLOBAL_CONST  DRM_CONST_STRING  g_dstrCertKeyType;
extern DRM_GLOBAL_CONST  DRM_CONST_STRING  g_dstrCertKeys;
extern DRM_GLOBAL_CONST  DRM_CONST_STRING  g_dstrCertECC256;
extern DRM_GLOBAL_CONST  DRM_CONST_STRING  g_dstrCertKeyUsages;
extern DRM_GLOBAL_CONST  DRM_CONST_STRING  g_dstrKeyUsageSign;
extern DRM_GLOBAL_CONST  DRM_CONST_STRING  g_dstrKeyUsageEncryptKey;
extern DRM_GLOBAL_CONST  DRM_CONST_STRING  g_dstrKeyUsageSignCRL;
extern DRM_GLOBAL_CONST  DRM_CONST_STRING  g_dstrKeyUsageIssuerAll;
extern DRM_GLOBAL_CONST  DRM_CONST_STRING  g_dstrKeyUsageIssuerIndiv;
extern DRM_GLOBAL_CONST  DRM_CONST_STRING  g_dstrKeyUsageIssuerDevice;
extern DRM_GLOBAL_CONST  DRM_CONST_STRING  g_dstrKeyUsageIssuerLink;
extern DRM_GLOBAL_CONST  DRM_CONST_STRING  g_dstrKeyUsageIssuerDomain;
extern DRM_GLOBAL_CONST  DRM_CONST_STRING  g_dstrKeyUsageIssuerSilverLight;
extern DRM_GLOBAL_CONST  DRM_CONST_STRING  g_dstrKeyUsageIssuerApplication;
extern DRM_GLOBAL_CONST  DRM_CONST_STRING  g_dstrKeyUsageIssuerCRLSigner;
extern DRM_GLOBAL_CONST  DRM_CONST_STRING  g_dstrKeyUsageIssuerMetering;
extern DRM_GLOBAL_CONST  DRM_CONST_STRING  g_dstrKeyUsageIssuerKeyFileSigner;
extern DRM_GLOBAL_CONST  DRM_CONST_STRING  g_dstrKeyUsageIssuerLicenseSigner;
extern DRM_GLOBAL_CONST  DRM_CONST_STRING  g_dstrKeyUsageIssuerServer;
extern DRM_GLOBAL_CONST  DRM_CONST_STRING  g_dstrKeyUsageIssuerIndivWindows;
extern DRM_GLOBAL_CONST  DRM_CONST_STRING  g_dstrKeyUsageIssuerIndivOsx;
extern DRM_GLOBAL_CONST  DRM_CONST_STRING  g_dstrKeyUsageSignKeyFile;
extern DRM_GLOBAL_CONST  DRM_CONST_STRING  g_dstrKeyUsageSignLicense;
extern DRM_GLOBAL_CONST  DRM_CONST_STRING  g_dstrKeyUsageSignResponse;
extern DRM_GLOBAL_CONST  DRM_CONST_STRING  g_dstrKeyUsagePRNDEncryptKey;
extern DRM_GLOBAL_CONST  DRM_CONST_STRING  g_dstrCertSigningKey;
extern DRM_GLOBAL_CONST  DRM_CONST_STRING  g_dstrCertSigningKeyValue;
extern DRM_GLOBAL_CONST  DRM_CONST_STRING  g_dstrPlatformIdentifier;
extern DRM_GLOBAL_CONST  DRM_CONST_STRING  g_dstrMeteringID;
extern DRM_GLOBAL_CONST  DRM_CONST_STRING  g_dstrMeteringUrl;
extern DRM_GLOBAL_CONST  DRM_CONST_STRING  g_dstrOne;
extern DRM_GLOBAL_CONST  DRM_CONST_STRING  g_dstrSigningKeyPrivKeyXPath;
extern DRM_GLOBAL_CONST  DRM_CONST_STRING  g_dstrSigningKeyPubKeyXPath;
extern DRM_GLOBAL_CONST  DRM_CONST_STRING  g_dstrSecurityVersionObject;
extern DRM_GLOBAL_CONST  DRM_CONST_STRING  g_dstrSecurityVersionObjectPlatformId;
extern DRM_GLOBAL_CONST  DRM_CONST_STRING  g_dstrSecurityVersionObjectNumber;

extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrDefaultSecurityLevel;
extern DRM_GLOBAL_CONST  DRM_CONST_STRING g_dstrWMDRMNDPRIVKEYPrime0;
extern DRM_GLOBAL_CONST  DRM_CONST_STRING g_dstrWMDRMNDPRIVKEYPrime1;
extern DRM_GLOBAL_CONST  DRM_CONST_STRING g_dstrWMDRMNDPRIVKEYCRTExponent0;
extern DRM_GLOBAL_CONST  DRM_CONST_STRING g_dstrWMDRMNDPRIVKEYCRTExponent1;
extern DRM_GLOBAL_CONST  DRM_CONST_STRING g_dstrWMDRMNDPRIVKEYIQMP;
extern DRM_GLOBAL_CONST  DRM_CONST_STRING g_dstrMSBinCert256KeyPUBKEY;
extern DRM_GLOBAL_CONST  DRM_CONST_STRING g_dstrMSRSA1024CertKeyPUBKEYModulus;
extern DRM_GLOBAL_CONST  DRM_CONST_STRING g_dstrMSRSA1024CertKeyPUBKEYExponent;
extern DRM_GLOBAL_CONST  DRM_CONST_STRING g_dstrMSBinCert256KeyPRIVKEY;
extern DRM_GLOBAL_CONST  DRM_CONST_STRING g_dstrMSRSA1024CertKeyPRIVKEYPrime0;
extern DRM_GLOBAL_CONST  DRM_CONST_STRING g_dstrMSRSA1024CertKeyPRIVKEYPrime1;
extern DRM_GLOBAL_CONST  DRM_CONST_STRING g_dstrMSRSA1024CertKeyPRIVKEYCRTExponent0;
extern DRM_GLOBAL_CONST  DRM_CONST_STRING g_dstrMSRSA1024CertKeyPRIVKEYCRTExponent1;
extern DRM_GLOBAL_CONST  DRM_CONST_STRING g_dstrMSRSA1024CertKeyPRIVKEYIQMP;

/*********************************
** DRM-related ASF Header GUIDs
*********************************/
extern DRM_GLOBAL_CONST  DRM_GUID g_guidASFHeaderObject;
extern DRM_GLOBAL_CONST  DRM_GUID g_guidASFDataObject;
extern DRM_GLOBAL_CONST  DRM_GUID g_guidASFContentEncryptionObject;
extern DRM_GLOBAL_CONST  DRM_GUID g_guidASFExtendedContentEncryptionObject;
extern DRM_GLOBAL_CONST  DRM_GUID g_guidASFHeaderExtensionObject;
extern DRM_GLOBAL_CONST  DRM_GUID g_guidASFAdvancedContentEncryptionObject;
extern DRM_GLOBAL_CONST  DRM_GUID g_guidASFFilePropertiesObject;
extern DRM_GLOBAL_CONST  DRM_GUID g_guidASFDigitalSignatureObject;
extern DRM_GLOBAL_CONST  DRM_GUID g_guidSystemIDPlayReady;
extern DRM_GLOBAL_CONST  DRM_GUID g_guidASFReserved1;
extern DRM_GLOBAL_CONST  DRM_GUID g_guidASFStreamPropertiesObject;
extern DRM_GLOBAL_CONST  DRM_GUID g_guidASFStreamPropertiesObjectEx;
extern DRM_GLOBAL_CONST  DRM_GUID g_guidASFXStreamTypeAcmAudio;
extern DRM_GLOBAL_CONST  DRM_GUID g_guidASFXStreamTypeIcmVideo;

/*
** ASF Parsing Constants
*/

/*
** ASF Header Object Minimum Sizes
*/

/*
**   sizeof( DRM_GUID )         ObjectID
** + sizeof( DRM_QWORD )        Object Size
*/
#define ASF_OBJ_MIN_SIZE        DRM_UI64LITERAL( 0, 24 )

/*
**   sizeof( DRM_GUID )         ObjectID
** + sizeof( DRM_QWORD )        Object Size
** + sizeof( DRM_DWORD )        Number of Sub Objects
** + sizeof( DRM_BYTE )         Alignment
** + sizeof( DRM_BYTE )         Architecture
*/
#define ASF_HDR_MIN_SIZE        DRM_UI64LITERAL( 0, 30 )

/*
**   sizeof( DRM_GUID )         ObjectID
** + sizeof( DRM_QWORD )        Object Size
** + sizeof( DRM_GUID )         id of header extension (clock) object
** + sizeof( DRM_WORD )         clock size/type
** + sizeof( DRM_DWORD )        clock data size
*/
#define ASF_HDREXT_MIN_SIZE     DRM_UI64LITERAL( 0, 46 )
#define ASF_V1_MIN_SIZE         DRM_UI64LITERAL( 0, 40 )
#define ASF_FILEPROP_MIN_SIZE   DRM_UI64LITERAL( 0, 104 )

/*
**   sizeof( DRM_GUID )         error correction type
** + sizeof( DRM_QWORD )        time offset
*/
#define ASF_STREAM_PROPERTIES_SKIP_SIZE  DRM_UI64LITERAL( 0, 24 )

/*
**   sizeof( DRM_GUID )         ObjectID
** + sizeof( DRM_QWORD )        Object Size
** + sizeof( DRM_GUID )         Stream Type
** + ASF_STREAM_PROPERTIES_SKIP_SIZE
** + sizeof( DRM_DWORD )        type-specific data length
** + sizeof( DRM_DWORD )        error correction data length
** + sizeof( DRM_WORD )         Flags
** + sizeof( DRM_DWORD )        Reserved
*/
#define ASF_STREAM_PROPERTIES_MIN_SIZE  DRM_UI64LITERAL( 0, 86 )

/*********************************
** ASF Audio Format Tags
*********************************/
#define DRMTOOLS_ASF_AUDIO_TYPE_WMA           0x0161 /* Windows Media Audio */
#define DRMTOOLS_ASF_AUDIO_TYPE_GSM_AMR_FIXED 0x7A21 /* GSM-AMR (fixed bitrate, no SID) */
#define DRMTOOLS_ASF_AUDIO_TYPE_GSM_AMR_VBR   0x7A22 /* GSM-AMR (variable bitrate, including SID) */

/*
** Constants for header evaluation
*/
extern DRM_GLOBAL_CONST  DRM_CONST_STRING      g_dstrTagV24LAURL;
extern DRM_GLOBAL_CONST  DRM_CONST_STRING      g_dstrTagV24LUIURL;

typedef struct
{
    const DRM_CHAR * m_pszScopeName;
    DRM_DWORD m_dwScopeID;


    /*
    ** The following are constants used only by the
    ** autobench tool for automated perf testing
    */
    DRM_DWORD m_dwABRPMCounter;
    DRM_DWORD m_dwABCPMCounter;
} DRM_SCOPE;

extern DRM_SCOPE g_Scopes[];

#ifndef PERF_SCOPE_ID
#define PERF_SCOPE_ID(ModuleID, FuncID, BlockID)    ((((ModuleID) & 0x0fff) << 19) + (((FuncID) & 0x0fff) << 7) + ((BlockID) & 0x007f))
#endif

    extern DRM_GLOBAL_CONST  DRM_CONST_STRING       g_dstrVersion_1_0;
    extern DRM_GLOBAL_CONST  DRM_CONST_STRING       g_dstrTagFallback;
    extern DRM_GLOBAL_CONST  DRM_CONST_STRING       g_dstrTagDNIUniqueID;
    extern DRM_GLOBAL_CONST  DRM_CONST_STRING       g_dstrDeviceRevocationList;
    extern DRM_GLOBAL_CONST  DRM_CONST_STRING       g_dstrPFRootTag;
    extern DRM_GLOBAL_CONST  DRM_CONST_STRING       g_dstrTagDataId;
    extern DRM_GLOBAL_CONST  DRM_CONST_STRING       g_dstrManufacturer;

EXIT_PK_NAMESPACE;

#endif /* __DRMTOOLSCONSTANTS_H__ */

