/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

/*
** Header file for the Error contract mechanism.
*/

#ifndef __DRMCONTRACT_H__
#define __DRMCONTRACT_H__

#include <drmtypes.h>

ENTER_PK_NAMESPACE;

/*
** The following are the API IDs for each top-level API with an Error Code Contract (ECC).
** The order here is alphabetical by name to match the ordering in the .chm help file:
**     PlayReady.chm --> Programming Reference --> DRM Functions
**
** IF YOU ADD/DELETE ANY APIs HERE, YOU MUST ALSO:
** 1) ENSURE THAT THE CHM FILE IS UP-TO-DATE
** 2) UPDATE THE g_prgdrErrorCodelist ARRAY IN 'drmcontract.c' TO MATCH THIS ORDERING
** 3) POSSIBLY UPDATE THE TEST FILE ...\Test\Utils\ErrorContractTest.pc.pk.ce.txt (HAS HARDCODED API IDs)
*/
enum ECC_DRM_API_List
{
    ECC_DRM_GENERIC_ERROR_LIST = 0,

    ECC_Drm_Content_UpdateEmbeddedStore,
    ECC_Drm_Content_SetProperty,
    ECC_Drm_Device_GetProperty,
    ECC_Drm_DomainCert_EnumNext,
    ECC_Drm_DomainCert_Find,
    ECC_Drm_DomainCert_InitEnum,
    ECC_Drm_Envelope_Close,
    ECC_Drm_Envelope_DuplicateFileContext,
    ECC_Drm_Envelope_GetSize,
    ECC_Drm_Envelope_GetOriginalFileName,
    ECC_Drm_Envelope_InitializeRead,
    ECC_Drm_Envelope_Open,
    ECC_Drm_Envelope_Read,
    ECC_Drm_Envelope_Seek,
    ECC_Drm_GetAdditionalResponseData,
    ECC_Drm_Initialize,
    ECC_Drm_JoinDomain_GenerateChallenge,
    ECC_Drm_JoinDomain_ProcessResponse,
    ECC_Drm_LeaveDomain_GenerateChallenge,
    ECC_Drm_LeaveDomain_ProcessResponse,
    ECC_Drm_License_GetProperty,
    ECC_Drm_LicenseAcq_GenerateChallenge,
    ECC_Drm_LicenseAcq_GenerateAck,
    ECC_Drm_LicenseAcq_GetContentHeader,
    ECC_Drm_LicenseAcq_ProcessAckResponse,
    ECC_Drm_LicenseAcq_ProcessResponse,
    ECC_Drm_LicenseQuery_GetState,
    ECC_Drm_LicenseQuery_IsAllowed,
    ECC_Drm_MeterCert_Delete,
    ECC_Drm_MeterCert_EnumNext,
    ECC_Drm_MeterCert_GenerateChallenge,
    ECC_Drm_MeterCert_InitEnum,
    ECC_Drm_MeterCert_Invalidate,
    ECC_Drm_MeterCert_ProcessResponse,
    ECC_Drm_MeterCert_Update,
    ECC_Drm_Metering_GenerateChallenge,
    ECC_Drm_Metering_ProcessResponse,
    ECC_Drm_ProcessCommand,
    ECC_Drm_ProcessRequest,
    ECC_Drm_Reader_Bind,
    ECC_Drm_Reader_Commit,
    ECC_Drm_Reader_Decrypt,
    ECC_Drm_Reader_InitDecrypt,
    ECC_Drm_Reinitialize,
    ECC_Drm_SecureClock_GenerateChallenge,
    ECC_Drm_SecureClock_GetValue,
    ECC_Drm_SecureClock_ProcessResponse,
    ECC_Drm_StoreMgmt_CleanupStore,
    ECC_Drm_StoreMgmt_DeleteLicenses,
    ECC_Drm_Uninitialize,
    ECC_Drm_Content_GetProperty,
    ECC_Drm_Envelope_WritePlayReadyObject,
    ECC_Drm_Content_UpdateEmbeddedStore_Commit,
    ECC_Drm_PlayReadyObject_ConvertFromWmdrmHeader,

    ECC_Drm_Prnd_GetMessageType,
    ECC_Drm_Prnd_Transmitter_BeginSession,
    ECC_Drm_Prnd_Transmitter_RegistrationRequest_Process,
    ECC_Drm_Prnd_Transmitter_RegistrationResponse_Generate,
    ECC_Drm_Prnd_Transmitter_RegistrationError_Generate,
    ECC_Drm_Prnd_Transmitter_ProximityDetectionResult_Generate,
    ECC_Drm_Prnd_Transmitter_ProximityDetectionStart_Process,
    ECC_Drm_Prnd_Transmitter_ProximityDetectionResponse_Process,
    ECC_Drm_Prnd_Transmitter_LicenseRequest_Process,
    ECC_Drm_Prnd_Transmitter_LicenseTransmit_Generate,
    ECC_Drm_Prnd_Transmitter_LicenseError_Generate,
    ECC_Drm_Prnd_Transmitter_EndSession,
    ECC_Drm_Prnd_Transmitter_PrepareLicensesForTransmit,
    ECC_Drm_Prnd_Transmitter_IsSessionStillValid,
    ECC_Drm_Prnd_Receiver_BeginSession,
    ECC_Drm_Prnd_Receiver_RegistrationRequest_Generate,
    ECC_Drm_Prnd_Receiver_RegistrationResponse_Process,
    ECC_Drm_Prnd_Receiver_RegistrationError_Process,
    ECC_Drm_Prnd_Receiver_ProximityDetectionStart_Generate,
    ECC_Drm_Prnd_Receiver_ProximityDetectionChallenge_Process,
    ECC_Drm_Prnd_Receiver_ProximityDetectionResult_Process,
    ECC_Drm_Prnd_Receiver_LicenseRequest_Generate,
    ECC_Drm_Prnd_Receiver_LicenseTransmit_Process,
    ECC_Drm_Prnd_Receiver_LicenseError_Process,
    ECC_Drm_Prnd_Receiver_EndSession,
};

/*
** Macro to interface with the DRM API.
** The argument 'apivalue' should be one of the values defined above
*/
#define ChkECC( apivalue, dr )                                                                     \
     do                                                                                            \
     {                                                                                             \
         if( DRM_CONTRACT_IsContractSupported()                                                    \
          && DRM_FAILED( dr )                                                                      \
          && !OEM_ECC_ExpectedErrorCode( apivalue, dr ) )                                          \
         {                                                                                         \
              TRACE( ( "Error code 0x%X doesn't match expected error codes for "#apivalue, dr ) ); \
              DRMASSERT( FALSE ) ;                                                                 \
         }                                                                                         \
     } while ( FALSE )

DRM_API DRM_BOOL DRM_CALL OEM_ECC_ExpectedErrorCode(
    __in const DRM_DWORD  f_uiAPIid,
    __in const DRM_RESULT f_dr );

EXIT_PK_NAMESPACE;

#endif   /* __DRMCONTRACT_H__ */

