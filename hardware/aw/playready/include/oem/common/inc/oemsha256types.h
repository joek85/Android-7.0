/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#ifndef __OEMSHA256TYPES_H__
#define __OEMSHA256TYPES_H__ 1

#include <drmtypes.h>

ENTER_PK_NAMESPACE;

/*
** Sha-256 defines
*/

#define SHA256_DIGEST_SIZE_IN_BYTES  32
#define SHA256_STATE_SIZE_IN_BYTES   256
#define SHA256_STATE_SIZE_IN_DWORDS  8
#define SHA256_STATE_SIZE_IN_INT64S  4
#define SHA256_BLOCK_SIZE_IN_BYTES   64
#define SHA256_BLOCK_SIZE_IN_DWORDS  16
#define SHA256_NUM_ITERATIONS        64
#define SHA256_NUM_FRONT_ITERATIONS  16

#ifndef DRM_SHA1_DIGEST_LEN
#define DRM_SHA1_DIGEST_LEN 20
#endif  /* DRM_SHA1_DIGEST_LEN */

/*
** Structs
*/
typedef struct __tagSHA256_CTX
{
    union
    {
#if DRM_64BIT_TARGET
        DRM_UINT64 m_rgu64State64[ SHA256_STATE_SIZE_IN_INT64S ]; /* force alignment */
#endif
        DRM_DWORD  m_rgdwState[ SHA256_STATE_SIZE_IN_DWORDS ];   /* state (ABCDEFGH) */
    } m_rgUnion; /* end union */

    DRM_DWORD m_rgdwCount[ 2 ]; /* number of bytes, msb first */
    DRM_DWORD m_rgdwBuffer[SHA256_BLOCK_SIZE_IN_DWORDS]; /* input buffer */
} DRM_SHA256_CONTEXT;

typedef struct __tagSHA256_Digest
{
    DRM_BYTE m_rgbDigest[ SHA256_DIGEST_SIZE_IN_BYTES ];
} DRM_SHA256_Digest;

typedef struct __tagDRM_SHA256_HMAC_CONTEXT
{
    DRM_BOOL           fInitialized;
DRM_OBFUS_FILL_BYTES(4)
    DRM_SHA256_CONTEXT shaContext;
    DRM_BYTE           shaDigest[SHA256_DIGEST_SIZE_IN_BYTES];
    DRM_BYTE           rgbBuffer[SHA256_BLOCK_SIZE_IN_BYTES];
} DRM_SHA256_HMAC_CONTEXT;

EXIT_PK_NAMESPACE;

#endif /* #ifndef __OEMSHA256TYPES_H__ */

