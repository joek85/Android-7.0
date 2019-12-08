/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#ifndef __DRM_METER_CONSTANTS_H
#define __DRM_METER_CONSTANTS_H

#include <drmtypes.h>
#include <oemcryptoctx.h>

ENTER_PK_NAMESPACE;

/*
** ------------------------------------------------------------
** XML strings used in the construction of meter data challenge
** ------------------------------------------------------------
*/
extern DRM_GLOBAL_CONST  DRM_EXPORT_VAR DRM_ANSI_CONST_STRING g_dastrMeterDataRootTag;

extern DRM_GLOBAL_CONST  DRM_EXPORT_VAR DRM_ANSI_CONST_STRING g_dastrMeterDataChallenge1Tag;

extern DRM_GLOBAL_CONST  DRM_EXPORT_VAR DRM_ANSI_CONST_STRING g_dastrMeterDataChallenge2Tag;

extern DRM_GLOBAL_CONST  DRM_EXPORT_VAR DRM_ANSI_CONST_STRING g_dastrMeterDataMeteringDataTag;

extern DRM_GLOBAL_CONST  DRM_EXPORT_VAR DRM_ANSI_CONST_STRING g_dastrMeterDataVersionTag;

extern DRM_GLOBAL_CONST  DRM_EXPORT_VAR DRM_ANSI_CONST_STRING g_dastrMeterDataMIDTag;

extern DRM_GLOBAL_CONST  DRM_EXPORT_VAR DRM_ANSI_CONST_STRING g_dastrMeterDataTIDTag;

extern DRM_GLOBAL_CONST  DRM_EXPORT_VAR DRM_ANSI_CONST_STRING g_dastrMeterDataCustomDataTag;

extern DRM_GLOBAL_CONST  DRM_EXPORT_VAR DRM_ANSI_CONST_STRING g_dastrMeterDataPartialDataTag;

extern DRM_GLOBAL_CONST  DRM_EXPORT_VAR DRM_ANSI_CONST_STRING g_dastrMeterDataDataTag;

extern DRM_GLOBAL_CONST  DRM_EXPORT_VAR DRM_ANSI_CONST_STRING g_dastrMeterDataDeviceCertTag;

extern DRM_GLOBAL_CONST  DRM_EXPORT_VAR DRM_ANSI_CONST_STRING g_dastrMeterDataRecordsTag;

extern DRM_GLOBAL_CONST  DRM_EXPORT_VAR DRM_ANSI_CONST_STRING g_dastrMeterDataRecordTag;

extern DRM_GLOBAL_CONST  DRM_EXPORT_VAR DRM_ANSI_CONST_STRING g_dastrMeterDataActionTag;


extern DRM_GLOBAL_CONST  DRM_EXPORT_VAR DRM_ANSI_CONST_STRING g_dastrMeterDataRootAttribName;
extern DRM_GLOBAL_CONST  DRM_EXPORT_VAR DRM_ANSI_CONST_STRING g_dastrMeterDataRootAttribValue;

extern DRM_GLOBAL_CONST  DRM_EXPORT_VAR DRM_ANSI_CONST_STRING g_dastrMeterDataChallenge2AttribName;
extern DRM_GLOBAL_CONST  DRM_EXPORT_VAR DRM_ANSI_CONST_STRING g_dastrMeterDataChallenge2AttribValue;

extern DRM_GLOBAL_CONST  DRM_EXPORT_VAR DRM_ANSI_CONST_STRING g_dastrMeterDataMeteringDataAttrib1Name;
extern DRM_GLOBAL_CONST  DRM_EXPORT_VAR DRM_ANSI_CONST_STRING g_dastrMeterDataMeteringDataAttrib1Value;

extern DRM_GLOBAL_CONST  DRM_EXPORT_VAR DRM_ANSI_CONST_STRING g_dastrMeterDataMeteringDataAttrib2Name;
extern DRM_GLOBAL_CONST  DRM_EXPORT_VAR DRM_ANSI_CONST_STRING g_dastrMeterDataMeteringDataAttrib2Value;

extern DRM_GLOBAL_CONST  DRM_EXPORT_VAR DRM_ANSI_CONST_STRING g_dastrMeterDataRecordAttribName;

extern DRM_GLOBAL_CONST  DRM_EXPORT_VAR DRM_ANSI_CONST_STRING g_dastrMeterDataActionAttrib1Name;

extern DRM_GLOBAL_CONST  DRM_EXPORT_VAR DRM_ANSI_CONST_STRING g_dastrMeterDataActionAttrib2Name;


extern DRM_GLOBAL_CONST  DRM_EXPORT_VAR DRM_ANSI_CONST_STRING g_dastrMeterDataVersionValue;

extern DRM_GLOBAL_CONST  DRM_EXPORT_VAR DRM_ANSI_CONST_STRING g_dastrMeterDataPartialDataValue;


/*
** ------------------------------------------------------
** XML strings used in the parsing of meter data response
** ------------------------------------------------------
*/
extern DRM_GLOBAL_CONST  DRM_EXPORT_VAR DRM_ANSI_CONST_STRING g_dastrMeterDataMeterCertPath;

extern DRM_GLOBAL_CONST  DRM_EXPORT_VAR DRM_ANSI_CONST_STRING g_dastrMeterDataEncryptedDataPath;

extern DRM_GLOBAL_CONST  DRM_EXPORT_VAR DRM_ANSI_CONST_STRING g_dastrMeterDataMIDPath;

extern DRM_GLOBAL_CONST  DRM_EXPORT_VAR DRM_ANSI_CONST_STRING g_dastrMeterDataTIDPath;

extern DRM_GLOBAL_CONST  DRM_EXPORT_VAR DRM_ANSI_CONST_STRING g_dastrMeterDataRecordsPath;

extern DRM_GLOBAL_CONST  DRM_EXPORT_VAR DRM_ANSI_CONST_STRING g_dastrMeterDataKIDPath;

extern DRM_GLOBAL_CONST  DRM_EXPORT_VAR DRM_ANSI_CONST_STRING g_dastrMeterDataKeyNamePath;

extern DRM_GLOBAL_CONST  DRM_EXPORT_VAR DRM_ANSI_CONST_STRING g_dastrMeterDataKeyNameValue;

/*
** ------------------------------------------------------------
** XML strings used in the construction of meter cert challenge
** ------------------------------------------------------------
*/
extern DRM_GLOBAL_CONST  DRM_EXPORT_VAR DRM_ANSI_CONST_STRING g_dastrMeterCertRootTag;

extern DRM_GLOBAL_CONST  DRM_EXPORT_VAR DRM_ANSI_CONST_STRING g_dastrMeterCertChallenge1Tag;

extern DRM_GLOBAL_CONST  DRM_EXPORT_VAR DRM_ANSI_CONST_STRING g_dastrMeterCertChallenge2Tag;

extern DRM_GLOBAL_CONST  DRM_EXPORT_VAR DRM_ANSI_CONST_STRING g_dastrMeterCertMIDTag;

extern DRM_GLOBAL_CONST  DRM_EXPORT_VAR DRM_ANSI_CONST_STRING g_dastrSupportsMeterCertSignatureTag;

extern DRM_GLOBAL_CONST  DRM_EXPORT_VAR DRM_ANSI_CONST_STRING g_dastrTrueValue;

extern DRM_GLOBAL_CONST  DRM_EXPORT_VAR DRM_ANSI_CONST_STRING g_dastrMeterCertVersionTag;

extern DRM_GLOBAL_CONST  DRM_EXPORT_VAR DRM_ANSI_CONST_STRING g_dastrMeterCertCustomDataTag;

extern DRM_GLOBAL_CONST  DRM_EXPORT_VAR DRM_ANSI_CONST_STRING g_dastrMeterCertRootAttribName;

extern DRM_GLOBAL_CONST  DRM_EXPORT_VAR DRM_ANSI_CONST_STRING g_dastrMeterCertRootAttribValue;


extern DRM_GLOBAL_CONST  DRM_EXPORT_VAR DRM_ANSI_CONST_STRING g_dastrMeterCertVersion;


/*
** ------------------------------------------------------
** XML strings used in the parsing of meter cert response
** ------------------------------------------------------
*/
extern DRM_GLOBAL_CONST  DRM_EXPORT_VAR DRM_ANSI_CONST_STRING g_dastrMeterCertResponsePath;

extern DRM_GLOBAL_CONST  DRM_EXPORT_VAR DRM_ANSI_CONST_STRING g_dastrMeterCertMeterCertPath;

/*
** -----------------------------
** Additional metering constants
** -----------------------------
*/
extern DRM_GLOBAL_CONST  DRM_CONST_STRING       g_dstrTagMeterCert;

EXIT_PK_NAMESPACE;

#endif /* __DRM_METER_CONSTANTS_H */

