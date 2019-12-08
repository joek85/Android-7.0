/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/


#ifndef __DRMDOMAINKEYXMRPARSERTYPES_H__
#define __DRMDOMAINKEYXMRPARSERTYPES_H__

#include <drmtypes.h>

ENTER_PK_NAMESPACE;

#define DRM_DOMKEYXMR_HEADER_CONSTANT 0x444b4559 /* DKEY */
#define DRM_DOMKEYXMR_HEADER_VERSION  1

#define DRM_DOMKEYXMR_SESSIONKEY_ENCTYPE_ECCP256 1

#define DRM_DOMKEYXMR_OBJTYPE_PRIVKEYCONTAINER  0x500
#define DRM_DOMKEYXMR_OBJTYPE_ACCOUNTID         0x501
#define DRM_DOMKEYXMR_OBJTYPE_NONCE             0x502
#define DRM_DOMKEYXMR_OBJTYPE_SESSIOINKEY       0x503
#define DRM_DOMKEYXMR_OBJTYPE_PRIVKEY           0x504

#define DRM_DOMKEYXMR_PRIVKEYOBJ_MIN_LENGTH 20

#define DRM_DOMKEYXMR_PRIVKEY_TYPE_ECCP256         2
#define DRM_DOMKEYXMR_PRIVKEY_ENCTYPE_AESCTR       2
#define DRM_DOMKEYXMR_PRIVKEY_ENCTYPE_MIXED_AESECB 3

/* Matches value in drmdomainconstants.h */
#define DRM_DOMAIN_JOIN_PROTOCOL_VERSION 2

/*
** XMR strucutre for DomainKeys data
*/
typedef struct _tagDRM_DOMKEYXMR_HEADER
{
    DRM_DWORD  dwConstant;
    DRM_DWORD  dwVersion;
    DRM_DWORD  dwLength;
    DRM_DWORD  dwSignedLength;
} DRM_DOMKEYXMR_HEADER;

typedef DRM_OBFUS_FIXED_ALIGN struct _tagDRM_DOMKEYXMR_SESSIONKEY
{
    DRM_WORD   wFlags;
    DRM_WORD   wType;
    DRM_DWORD  dwLength;
    DRM_WORD   wKeyType;
    DRM_WORD   wEncryptionType;
    DRM_DWORD  dwKeyLength;
DRM_OBFUS_PTR_TOP
    DRM_BYTE  *pbData;
DRM_OBFUS_PTR_BTM
} DRM_DOMKEYXMR_SESSIONKEY;

typedef struct _tagDRM_DOMKEYXMR_PRIVKEYCONTAINER
{
    DRM_WORD  wFlags;
    DRM_WORD  wType;
    DRM_DWORD dwLength;
} DRM_DOMKEYXMR_PRIVKEYCONTAINER;

typedef DRM_OBFUS_FIXED_ALIGN struct _tagDRM_DOMKEYXMR_PRIVKEY
{
    DRM_WORD  wFlags;
    DRM_WORD  wType;
    DRM_DWORD dwLength;
    DRM_DWORD dwRevision;
    DRM_WORD  wKeyType;
    DRM_WORD  wEncryptionType;
    DRM_DWORD dwKeyLength;
DRM_OBFUS_FILL_BYTES(4)
DRM_OBFUS_PTR_TOP
    DRM_BYTE *pbData;
DRM_OBFUS_PTR_BTM
} DRM_DOMKEYXMR_PRIVKEY;

EXIT_PK_NAMESPACE;

#endif /* __DRMDOMAINKEYXMRPARSERTYPES_H__ */

