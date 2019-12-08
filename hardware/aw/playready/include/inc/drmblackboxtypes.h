/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#ifndef __DRMBLACKBOXTYPES_H__
#define __DRMBLACKBOXTYPES_H__

#include <drmlicense.h>
#include <drmdomainstoretypes.h>
#include <drmbcertcachetypes.h>
#include <drmxmrformattypes.h>

#include <oemcommon.h>
#include <oemeccp256.h>
#include <oemcryptoctx.h>
#include <oemcocktailtypes.h>

ENTER_PK_NAMESPACE;

typedef DRM_OBFUS_FIXED_ALIGN struct __tagDRM_CLIENT_INFO
{
    DRM_ANSI_STRING m_dastrClientVersion;
} DRM_CLIENT_INFO;

#define DRM_HDS_DOMAIN_KEY_RECORD_MAX_SIZE  ( sizeof(DRM_DWORD) + ECC_P256_CIPHERTEXT_SIZE_IN_BYTES + ( 2 * ( sizeof(PUBKEY_P256) + sizeof(PRIVKEY_P256) ) ) )

/*
** Contains checksum data
*/
typedef DRM_OBFUS_FIXED_ALIGN struct __tagDRM_BB_CHECKSUM_CONTEXT
{
    DRM_BOOL   m_fIgnoreChecksum;
    DRM_BYTE   m_rgbChecksumData[DRM_MAX_CHECKSUM];
    DRM_DWORD  m_cbChecksum;  /* Should be 20 bytes for normal checksum, 24 bytes for V1 header checksum */
    DRM_KID    m_KID;
DRM_OBFUS_PTR_TOP
    DRM_BYTE  *m_pbV1KID;     /* If not NULL then use the old V1 checksum algorithm */
DRM_OBFUS_PTR_BTM
} DRM_BB_CHECKSUM_CONTEXT;

typedef DRM_OBFUS_FIXED_ALIGN struct __tagDRM_XMR_BINDING_INFO
{
DRM_OBFUS_PTR_TOP
    DRM_XMRFORMAT_CONTENT_KEY      *m_pxmrContentKey;
DRM_OBFUS_PTR_BTM
DRM_OBFUS_PTR_TOP
    DRM_XMRFORMAT_DEVICE_KEY       *m_pxmrDeviceEccKey;
DRM_OBFUS_PTR_BTM
DRM_OBFUS_PTR_TOP
    DRM_XMRFORMAT_SIGNATURE        *m_pxmrSignature;
DRM_OBFUS_PTR_BTM
DRM_OBFUS_PTR_TOP
    DRM_XMRFORMAT_DOMAIN_ID        *m_pxmrDomainID;
DRM_OBFUS_PTR_BTM
DRM_OBFUS_PTR_TOP
    DRM_XMRFORMAT                  *m_pxmrLicense;    /* An HMAC signature of this license will be calculated and compared to m_pbSignature */
DRM_OBFUS_PTR_BTM


DRM_OBFUS_PTR_TOP
    DRM_DOMAINSTORE_CONTEXT        *m_pDomainStoreContext;
DRM_OBFUS_PTR_BTM
    DRM_DWORD                       m_cbDomainKeyRecord;
    DRM_BYTE                        m_rgbDomainKeyRecord[DRM_HDS_DOMAIN_KEY_RECORD_MAX_SIZE];
} DRM_XMR_BINDING_INFO;

typedef DRM_OBFUS_FIXED_ALIGN struct __tagDRM_BINDING_INFO
{
    DRM_BB_CHECKSUM_CONTEXT m_oChecksum;   /* Contains Checksum data */

    union
    {
        DRM_XMR_BINDING_INFO m_XMR;
    } m_BindingInfo;
} DRM_BINDING_INFO;

EXIT_PK_NAMESPACE;

#endif /* __DRMBLACKBOXTYPES_H__ */

