/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#ifndef __DRM_TOOLS_INIT_H__
#define __DRM_TOOLS_INIT_H__ 1

#include <drmmanager.h>

ENTER_PK_NAMESPACE;

#define TOOLS_DRM_BUFFER_SIZE MINIMUM_APPCONTEXT_OPAQUE_BUFFER_SIZE

DRM_API DRM_RESULT DRM_CALL DRMTOOLS_DrmInitializeWithOpaqueBuffer(
    __in_opt                               DRM_VOID         *f_pOEMContext,
    __in                             const DRM_CONST_STRING *f_pdstrDeviceStoreName,
    __in                                   DRM_DWORD         f_cbOpaqueBuffer,
    __deref_out_bcount( f_cbOpaqueBuffer ) DRM_BYTE        **f_ppbOpaqueBuffer,
    __out                                  DRM_APP_CONTEXT **f_ppAppContext );

DRM_API_VOID DRM_VOID DRM_CALL DRMTOOLS_DrmUninitializeWithOpaqueBuffer(
    __out   DRM_BYTE        **f_ppbOpaqueBuffer,
    __out   DRM_APP_CONTEXT **f_ppAppContext );

EXIT_PK_NAMESPACE;

#endif /* end #ifndef __DRM_TOOLS_INIT_H__ */

