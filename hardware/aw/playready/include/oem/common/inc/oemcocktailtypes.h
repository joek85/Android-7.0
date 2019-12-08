/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#ifndef __OEMCOCKTAILTYPES_H__
#define __OEMCOCKTAILTYPES_H__

#include <drmerr.h>
#include <oemdes.h>
#include <oemrc4types.h>
#include <oemcbc.h>
#include <oemsha1.h>
#include <oemaescommon.h>

ENTER_PK_NAMESPACE;

#define DRMCIPHERKEYLEN_RC4        7

/*
** Legacy PDRM content acquired through ILA will
** contain a 6-byte Cocktail content key. This is
** only valid for XMR licenses.
*/
#define DRMCIPHERKEYLEN_RC4_PDRM   6

#define DRM_CPHR_CB_FOR_INITDECRYPT 15

#define DRMCIPHERKEYLEN_MAX DRM_MAX( DRM_AES_MAXKEYSIZE, DRMCIPHERKEYLEN_RC4 )

/* Constant indicating the number of times SHA1 is used to get the hash of key. */
#define SHA_ITERATIONS      5
/* CHECKSUM_LENGTH indicates the number of bytes in final hash value to use as CHECKSUM. */
#define CHECKSUM_LENGTH     7

typedef struct __tagOEM_COCKTAIL_CONTEXT
{
    DRM_BOOL      fInited;
    DRM_BYTE      shaOut [DRM_SHA1_DIGEST_LEN];
    DRM_DWORD     desS1  [2];
    DRM_DWORD     desS2  [2];
    DESTable      destable;
    DRM_CBCKey    mackey;
    DRM_CBCKey    invmackey;
    RC4_KEYSTRUCT rc4ks;
    DRM_CBCState  cbcstate;
    DRM_DWORD     rc4key[2];
    DRM_DWORD     rguiLast8[2];
    DRM_DWORD     cbPacket;
    DRM_DWORD     iPacket;
    DRM_BOOL      fDecryptInited;
} OEM_COCKTAIL_CONTEXT;

EXIT_PK_NAMESPACE;

#endif /* __OEMCOCKTAILTYPES_H__ */

