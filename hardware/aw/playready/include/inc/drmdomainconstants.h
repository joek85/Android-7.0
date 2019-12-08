/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#ifndef __DRM_DOMAIN_CONSTANTS_H
#define __DRM_DOMAIN_CONSTANTS_H

ENTER_PK_NAMESPACE;

/*
** ---------------------------------------------------------------------
** XML strings used in the construction of domain join challenge
** ---------------------------------------------------------------------
*/
extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrDomJoinRootTag;

extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrDomJoinVersionTag;

extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrDomJoinChallenge1Tag;

extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrDomJoinChallenge2Tag;

extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrDomJoinDomainTag;

extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrDomJoinDataTag;

extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrDomJoinServiceIDTag;

extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrDomJoinAccountIDTag;

extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrDomJoinRevisionTag;

extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrDomJoinFriendlyNameTag;

extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrDomJoinCustomDataTag;

extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrDomJoinCertificateChainTag;


extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrDomJoinRootAttribName;
extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrDomJoinRootAttribValue;

extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrDomJoinChallenge2AttribName;
extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrDomJoinChallenge2AttribValue;

extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrDomJoinDomainAttrib1Name;
extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrDomJoinDomainAttrib1Value;

extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrDomJoinDomainAttrib2Name;
extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrDomJoinDomainAttrib2Value;

extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrDomJoinCertificateChainAttribName;
extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrDomJoinCertificateChainAttribValue;

extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrDomainJoinVersion;



/*
** -----------------------------------------------------------------------
** XML strings used in the construction of domain leave challenge
** -----------------------------------------------------------------------
*/
extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrDomLeaveRootTag;

extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrDomLeaveVersionTag;

extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrDomLeaveChallenge1Tag;

extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrDomLeaveChallenge2Tag;

extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrDomLeaveDomainTag;

extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrDomLeaveDataTag;

extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrDomLeaveAccountIDTag;

extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrDomLeaveCustomDataTag;

extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrDomLeaveCertificateChainTag;


extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrDomLeaveRootAttribName;
extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrDomLeaveRootAttribValue;

extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrDomLeaveChallenge2AttribName;
extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrDomLeaveChallenge2AttribValue;

extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrDomLeaveDomainAttrib1Name;
extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrDomLeaveDomainAttrib1Value;

extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrDomLeaveDomainAttrib2Name;
extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrDomLeaveDomainAttrib2Value;

extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrDomLeaveCertificateChainAttribName;
extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrDomLeaveCertificateChainAttribValue;

extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrDomainLeaveVersion;



/*
** ---------------------------------------------------------------
** XML strings used in the parsing of domain join response
** ---------------------------------------------------------------
*/
extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrDomJoinResponsePath;

extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrDomJoinDataPath;

extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrDomJoinDataPath;

extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrDomJoinVersionPath;

extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrDomJoinServiceIDPath;

extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrDomJoinAccountIDPath;

extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrDomJoinRevisionPath;

extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrDomJoinCertificateChainPath;

extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrDomJoinDomainKeysPath;

extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrDomJoinSigningCertPath;

/*
** -----------------------------------------------------------------
** XML strings used in the parsing of domain leave response
** -----------------------------------------------------------------
*/
extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrDomLeaveAccountIDPath;


EXIT_PK_NAMESPACE;

#endif /* __DRM_DOMAIN_CONSTANTS_H */

