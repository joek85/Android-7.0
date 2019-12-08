/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#ifndef __DRMBCERTFORMATBUILDER_H__
#define __DRMBCERTFORMATBUILDER_H__

#include <drmxbbuilder.h>
#include <drmbcertconstants.h>
#include <drmbcertformat_generated.h>
#include <drmbcertformatparser.h>
#include <oemtee.h>

ENTER_PK_NAMESPACE;

#define DRM_BCERTFORMAT_PAD_AMOUNT(x) ( ( (x) > 0 ) ? ( ( sizeof(DRM_DWORD) - ( (x) % sizeof(DRM_DWORD) ) ) % sizeof(DRM_DWORD) ) : 0 )

#define DRM_BCERTFORMAT_BUILDER_CALCULATE_SIGNATUREINFO_SIZE(cbSignatureLength, cbIssuerKeyLength) \
            ( XB_BASE_OBJECT_LENGTH                                                                \
            + sizeof( DRM_WORD )                                /* signature type    */            \
            + sizeof( DRM_WORD )                                /* signature length  */            \
            + ( cbSignatureLength )                             /* signature data    */            \
            + DRM_BCERTFORMAT_PAD_AMOUNT( cbSignatureLength )   /* padding           */            \
            + sizeof( DRM_DWORD )                               /* issuer key length */            \
            + ( cbIssuerKeyLength )                             /* issuer key value  */            \
            + DRM_BCERTFORMAT_PAD_AMOUNT( cbIssuerKeyLength ) ) /* padding           */

#define DRM_BCERTFORMAT_BUILDER_CALCULATE_EXTDATA_RECORD_SIZE(cbBlobLength)                        \
            ( XB_BASE_OBJECT_LENGTH                             /* extended data record header */  \
            + sizeof( DRM_DWORD )                               /* data length                 */  \
            + cbBlobLength                                      /* data blob                   */  \
            + DRM_BCERTFORMAT_PAD_AMOUNT( cbBlobLength ) )      /* padding                     */

#define DRM_BCERTFORMAT_BUILDER_CALCULATE_EXTDATA_SIGNATURE_SIZE(cbSignatureLength)                \
            ( XB_BASE_OBJECT_LENGTH                             /* signature obj header        */  \
            + sizeof( DRM_WORD )                                /* signature type              */  \
            + sizeof( DRM_WORD )                                /* signature length            */  \
            + cbSignatureLength                                 /* signature data              */  \
            + DRM_BCERTFORMAT_PAD_AMOUNT( cbSignatureLength ) ) /* padding                     */

#define DRM_BCERTFORMAT_BUILDER_CALCULATE_EXTDATA_CONTAINER_SIZE(cbBlobLength, cbSignatureLength)  \
            ( XB_BASE_OBJECT_LENGTH                             /* container obj header        */  \
            + DRM_BCERTFORMAT_BUILDER_CALCULATE_EXTDATA_RECORD_SIZE(cbBlobLength)                  \
            + DRM_BCERTFORMAT_BUILDER_CALCULATE_EXTDATA_SIGNATURE_SIZE(cbSignatureLength) )

#define ChkDRAllowXBObjectExists(expr) do {                \
            dr = (expr);                                    \
            if( dr == DRM_E_XB_OBJECT_ALREADY_EXISTS )      \
            {                                               \
                dr = DRM_SUCCESS;                           \
            }                                               \
            else if( DRM_FAILED( dr ) )                     \
            {                                               \
                goto ErrorExit;                             \
            }                                               \
        } while(FALSE)

#define DRM_BCERTFORMAT_MAX_MANUFACTURER_STRING_LENGTH  128   /* bytes */
typedef struct _tagDrmBCertFormatManufacturerString
{
    DRM_DWORD   cb;
    DRM_BYTE    rgb[ DRM_BCERT_MAX_MANUFACTURER_STRING_LENGTH ];
} DRM_BCERTFORMAT_MANUFACTURER_STRING;

typedef struct _tagDrmBCertFormatManufacturerStrings
{
    DRM_BCERTFORMAT_MANUFACTURER_STRING  ManufacturerName;
    DRM_BCERTFORMAT_MANUFACTURER_STRING  ModelName;
    DRM_BCERTFORMAT_MANUFACTURER_STRING  ModelNumber;
} DRM_BCERTFORMAT_MANUFACTURER_STRINGS;

typedef struct __tagDRM_BCERTFORMAT_BUILDER_KEYDATA
{
    DRM_WORD      wKeyType;
    DRM_WORD      cbKey;
    DRM_DWORD     cKeyUsages;
    DRM_BYTE      rgbKey[DRM_BCERT_MAX_PUBKEY_VALUE_LENGTH];
    DRM_DWORD     rgdwKeyUsages[DRM_BCERT_MAX_KEY_USAGES];
} DRM_BCERTFORMAT_BUILDER_KEYDATA;

typedef struct __tagDRM_BCERTFORMAT_BUILDER_CONTEXT_INTERNAL
{
    DRM_BCERTFORMAT_CHAIN                oCertChain;
    DRM_BCERTFORMAT_CERT                 oNewCert;
    DRM_XB_BUILDER_CONTEXT               oBuilderCtx;
    DRM_BCERTFORMAT_MANUFACTURER_STRINGS oManufactureStrings;
    DRM_BCERTFORMAT_BUILDER_KEYDATA     *pKeyData;
    DRM_DWORD                            cbExtPrivKey;
    const DRM_BYTE                      *pbExtPrivKey;
} DRM_BCERTFORMAT_BUILDER_CONTEXT_INTERNAL;

#define DRM_BCERTFORMAT_BUILDER_CONTEXT_BUFFER_SIZE sizeof( DRM_BCERTFORMAT_BUILDER_CONTEXT_INTERNAL )

typedef struct __tagDRM_BCERTFORMAT_BUILDER_CONTEXT
{
    /*
    ** This data is Opaque.  Do not set any value in it.
    */
    DRM_BYTE rgbOpaqueBuffer[ DRM_BCERTFORMAT_BUILDER_CONTEXT_BUFFER_SIZE ];
} DRM_BCERTFORMAT_BUILDER_CONTEXT;

typedef union __tagDRM_BCERTFORMAT_CERT_TYPE_DATA
{
    struct _tagDrmBCertDomainData
    {
        DRM_ID     ServiceID;
        DRM_ID     AccountID;
        DRM_DWORD  dwRevision;   /* a timestamp */
        DRM_DWORD  cbDomainURL;
        DRM_BYTE   rgbDomainURL[DRM_BCERT_MAX_URL_LENGTH];
    } Domain;

    struct _tagDrmBCertPCData
    {
        DRM_DWORD  dwSecurityVersion;  /* A.B.C.D */
    } PC;

    struct _tagDrmSilverlightData
    {
        DRM_DWORD  dwSecurityVersion;  /* A.B.C.D */
        DRM_DWORD  dwPlatformIdentifier;
    } Silverlight;

    struct _tagDrmBCertDeviceData
    {
        DRM_DWORD  cbMaxLicense;
        DRM_DWORD  cbMaxHeader;
        DRM_DWORD  dwMaxChainDepth;
    } Device;

    struct _tagDrmBCertMeteringData
    {
        DRM_ID     MeteringID;
        DRM_DWORD  cbMeteringURL;
        DRM_BYTE   rgbMeteringURL[DRM_BCERT_MAX_URL_LENGTH];
    } Metering;

    struct _tagDrmBCertServerData
    {
        DRM_DWORD  dwWarningDays;
    } Server;
} DRM_BCERTFORMAT_CERT_TYPE_DATA;

typedef struct __tagDrmBCertFormatCertSignatureInfo
{
    DRM_DWORD        cbCertificate;
    DRM_DWORD        cbSignature;
    const DRM_BYTE  *pbCertficate;
    DRM_BYTE        *pbSignature;
} DRM_BCERTFORMAT_CERT_SIGNATURE_INFO;

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL DRM_BCERTFORMAT_SetKeyInfo(
    __inout_ecount( 1 )                             DRM_BCERTFORMAT_BUILDER_CONTEXT     *f_pBuilderCtx,
    __in                                            DRM_DWORD                            f_cKeys,
    __in_ecount( f_cKeys )                          DRM_BCERTFORMAT_BUILDER_KEYDATA     *f_pKeys ) DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL DRM_BCERTFORMAT_SetManufacturerName(
    __inout_ecount( 1 )                             DRM_BCERTFORMAT_BUILDER_CONTEXT     *f_pBuilderCtx,
    __in                                            DRM_DWORD                            f_cbManufacturerName,
    __in_bcount( f_cbManufacturerName )       const DRM_BYTE                            *f_pbManufacturerName ) DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL DRM_BCERTFORMAT_SetModelName(
    __inout_ecount( 1 )                             DRM_BCERTFORMAT_BUILDER_CONTEXT     *f_pBuilderCtx,
    __in                                            DRM_DWORD                            f_cbModelName,
    __in_bcount( f_cbModelName )              const DRM_BYTE                            *f_pbModelName ) DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL DRM_BCERTFORMAT_SetModelNumber(
    __inout_ecount( 1 )                             DRM_BCERTFORMAT_BUILDER_CONTEXT     *f_pBuilderCtx,
    __in                                            DRM_DWORD                            f_cbModelNumber,
    __in_bcount( f_cbModelNumber )            const DRM_BYTE                            *f_pbModelNumber ) DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL DRM_BCERTFORMAT_SetFeatures(
    __inout_ecount( 1 )                             DRM_BCERTFORMAT_BUILDER_CONTEXT     *f_pBuilderCtx,
    __in                                            DRM_DWORD                            f_cFeatures,
    __in_ecount( f_cFeatures )                const DRM_DWORD                           *f_pdwFeatures ) DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL DRM_BCERTFORMAT_SetSecurityLevel(
    __inout_ecount( 1 )                             DRM_BCERTFORMAT_BUILDER_CONTEXT     *f_pBuilderCtx,
    __in                                            DRM_DWORD                            f_dwSecurityLevel ) DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL DRM_BCERTFORMAT_SetBuilderFlags(
    __inout_ecount( 1 )                             DRM_BCERTFORMAT_BUILDER_CONTEXT     *f_pBuilderCtx,
    __in                                            DRM_DWORD                            f_dwChainHeaderFlags,
    __in                                            DRM_DWORD                            f_dwBasicInfoFlags,
    __in                                            DRM_DWORD                            f_dwManufacturerFlags ) DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL DRM_BCERTFORMAT_SetSecurityVersion(
     __inout_ecount( 1 )                            DRM_BCERTFORMAT_BUILDER_CONTEXT     *f_pBuilderCtx,
     __in                                           DRM_DWORD                            f_dwPlatformID,
     __in                                           DRM_DWORD                            f_dwSecurityVersion ) DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL DRM_BCERTFORMAT_SetExpirationDate(
    __inout_ecount( 1 )                             DRM_BCERTFORMAT_BUILDER_CONTEXT     *f_pBuilderCtx,
    __in                                            DRM_DWORD                            f_dwExpirationDate ) DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL DRM_BCERTFORMAT_SetExtData(
    __inout_ecount( 1 )                             DRM_BCERTFORMAT_BUILDER_CONTEXT     *f_pBuilderCtx,
    __in                                            DRM_DWORD                            f_cbExtData,
    __in_bcount( f_cbExtData )                const DRM_BYTE                            *f_pbExtData ) DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL DRM_BCERTFORMAT_SetExtDataKeyInfo(
    __inout_ecount( 1 )                             DRM_BCERTFORMAT_BUILDER_CONTEXT     *f_pBuilderCtx,
    __in                                            DRM_WORD                             f_wKeyType,
    __in                                            DRM_WORD                             f_wKeyLen,
    __in                                            DRM_DWORD                            f_dwFlags,
    __in                                            DRM_DWORD                            f_cbPrivateSigningKey,
    __in_bcount( f_cbPrivateSigningKey )      const DRM_BYTE                            *f_pbPrivateSigningKey,
    __in                                            DRM_DWORD                            f_cbPublicSigningKey,
    __in_bcount( f_cbPublicSigningKey )       const DRM_BYTE                            *f_pbPublicSigningKey ) DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL DRM_BCERTFORMAT_LoadCertificateChain(
    __inout_opt                                     OEM_TEE_CONTEXT                     *f_pOemTeeContext,
    __inout                                         DRM_STACK_ALLOCATOR_CONTEXT         *f_pStack,
    __in                                            DRM_DWORD                            f_cbCertChain,
    __in_bcount( f_cbCertChain )              const DRM_BYTE                            *f_pbCertChain,
    __out_ecount( 1 )                               DRM_BCERTFORMAT_CERT               **f_ppCertWeakRef,
    __out_ecount( 1 )                               DRM_BCERTFORMAT_BUILDER_CONTEXT     *f_pBuilderCtx,
    __out_ecount( 1 )                               DRM_BCERTFORMAT_PARSER_CONTEXT      *f_pParserCtx ) DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL DRM_BCERTFORMAT_SaveCertificateChain(
    __inout                                         DRM_BCERTFORMAT_BUILDER_CONTEXT     *f_pBuilderCtx,
    __inout_opt                                     OEM_TEE_CONTEXT                     *f_pOemTeeContext,
    __in                                      const PRIVKEY_P256                        *f_pIssuerPrivateKey,
    __in                                      const PUBKEY_P256                         *f_pIssuerPublicKey,
    __out                                           DRM_DWORD                           *f_pcbCertChain,
    __out_bcount_opt( *f_pcbCertChain )             DRM_BYTE                            *f_pbCertChain ) DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL DRM_BCERTFORMAT_StartCertificateChain(
    __inout                                         DRM_STACK_ALLOCATOR_CONTEXT         *f_pStack,
    __in                                            DRM_DWORD                            f_cbParentChain,
    __in_bcount_opt( f_cbParentChain )        const DRM_BYTE                            *f_pbParentChain,
    __in                                            DRM_DWORD                            f_dwCertType,
    __in                                      const DRM_BCERTFORMAT_CERT_TYPE_DATA      *f_pCertData,
    __in                                      const DRM_ID                              *f_pCertificateID,
    __in_opt                                  const DRM_ID                              *f_pClientID,
    __out_ecount( 1 )                               DRM_BCERTFORMAT_BUILDER_CONTEXT     *f_pBuilderCtx ) DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL DRM_BCERTFORMAT_FinishCertificateChain(
    __inout_ecount( 1 )                             DRM_BCERTFORMAT_BUILDER_CONTEXT     *f_pBuilderCtx,
    __inout_opt                                     OEM_TEE_CONTEXT                     *f_pOemTeeContext,
    __in_opt                                  const PRIVKEY_P256                        *f_pIssuerPrivateKey,
    __in                                      const PUBKEY_P256                         *f_pIssuerPublicKey,
    __inout_ecount( 1 )                             DRM_DWORD                           *f_pcbCertificateChain,
    __inout_bcount_opt( *f_pcbCertificateChain )    DRM_BYTE                            *f_pbCertificateChain,
    __inout_opt                                     DRM_BCERTFORMAT_CERT_SIGNATURE_INFO *f_pSignatureInfo ) DRM_NO_INLINE_ATTRIBUTE;

EXIT_PK_NAMESPACE;

#endif /* __DRMBCERTFORMATBUILDER_H__ */
