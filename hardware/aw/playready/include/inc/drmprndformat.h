/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#ifndef _DRMPRNDFORMAT_H_
#define _DRMPRNDFORMAT_H_

#include <drmpragmas.h>
#include <drmxbbuilder.h>
#include <oemcryptoctx.h>
#include <drmprndprotocoltypes.h>
#include <drmprndformats_generated.h>

ENTER_PK_NAMESPACE;

#define DRM_CHECK_CUSTOM_DATA_TYPE( customDataType )                                        \
    ChkBOOL(                                                                                \
            (customDataType) == NULL                                                        \
        ||  ((DRM_BYTE*)(customDataType))[0] != 'M'                                         \
        ||  ((DRM_BYTE*)(customDataType))[1] != 'S'                                         \
        ||  ((DRM_BYTE*)(customDataType))[2] != 'F'                                         \
        ||  ((DRM_BYTE*)(customDataType))[3] != 'T',                                        \
         DRM_E_PRND_INVALID_CUSTOM_DATA_TYPE )

typedef enum
{
    DRM_PRND_MESSAGE_TYPE_INVALID                       =  0,
    DRM_PRND_MESSAGE_TYPE_REGISTRATION_REQUEST          =  1,
    DRM_PRND_MESSAGE_TYPE_REGISTRATION_RESPONSE         =  2,
    DRM_PRND_MESSAGE_TYPE_REGISTRATION_ERROR            =  3,
    DRM_PRND_MESSAGE_TYPE_PROXIMITY_DETECTION_START     =  4,
    DRM_PRND_MESSAGE_TYPE_PROXIMITY_DETECTION_CHALLENGE =  5,
    DRM_PRND_MESSAGE_TYPE_PROXIMITY_DETECTION_RESPONSE  =  6,
    DRM_PRND_MESSAGE_TYPE_PROXIMITY_DETECTION_RESULT    =  7,
    DRM_PRND_MESSAGE_TYPE_LICENSE_REQUEST               =  8,
    DRM_PRND_MESSAGE_TYPE_LICENSE_TRANSMIT              =  9,
    DRM_PRND_MESSAGE_TYPE_LICENSE_ERROR                 = 10,
} DRM_PRND_MESSAGE_TYPE;

/******************************************************************************
**
** PRND format utility functions
**
******************************************************************************/
DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL DRM_PRND_TxRx_GetMessageType(
    __in_bcount( f_cbUnknownMessage )                           const DRM_BYTE                  *f_pbUnknownMessage,
    __in                                                              DRM_DWORD                  f_cbUnknownMessage,
    __out                                                             DRM_PRND_MESSAGE_TYPE     *f_peMessageType ) DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_API_VOID DRM_VOID DRM_CALL DRM_PRND_TxRx_IncrementLMID(
    __inout_ecount( 1 )                                               DRM_GUID                                         *f_pguidLMID ) DRM_NO_INLINE_ATTRIBUTE;

/******************************************************************************
**
** PRND Message Signatures
**
******************************************************************************/
#define DRM_PRND_SIGNATURE_SIZE_AES_OMAC1_MI                    DRM_AES_BLOCKLEN
#define DRM_PRND_SIGNATURE_SIZE_ECC256_RECEIVER_SIGNING_KEY     ECDSA_P256_SIGNATURE_SIZE_IN_BYTES

typedef enum
{
    DRM_PRND_SIGNATURE_OPERATION_INVALID,
    DRM_PRND_SIGNATURE_OPERATION_SIGN_USING_ECC256_RECEIVER_SIGNING_KEY,
    DRM_PRND_SIGNATURE_OPERATION_VERIFY_USING_ECC256_RECEIVER_SIGNING_KEY,
    DRM_PRND_SIGNATURE_OPERATION_SIGN_USING_AES_OMAC1_WITH_MI,
    DRM_PRND_SIGNATURE_OPERATION_VERIFY_USING_AES_OMAC1_WITH_MI,
} DRM_PRND_SIGNATURE_OPERATION;

PREFAST_PUSH_DISABLE_EXPLAINED(__WARNING_NONCONST_PARAM_25004, "Prefast Noise: Callback contexts should never be const")
typedef DRM_RESULT ( DRM_CALL *DRM_PRND_FORMAT_SIGNATURE_CB )(
    __in_ecount( 1 )                      DRM_VOID                                      *f_pvSignatureContext,
    __in                                  DRM_PRND_SIGNATURE_OPERATION                   f_eSignatureOperation,
    __in_bcount( f_cbData )         const DRM_BYTE                                      *f_pbData,
    __in                                  DRM_DWORD                                      f_cbData,
    __out_bcount( f_cbSignatureBuffer )   DRM_BYTE                                      *f_pbSignatureBuffer,
    __in                                  DRM_DWORD                                      f_cbSignatureBuffer,
    __in                                  DRM_DWORD                                      f_iSignatureBuffer,
    __in_opt                        const DRM_XB_BYTEARRAY                              *f_pxbbaMIMKEncryptedWithReceiverPubkey,        /* Only used for DRM_PRND_SIGNATURE_OPERATION_VERIFY_USING_AES_OMAC1_WITH_MI    */
    __in_opt                        const DRM_XB_BYTEARRAY                              *f_pxbbaReceiverCertificate );                  /* Only used for DRM_PRND_SIGNATURE_OPERATION_VERIFY_USING_ECC256_RECEIVER_SIGNING_KEY */
PREFAST_POP /* __WARNING_NONCONST_PARAM_25004 */

/******************************************************************************
**
** PRND Registration Request Message [RX->TX]
**
******************************************************************************/

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL DRM_PRND_RegistrationRequestMessage_Start(
    __inout_bcount( f_cbStack )           DRM_BYTE                                      *f_pbStack,
    __in                                  DRM_DWORD                                      f_cbStack,
    __inout_ecount( 1 )                   DRM_XB_BUILDER_CONTEXT                        *f_pcontextBuilder ) DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL DRM_PRND_RegistrationRequestMessage_Finish(
    __in_ecount( 1 )                const DRM_XB_BUILDER_CONTEXT                        *f_pcontextBuilder,
    __inout_bcount( *f_pcbMessage )       DRM_BYTE                                      *f_pbMessage,
    __inout                               DRM_DWORD                                     *f_pcbMessage,
    __in                                  DRM_VOID                                      *f_pvSignatureContext ) DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL DRM_PRND_RegistrationRequestMessage_Parse(
    __inout_bcount( f_cbStack )           DRM_BYTE                                      *f_pbStack,
    __in                                  DRM_DWORD                                      f_cbStack,
    __in_bcount( f_cbMessage )      const DRM_BYTE                                      *f_pbMessage,
    __in                                  DRM_DWORD                                      f_cbMessage,
    __out                                 DRM_PRND_REGISTRATION_REQUEST_MESSAGE         *f_pMessage,
    __in                                  DRM_VOID                                      *f_pvSignatureContext ) DRM_NO_INLINE_ATTRIBUTE;

/******************************************************************************
**
** PRND Registration Response Message [TX->RX]
**
******************************************************************************/

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL DRM_PRND_RegistrationResponseMessage_Start(
    __inout_bcount( f_cbStack )           DRM_BYTE                                      *f_pbStack,
    __in                                  DRM_DWORD                                      f_cbStack,
    __inout_ecount( 1 )                   DRM_XB_BUILDER_CONTEXT                        *f_pcontextBuilder ) DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL DRM_PRND_RegistrationResponseMessage_Finish(
    __in_ecount( 1 )                const DRM_XB_BUILDER_CONTEXT                        *f_pcontextBuilder,
    __inout_bcount( *f_pcbMessage )       DRM_BYTE                                      *f_pbMessage,
    __inout                               DRM_DWORD                                     *f_pcbMessage,
    __in                                  DRM_VOID                                      *f_pvSignatureContext ) DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL DRM_PRND_RegistrationResponseMessage_Parse(
    __inout_bcount( f_cbStack )           DRM_BYTE                                      *f_pbStack,
    __in                                  DRM_DWORD                                      f_cbStack,
    __in_bcount( f_cbMessage )      const DRM_BYTE                                      *f_pbMessage,
    __in                                  DRM_DWORD                                      f_cbMessage,
    __out                                 DRM_PRND_REGISTRATION_RESPONSE_MESSAGE        *f_pXBMessage,
    __in                                  DRM_VOID                                      *f_pvSignatureContext ) DRM_NO_INLINE_ATTRIBUTE;

/******************************************************************************
**
** PRND Registration Error Message [TX->RX]
**
******************************************************************************/

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL DRM_PRND_RegistrationErrorMessage_Start(
    __inout_bcount( f_cbStack )           DRM_BYTE                                      *f_pbStack,
    __in                                  DRM_DWORD                                      f_cbStack,
    __inout_ecount( 1 )                   DRM_XB_BUILDER_CONTEXT                        *f_pcontextBuilder ) DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL DRM_PRND_RegistrationErrorMessage_Finish(
    __in_ecount( 1 )                const DRM_XB_BUILDER_CONTEXT                        *f_pcontextBuilder,
    __inout_bcount( *f_pcbMessage )       DRM_BYTE                                      *f_pbMessage,
    __inout                               DRM_DWORD                                     *f_pcbMessage ) DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL DRM_PRND_RegistrationErrorMessage_Parse(
    __inout_bcount( f_cbStack )           DRM_BYTE                                      *f_pbStack,
    __in                                  DRM_DWORD                                      f_cbStack,
    __in_bcount( f_cbMessage )      const DRM_BYTE                                      *f_pbMessage,
    __in                                  DRM_DWORD                                      f_cbMessage,
    __out                                 DRM_PRND_REGISTRATION_ERROR_MESSAGE           *f_pMessage ) DRM_NO_INLINE_ATTRIBUTE;

/******************************************************************************
**
** PRND Proximity Detection Start Messasge [RX->TX]
**
******************************************************************************/

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL DRM_PRND_ProximityDetectionStartMessage_Start(
    __inout_bcount( f_cbStack )           DRM_BYTE                                      *f_pbStack,
    __in                                  DRM_DWORD                                      f_cbStack,
    __inout_ecount( 1 )                   DRM_XB_BUILDER_CONTEXT                        *f_pcontextBuilder ) DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL DRM_PRND_ProximityDetectionStartMessage_Finish(
    __in_ecount( 1 )                const DRM_XB_BUILDER_CONTEXT                        *f_pcontextBuilder,
    __inout_bcount( *f_pcbMessage )       DRM_BYTE                                      *f_pbMessage,
    __inout                               DRM_DWORD                                     *f_pcbMessage ) DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL DRM_PRND_ProximityDetectionStartMessage_Parse(
    __inout_bcount( f_cbStack )           DRM_BYTE                                      *f_pbStack,
    __in                                  DRM_DWORD                                      f_cbStack,
    __in_bcount( f_cbMessage )      const DRM_BYTE                                      *f_pbMessage,
    __in                                  DRM_DWORD                                      f_cbMessage,
    __out                                 DRM_PRND_PROXIMITY_DETECTION_START_MESSAGE    *f_pMessage ) DRM_NO_INLINE_ATTRIBUTE;

/******************************************************************************
**
** PRND Proximity Detection Challenge Messasge [TX->RX]
**
******************************************************************************/
DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL DRM_PRND_ProximityDetectionChallengeMessage_Start(
    __inout_bcount( f_cbStack )           DRM_BYTE                                      *f_pbStack,
    __in                                  DRM_DWORD                                      f_cbStack,
    __inout_ecount( 1 )                   DRM_XB_BUILDER_CONTEXT                        *f_pcontextBuilder ) DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL DRM_PRND_ProximityDetectionChallengeMessage_Finish(
    __in_ecount( 1 )                const DRM_XB_BUILDER_CONTEXT                        *f_pcontextBuilder,
    __inout_bcount( *f_pcbMessage )       DRM_BYTE                                      *f_pbMessage,
    __inout                               DRM_DWORD                                     *f_pcbMessage ) DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL DRM_PRND_ProximityDetectionChallengeMessage_Parse(
    __inout_bcount( f_cbStack )           DRM_BYTE                                       *f_pbStack,
    __in                                  DRM_DWORD                                       f_cbStack,
    __in_bcount( f_cbMessage )      const DRM_BYTE                                       *f_pbMessage,
    __in                                  DRM_DWORD                                       f_cbMessage,
    __out                                 DRM_PRND_PROXIMITY_DETECTION_CHALLENGE_MESSAGE *f_pMessage ) DRM_NO_INLINE_ATTRIBUTE;

/******************************************************************************
**
** PRND Proximity Detection Response Messasge [RX->TX]
**
******************************************************************************/
DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL DRM_PRND_ProximityDetectionResponseMessage_Start(
    __inout_bcount( f_cbStack )           DRM_BYTE                                      *f_pbStack,
    __in                                  DRM_DWORD                                      f_cbStack,
    __inout_ecount( 1 )                   DRM_XB_BUILDER_CONTEXT                        *f_pcontextBuilder ) DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL DRM_PRND_ProximityDetectionResponseMessage_Finish(
    __in_ecount( 1 )                const DRM_XB_BUILDER_CONTEXT                        *f_pcontextBuilder,
    __inout_bcount( *f_pcbMessage )       DRM_BYTE                                      *f_pbMessage,
    __inout                               DRM_DWORD                                     *f_pcbMessage ) DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL DRM_PRND_ProximityDetectionResponseMessage_Parse(
    __inout_bcount( f_cbStack )           DRM_BYTE                                      *f_pbStack,
    __in                                  DRM_DWORD                                      f_cbStack,
    __in_bcount( f_cbMessage )      const DRM_BYTE                                      *f_pbMessage,
    __in                                  DRM_DWORD                                      f_cbMessage,
    __out                                 DRM_PRND_PROXIMITY_DETECTION_RESPONSE_MESSAGE *f_pMessage ) DRM_NO_INLINE_ATTRIBUTE;

/******************************************************************************
**
** PRND Proximity Detection Result Messasge [TX->RX]
**
******************************************************************************/
DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL DRM_PRND_ProximityDetectionResultMessage_Start(
    __inout_bcount( f_cbStack )           DRM_BYTE                                      *f_pbStack,
    __in                                  DRM_DWORD                                      f_cbStack,
    __inout_ecount( 1 )                   DRM_XB_BUILDER_CONTEXT                        *f_pcontextBuilder ) DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL DRM_PRND_ProximityDetectionResultMessage_Finish(
    __in_ecount( 1 )                const DRM_XB_BUILDER_CONTEXT                        *f_pcontextBuilder,
    __inout_bcount( *f_pcbMessage )       DRM_BYTE                                      *f_pbMessage,
    __inout                               DRM_DWORD                                     *f_pcbMessage ) DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL DRM_PRND_ProximityDetectionResultMessage_Parse(
    __inout_bcount( f_cbStack )           DRM_BYTE                                      *f_pbStack,
    __in                                  DRM_DWORD                                      f_cbStack,
    __in_bcount( f_cbMessage )      const DRM_BYTE                                      *f_pbMessage,
    __in                                  DRM_DWORD                                      f_cbMessage,
    __out                                 DRM_PRND_PROXIMITY_DETECTION_RESULT_MESSAGE   *f_pMessage ) DRM_NO_INLINE_ATTRIBUTE;

/******************************************************************************
**
** PRND License Request Messasge [RX->TX]
**
******************************************************************************/
DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL DRM_PRND_LicenseRequestMessage_Start(
    __inout_bcount( f_cbStack )           DRM_BYTE                                      *f_pbStack,
    __in                                  DRM_DWORD                                      f_cbStack,
    __inout_ecount( 1 )                   DRM_XB_BUILDER_CONTEXT                        *f_pcontextBuilder ) DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL DRM_PRND_LicenseRequestMessage_Finish(
    __in_ecount( 1 )                const DRM_XB_BUILDER_CONTEXT                        *f_pcontextBuilder,
    __inout_bcount( *f_pcbMessage )       DRM_BYTE                                      *f_pbMessage,
    __inout                               DRM_DWORD                                     *f_pcbMessage,
    __in                                  DRM_VOID                                      *f_pvSignatureContext ) DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL DRM_PRND_LicenseRequestMessage_Parse(
    __inout_bcount( f_cbStack )           DRM_BYTE                                      *f_pbStack,
    __in                                  DRM_DWORD                                      f_cbStack,
    __in_bcount( f_cbMessage )      const DRM_BYTE                                      *f_pbMessage,
    __in                                  DRM_DWORD                                      f_cbMessage,
    __out                                 DRM_PRND_LICENSE_REQUEST_MESSAGE              *f_pMessage,
    __in                                  DRM_VOID                                      *f_pvSignatureContext ) DRM_NO_INLINE_ATTRIBUTE;

/******************************************************************************
**
** PRND License Transmit Messasge [TX->RX]
**
******************************************************************************/

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL DRM_PRND_LicenseTransmitMessage_Start(
    __inout_bcount( f_cbStack )           DRM_BYTE                                      *f_pbStack,
    __in                                  DRM_DWORD                                      f_cbStack,
    __inout_ecount( 1 )                   DRM_XB_BUILDER_CONTEXT                        *f_pcontextBuilder ) DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL DRM_PRND_LicenseTransmitMessage_Finish(
    __in_ecount( 1 )                const DRM_XB_BUILDER_CONTEXT                        *f_pcontextBuilder,
    __inout_bcount( *f_pcbMessage )       DRM_BYTE                                      *f_pbMessage,
    __inout                               DRM_DWORD                                     *f_pcbMessage,
    __in                                  DRM_VOID                                      *f_pvSignatureContext ) DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL DRM_PRND_LicenseTransmitMessage_Parse(
    __inout_bcount( f_cbStack )           DRM_BYTE                                      *f_pbStack,
    __in                                  DRM_DWORD                                      f_cbStack,
    __in_bcount( f_cbMessage )      const DRM_BYTE                                      *f_pbMessage,
    __in                                  DRM_DWORD                                      f_cbMessage,
    __out                                 DRM_PRND_LICENSE_TRANSMIT_MESSAGE             *f_pXBMessage,
    __in                                  DRM_VOID                                      *f_pvSignatureContext ) DRM_NO_INLINE_ATTRIBUTE;

/******************************************************************************
**
** PRND License Error Message [TX->RX]
**
******************************************************************************/
DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL DRM_PRND_LicenseErrorMessage_Start(
    __inout_bcount( f_cbStack )           DRM_BYTE                                      *f_pbStack,
    __in                                  DRM_DWORD                                      f_cbStack,
    __inout_ecount( 1 )                   DRM_XB_BUILDER_CONTEXT                        *f_pcontextBuilder ) DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL DRM_PRND_LicenseErrorMessage_Finish(
    __in_ecount( 1 )                const DRM_XB_BUILDER_CONTEXT                        *f_pcontextBuilder,
    __inout_bcount( *f_pcbMessage )       DRM_BYTE                                      *f_pbMessage,
    __inout                               DRM_DWORD                                     *f_pcbMessage ) DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL DRM_PRND_LicenseErrorMessage_Parse(
    __inout_bcount( f_cbStack )           DRM_BYTE                                      *f_pbStack,
    __in                                  DRM_DWORD                                      f_cbStack,
    __in_bcount( f_cbMessage )      const DRM_BYTE                                      *f_pbMessage,
    __in                                  DRM_DWORD                                      f_cbMessage,
    __out                                 DRM_PRND_LICENSE_ERROR_MESSAGE                *f_pMessage ) DRM_NO_INLINE_ATTRIBUTE;

EXIT_PK_NAMESPACE;

#endif /* _DRMPRNDFORMAT_H_ */

