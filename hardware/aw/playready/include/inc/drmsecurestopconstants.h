/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#ifndef _DRMSECURESTOPCONSTANTS_H_
#define _DRMSECURESTOPCONSTANTS_H_ 1

#include <drmtypes.h>

ENTER_PK_NAMESPACE;

/*
** XML strings used in the construction of a secure stop challenge
*/

extern DRM_GLOBAL_CONST DRM_ANSI_CONST_STRING g_dastrSecureStopRootTag;
extern DRM_GLOBAL_CONST DRM_ANSI_CONST_STRING g_dastrSecureStopRootAttribName;
extern DRM_GLOBAL_CONST DRM_ANSI_CONST_STRING g_dastrSecureStopRootAttribValue;
extern DRM_GLOBAL_CONST DRM_ANSI_CONST_STRING g_dastrSecureStopChallenge1Tag;
extern DRM_GLOBAL_CONST DRM_ANSI_CONST_STRING g_dastrSecureStopChallenge2Tag;
extern DRM_GLOBAL_CONST DRM_ANSI_CONST_STRING g_dastrSecureStopChallenge2AttribName;
extern DRM_GLOBAL_CONST DRM_ANSI_CONST_STRING g_dastrSecureStopChallenge2AttribValue;
extern DRM_GLOBAL_CONST DRM_ANSI_CONST_STRING g_dastrSecureStopDataTag;
extern DRM_GLOBAL_CONST DRM_ANSI_CONST_STRING g_dastrSecureStopDataAttrib1Name;
extern DRM_GLOBAL_CONST DRM_ANSI_CONST_STRING g_dastrSecureStopDataAttrib1Value;
extern DRM_GLOBAL_CONST DRM_ANSI_CONST_STRING g_dastrSecureStopDataAttrib2Name;
extern DRM_GLOBAL_CONST DRM_ANSI_CONST_STRING g_dastrSecureStopDataAttrib2Value;
extern DRM_GLOBAL_CONST DRM_ANSI_CONST_STRING g_dastrSecureStopVersionTag;
extern DRM_GLOBAL_CONST DRM_ANSI_CONST_STRING g_dastrSecureStopVersionValue;
extern DRM_GLOBAL_CONST DRM_ANSI_CONST_STRING g_dastrSecureStopPublisherIDTag;
extern DRM_GLOBAL_CONST DRM_ANSI_CONST_STRING g_dastrSecureStopTransactionIDTag;
extern DRM_GLOBAL_CONST DRM_ANSI_CONST_STRING g_dastrSecureStopCustomDataTag;
extern DRM_GLOBAL_CONST DRM_ANSI_CONST_STRING g_dastrSecureStopClearDataTag;
extern DRM_GLOBAL_CONST DRM_ANSI_CONST_STRING g_dastrSecureStopRecordsTag;
extern DRM_GLOBAL_CONST DRM_ANSI_CONST_STRING g_dastrSecureStopRecordTag;
extern DRM_GLOBAL_CONST DRM_ANSI_CONST_STRING g_dastrSecureStopRecordAttribName;
extern DRM_GLOBAL_CONST DRM_ANSI_CONST_STRING g_dastrSecureStopStartTimeTag;
extern DRM_GLOBAL_CONST DRM_ANSI_CONST_STRING g_dastrSecureStopUpdateTimeTag;
extern DRM_GLOBAL_CONST DRM_ANSI_CONST_STRING g_dastrSecureStopStoppedTag;
extern DRM_GLOBAL_CONST DRM_ANSI_CONST_STRING g_dastrSecureStopStoppedValueTrue;
extern DRM_GLOBAL_CONST DRM_ANSI_CONST_STRING g_dastrSecureStopStoppedValueFalse;
extern DRM_GLOBAL_CONST DRM_ANSI_CONST_STRING g_dastrSecureStopCertificateChainTag;

/*
** XML strings used in the parsing of a secure stop response
*/

extern DRM_GLOBAL_CONST DRM_ANSI_CONST_STRING g_dastrSecureStopResponsePath;
extern DRM_GLOBAL_CONST DRM_ANSI_CONST_STRING g_dastrSecureStopVersionPath;
extern DRM_GLOBAL_CONST DRM_ANSI_CONST_STRING g_dastrSecureStopPublisherIDPath;
extern DRM_GLOBAL_CONST DRM_ANSI_CONST_STRING g_dastrSecureStopTransactionIDPath;
extern DRM_GLOBAL_CONST DRM_ANSI_CONST_STRING g_dastrSecureStopPublisherCertTag;
extern DRM_GLOBAL_CONST DRM_ANSI_CONST_STRING g_dastrSecureStopCustomDataPath;
extern DRM_GLOBAL_CONST DRM_ANSI_CONST_STRING g_dastrSecureStopEncryptedDataPath;
extern DRM_GLOBAL_CONST DRM_ANSI_CONST_STRING g_dastrSecureStopKeyNamePath;
extern DRM_GLOBAL_CONST DRM_ANSI_CONST_STRING g_dastrSecureStopKeyNameValue;
extern DRM_GLOBAL_CONST DRM_ANSI_CONST_STRING g_dastrSecureStopRecordsPath;
extern DRM_GLOBAL_CONST DRM_ANSI_CONST_STRING g_dastrSecureStopSessionIDPath;
extern DRM_GLOBAL_CONST DRM_ANSI_CONST_STRING g_dastrSecureStopSignaturePath;

EXIT_PK_NAMESPACE;

#endif /* _DRMSECURESTOPCONSTANTS_H_ */
