/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#ifndef _DRMKEYFILECERTTYPES_H_
#define _DRMKEYFILECERTTYPES_H_

#include <oemcommon.h>

ENTER_PK_NAMESPACE;

typedef enum DRM_KF_CERT_TYPE
{
    eKF_CERT_TYPE_INVALID,
    eKF_CERT_TYPE_NDT,
    eKF_CERT_TYPE_WMDRM,
    eKF_CERT_TYPE_PLAYREADY,
} DRM_KF_CERT_TYPE;

EXIT_PK_NAMESPACE;

#endif /* _DRMKEYFILECERTTYPES_H_ */

