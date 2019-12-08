/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#ifndef __DRMDOMAINKEYXMRPARSER_H__
#define __DRMDOMAINKEYXMRPARSER_H__

#include <drmdomainkeyxmrparsertypes.h>

ENTER_PK_NAMESPACE;

DRM_API DRM_RESULT DRM_CALL DRM_DOMKEYXMR_GetHeader(
   __in_bcount(f_cbData) const DRM_BYTE              *f_pbData,
   __in                        DRM_DWORD              f_cbData,
   __inout_ecount(1)           DRM_DWORD             *f_pbOffset,
   __inout_ecount(1)           DRM_DOMKEYXMR_HEADER  *f_poHeader );

DRM_API DRM_RESULT DRM_CALL DRM_DOMKEYXMR_GetSessionKey(
   __in_bcount(f_cbData) DRM_BYTE  *f_pbData,
   __in                  DRM_DWORD  f_cbData,
   __inout_ecount(1)     DRM_DWORD *f_pbOffset,
   __inout_ecount(1)     DRM_DOMKEYXMR_SESSIONKEY  *f_poKeyObj);

DRM_API DRM_RESULT DRM_CALL DRM_DOMKEYXMR_GetPrivKeyContainer(
   __in_bcount(f_cbData) const DRM_BYTE                        *f_pbData,
   __in                        DRM_DWORD                        f_cbData,
   __inout_ecount(1)           DRM_DWORD                       *f_pbOffset,
   __inout_ecount(1)           DRM_DOMKEYXMR_PRIVKEYCONTAINER  *f_poKeyContainer );

DRM_API DRM_RESULT DRM_CALL DRM_DOMKEYXMR_GetPrivkey(
   __in_bcount(f_cbData) DRM_BYTE  *f_pbData,
   __in                  DRM_DWORD  f_cbData,
   __inout_ecount(1)     DRM_DWORD *f_pbOffset,
   __inout_ecount(1)     DRM_DOMKEYXMR_PRIVKEY *f_poKeyObj);

EXIT_PK_NAMESPACE;

#endif /* __DRMDOMAINKEYXMRPARSER_H__ */

