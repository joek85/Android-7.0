/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#ifndef __DRM_XMRFORMAT_BUILDER_H__
#define __DRM_XMRFORMAT_BUILDER_H__

#include <drmxmrformattypes.h>
#include <drmxbbuilder.h>
#include <drmxmrformat_generated.h>

ENTER_PK_NAMESPACE;

typedef struct __tagDRM_XMRFORMAT_BUILDER_CONTEXT_INTERNAL
{
    DRM_BOOL               fConvertAuxKeyLocations;
    DRM_XMRFORMAT          oLicense;
    DRM_XB_BUILDER_CONTEXT oBuilderCtx;
} DRM_XMRFORMAT_BUILDER_CONTEXT_INTERNAL;

#define DRM_XMRFORMAT_BUILDER_CONTEXT_BUFFER_SIZE sizeof( DRM_XMRFORMAT_BUILDER_CONTEXT_INTERNAL )

typedef struct __tagDRM_XMRFORMAT_BUILDER_CONTEXT
{
    /*
    ** This data is Opaque.  Do not set any value in it.
    */
    DRM_BYTE rgbOpaqueBuffer[ DRM_XMRFORMAT_BUILDER_CONTEXT_BUFFER_SIZE ];
} DRM_XMRFORMAT_BUILDER_CONTEXT;

/******************************************************************************
**
** Function :   DRM_XMRFORMAT_CB_SIGNATURE
**
** Synopsis :   Callback function provided by caller of XMR Builder APIs to
**              create a signature using the integrity key. Integrity key is not
**              passed to the builder APIs for security reasons. In the client,
**              the clear integrity key should stay within the blackbox.
**
** Arguments :  f_pvSignatureContext  - Context needed by caller to retrieve
**                                      appropriate integrity key
**              f_pbData              - Data to be signed
**              f_cbData              - Lentgth of pbData
**              f_pbSignatureBuffer   - Buffer in which to put the signature
**              f_cbSignatureBuffer   - Length of signature buffer
**              f_ibSignatureBuffer   - Index in buffer at which signature should
**                                      be put
**
** Returns :
**
** Notes :
**
******************************************************************************/
typedef DRM_RESULT ( DRM_CALL *DRM_XMRFORMAT_CB_SIGNATURE )(
    __in_opt                              const DRM_VOID                       *f_pvSignatureContext,
    __in_bcount( f_cbData )               const DRM_BYTE                       *f_pbData,
    __in                                  const DRM_DWORD                       f_cbData,
    __out_bcount( f_cbSignatureBuffer )         DRM_BYTE                       *f_pbSignatureBuffer,
    __in                                  const DRM_DWORD                       f_cbSignatureBuffer,
    __in                                  const DRM_DWORD                       f_ibSignatureBuffer );

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL DRM_XMRFORMAT_StartLicense(
    __inout                                     DRM_STACK_ALLOCATOR_CONTEXT    *f_pStack,
    __in_ecount( 1 )                      const DRM_ID                         *f_pidRights,
    __in                                        DRM_DWORD                       f_dwVersion,
    __in_ecount_opt( 1 )                  const DRM_XMRFORMAT                  *f_pRebindLicense,
    __inout_ecount( 1 )                         DRM_XMRFORMAT_BUILDER_CONTEXT  *f_pcontextBuilder ) DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL DRM_XMRFORMAT_FinishLicense (
    __in_ecount( 1 ) const                      DRM_XMRFORMAT_BUILDER_CONTEXT  *f_pcontextBuilder,
    __in_ecount_opt( 1 ) const                  DRM_VOID                       *f_pvSignatureContext,
    __in_opt                                    DRM_XMRFORMAT_CB_SIGNATURE      f_pfnSignature,
    __in                                        DRM_DWORD                       f_cbSignature,
    __in                                        DRM_WORD                        f_wSignatureType,
    __inout_ecount( 1 )                         DRM_DWORD                      *f_pcbLicense,
    __inout_bcount_part_opt( *f_pcbLicense, *f_pcbLicense )
                                                DRM_BYTE                       *f_pbLicense ) DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL DRM_XMRFORMAT_AllowPlaybackRights(
    __inout_ecount( 1 )                         DRM_XMRFORMAT_BUILDER_CONTEXT  *f_pcontextBuilder ) DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL DRM_XMRFORMAT_AllowCopyRights(
    __inout_ecount( 1 )                         DRM_XMRFORMAT_BUILDER_CONTEXT  *f_pcontextBuilder ) DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL DRM_XMRFORMAT_AddUnknownObject(
    __inout_ecount( 1 )                         DRM_XMRFORMAT_BUILDER_CONTEXT  *f_pcontextBuilder,
    __in                                        DRM_WORD                        f_wObjectType,
    __in                                        DRM_BOOL                        f_fDuplicatesAllowed,
    __in                                        DRM_WORD                        f_wParent,
    __in                                        DRM_WORD                        f_wFlags,
    __in                                        DRM_DWORD                       f_cbObject,
    __in_bcount_opt( f_cbObject )         const DRM_BYTE                       *f_pbObject ) DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL DRM_XMRFORMAT_SetSerialNumber(
    __inout                                     DRM_XMRFORMAT_BUILDER_CONTEXT  *f_pxmrBuilder,
    __in                                        DRM_DWORD                       f_cbSerialNumber,
    __in_bcount( f_cbSerialNumber )       const DRM_BYTE                       *f_pbSerialNumber ) DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL DRM_XMRFORMAT_SetSecurityLevel(
    __inout                                     DRM_XMRFORMAT_BUILDER_CONTEXT  *f_pxmrBuilder,
    __in                                        DRM_WORD                        f_wSecurityLevel ) DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL DRM_XMRFORMAT_SetGlobalRights(
    __inout                                     DRM_XMRFORMAT_BUILDER_CONTEXT  *f_pxmrBuilder,
    __in                                        DRM_WORD                        f_wRights ) DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL DRM_XMRFORMAT_SetExpirationDate(
    __inout                                     DRM_XMRFORMAT_BUILDER_CONTEXT  *f_pxmrBuilder,
    __in                                        DRM_DWORD                       f_dwBeginDate,
    __in                                        DRM_DWORD                       f_dwEndDate,
    __in                                        DRM_DWORD                       f_dwSubtractFromBeginDate,
    __in                                        DRM_DWORD                       f_dwAddToEndDate ) DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL DRM_XMRFORMAT_SetExpirationAfterFirstPlay(
    __inout                                     DRM_XMRFORMAT_BUILDER_CONTEXT  *f_pxmrBuilder,
    __in                                        DRM_DWORD                       f_dwExpirationAfterFirstPlay ) DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL DRM_XMRFORMAT_SetRealTimeExpiration(
    __inout                                     DRM_XMRFORMAT_BUILDER_CONTEXT  *f_pxmrBuilder,
    __in                                        DRM_BOOL                        f_fRealTimeExpiration ) DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL DRM_XMRFORMAT_SetRestrictedSourceID(
    __inout                                     DRM_XMRFORMAT_BUILDER_CONTEXT  *f_pxmrBuilder,
    __in                                        DRM_BOOL                        f_fRestrictedSourceID ) DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL DRM_XMRFORMAT_SetSourceID(
    __inout                                     DRM_XMRFORMAT_BUILDER_CONTEXT  *f_pxmrBuilder,
    __in                                        DRM_DWORD                       f_dwSourceID ) DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL DRM_XMRFORMAT_SetGracePeriod(
    __inout                                     DRM_XMRFORMAT_BUILDER_CONTEXT  *f_pxmrBuilder,
    __in                                        DRM_DWORD                       f_dwGracePeriod ) DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL DRM_XMRFORMAT_SetOutputProtectionLevels(
    __inout                                     DRM_XMRFORMAT_BUILDER_CONTEXT  *f_pxmrBuilder,
    __in                                        DRM_WORD                        f_wCompressedDigitalVideo,
    __in                                        DRM_WORD                        f_wUncompressedDigitalVideo,
    __in                                        DRM_WORD                        f_wAnalogVideo,
    __in                                        DRM_WORD                        f_wCompressedDigitalAudio,
    __in                                        DRM_WORD                        f_wUncompressedDigitalAudio ) DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL DRM_XMRFORMAT_AddExplicitOutputProtection(
    __inout                                     DRM_XMRFORMAT_BUILDER_CONTEXT  *f_pxmrBuilder,
    __in                                        DRM_WORD                        f_wXmrObjectType,
    __in_ecount( 1 )                      const DRM_GUID                       *f_pOutputProtectionID,
    __in                                        DRM_DWORD                       f_cbConfigData,
    __in_bcount_opt( f_cbConfigData )     const DRM_BYTE                       *f_pbConfigData ) DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL DRM_XMRFORMAT_AddPlayEnablers(
    __inout                                     DRM_XMRFORMAT_BUILDER_CONTEXT  *f_pxmrBuilder,
    __in                                        DRM_DWORD                       f_cPlayEnablers,
    __in_ecount( f_cPlayEnablers )        const DRM_GUID                       *f_pguidPlayEnablers ) DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL DRM_XMRFORMAT_AddCopyEnablers(
    __inout                                     DRM_XMRFORMAT_BUILDER_CONTEXT  *f_pxmrBuilder,
    __in                                        DRM_DWORD                       f_cCopyEnablers,
    __in_ecount( f_cCopyEnablers )        const DRM_GUID                       *f_pguidCopyEnablers ) DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL DRM_XMRFORMAT_SetDeviceKey(
    __inout                                     DRM_XMRFORMAT_BUILDER_CONTEXT  *f_pxmrBuilder,
    __in                                        DRM_WORD                        f_wEccCurveType,
    __in                                        DRM_DWORD                       f_cbPublicKey,
    __in_bcount( f_cbPublicKey )          const DRM_BYTE                       *f_pbPublicKey ) DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL DRM_XMRFORMAT_SetIssueDate(
    __inout                                     DRM_XMRFORMAT_BUILDER_CONTEXT  *f_pxmrBuilder,
    __in                                        DRM_DWORD                       f_dwIssueDate ) DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL DRM_XMRFORMAT_SetRevocationVersion(
    __inout                                     DRM_XMRFORMAT_BUILDER_CONTEXT  *f_pxmrBuilder,
    __in                                        DRM_DWORD                       f_dwVersion ) DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL DRM_XMRFORMAT_SetEmbeddedBehavior(
    __inout                                     DRM_XMRFORMAT_BUILDER_CONTEXT  *f_pxmrBuilder,
    __in                                        DRM_WORD                        f_wEmbeddedBehavior ) DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL DRM_XMRFORMAT_SetContentKey(
    __inout                                     DRM_XMRFORMAT_BUILDER_CONTEXT  *f_pxmrBuilder,
    __in                                        DRM_WORD                        f_wSymmetricCipherType,
    __in                                        DRM_WORD                        f_wEncryptionCipherType,
    __in_ecount( 1 )                      const DRM_ID                         *f_pKeyID,
    __in                                        DRM_DWORD                       f_cbEncryptedKey,
    __in_bcount( f_cbEncryptedKey )       const DRM_BYTE                       *f_pbEncryptedKey ) DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL DRM_XMRFORMAT_SetOptimizedContentKey(
    __inout                                     DRM_XMRFORMAT_BUILDER_CONTEXT  *f_pxmrBuilder,
    __in                                        DRM_WORD                        f_wEncryptionCipherType,
    __in                                        DRM_DWORD                       f_cbKeyData,
    __in_bcount( f_cbKeyData )            const DRM_BYTE                       *f_pbKeyData ) DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL DRM_XMRFORMAT_SetUplinkKID(
    __inout                                     DRM_XMRFORMAT_BUILDER_CONTEXT  *f_pxmrBuilder,
    __in_ecount( 1 )                      const DRM_ID                         *f_pKeyID,
    __in                                        DRM_DWORD                       f_cbCheckSum,
    __in_bcount( f_cbCheckSum )           const DRM_BYTE                       *f_pbCheckSum ) DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL DRM_XMRFORMAT_SetUplinkX(
    __inout                                     DRM_XMRFORMAT_BUILDER_CONTEXT  *f_pxmrBuilder,
    __in_ecount( 1 )                      const DRM_ID                         *f_pUplinkXID,
    __in                                        DRM_DWORD                       f_cbChecksum,
    __in_bcount( f_cbChecksum )           const DRM_BYTE                       *f_pbChecksum,
    __in                                        DRM_WORD                        f_cLocations,
    __in_ecount( f_cLocations )           const DRM_DWORD                      *f_pdwLocations ) DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL DRM_XMRFORMAT_SetAuxKeyEntries(
    __inout                                     DRM_XMRFORMAT_BUILDER_CONTEXT  *f_pxmrBuilder,
    __in                                        DRM_WORD                        f_cAuxEntries,
    __in_ecount( f_cAuxEntries )          const DRM_XMRFORMAT_AUX_KEY_ENTRY    *f_pAuxEntries ) DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL DRM_XMRFORMAT_SetMeteringID(
    __inout                                     DRM_XMRFORMAT_BUILDER_CONTEXT  *f_pxmrBuilder,
    __in_ecount( 1 )                      const DRM_ID                         *f_pMeteringID ) DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL DRM_XMRFORMAT_SetRemovalDate(
    __inout                                     DRM_XMRFORMAT_BUILDER_CONTEXT  *f_pxmrBuilder,
    __in                                        DRM_DWORD                       f_dwRemovalDate ) DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL DRM_XMRFORMAT_SetCopyCount(
    __inout                                     DRM_XMRFORMAT_BUILDER_CONTEXT  *f_pxmrBuilder,
    __in                                        DRM_DWORD                       f_dwCopyCount ) DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL DRM_XMRFORMAT_SetMoveEnabler(
    __inout                                     DRM_XMRFORMAT_BUILDER_CONTEXT  *f_pxmrBuilder,
    __in                                        DRM_DWORD                       f_dwMoveEnabler ) DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL DRM_XMRFORMAT_SetDomain(
    __inout                                     DRM_XMRFORMAT_BUILDER_CONTEXT  *f_pxmrBuilder,
    __in_ecount( 1 )                      const DRM_ID                         *f_pDomainID,
    __in                                        DRM_DWORD                       f_dwRevision ) DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL DRM_XMRFORMAT_AddPolicyMetadata(
     __inout                                    DRM_XMRFORMAT_BUILDER_CONTEXT  *f_pxmrBuilder,
     __in_ecount( 1 )                     const DRM_ID                         *f_pMetadataID,
     __in                                       DRM_DWORD                       f_cbMetadata,
     __in_bcount( f_cbMetadata )          const DRM_BYTE                       *f_pbMetadata ) DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL DRM_XMRFORMAT_CreateExtensibleRestriction(
    __in_ecount( 1 )                      const DRM_XMRFORMAT_BUILDER_CONTEXT  *f_pcontextBuilder,
    __in                                        DRM_WORD                        f_wPolicyLen,
    __in_bcount( f_wPolicyLen )           const DRM_BYTE                       *f_pbPolicy,
    __in                                        DRM_DWORD                       f_dwSecureDataSize,
    __in                                        DRM_BYTE                        f_bStateType,
    __in                                        DRM_WORD                        f_wStateLen,
    __in_bcount( f_wStateLen )                  DRM_VOID                       *f_pvState,
    __inout_ecount( 1 )                         DRM_DWORD                      *f_cbRestriction,
    __deref_inout_bcount( *f_cbRestriction )    DRM_BYTE                      **f_ppbRestriction ) DRM_NO_INLINE_ATTRIBUTE;


EXIT_PK_NAMESPACE;

#endif  /* __DRM_XMRFORMAT_BUILDER_H__ */
