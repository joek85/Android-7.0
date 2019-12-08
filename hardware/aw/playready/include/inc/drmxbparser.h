/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#ifndef __DRM_XB_PARSER_H__
#define __DRM_XB_PARSER_H__

#include <drmxb.h>
#include <drmstkalloctypes.h>

ENTER_PK_NAMESPACE;


DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL DRM_XB_UnpackBinary(
    __in_bcount( f_cb )       const DRM_BYTE                    *f_pb,
    __in                            DRM_DWORD                    f_cb,
    __inout                         DRM_STACK_ALLOCATOR_CONTEXT *f_pStack,
    __in_ecount( f_cformat )  const DRM_XB_FORMAT_DESCRIPTION   *f_pformat,
    __in                            DRM_DWORD                    f_cformat,
    __out_opt                       DRM_DWORD                   *f_pdwVersionFound,
    __out_opt                       DRM_DWORD                   *f_pcbParsed,
    __inout                         DRM_VOID                    *f_pStruct ) DRM_NO_INLINE_ATTRIBUTE;


DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL DRM_XB_FindObject(
    __in                            DRM_WORD                     f_wObjectType,
    __in_ecount( 1 )          const DRM_XB_FORMAT_DESCRIPTION   *f_pformat,
    __in_bcount( f_cb )       const DRM_BYTE                    *f_pb,
    __in                            DRM_DWORD                    f_cb,
    __out                           DRM_DWORD                   *f_piObject,
    __out                           DRM_DWORD                   *f_pcbObject ) DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL DRM_XB_UnpackHeader(
    __in_bcount( f_cb )       const DRM_BYTE                    *f_pb,
    __in                            DRM_DWORD                    f_cb,
    __inout                         DRM_STACK_ALLOCATOR_CONTEXT *f_pStack,
    __in_ecount( f_cformat )  const DRM_XB_FORMAT_DESCRIPTION   *f_pformat,
    __in                            DRM_DWORD                    f_cformat,
    __out_opt                       DRM_DWORD                   *f_pdwVersionFound,
    __out_opt                       DRM_DWORD                   *f_pcbParsed,
    __out_opt                       DRM_DWORD                   *f_pcbContainer,
    __out_opt                       DRM_DWORD                   *f_piFormat,
    __inout                         DRM_VOID                    *f_pStruct ) DRM_NO_INLINE_ATTRIBUTE;

EXIT_PK_NAMESPACE;

#endif  /* __DRM_XB_PARSER_H__ */

