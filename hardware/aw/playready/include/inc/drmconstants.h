/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#ifndef __DRMSTRINGS_H__
#define __DRMSTRINGS_H__

#include <drmtypes.h>
#include <oemcryptoctx.h>
#include <oemeccp256.h>

ENTER_PK_NAMESPACE;

    /* License constants */
    extern DRM_GLOBAL_CONST  DRM_LID g_rgbSecStoreGlobalName;

    /* Character constants */
    extern DRM_GLOBAL_CONST  DRM_WCHAR              g_wchNull;
    extern DRM_GLOBAL_CONST  DRM_WCHAR              g_wchMinus;
    extern DRM_GLOBAL_CONST  DRM_WCHAR              g_wchPlus;
    extern DRM_GLOBAL_CONST  DRM_WCHAR              g_wchForwardSlash;
    extern DRM_GLOBAL_CONST  DRM_WCHAR              g_wchColon;
    extern DRM_GLOBAL_CONST  DRM_WCHAR              g_wchComma;
    extern DRM_GLOBAL_CONST  DRM_WCHAR              g_wchQuote;
    extern DRM_GLOBAL_CONST  DRM_WCHAR              g_wchSingleQuote;
    extern DRM_GLOBAL_CONST  DRM_WCHAR              g_wchNewLine;
    extern DRM_GLOBAL_CONST  DRM_WCHAR              g_wchBackSlash;
    extern DRM_GLOBAL_CONST  DRM_WCHAR              g_wch0;
    extern DRM_GLOBAL_CONST  DRM_WCHAR              g_wch1;
    extern DRM_GLOBAL_CONST  DRM_WCHAR              g_wch2;
    extern DRM_GLOBAL_CONST  DRM_WCHAR              g_wch9;
    extern DRM_GLOBAL_CONST  DRM_WCHAR              g_wcha;
    extern DRM_GLOBAL_CONST  DRM_WCHAR              g_wchd;
    extern DRM_GLOBAL_CONST  DRM_WCHAR              g_wchf;
    extern DRM_GLOBAL_CONST  DRM_WCHAR              g_wchh;
    extern DRM_GLOBAL_CONST  DRM_WCHAR              g_wchm;
    extern DRM_GLOBAL_CONST  DRM_WCHAR              g_wchn;
    extern DRM_GLOBAL_CONST  DRM_WCHAR              g_wchs;
    extern DRM_GLOBAL_CONST  DRM_WCHAR              g_wchx;
    extern DRM_GLOBAL_CONST  DRM_WCHAR              g_wchy;
    extern DRM_GLOBAL_CONST  DRM_WCHAR              g_wchz;
    extern DRM_GLOBAL_CONST  DRM_WCHAR              g_wchA;
    extern DRM_GLOBAL_CONST  DRM_WCHAR              g_wchF;
    extern DRM_GLOBAL_CONST  DRM_WCHAR              g_wchG;
    extern DRM_GLOBAL_CONST  DRM_WCHAR              g_wchM;
    extern DRM_GLOBAL_CONST  DRM_WCHAR              g_wchT;
    extern DRM_GLOBAL_CONST  DRM_WCHAR              g_wchX;
    extern DRM_GLOBAL_CONST  DRM_WCHAR              g_wchZ;
    extern DRM_GLOBAL_CONST  DRM_WCHAR              g_wchUnderscore;
    extern DRM_GLOBAL_CONST  DRM_WCHAR              g_wchPeriod;
    extern DRM_GLOBAL_CONST  DRM_WCHAR              g_wchQuestionMark;
    extern DRM_GLOBAL_CONST  DRM_WCHAR              g_wchExclamationMark;
    extern DRM_GLOBAL_CONST  DRM_WCHAR              g_wchOpenParen;
    extern DRM_GLOBAL_CONST  DRM_WCHAR              g_wchCloseParen;
    extern DRM_GLOBAL_CONST  DRM_WCHAR              g_wchPound;
    extern DRM_GLOBAL_CONST  DRM_WCHAR              g_wchSpace;
    extern DRM_GLOBAL_CONST  DRM_WCHAR              g_wchTab;
    extern DRM_GLOBAL_CONST  DRM_WCHAR              g_wchLineFeed;
    extern DRM_GLOBAL_CONST  DRM_WCHAR              g_wchVerticalTab;
    extern DRM_GLOBAL_CONST  DRM_WCHAR              g_wchFormFeed;
    extern DRM_GLOBAL_CONST  DRM_WCHAR              g_wchCarriageReturn;
    extern DRM_GLOBAL_CONST  DRM_WCHAR              g_wchEqual;
    extern DRM_GLOBAL_CONST  DRM_WCHAR              g_wchOpenCurly;
    extern DRM_GLOBAL_CONST  DRM_WCHAR              g_wchCloseCurly;
    extern DRM_GLOBAL_CONST  DRM_WCHAR              g_wchLessThan;
    extern DRM_GLOBAL_CONST  DRM_WCHAR              g_wchGreaterThan;
    extern DRM_GLOBAL_CONST  DRM_WCHAR              g_wchLeftBracket;
    extern DRM_GLOBAL_CONST  DRM_WCHAR              g_wchRightBracket;
    extern DRM_GLOBAL_CONST  DRM_WCHAR              g_wchAsterisk;
    extern DRM_GLOBAL_CONST  DRM_WCHAR              g_wchPercent;
    extern DRM_GLOBAL_CONST  DRM_WCHAR              g_wchSemiColon;
    extern DRM_GLOBAL_CONST  DRM_WCHAR              g_wchAmpersand;
    extern DRM_GLOBAL_CONST  DRM_WCHAR              g_wchPipe;

    /* Character constants - ANSI */
    extern DRM_GLOBAL_CONST  DRM_CHAR               g_chForwardSlash;
    extern DRM_GLOBAL_CONST  DRM_CHAR               g_chPeriod;
    extern DRM_GLOBAL_CONST  DRM_CHAR               g_chNull;
    extern DRM_GLOBAL_CONST  DRM_CHAR               g_chMinus;
    extern DRM_GLOBAL_CONST  DRM_CHAR               g_chPlus;
    extern DRM_GLOBAL_CONST  DRM_CHAR               g_ch0;
    extern DRM_GLOBAL_CONST  DRM_CHAR               g_ch9;
    extern DRM_GLOBAL_CONST  DRM_CHAR               g_cha;
    extern DRM_GLOBAL_CONST  DRM_CHAR               g_chA;

    /* Misc strings shared across disparate functional areas */
    extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING  g_adstrLicenseRespTag;
    extern DRM_GLOBAL_CONST  DRM_CONST_STRING       g_dstrGUID;
    extern DRM_GLOBAL_CONST  DRM_CONST_STRING       g_dstrFlag;
    extern DRM_GLOBAL_CONST  DRM_CONST_STRING       g_dstrSecureClockNotSet;
    extern DRM_GLOBAL_CONST  DRM_CONST_STRING       g_dstrSecureClockSet;
    extern DRM_GLOBAL_CONST  DRM_CONST_STRING       g_dstrSecureClockNeedsRefresh;
    extern DRM_GLOBAL_CONST  DRM_CONST_STRING       g_dstrExprVarSavedDateTime;
    extern DRM_GLOBAL_CONST  DRM_CONST_STRING       g_dstrTagMetering;

    /* Rights */
    extern DRM_GLOBAL_CONST  DRM_CONST_STRING       g_dstrWMDRM_RIGHT_NONE;
    extern DRM_GLOBAL_CONST  DRM_CONST_STRING       g_dstrWMDRM_RIGHT_PLAYBACK;
    extern DRM_GLOBAL_CONST  DRM_CONST_STRING       g_dstrWMDRM_RIGHT_COLLABORATIVE_PLAY;
    extern DRM_GLOBAL_CONST  DRM_CONST_STRING       g_dstrWMDRM_RIGHT_COPY;
    extern DRM_GLOBAL_CONST  DRM_CONST_STRING       g_dstrWMDRM_RIGHT_CREATE_THUMBNAIL_IMAGE;
    extern DRM_GLOBAL_CONST  DRM_CONST_STRING       g_dstrWMDRM_RIGHT_COPY_TO_CD;
    extern DRM_GLOBAL_CONST  DRM_CONST_STRING       g_dstrWMDRM_RIGHT_MOVE;

    /* Script varibles used for license properties. */
    extern DRM_GLOBAL_CONST  DRM_CONST_STRING       g_dstrDRM_LS_COUNT_ATTR;
    extern DRM_GLOBAL_CONST  DRM_CONST_STRING       g_dstrDRM_LS_FIRSTUSE_ATTR;
    extern DRM_GLOBAL_CONST  DRM_CONST_STRING       g_dstrDRM_LS_FIRSTSTORE_ATTR;
    extern DRM_GLOBAL_CONST  DRM_CONST_STRING       g_dstrDRM_LS_PLAYCOUNT_ATTR;
    extern DRM_GLOBAL_CONST  DRM_CONST_STRING       g_dstrDRM_LS_COPYCOUNT_ATTR;
    extern DRM_GLOBAL_CONST  DRM_CONST_STRING       g_dstrDRM_LS_DELETED_ATTR;

    /* Shared XML tags */
    extern DRM_GLOBAL_CONST  DRM_CONST_STRING       g_dstrAttributeVersion;
    extern DRM_GLOBAL_CONST  DRM_CONST_STRING       g_dstrTagData;
    extern DRM_GLOBAL_CONST  DRM_CONST_STRING       g_dstrTagIndex;
    extern DRM_GLOBAL_CONST  DRM_CONST_STRING       g_dstrTagPubkey;
    extern DRM_GLOBAL_CONST  DRM_CONST_STRING       g_dstrTagValue;
    extern DRM_GLOBAL_CONST  DRM_CONST_STRING       g_dstrTagSignature;
    extern DRM_GLOBAL_CONST  DRM_CONST_STRING       g_dstrTagHashAlg;
    extern DRM_GLOBAL_CONST  DRM_CONST_STRING       g_dstrTagSignAlg;
    extern DRM_GLOBAL_CONST  DRM_CONST_STRING       g_dstrSHA;
    extern DRM_GLOBAL_CONST  DRM_CONST_STRING       g_dstrMSDRM;
    extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING  g_dastrAttributeType;
    extern DRM_GLOBAL_CONST  DRM_CONST_STRING       g_dstrAttributeType;
    extern DRM_GLOBAL_CONST  DRM_CONST_STRING       g_dstrTagCertificate;
    extern DRM_GLOBAL_CONST  DRM_CONST_STRING       g_dstrTagWrmHeader;
    extern DRM_GLOBAL_CONST  DRM_CONST_STRING       g_dstrAttributeVersion2Value;
    extern DRM_GLOBAL_CONST  DRM_CONST_STRING       g_dstrAttributeVersion4Value;
    extern DRM_GLOBAL_CONST  DRM_CONST_STRING       g_dstrAttributeVersion4_1Value;
    extern DRM_GLOBAL_CONST  DRM_CONST_STRING       g_dstrAttributeVersion4_2Value;
    extern DRM_GLOBAL_CONST  DRM_CONST_STRING       g_dstrTagLAINFO;
    extern DRM_GLOBAL_CONST  DRM_CONST_STRING       g_dstrTagV4DATA;
    extern DRM_GLOBAL_CONST  DRM_CONST_STRING       g_dstrTagLAURL;
    extern DRM_GLOBAL_CONST  DRM_CONST_STRING       g_dstrTagLUIURL;
    extern DRM_GLOBAL_CONST  DRM_CONST_STRING       g_dstrTagDSID;
    extern DRM_GLOBAL_CONST  DRM_CONST_STRING       g_dstrTagKID;
    extern DRM_GLOBAL_CONST  DRM_CONST_STRING       g_dstrTagKIDS;
    extern DRM_GLOBAL_CONST  DRM_CONST_STRING       g_dstrTagChecksum;
    extern DRM_GLOBAL_CONST  DRM_CONST_STRING       g_dstrTagAlgID;
    extern DRM_GLOBAL_CONST  DRM_CONST_STRING       g_dstrTagUplink;
    extern DRM_GLOBAL_CONST  DRM_CONST_STRING       g_dstrTagDecryptorSetup;
    extern DRM_GLOBAL_CONST  DRM_CONST_STRING       g_dstrTagProtectInfo;
    extern DRM_GLOBAL_CONST  DRM_CONST_STRING       g_dstrTagKeyLen;
    extern DRM_GLOBAL_CONST  DRM_CONST_STRING       g_dstrKeyLenNodeDataCocktail;
    extern DRM_GLOBAL_CONST  DRM_CONST_STRING       g_dstrKeyLenNodeDataAESCTR;
    extern DRM_GLOBAL_CONST  DRM_CONST_STRING       g_dstrTagCOCKTAIL;
    extern DRM_GLOBAL_CONST  DRM_CONST_STRING       g_dstrTagAESCTR;
    extern DRM_GLOBAL_CONST  DRM_CONST_STRING       g_dstrTagCustomAttributes;
    extern DRM_GLOBAL_CONST  DRM_CONST_STRING       g_dstrTagONDEMAND;
    extern DRM_GLOBAL_CONST  DRM_CONST_STRING       g_dstrEmptyWRMHeaderV4_1;
    extern DRM_GLOBAL_CONST  DRM_CONST_STRING       g_dstrEmptyWRMHeaderV4_2;
    extern DRM_GLOBAL_CONST  DRM_CONST_STRING       g_dstrTagAction;
    extern DRM_GLOBAL_CONST  DRM_CONST_STRING       g_dstrTagSecurityVersion;
    extern DRM_GLOBAL_CONST  DRM_CONST_STRING       g_dstrTagMID;
    extern DRM_GLOBAL_CONST  DRM_CONST_STRING       g_dstrTagTID;
    extern DRM_GLOBAL_CONST  DRM_CONST_STRING       g_dstrTagURL;
    extern DRM_GLOBAL_CONST  DRM_CONST_STRING       g_dstrLabelValue;
    extern DRM_GLOBAL_CONST  DRM_CONST_STRING       g_dstrChallenge;
    extern DRM_GLOBAL_CONST  DRM_CONST_STRING       g_dstrTagCertificateChain;
    extern DRM_GLOBAL_CONST  DRM_CONST_STRING       g_dstrTagPrivateKey;
    extern DRM_GLOBAL_CONST  DRM_CONST_STRING       g_dstrTagDataPubKey;
    extern DRM_GLOBAL_CONST  DRM_CONST_STRING       g_dstrTemplate;
    extern DRM_GLOBAL_CONST  DRM_CONST_STRING       g_dstrTagDataSecVerPlatform;

    /* Shared Certificate tags */
    extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING  g_dastrAttributeAlgorithm;
    extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING  g_dastrAttributeVersionWMDRM;
    extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING  g_dastrKeyUsageSignCert;
    extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING  g_dastrKeyUsageEncryptKey;
    extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING  g_dastrOne;
    extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING  g_dastrPrefixManufacturer;
    extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING  g_dastrTagCanonicalization;
    extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING  g_dastrTagCertificateCollection;
    extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING  g_dastrTagDigestMethod;
    extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING  g_dastrURIDSigSHA1;
    extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING  g_dastrTagDigestValue;
    extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING  g_dastrTagSignatureValue;
    extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING  g_dastrTagKeyInfo;
    extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING  g_dastrTagPublicKey;
    extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING  g_dastrTagPrivateKey;
    extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING  g_dastrTagKeyValue;
    extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING  g_dastrTagRSAKeyValue;
    extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING  g_dastrTagModulus;
    extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING  g_dastrTagExponent;
    extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING  g_dastrTagManufacturerName;
    extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING  g_dastrTagManufacturerData;
    extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING  g_dastrURIRSASHA1;
    extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING  g_dastrURIRSASHA1_Old;
    extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING  g_dastrTagReference;
    extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING  g_dastrTagTransforms;
    extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING  g_dastrTagTransform;
    extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING  g_dastrURITransformMSCert;
    extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING  g_dastrURITransformMSCertColl;
    extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING  g_dastrURITransformMSCert_Old;
    extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING  g_dastrURITransformC14N;
    extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING  g_dastrTagWMDRMCertificate;
    extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING  g_dastrTagWMDRMData;
    extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING  g_dastrTagWMDRMCertPublicKey;
    extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING  g_dastrTagWMDRMCertSecurityVersion;
    extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING  g_dastrTagWMDRMCertSecurityLevel;
    extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING  g_dastrTagWMDRMCertFeatures;
    extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING  g_dastrTagWMDRMCertKeyUsage;
    extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING  g_dastrFeaturesWMDRMTransmitter;
    extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING  g_dastrFeaturesWMDRMReceiver;
    extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING  g_dastrTagSignature;
    extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING  g_dastrTagSignatureMethod;
    extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING  g_dastrTagSignedInfo;
    extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING  g_dastrURIC14N;
    extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING  g_dastrVersionWMDRM;
    extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING  g_dastrWMDRMCertExponent;
    extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING  g_dastrPrefixMicrosoftCert;
    extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING  g_dastrMSNDRootPubKeyB64;

    /* Well-known public Keys */
    extern DRM_GLOBAL_CONST  PUBKEY_P256            g_ECC256MSPlayReadyRootIssuerPubKey;

    /* Revocation GUIDs */
    DRM_DECLARE_GUID( g_guidRevocationTypeRevInfo );
    DRM_DECLARE_GUID( g_guidRevocationTypeRevInfo2 );
    DRM_DECLARE_GUID( g_guidRevocationTypeWMDRMNET );
    DRM_DECLARE_GUID( g_guidRevocationTypePlayReadySilverLightRuntime );
    DRM_DECLARE_GUID( g_guidRevocationTypePlayReadySilverLightApplication );
    DRM_DECLARE_GUID( g_guidRevocationTypeGRL );

    /* Actions GUIDs. Used with DRM_LEVL_PerformOperationsXMR() API */
    DRM_DECLARE_GUID( DRM_ACTION_PLAY );
    DRM_DECLARE_GUID( DRM_ACTION_COPY );
    DRM_DECLARE_GUID( DRM_ACTION_MOVE );
    DRM_DECLARE_GUID( DRM_ACTION_CREATE_THUMBNAIL );
    DRM_DECLARE_GUID( DRM_ACTION_COLLABORATIVE_PLAY );
    DRM_DECLARE_GUID( DRM_ACTION_COPY_TO_CD );
    DRM_DECLARE_GUID( DRM_ACTION_COPY_TO_PC );
    DRM_DECLARE_GUID( DRM_ACTION_COPY_TO_DEVICE );

    DRM_DECLARE_GUID( DRM_PR_PROTECTION_SYSTEM_ID );

    DRM_DECLARE_GUID( g_guidNull );

    /* PlayEnabler GUIDS */
    DRM_DECLARE_GUID( DRM_PLAYENABLER_UNKNOWN_OUTPUT );
    DRM_DECLARE_GUID( DRM_PLAYENABLER_CONSTRAINED_RESOLUTION_UNKNOWN_OUTPUT );

    extern DRM_GLOBAL_CONST DRM_ID g_idNull;

    /* Versioning constants */
    extern DRM_GLOBAL_CONST  DRM_BYTE CERT_VER       [ DRM_VERSION_LEN ]; /* Indicates the public root key needed to verify the license server certificates. */
    extern DRM_GLOBAL_CONST  DRM_BYTE CLIENT_ID_VER  [ DRM_VERSION_LEN ]; /* The version for client id. */

EXIT_PK_NAMESPACE;

#endif /* __DRMSTRINGS_H__ */

