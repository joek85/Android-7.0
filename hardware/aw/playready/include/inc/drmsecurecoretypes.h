/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#ifndef _DRMSECURECORETYPES_H_
#define _DRMSECURECORETYPES_H_ 1

#include <drmkeyfilecerttypes.h>
#include <drmteetypes.h>
#include <drmblackboxtypes.h>
#include <oemteetypes.h>

ENTER_PK_NAMESPACE;

typedef DRM_RESULT( DRM_CALL* DRM_SECURECORE_TEE_DATA_STORE_PASSWORD_CALLBACK )(
    __inout                               DRM_VOID    *f_pvUserCtx,
    __in                                  DRM_BOOL     f_fRead,
    __in                                  DRM_DWORD    f_cbToSign,
    __in_bcount( f_cbToSign )       const DRM_BYTE    *f_pbToSign,
    __inout_bcount( DRM_SHA1_DIGEST_LEN ) DRM_BYTE    *f_pbPasswordSST );

/*
** Maximum number of nonces to be stored in memory in a single session
*/
#define DRM_MAX_NONCE_COUNT_PER_SESSION ((DRM_DWORD)(100))

typedef struct __tagDRM_NKB_DATA
{
    DRM_TEE_BYTE_BLOB     rgNKBs[DRM_MAX_NONCE_COUNT_PER_SESSION];
    DRM_ID                rgNonces[DRM_MAX_NONCE_COUNT_PER_SESSION];
    DRM_DWORD             cNonces;
} DRM_NKB_DATA;

typedef struct __tagDRM_SECURECORE_TEE_DATA
{
    DRM_TEE_PROXY_CONTEXT *pTeeCtx;
    DRM_TEE_BYTE_BLOB      oCertificate;
    DRM_TEE_BYTE_BLOB      oPPKB;
    DRM_TEE_BYTE_BLOB      oRKB;
    DRM_TEE_BYTE_BLOB      oSPKB;
    DRM_NKB_DATA           oNKBData;
    DRM_RESULT             drError;     /* Hold error codes returned from the TEE that will trigger reprovisioning */

    DRM_SECURECORE_TEE_DATA_STORE_PASSWORD_CALLBACK     pfnStorePasswordCallback;
} DRM_SECURECORE_TEE_DATA;

typedef struct __tagDRM_SECURECORE_INTERNAL_DATA
{
    DRM_VOID         *pOpaqueKeyFileContext;
    DRM_VOID         *pOpaqueBlackBoxContext;
    DRM_VOID         *pOpaqueKeyHandleCallbacks;
} DRM_SECURECORE_INTERNAL_DATA;

typedef struct __tagDRM_SECURECORE_CONTEXT
{
    union
    {
        DRM_SECURECORE_TEE_DATA             *pSecureCoreTeeData;
        DRM_SECURECORE_INTERNAL_DATA        *pSecureCoreInternalData;
    } uData;

    DRM_VOID                                *pOEMContext;
    DRM_CLIENT_INFO                         *pClientInfo;
    DRM_CRYPTO_CONTEXT                      *pCryptoContext;
    DRM_BINARY_DEVICE_CERT_CACHED_VALUES    *pCertCache;
    DRM_VOID                                *pFuncTbl;
    DRM_BOOL                                 fIsRunningInHWDRM;
    DRM_BOOL                                 fDisableBlobCache;
} DRM_SECURECORE_CONTEXT;

#define DRM_SECURECORE_DisableBlobCache( __pSecureCoreCtx ) do { (__pSecureCoreCtx)->fDisableBlobCache = TRUE; } while(FALSE)

EXIT_PK_NAMESPACE;

#endif /* _DRMSECURECORETYPES_H_ */

