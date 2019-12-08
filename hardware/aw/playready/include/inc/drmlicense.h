/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/


#ifndef __DRMLICENSE_H__
#define __DRMLICENSE_H__

#include <drmtypes.h>
#include <oemcryptoctx.h>

ENTER_PK_NAMESPACE;

#define DRM_MAX_LICENSESIZE           36864                        /* 36 * 1024 == 36K max license size */
#define MAX_DEVICE_CERT_SIZE          36864                        /* 36 * 1024 == 36K max device cert size */
#define MAX_DEVICE_CERT_TEMPLATE_SIZE ( ( MAX_DEVICE_CERT_SIZE * 6 ) / 10 )/* 60% of max devcert */

#define DRM_AES_CK_CHECKSUM_LENGTH 8

#define LEGACY_KID_B64ENC_LEN   24
#define DRM_MAX_CHECKSUM        DRM_MAX( DRM_MAX( CHECKSUM_LENGTH, LEGACY_KID_B64ENC_LEN ), DRM_AES_CK_CHECKSUM_LENGTH )

typedef struct __tagDRM_V1Header
{
    DRM_BYTE  *pbKeyID;
    DRM_DWORD  cbKeyID;
    DRM_BYTE  *pbSecretData;
    DRM_DWORD  cbSecretData;
    DRM_BYTE  *pbURL;
    DRM_DWORD  cbURL;
    DRM_BYTE   bBuffer[1];
} DRM_V1Header;

EXIT_PK_NAMESPACE;

#endif /* __DRMLICENSE_H__ */

