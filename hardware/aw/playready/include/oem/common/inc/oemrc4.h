/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#ifndef __OEMRC4_H__
#define __OEMRC4_H__

#include <drmerr.h>
#include <oemrc4types.h>

ENTER_EXTERN_C_NAMESPACE;

DRM_API_VOID DRM_VOID DRM_CALL DRM_RC4_KeySetupImpl(
    __out      RC4_KEYSTRUCT  *pKS,
    __in       DRM_DWORD       cbKey,
    __in const DRM_BYTE       *pbKey );

DRM_API_VOID DRM_VOID DRM_CALL DRM_RC4_CipherImpl(
    __inout                  RC4_KEYSTRUCT *pKS,
    __in                     DRM_DWORD      cbBuffer,
    __inout_bcount(cbBuffer) DRM_BYTE      *pbBuffer );

EXIT_EXTERN_C_NAMESPACE;

ENTER_PK_NAMESPACE;

DRM_API_VOID DRM_VOID DRM_CALL DRM_RC4_ZeroKey(
    __inout           RC4_KEYSTRUCT    *pKS );

#if !DRM_INLINING_SUPPORTED

DRM_API_VOID DRM_VOID DRM_CALL DRM_RC4_KeySetup(
    __out      RC4_KEYSTRUCT  *pKS,
    __in       DRM_DWORD       cbKey,
    __in const DRM_BYTE       *pbKey );

DRM_API_VOID DRM_VOID DRM_CALL DRM_RC4_Cipher(
    __inout                  RC4_KEYSTRUCT *pKS,
    __in                     DRM_DWORD      cbBuffer,
    __inout_bcount(cbBuffer) DRM_BYTE      *pbBuffer );

#else   /* !DRM_INLINING_SUPPORTED */

/*********************************************************************
**
**  Function:  DRM_RC4_KeySetup
**
**  Synopsis:  Generate the key control structure.  Key can be any size.
**
**  Arguments:
**     [pKS] -- A KEYSTRUCT structure that will be initialized.
**     [cbKey] -- Size of the key, in bytes.
**     [pbKey] -- Pointer to the key.
**
**  Returns:  None
**
*********************************************************************/

DRM_ALWAYS_INLINE DRM_API_VOID DRM_VOID DRM_CALL DRM_RC4_KeySetup(
    __out      RC4_KEYSTRUCT  *pKS,
    __in       DRM_DWORD       cbKey,
    __in const DRM_BYTE       *pbKey )
{
    DRM_RC4_KeySetupImpl( pKS, cbKey, pbKey );
}

/*********************************************************************
**
**  Function:  DRM_RC4_Cipher
**
**  Synopsis:
**
**  Arguments:
**     [pKS] -- Pointer to the KEYSTRUCT created using DRM_RC4_KeySetup.
**     [cbBuffer] -- Size of buffer, in bytes.
**     [pbBuffer] -- Buffer to be encrypted in place.
**
**  Returns:  None
*********************************************************************/

DRM_ALWAYS_INLINE DRM_API_VOID DRM_VOID DRM_CALL DRM_RC4_Cipher(
    __inout                  RC4_KEYSTRUCT *pKS,
    __in                     DRM_DWORD      cbBuffer,
    __inout_bcount(cbBuffer) DRM_BYTE      *pbBuffer )
{
    DRM_RC4_CipherImpl( pKS, cbBuffer, pbBuffer );
}

#endif   /* !DRM_INLINING_SUPPORTED */

EXIT_PK_NAMESPACE;

#endif /* __OEMRC4_H__ */

