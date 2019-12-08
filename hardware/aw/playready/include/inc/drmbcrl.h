/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#ifndef __DRMBCRL_H__
#define __DRMBCRL_H__

#include <drmbcrltypes.h>
#include <oemcryptoctx.h>
#include <oemeccp256.h>
#include <oemparsers.h>
#include <drmbcertformatparser.h>
#include <drmrevocationtypes.h>
#include <drmsecurecoretypes.h>

ENTER_PK_NAMESPACE;

/*********************************************************************
**
**  Parses Playready/Silverlight CRL from binary to data structure.
**  Verifies signature and certificate chain.
**
**  NOTE: The parser does not make copies of the DRM_RevocationEntry
**        data, it just points to them in the f_pbCrlData buffer so
**        you cannot free the f_pbCrlData and still have a valid f_poCrl
**        data structure.
**
*********************************************************************/

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL DRM_BCrl_ParseCrl(
    __in_bcount(f_cbCrlData)          const DRM_BYTE                        *f_pbCrlData,
    __in                              const DRM_DWORD                        f_cbCrlData,
    __out                                   DRM_BCRL_Signed                 *f_poCrl,
    __in_ecount(1)                    const PUBKEY_P256                     *f_pRootPubkey,
    __inout_opt                             DRM_CRYPTO_CONTEXT              *f_pCryptoCtx,
    __inout_opt                             DRM_SECURECORE_CONTEXT          *f_pSecureCoreCtx,
    __in_opt                                DRM_SECSTORE_CONTEXT            *f_pcontextSST,
    __in_opt                                DRM_DST                         *f_pDatastore ) DRM_NO_INLINE_ATTRIBUTE;

/*********************************************************************
**
**  Checks to see if the given certificate is in the given CRL file.
**  Internally the API uses the DRM_BCrl_ParseCrl method to parse
**  and verify the CRL.  Then it walks through the entries in the CRL
**  to see if the given certificate is revoked.  The API returns
**  DRM_E_CERTIFICATE_REVOKED if any certificate in the chain is revoked.
**
*********************************************************************/

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL DRM_BCrl_CheckIfPlayReadyCertRevoked(
    __inout                                 DRM_BCERTFORMAT_PARSER_CONTEXT  *f_pCertParserCtx,
    __in                              const DRM_BCERTFORMAT_CHAIN           *f_pPlayReadyCertChain,
    __in_bcount(f_cbPlayReadyCRL)     const DRM_BYTE                        *f_pbPlayReadyCRL,
    __in                                    DRM_DWORD                        f_cbPlayReadyCRL,
    __out_opt                               DRM_SHA256_Digest               *f_pRevokedCertDigest,
    __out_opt                               DRM_DWORD                       *f_pdwPlayReadyCRLVer,
    __inout_opt                             DRM_CRYPTO_CONTEXT              *f_pCryptoCtx,
    __inout_opt                             DRM_SECURECORE_CONTEXT          *f_pSecureCoreCtx,
    __in_opt                                DRM_SECSTORE_CONTEXT            *f_pcontextSST,
    __in_opt                                DRM_DST                         *f_pDatastore ) DRM_NO_INLINE_ATTRIBUTE;

EXIT_PK_NAMESPACE;

#endif /* __DRMBCRL_H__ */


