/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#ifndef __DRM_TOOLS_NETIO_H
#define __DRM_TOOLS_NETIO_H

#include <drmtypes.h>

ENTER_PK_NAMESPACE;

typedef enum __tageDRM_TOOLS_NET_ACTION {
    eDRM_TOOLS_NET_UNKNOWN               = 0,
    eDRM_TOOLS_NET_LEGACY                = 1,
    eDRM_TOOLS_NET_LICGET                = 2,
    eDRM_TOOLS_NET_LICACK                = 4,
    eDRM_TOOLS_NET_DOMLEAVE              = 8,
    eDRM_TOOLS_NET_DOMJOIN               = 16,
    eDRM_TOOLS_NET_DOWNLOAD              = 32,
    eDRM_TOOLS_NET_METERCERT             = 64,
    eDRM_TOOLS_NET_METERDATA             = 128,
    eDRM_TOOLS_NET_SERVERMETERING        = 256,
    eDRM_TOOLS_NET_ACTIVATIONGET         = 512,
    eDRM_TOOLS_NET_ACTIVATIONPOST        = 1024,
    eDRM_TOOLS_NET_PROVISIONPOST         = 2048,
    eDRM_TOOLS_NET_BOOTSTRAP             = 4096,
    eDRM_TOOLS_NET_LICGET_LEGACY         = eDRM_TOOLS_NET_LICGET         | eDRM_TOOLS_NET_LEGACY,
    eDRM_TOOLS_NET_METERCERT_LEGACY      = eDRM_TOOLS_NET_METERCERT      | eDRM_TOOLS_NET_LEGACY,
    eDRM_TOOLS_NET_METERDATA_LEGACY      = eDRM_TOOLS_NET_METERDATA      | eDRM_TOOLS_NET_LEGACY,
    eDRM_TOOLS_NET_SERVERMETERING_LEGACY = eDRM_TOOLS_NET_SERVERMETERING | eDRM_TOOLS_NET_LEGACY
} eDRM_TOOLS_NET_ACTION;


DRM_API DRM_BOOL DRM_CALL DRM_TOOLS_NETIO_IsNetIOSupported();
DRM_API DRM_BOOL DRM_CALL DRM_TOOLS_NETIO_IsNetIOUnsupported();

PREFAST_PUSH_DISABLE_EXPLAINED(__WARNING_NONCONST_BUFFER_PARAM_25033, "Function signature must match across various implementations");
DRM_RESULT DRM_CALL DRM_TOOLS_NETIO_SendData(
    __in_z                                    const DRM_CHAR              *f_pszUrl,
    __in                                            eDRM_TOOLS_NET_ACTION  f_eAction,
    __in_bcount( f_cbChallenge )              const DRM_BYTE              *f_pbChallenge,
    __in                                            DRM_DWORD              f_cbChallenge,
    __deref_out_bcount( *f_pcbResponse )            DRM_BYTE             **f_ppbResponse,
    __out_ecount( 1 )                               DRM_DWORD             *f_pcbResponse,
    __out_bcount_opt( *f_pcchServerBlobOut )        DRM_CHAR             **f_ppchServerBlobOut,
    __out_opt                                       DRM_DWORD             *f_pcchServerBlobOut );
PREFAST_POP /* Function signature must match across various implementations */

EXIT_PK_NAMESPACE;

#endif /* __DRM_TOOLS_NETIO_H */
