/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#ifndef __DRM_LICACQ_V3_CONSTANTS_H
#define __DRM_LICACQ_V3_CONSTANTS_H

ENTER_PK_NAMESPACE;

/*
** ---------------------------------------------------------
** XML strings used in the construction of license challenge
** ---------------------------------------------------------
*/
extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrLicAcqRootTag;

extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrLicAcqVersionTag;

extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrLicAcqChallenge1Tag;

extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrLicAcqChallenge2Tag;

extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrLicAcqLATag;

extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrLicAcqContentHeaderTag;

extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrLicAcqSecurityVersionTag;

extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrLicAcqAppSecurityTag;

extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrLicAcqSubjectID1Tag;

extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrLicAcqSubjectID2Tag;

extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrLicAcqDRMKVersionTag;

extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrLicAcqRevocationListsTag;

extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrLicAcqRevListInfoTag;

extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrLicAcqRevListIDTag;

extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrLicAcqRevVersionTag;

extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrLicAcqCertificateChainsTag;

extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrLicAcqCertificateChainTag;

extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrLicAcqDataTag;

extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrLicAcqUplinksTag;

extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrLicAcqUplinkTag;

extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrLicAcqKIDTag;

extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrLicAcqCategoryTag;

extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrLicAcqCountTag;

extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrLicAcqDateTag;

extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrLicAcqVagueTag;

extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrLicAcqCustomDataTag;

extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrLicAcqLicenseNonceTag;

extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrLicAcqClientTimeTag;

extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrLicAcqRootAttribName;
extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrLicAcqRootAttribValue;

extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrLicAcqChallenge2AttribName;
extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrLicAcqChallenge2AttribValue;

extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrLicAcqChallengeVAAttrib1Name;
extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrLicAcqChallengeVAAttrib1Value;

extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrLicAcqChallengeVAAttrib2Name;
extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrLicAcqChallengeVAAttrib2Value;

extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrLicAcqVersion1Value;
extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrLicAcqVersion4Value;


/*
** ---------------------------------------------------------------
** XML strings used in the construction of license acknowledgement
** ---------------------------------------------------------------
*/
extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrLicAckRootTag;

extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrLicAckVersionTag;

extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrLicAckChallenge1Tag;

extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrLicAckChallenge2Tag;

extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrLicAckAckTag;

extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrLicAckDataTag;

extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrLicAckTransactionIDTag;

extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrLicAckLicenseStorageResultsTag;

extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrLicAckLicenseTag;

extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrLicAckKIDTag;

extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrLicAckLIDTag;

extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrLicAckResultTag;

extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrLicAckCertificateChainTag;


extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrLicAckRootAttribName;
extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrLicAckRootAttribValue;

extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrLicAckChallenge2AttribName;
extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrLicAckChallenge2AttribValue;

extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrLicAckAckAttrib1Name;
extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrLicAckAckAttrib1Value;

extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrLicAckAckAttrib2Name;
extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrLicAckAckAttrib2Value;

extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrLicAckVersionValue;


/*
** -----------------------------------------------------
** XML strings used in the construction of SOAP envelope
** -----------------------------------------------------
*/
extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrXMLRootTag;

extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrSOAPEnvelopeTag;

extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrSOAPBodyTag;


extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrSOAPEnvelopeAttrib1Name;
extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrSOAPEnvelopeAttrib1Value;

extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrSOAPEnvelopeAttrib2Name;
extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrSOAPEnvelopeAttrib2Value;

extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrSOAPEnvelopeAttrib3Name;
extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrSOAPEnvelopeAttrib3Value;


/*
** ---------------------------------------------------
** XML strings used in the parsing of license response
** ---------------------------------------------------
*/
extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrLicLicensesPath;

extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrLicLicensePath;

extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrAcquireLicenseResultTag;

extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrLicRevInfoPath;

extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrRevInfo;

extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrLicRevocationPath;

extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrLicTransactionIDPath;
/*
extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrLicRevocationAttribName;
*/
extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrLicSLKPath;

extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrLicRevocationAttribName;

extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrLicListIdPath;

extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrLicListDataPath;

extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrLicNoncePath;

extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrLicResponseIdPath;

extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrLicSigningCertPath;


/*
** ---------------------------------------------------------------
** XML strings used in the parsing of license acknowledge response
** ---------------------------------------------------------------
*/
extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrAcquireLicenseResponseTag;


EXIT_PK_NAMESPACE;

#endif /* __DRM_LICACQ_V3_CONSTANTS_H */
