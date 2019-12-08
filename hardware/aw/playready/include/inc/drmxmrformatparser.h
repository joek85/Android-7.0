/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#ifndef __DRM_XMRFORMAT_PARSER_H__
#define __DRM_XMRFORMAT_PARSER_H__

#include <oemparsers.h>
#include <drmstkalloc.h>
#include <drmxmrformattypes.h>
#include <drmxmrformat_generated.h>

ENTER_PK_NAMESPACE;

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL DRM_XMRFORMAT_Parse(
    __in                              DRM_DWORD                    f_cbXMR,
    __in_bcount( f_cbXMR )      const DRM_BYTE                    *f_pbXMR,
    __inout                           DRM_STACK_ALLOCATOR_CONTEXT *f_pStack,
    __out_opt                         DRM_DWORD                   *f_pcbParsed,
    __out                             DRM_XMRFORMAT               *f_pXmrLicense ) DRM_NO_INLINE_ATTRIBUTE;

DRM_API DRM_RESULT DRM_CALL DRM_XMRFORMAT_RequiresSST(
    __in  const DRM_XMRFORMAT *f_pXmrLicense,
    __out       DRM_BOOL      *f_pfRequiresSST );


EXIT_PK_NAMESPACE;

#endif  /* __DRM_XMRFORMAT_PARSER_H__ */
