/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/


#ifndef __OEMCIPHERTYPES_H__
#define __OEMCIPHERTYPES_H__

#include <oemcocktailtypes.h>
#include <oemaeskey.h>
#include <oemcommon.h>
#include <drmteetypes.h>

ENTER_PK_NAMESPACE;

typedef enum _DRM_SUPPORTED_CIPHERS
{
    eDRM_CIPHER_INVALID     = 0,
    eDRM_RC4_CIPHER         = 1,
    eDRM_AES_COUNTER_CIPHER = 2,
} DRM_SUPPORTED_CIPHERS;

typedef struct __tagDRM_AES_CIPHER_CONTEXT
{
    DRM_BYTE          rgbContentKey[DRM_AES_MAXKEYSIZE];
    DRM_AES_KEY       aesKey;
DRM_OBFUS_PTR_TOP
    OEM_CRYPTO_HANDLE hAesKey;
DRM_OBFUS_PTR_BTM
} DRM_AES_CIPHER_CONTEXT;

#define DRM_OPAQUE_CIPHER_CONTEXT_SIZE 176

typedef struct __tagDRM_OPAQUE_CIPHER_CONTEXT
{
    DRM_BYTE      rgbContentKey[DRM_OPAQUE_CIPHER_CONTEXT_SIZE];
} DRM_OPAQUE_CIPHER_CONTEXT;

typedef struct __tagDRM_CIPHER_CONTEXT_TEE
{
    DRM_TEE_BYTE_BLOB         oCDKB;
    DRM_TEE_BYTE_BLOB         oOEMKeyInfo;
} DRM_CIPHER_CONTEXT_TEE;

typedef struct __tagDRM_CIPHER_CONTEXT
{
    DRM_SUPPORTED_CIPHERS    eCipherType;
    DRM_LID                  oLID;
    DRM_BOOL                 fInited;
    DRM_DWORD                cbContentKey;
    DRM_BOOL                 fDisableClone;
    DRM_UINT64               ui64RealTimeExpiration;
    DRM_UINT64               ui64RealTimeExpirationTicsAfterFirstPlay;
    DRM_UINT64               ui64cbProcessed;
DRM_OBFUS_PTR_TOP
    DRM_VOID                *pOEMContext;
DRM_OBFUS_PTR_BTM
DRM_OBFUS_PTR_TOP
    DRM_VOID                *pOpaqueBbx;
DRM_OBFUS_PTR_BTM
    union _tagcipher
    {
        OEM_COCKTAIL_CONTEXT      cipherRC4;
        DRM_AES_CIPHER_CONTEXT    cipherAES;
        DRM_OPAQUE_CIPHER_CONTEXT cipherOPAQUE;
        DRM_CIPHER_CONTEXT_TEE    cipherTEE;
    } cipher;
    DRM_VOID                *pFuncTbl;
} DRM_CIPHER_CONTEXT;

EXIT_PK_NAMESPACE;

#endif /* __OEMCIPHERTYPES_H__ */

