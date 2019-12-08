/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#ifndef __DRM_XML_SIG_CONSTANTS_H
#define __DRM_XML_SIG_CONSTANTS_H

#include <oemeccp256.h>

ENTER_PK_NAMESPACE;

/* Well known WMRM ECC-256 public key */
extern DRM_GLOBAL_CONST  PUBKEY_P256 g_oWMRMECC256PubKey;

/* Well known Move Service ECC-256 public key */
extern DRM_GLOBAL_CONST  PUBKEY_P256 g_pubkeyMoveService;

/*
** -------------------------------------------------------------
** XML strings used in the construction of signature XML segment
** -------------------------------------------------------------
*/
extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrSigRootTag;

extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrSigSignedInfoTag;

extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrSigCanonicalizationMethodTag;

extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrSigSignatureMethodTag;

extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrSigReferenceTag;

extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrSigDigestMethodTag;

extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrSigDigestValueTag;

extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrSigSignatureValueTag;

extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrSigKeyInfoTag;

extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrSigKeyValueTag;

extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrSigECCKeyValueTag;

extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrSigPublicKeyTag;


extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrSigRootAttribName;
extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrSigRootAttribValue;

extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrSigSignedInfoAttribName;
extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrSigSignedInfoAttribValue;

extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrSigCanonicalizationMethodAttribName;
extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrSigCanonicalizationMethodAttribValue;

extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrSigSignatureMethodAttribName;
extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrSigSignatureMethodAttribValue;

extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrSigDigestMethodAttribName;
extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrSigDigestMethodAttribValue;

extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrSigKeyInfoAttribName;
extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrSigKeyInfoAttribValue;



/*
** -----------------------------------------------------------------------
** XML strings used in the construction of XML encrypted data segment
** -----------------------------------------------------------------------
*/
extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrSigEncryptedDataTag;

extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrSigEncryptionMethodTag;

extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrSigEncryptedKeyTag;

extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrSigCipherDataTag;

extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrSigCipherValueTag;


extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrSigEncryptedDataAttrib1Name;
extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrSigEncryptedDataAttrib1Value;

extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrSigEncryptedDataAttrib2Name;
extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrSigEncryptedDataAttrib2Value;

extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrSigEncryptedDataAttrib3Name;
extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrSigEncryptedDataAttrib3Value;

extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrSigEncryptionMethodAttrib1Name;
extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrSigEncryptionMethodAttrib1Value;

extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrSigEncryptionMethodAttrib2Name;
extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrSigEncryptionMethodAttrib2Value;

extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrSigKeyInfoAttrib1Name;
extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrSigKeyInfoAttrib1Value;

extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrSigKeyNameTag;
extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrSigKeyInfoKeyName1;
extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrSigKeyInfoKeyName2;
extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrSigKeyInfoKeyName3;
extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrSigKeyInfoKeyName4;

extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrKeyInfoKeyNamePath;

extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrSigReferenceAttribName;
extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrSigReferenceAttribValue;

extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrSigEncryptedKeyAttribName;
extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrSigEncryptedKeyAttribValue;

extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrRSAPrefixMSLP;
extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrRSAURIMSLP;
extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrTagRSAKeyPair;

/*
** --------------------------------------------------------
** XML strings used in the parsing of signature XML segment
** --------------------------------------------------------
*/
extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrSigSignedInfoPath;
extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrSigSignatureValuePath;
extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrSigHashPath;
extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrSigPublicKeyPath;
extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrMovCopySignaturePath;
extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrMovCopyResponsePath;
extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrMovRefreshSignaturePath;
extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrMovRefreshResponsePath;
extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrMovMoveSignaturePath;
extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrMovMoveResponsePath;
extern DRM_GLOBAL_CONST  DRM_EXPORT_VAR DRM_ANSI_CONST_STRING g_dastrMeteringMeterCertTag;
extern DRM_GLOBAL_CONST  DRM_EXPORT_VAR DRM_ANSI_CONST_STRING g_dastrMeterDataSignaturePath;
extern DRM_GLOBAL_CONST  DRM_EXPORT_VAR DRM_ANSI_CONST_STRING g_dastrMeterDataMeteringResponsePath;
extern DRM_GLOBAL_CONST  DRM_EXPORT_VAR DRM_ANSI_CONST_STRING g_dastrMeterDataCustomDataPath;
extern DRM_GLOBAL_CONST  DRM_EXPORT_VAR DRM_ANSI_CONST_STRING g_dastrMeterCertSignaturePath;
extern DRM_GLOBAL_CONST  DRM_EXPORT_VAR DRM_ANSI_CONST_STRING g_dastrMeterCertMeteringCertificateResponsePath;
extern DRM_GLOBAL_CONST  DRM_EXPORT_VAR DRM_ANSI_CONST_STRING g_dastrMeterCertCustomDataPath;
extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrDomJoinCustomDataPath;
extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrDomJoinDomainPath;
extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrDomJoinSignaturePath;
extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrDomJoinSigningCertTag;
extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrDomLeaveCustomDataPath;
extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrDomLeaveDomainPath;
extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrDomLeaveSignaturePath;
extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrLicAcqCustomDataPath;
extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrLicLicenseResponsePath;
extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrLicSignaturePath;
extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrLicAcqSigningCertTag;
extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrLicAckCustomDataPath;
extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrLicAcqLAckPath;
extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrLicAckSignaturePath;


/*
** -------------------------------------------------------------
** XML strings used in the parsing of XML encrypted data segment
** -------------------------------------------------------------
*/
extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrSigKeyInfoPath;

extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrSigEncryptedKeyPath;

extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrSigCipherValuePath;

EXIT_PK_NAMESPACE;

#endif /* __DRM_XML_SIG_CONSTANTS_H */

