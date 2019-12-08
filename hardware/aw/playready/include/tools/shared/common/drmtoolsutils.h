/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#ifndef __DRM_TOOLS_UTILS_H__
#define __DRM_TOOLS_UTILS_H__ 1

#include <drmxmlbuilder.h>
#include <oemeccp256.h>
#include <oemcommon.h>
#include <oemrsacommon.h>
#include <drmtypes.h>
#include <drmoutputleveltypes.h>
#include <drmcallbacktypes.h>
#include <drmbytemanip.h>

ENTER_PK_NAMESPACE;

DRM_API_VOID DRM_VOID DRM_CALL PrintCopyrightInfo( __in_z const DRM_CHAR *f_pszAppDesc );

DRM_API_VOID DRM_VOID DRM_CALL PrintCopyrightAndVersionInfo(
    __in_z const DRM_CHAR  *f_pszAppDesc,
    __in         DRM_DWORD  f_dwMajorVersion,
    __in         DRM_DWORD  f_dwMinorVersion );

DRM_API DRM_RESULT DRM_CALL DRMTOOLS_TestKeyPair(
    __in                             DRM_CRYPTO_CONTEXT                *f_pContextCrypto,
    __in                             DRM_PKCRYPTO_SUPPORTED_ALGORITHMS  f_eAlgorithm,
    __in_bcount( f_cbPubKey )  const DRM_BYTE                          *f_pbPubKey,
    __in                             DRM_DWORD                          f_cbPubKey,
    __in_bcount( f_cbPrivKey ) const DRM_BYTE                          *f_pbPrivKey,
    __in                             DRM_DWORD                          f_cbPrivKey );

DRM_API DRM_RESULT DRM_CALL DRMTOOLS_CreateKeyPair(
    __in                        DRM_CRYPTO_CONTEXT                        *f_pContextCrypto,
    __in                        DRM_PKCRYPTO_SUPPORTED_ALGORITHMS          f_eAlgorithm,
    __in                        DRM_RSA_SUPPORTED_KEY_LENGTHS_2048BIT      f_eRSAKeyLength,
    __out_bcount( f_cbPubKey )  DRM_BYTE                                  *f_pbPubKey,
    __in                        DRM_DWORD                                  f_cbPubKey,
    __out_bcount( f_cbPrivKey ) DRM_BYTE                                  *f_pbPrivKey,
    __in                        DRM_DWORD                                  f_cbPrivKey,
    __out_opt                   DRM_STRING                                *f_pdstrPubKey,
    __out_opt                   DRM_STRING                                *f_pdstrPrivKey );

DRM_API DRM_RESULT  DRM_CALL DRMTOOLS_ReallocXmlContext(
    __inout    _XMBContext        **f_ppbXmlContext);

DRM_API DRM_RESULT DRM_CALL DRMTOOLS_Base64EncodeOfByteArray(
    __in_bcount( f_cbArray ) const DRM_BYTE   *f_pbArray,
    __in                     const DRM_DWORD   f_cbArray,
    __inout                        DRM_STRING *f_pdstrB64 );

DRM_API DRM_RESULT DRM_CALL DRMTOOLS_OpenOutputFile(
    __in  const   DRM_CONST_STRING    *f_pdstrFileName,
    __out         OEM_FILEHDL         *f_pFileHandle );

DRM_API DRM_RESULT DRM_CALL DRMTOOLS_ReadFileData(
    __in  const DRM_CONST_STRING    *f_pdstrFileName,
    __out       DRM_STRING          *f_pdstrFileData);

DRM_API DRM_RESULT DRM_CALL DRMTOOLS_ReadBinaryFileData(
    __in  const DRM_CONST_STRING    *f_pdstrFileName,
    __out       DRM_BYTE            **f_ppbData,
    __out       DRM_DWORD           *f_cbData );

DRM_API DRM_RESULT DRM_CALL DRMTOOLS_WriteBufferToFile(
    __in const DRM_CONST_STRING     *f_pdstrOutputFile,
    __in const DRM_BYTE             *pbBuffer,
    __in const DRM_DWORD            cbBuffer );

DRM_API_VOID DRM_VOID DRM_CALL DRMTOOLS_PrintErrorCode(
    __in DRM_RESULT dr );

DRM_API DRM_RESULT DRM_CALL DRMTOOLS_PrintOPLOutputIDs(  __in const DRM_OPL_OUTPUT_IDS *f_pOPLs);

DRM_API DRM_RESULT DRM_CALL DRMTOOLS_PrintVideoOutputProtectionIDs( __in const DRM_VIDEO_OUTPUT_PROTECTION_IDS_EX *f_pOPLs);

DRM_API DRM_RESULT DRM_CALL DRMTOOLS_PrintPolicyCallback(
    __in const DRM_VOID                *f_pvPolicyCallbackData,
    __in       DRM_POLICY_CALLBACK_TYPE f_dwCallbackType );

EXIT_PK_NAMESPACE;

#endif /* end #ifndef __DRM_TOOLS_UTILS_H__ */

