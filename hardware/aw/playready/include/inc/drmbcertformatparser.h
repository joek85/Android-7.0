/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#ifndef __DRMBCERTFORMATPARSER_H__
#define __DRMBCERTFORMATPARSER_H__

#include <drmxbparser.h>
#include <drmbcertconstants.h>
#include <drmbcertformat_generated.h>
#include <oemtee.h>
#include <drmrevocationtypes.h>

ENTER_PK_NAMESPACE;

PREFAST_PUSH_DISABLE_EXPLAINED(__WARNING_POOR_DATA_ALIGNMENT_25021,"Ignore poor data alignment" );

typedef struct __tagDRM_BCERTFORMAT_VERIFICATIONRESULT {
    /*
    ** Number of the cert in the chain, zero-based with leaf cert as 0
    */
    DRM_DWORD   cCertNumber;
    /*
    ** Error code
    */
    DRM_RESULT  drResult;
} DRM_BCERTFORMAT_VERIFICATIONRESULT;


typedef struct __tagDRM_BCERTFORMAT_PARSER_CONTEXT_INTERNAL
{
    DRM_BOOL                             fInitialized;
    DRM_BOOL                             fCheckSignature;
    DRM_DWORD                            iCurrentCert;
    DRMFILETIME                          ftCurrentTime;
    DRM_BOOL                             fRootPubKeySet;
    PUBKEY_P256                          oRootPubKey;
    DRM_DWORD                            dwExpectedLeafCertType;
    DRM_BOOL                             fDontFailOnMissingExtData;
    DRM_DWORD                            dwRequiredKeyUsageMask;
    DRM_DWORD                            iResult;
    DRM_DWORD                            cResults;
    DRM_STACK_ALLOCATOR_CONTEXT         *pStack;
    DRM_BCERTFORMAT_CERT                 rgoCertCache[DRM_BCERT_MAX_CERTS_PER_CHAIN];
    OEM_TEE_CONTEXT                     *pOemTeeCtx;
    DRM_BCERTFORMAT_VERIFICATIONRESULT  *pResults;
} DRM_BCERTFORMAT_PARSER_CONTEXT_INTERNAL;


#define DRM_BCERTFORMAT_PARSER_CONTEXT_BUFFER_SIZE sizeof( DRM_BCERTFORMAT_PARSER_CONTEXT_INTERNAL )

typedef struct __tagDRM_BCERTFORMAT_PARSER_CONTEXT
{
    /*
    ** This data is Opaque.  Do not set any value in it.
    */
    DRM_BYTE rgbOpaqueBuffer[ DRM_BCERTFORMAT_PARSER_CONTEXT_BUFFER_SIZE ];
} DRM_BCERTFORMAT_PARSER_CONTEXT;

PREFAST_POP; /* __WARNING_POOR_DATA_ALIGNMENT_25021 */

#define DRM_BCERTFORMAT_CHKVERIFICATIONERR( pVerificationCtx, fCondition, dwErr ) \
do {                                                                              \
    if( !(fCondition) )                                                           \
    {                                                                             \
        DRM_BCERTFORMAT_PARSER_CONTEXT_INTERNAL *_pVCtx = (pVerificationCtx);     \
        DRM_RESULT _drErr = (dwErr);                                              \
        if( _pVCtx->iResult >= _pVCtx->cResults )                                 \
        {                                                                         \
            ChkDR( _drErr );                                                      \
        }                                                                         \
        else                                                                      \
        {                                                                         \
            _pVCtx->pResults[_pVCtx->iResult].cCertNumber = _pVCtx->iCurrentCert; \
            _pVCtx->pResults[_pVCtx->iResult].drResult    = _drErr;               \
            _pVCtx->iResult = _pVCtx->iResult + 1;                                \
        }                                                                         \
    }                                                                             \
} while(FALSE)

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL DRM_BCERTFORMAT_OverrideRootPublicKey(
    __inout_ecount( 1 )                     DRM_BCERTFORMAT_PARSER_CONTEXT        *f_pParserCtx,
    __in_ecount_opt( 1 )              const PUBKEY_P256                           *f_pRootPubKey ) DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL DRM_BCERTFORMAT_InitializeParserContext(
    __in                                    DRM_BOOL                               f_fCheckSignature,
    __in                                    DRM_BOOL                               f_fDontFailOnMissingExtData,
    __in                                    DRM_DWORD                              f_cRequiredKeyUsages,
    __in_ecount_opt( f_cRequiredKeyUsages )
                                      const DRM_DWORD                             *f_pdwRequiredKeyUsages,
    __in                                    DRM_DWORD                              f_cResults,
    __in_ecount_opt( f_cResults )           DRM_BCERTFORMAT_VERIFICATIONRESULT    *f_pResults,
    __inout_ecount( 1 )                     DRM_BCERTFORMAT_PARSER_CONTEXT        *f_pParserCtx ) DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL DRM_BCERTFORMAT_GetCertificate(
    __inout_ecount( 1 )                     DRM_BCERTFORMAT_PARSER_CONTEXT        *f_pParserCtx,
    __in                              const DRM_BCERTFORMAT_CERT_HEADER           *f_pCertificateHeader,
    __out_ecount_opt( 1 )                   DRM_DWORD                             *f_pcbParsed,
    __out_ecount_opt( 1 )                   DRM_BCERTFORMAT_CERT                  *f_pCert ) DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL DRM_BCERTFORMAT_ParseCertificateChain(
    __inout_ecount( 1 )                     DRM_BCERTFORMAT_PARSER_CONTEXT        *f_pParserCtx,
    __inout_opt                             OEM_TEE_CONTEXT                       *f_pOemTeeContext,
    __in_ecount_opt( 1 )              const DRMFILETIME                           *f_pftCurrentTime,
    __in                                    DRM_DWORD                              f_dwExpectedLeafCertType,
    __inout                                 DRM_STACK_ALLOCATOR_CONTEXT           *f_pStack,
    __in                                    DRM_DWORD                              f_cbCertData,
    __in_bcount( f_cbCertData )       const DRM_BYTE                              *f_pbCertData,
    __inout_ecount_opt( 1 )                 DRM_DWORD                             *f_cbParsed,
    __out_ecount( 1 )                       DRM_BCERTFORMAT_CHAIN                 *f_pChain,
    __out_ecount_opt( 1 )                   DRM_BCERTFORMAT_CERT                  *f_pLeafMostCert ) DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL DRM_BCERTFORMAT_VerifyCertificateChain(
    __inout_opt                             OEM_TEE_CONTEXT                       *f_pOemTeeContext,
    __inout_opt                             DRM_BCERTFORMAT_PARSER_CONTEXT        *f_pParserCtx,
    __in_ecount_opt( 1 )              const DRMFILETIME                           *f_pftCurrentTime,
    __in                                    DRM_DWORD                              f_dwCertType,
    __in                                    DRM_DWORD                              f_dwMinSecurityLevel,
    __in                                    DRM_DWORD                              f_cbCertData,
    __in_bcount( f_cbCertData )       const DRM_BYTE                              *f_pbCertData ) DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL DRM_BCERTFORMAT_GetPublicKeyByUsageFromChain(
    __inout_ecount( 1 )                     DRM_BCERTFORMAT_PARSER_CONTEXT        *f_pParserCtx,
    __in                              const DRM_BCERTFORMAT_CHAIN                 *f_pChain,
    __in                                    DRM_DWORD                              f_dwKeyUsage,
    __inout_ecount( 1 )                     PUBKEY_P256                           *f_pPubkey,
    __out_ecount_opt( 1 )                   DRM_DWORD                             *f_pdwKeyUsageSet,
    __inout_ecount_opt( 1 )                 DRM_BCERTFORMAT_CERT                  *f_pCert,
    __out_ecount_opt( 1 )                   DRM_DWORD                             *f_pdwCertKeyIndex ) DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL DRM_BCERTFORMAT_GetPublicKeyFromCert(
    __in_ecount( 1 )                  const DRM_BCERTFORMAT_CERT                  *f_pCert,
    __inout_ecount( 1 )                     PUBKEY_P256                           *f_pPubkey ) DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL DRM_BCERTFORMAT_GetPublicKeyByUsage(
    __inout_opt                             DRM_BCERTFORMAT_PARSER_CONTEXT        *f_pParserCtx,
    __inout_opt                             OEM_TEE_CONTEXT                       *f_pOemTeeContext,
    __in                              const DRM_DWORD                              f_cbCertData,
    __in_bcount(f_cbCertData)         const DRM_BYTE                              *f_pbCertData,
    __in                              const DRM_DWORD                              f_dwCertIndex,
    __in                              const DRM_DWORD                              f_dwKeyUsage,
    __out_ecount(1)                         PUBKEY_P256                           *f_pPubkey,
    __out_opt                               DRM_DWORD                             *f_pdwKeyUsageSet,
    __out_ecount_opt(1)                     DRM_DWORD                             *f_pdwCertKeyIndex ) DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL DRM_BCERTFORMAT_GetPublicKey(
    __inout_opt                             DRM_BCERTFORMAT_PARSER_CONTEXT        *f_pParserCtx,
    __inout_opt                             OEM_TEE_CONTEXT                       *f_pOemTeeContext,
    __in                              const DRM_DWORD                              f_cbCertData,
    __in_bcount(f_cbCertData)         const DRM_BYTE                              *f_pbCertData,
    __out_ecount(1)                         PUBKEY_P256                           *f_pPubkey ) DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL DRM_BCERTFORMAT_VerifyChildUsage(
    __in                              const DRM_DWORD                              f_dwChildKeyUsageMask,
    __in                              const DRM_DWORD                              f_dwParentKeyUsageMask ) DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL DRM_BCERTFORMAT_GetDigestValuesOutOfCertChain(
    __inout                                 DRM_BCERTFORMAT_PARSER_CONTEXT        *f_pCertParserCtx,
    __in                              const DRM_BCERTFORMAT_CHAIN                 *f_pCertChain,
    __inout                                 DRM_DWORD                             *f_pcDigestValues,
    __out_ecount_opt( *f_pcDigestValues  )  DRM_RevocationEntry                   *f_pDigestValues ) DRM_NO_INLINE_ATTRIBUTE;

EXIT_PK_NAMESPACE;

#endif /* __DRMBCERTFORMATPARSER_H__ */
