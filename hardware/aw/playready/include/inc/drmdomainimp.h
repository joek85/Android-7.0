/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#ifndef __DRM_DOMAIN_IMP_H
#define __DRM_DOMAIN_IMP_H

#include <drmlicevaltypes.h>
#include <drmxmlbuildera.h>
#include <drmdomainstoretypes.h>
#include <drmdomainkeyxmrparser.h>
#include <drmmodulesupport.h>

ENTER_PK_NAMESPACE;

/*
** The following constants define the types of custom data that is sent to
** both the domain join and domain leave challenge.
** Any custom data should belong to one and only one of the following types.
**
** NULL custom data.
*/
#define DRM_REGISTER_NULL_DATA            0

/*
** XML segment that contains only the data.
*/
#define DRM_REGISTER_CUSTOM_DATA          1

/*
** XML segment that contains both the DID and the data.
*/
#define DRM_REGISTER_INDIRECT_QUERY_DATA  2

typedef DRM_DOMAINSTORE_ENUM_CONTEXT DRM_DOMAIN_CERT_ENUM_CONTEXT;

/*
** List of license response processing flags
*/
typedef enum _DRM_PROCESS_DJ_RESPONSE_FLAG
{
    DRM_PROCESS_DJ_RESPONSE_NO_FLAGS                  = 0x00000000L,      /* Opt-in signature verification (default). */
    DRM_PROCESS_DJ_RESPONSE_SIGNATURE_NOT_REQUIRED    = 0x00000001L,      /* Signature is not required, i.e. Opt-out signature verification. */
} DRM_PROCESS_DJ_RESPONSE_FLAG;

/*
** ---------------------------------------
** Domain implementation API functions
** ---------------------------------------
*/
DRM_API DRM_RESULT DRM_CALL DRM_DOM_GenerateJoinChallenge(
    __inout                                    DRM_SECURECORE_CONTEXT   *f_pSecureCoreCtx,
    __in                                       DRM_DOMAINSTORE_CONTEXT  *f_poDomainStoreContext,
    __in                                       DRM_DWORD                 f_dwFlags,
    __in_opt                                   DRM_DOMAIN_ID            *f_poDomainID,
    __in_ecount_opt( f_cchFriendlyName ) const DRM_CHAR                 *f_pchFriendlyName,
    __in                                       DRM_DWORD                 f_cchFriendlyName,
    __in_bcount_opt( f_cbDeviceCert )    const DRM_BYTE                 *f_pbDeviceCert,
    __in                                       DRM_DWORD                 f_cbDeviceCert,
    __in_ecount_opt( f_cchData)          const DRM_CHAR                 *f_pchData,
    __in                                       DRM_DWORD                 f_cchData,
    __out_bcount_opt( *f_pcbChallenge )        DRM_BYTE                 *f_pbChallenge,
    __inout                                    DRM_DWORD                *f_pcbChallenge,
    __in_ecount( 1 )                     const PUBKEY_P256              *f_poECC256SignPubKey );

DRM_API DRM_RESULT DRM_CALL DRM_DOM_ProcessJoinResponse(
    __inout                        DRM_SECURECORE_CONTEXT  *f_pSecureCoreCtx,
    __in                           DRM_DOMAINSTORE_CONTEXT *f_poDomainStoreContext,
    __inout_bcount( f_cbResponse ) DRM_BYTE                *f_pbResponse,
    __in                           DRM_DWORD                f_cbResponse,
    __out                          DRM_RESULT              *f_pResult,
    __out_opt                      DRM_DOMAIN_ID           *f_poDomainID );

DRM_API DRM_RESULT DRM_CALL DRM_DOM_GenerateLeaveChallenge(
    __inout                                   DRM_SECURECORE_CONTEXT    *f_pSecureCoreCtx,
    __in                                      DRM_DOMAINSTORE_CONTEXT   *f_poDomainStoreContext,
    __in                                      DRM_DWORD                  f_dwFlags,
    __in_opt                                  DRM_DOMAIN_ID             *f_poDomainID,
    __in_bcount_opt( f_cbDeviceCert )   const DRM_BYTE                  *f_pbDeviceCert,
    __in                                      DRM_DWORD                  f_cbDeviceCert,
    __in_ecount_opt( f_cchData)         const DRM_CHAR                  *f_pchData,
    __in                                      DRM_DWORD                  f_cchData,
    __out_bcount_opt( *f_pcbChallenge )       DRM_BYTE                  *f_pbChallenge,
    __inout                                   DRM_DWORD                 *f_pcbChallenge,
    __in_ecount( 1 )                    const PUBKEY_P256               *f_poECC256SignPubKey );

DRM_API DRM_RESULT DRM_CALL DRM_DOM_ProcessLeaveResponse(
    __inout                           DRM_SECURECORE_CONTEXT    *f_pSecureCoreCtx,
    __in_bcount( f_cbResponse )       DRM_BYTE                  *f_pbResponse,
    __in                              DRM_DWORD                  f_cbResponse,
    __out_ecount(1)                   DRM_RESULT                *f_pResult );

DRM_API DRM_RESULT DRM_CALL DRM_DOM_FindCert(
    __in                                       DRM_DOMAINSTORE_CONTEXT *f_poDomainStoreContext,
    __in                                 const DRM_DOMAIN_ID           *f_poDomainID,
    __out_bcount_opt( *f_pcbDomainCert )       DRM_BYTE                *f_pbDomainCert,
    __inout                                    DRM_DWORD               *f_pcbDomainCert );

DRM_API DRM_RESULT DRM_CALL DRM_DOM_InitCertEnum(
    __in  DRM_DOMAINSTORE_CONTEXT      *f_poDomainStoreContext,
    __out DRM_DOMAIN_CERT_ENUM_CONTEXT *f_poDomainCertEnumContext );

DRM_API DRM_RESULT DRM_CALL DRM_DOM_EnumNextCert(
    __in  DRM_DOMAIN_CERT_ENUM_CONTEXT  *f_poDomainCertEnumContext,
    __out DRM_DWORD                     *f_pcchDomainCert,
    __out DRM_DOMAINCERT_INFO           *f_poDomainCertInfo );

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL DRM_DOM_IsLicenseValidForELS(
    __in          DRM_DOMAINSTORE_CONTEXT     *f_pDomainStore,
    __inout const DRM_XMRFORMAT               *f_pXMRLicense,
    __inout       DRM_BOOL                    *f_pfValidForELS ) DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL DRM_DOM_ParseDomainResponseKeys(
    __in                                        DRM_DWORD                          f_cbDomainKeysData,
    __in_bcount( f_cbDomainKeysData )     const DRM_BYTE                          *f_pbDomainKeysData,
    __in                                        DRM_DWORD                          f_cbDomainCert,
    __in_bcount( f_cbDomainCert )         const DRM_BYTE                          *f_pbDomainCert,
    __in                                        DRM_DWORD                          f_cbMachineCertificate,
    __in_bcount( f_cbMachineCertificate ) const DRM_BYTE                          *f_pbMachineCertificate,
    __out                                       DRM_DOMKEYXMR_HEADER              *f_pHeader,
    __out                                       DRM_DOMKEYXMR_SESSIONKEY          *f_pSessionKey,
    __out                                       DRM_DOMKEYXMR_PRIVKEYCONTAINER    *f_pPrivKeyContainer,
    __out                                       DRM_DWORD                         *f_pibDomainKeysDataPrivkeys,
    __out                                       DRM_DWORD                         *f_pcDomainKeys ) DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL DRM_DOM_LA_CalcDomainCertsCharCount(
    __in        DRM_DOMAINSTORE_CONTEXT *f_poDomainStoreContext,
    __in  const DRM_GUID                *f_poServiceID,
    __out       DRM_DWORD               *f_pcchDomainCerts ) DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL DRM_DOM_LA_BuildDomainCertsXML(
    __in          DRM_DOMAINSTORE_CONTEXT *f_poDomainStoreContext,
    __in    const DRM_GUID                *f_poServiceID,
    __inout       _XMBContextA            *f_poXMLContext ) DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL DRM_DOM_ARCLK_ClockRollbackDetected(
    __in    DRM_DOMAINSTORE_CONTEXT *f_poDomainStoreContext,
    __inout DRM_SECURECORE_CONTEXT  *f_pSecureCoreCtx,
    __in    DRM_UINT64               f_ui64GlobalSavedTime ) DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL DRM_DOM_ARCLK_LEVL_DisableOnRollback(
    __inout       DRM_LICEVAL_CONTEXT *f_pContextLicEval,
    __in    const DRM_XMRFORMAT       *f_pXMR,
    __in          DRM_UINT64           f_u64MachineDateTime,
    __out         DRM_BOOL            *f_pfDisabled ) DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL DRM_DOM_ARCLK_CreateDomainStorePassword(
    __in                          const DRM_GUID                *f_poAccountID,
    __inout                             DRM_SECURECORE_CONTEXT  *f_pSecureCoreCtx,
    __out_bcount( DRM_SHA1_DIGEST_LEN ) DRM_BYTE                 f_rgbPasswordSST[ DRM_SHA1_DIGEST_LEN ] ) DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL DRM_DOM_ARCLK_GetDomainSavedDateTime(
    __in_bcount( DRM_SHA1_DIGEST_LEN ) const DRM_BYTE                 f_rgbPasswordSST[ DRM_SHA1_DIGEST_LEN ],
    __inout_opt                              DRM_VOID                *f_pOEMContext,
    __in                                     DRM_DOMAINSTORE_CONTEXT *f_poDomainStoreContext,
    __in                               const DRM_GUID                *f_poAccountID,
    __out_opt                                DRM_UINT64              *f_puiDomainSavedDateTime ) DRM_NO_INLINE_ATTRIBUTE;

DRM_API DRM_RESULT DRM_DOM_InitDomainInfoForXMRBind(
    DRM_DOMAINSTORE_CONTEXT *f_pDomainStoreContext,
    DRM_XMR_BINDING_INFO    *f_pXmrInfo );

EXIT_PK_NAMESPACE;

#endif /* __DRM_DOMAIN_IMP_H */

