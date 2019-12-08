/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#ifndef __OEMDEBUG_H__
#define __OEMDEBUG_H__

#include <drmnamespace.h>
#include <drmtypes.h>
#include <stdarg.h>

ENTER_PK_NAMESPACE;
#if DRM_BUILD_PROFILE == DRM_BUILD_PROFILE_ANDROID
typedef va_list DRM_VA_LIST;
#elif DRM_BUILD_PROFILE == DRM_BUILD_PROFILE_LINUX
typedef va_list DRM_VA_LIST;
#else
typedef DRM_VOID* DRM_VA_LIST;
#endif

typedef DRM_VOID ( DRM_CALL *Oem_Debug_Trace_Hook_Callback )(
    __in const DRM_CHAR     *f_pszFormat,
    __in       DRM_VA_LIST   f_ArgList );

typedef DRM_VOID ( DRM_CALL *Oem_Debug_Assert_Hook_Callback )(
    __in           DRM_BOOL  fAssert,
    __in_opt const DRM_CHAR *assertcmd,
    __in_opt const DRM_CHAR *file,
    __in           DRM_LONG  line );

DRM_API_VOID DRM_VOID DRM_CALL Oem_Debug_Trace(
    __in const DRM_CHAR *szFmt,
    ... );

DRM_API_VOID DRM_VOID DRM_CALL Oem_Debug_HookTrace(
    __in Oem_Debug_Trace_Hook_Callback pNewTraceFcn );

DRM_API_VOID DRM_VOID DRM_CALL Oem_Debug_Assert(
    __in           DRM_BOOL  fAssert,
    __in_opt const DRM_CHAR *assertcmd,
    __in_opt const DRM_CHAR *file,
    __in           DRM_LONG  line );

DRM_API_VOID DRM_VOID DRM_CALL Oem_Debug_HookAssert(
    __in Oem_Debug_Assert_Hook_Callback pNewAssertFcn );


EXIT_PK_NAMESPACE;

#endif /* __OEMDEBUG_H__ */




