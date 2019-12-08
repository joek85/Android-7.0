/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#ifndef __DRM_XB_BUILDER_H__
#define __DRM_XB_BUILDER_H__ 1

#include <drmxb.h>
#include <drmstkalloctypes.h>

ENTER_PK_NAMESPACE;

typedef struct __tagDRM_XB_BUILDER_NODE
{
    DRM_WORD     wType;
    DRM_DWORD    cbLength;
    /*
    ** This can either be an object or a linked list of
    ** builder nodes depending on whether this is a
    ** leaf node or not
    */
    const DRM_VOID *pvObject;
} DRM_XB_BUILDER_NODE;

typedef struct __tagDRM_XB_BUILDER_LISTNODE
{
    DRM_XB_BUILDER_NODE                  Node;
    struct __tagDRM_XB_BUILDER_LISTNODE *pNext;
} DRM_XB_BUILDER_LISTNODE;

typedef struct __tagDRM_XB_BUILDER_UNKNOWNOBJECT_LISTNODE
{
    DRM_XB_BUILDER_LISTNODE                            listNode;
    DRM_BOOL                                           fDuplicateAllowed;
    DRM_WORD                                           wFlags;
    DRM_WORD                                           wParent;
    struct __tagDRM_XB_BUILDER_UNKNOWNOBJECT_LISTNODE *pNext;
} DRM_XB_BUILDER_UNKNOWNOBJECT_LISTNODE;

typedef struct __tagDRM_XB_BUILDER_CONTEXT_INTERNAL
{
    DRM_DWORD                                 dwVersion;

    /*  In case of nodes which can have duplicates, the array below will hold the
    **  pointer to the last node of that type. In those cases, it is just used
    **  to keep track of the types of nodes added
    */
    DRM_XB_BUILDER_NODE                    **rgpObjectNodes;
    DRM_XB_BUILDER_UNKNOWNOBJECT_LISTNODE   *pUnknownObjects;
    DRM_STACK_ALLOCATOR_CONTEXT              oStack;
    DRM_STACK_ALLOCATOR_CONTEXT             *pcontextStack;
    const DRM_XB_FORMAT_DESCRIPTION         *pformat;

#if DRM_OBFUS_NEED_PADDING
    DRM_VOID *m_rgpvWM7Padding[4];
#endif /* DRM_OBFUS_NEED_PADDING */
} DRM_XB_BUILDER_CONTEXT_INTERNAL;

#define DRM_XB_BUILDER_CONTEXT_BUFFER_SIZE sizeof( DRM_XB_BUILDER_CONTEXT_INTERNAL )

typedef struct __tagDRM_XB_BUILDER_CONTEXT
{
    /*
    ** This data is Opaque.  Do not set any value in it.
    */
    DRM_BYTE rgbOpaqueBuffer[ DRM_XB_BUILDER_CONTEXT_BUFFER_SIZE ];
} DRM_XB_BUILDER_CONTEXT;

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL DRM_XB_StartFormatFromStack(
    __inout                                     DRM_STACK_ALLOCATOR_CONTEXT *f_pStack,
    __in                                        DRM_DWORD                    f_dwVersion,
    __inout_ecount( 1 )                         DRM_XB_BUILDER_CONTEXT      *f_pcontextBuilder,
    __in                                  const DRM_XB_FORMAT_DESCRIPTION   *f_pformat ) DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL DRM_XB_StartFormat(
    __inout_bcount( f_cbStack )                 DRM_BYTE                    *f_pbStack,
    __in                                        DRM_DWORD                    f_cbStack,
    __in                                        DRM_DWORD                    f_dwVersion,
    __inout_ecount( 1 )                         DRM_XB_BUILDER_CONTEXT      *f_pcontextBuilder,
    __in                                  const DRM_XB_FORMAT_DESCRIPTION   *f_pformat ) DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL DRM_XB_FinishFormat(
    __in_ecount( 1 )                      const DRM_XB_BUILDER_CONTEXT    *f_pcontextBuilder,
    __inout_bcount_opt( *f_pcbLicense )         DRM_BYTE                  *f_pbLicense,
    __inout_ecount( 1 )                         DRM_DWORD                 *f_pcbLicense ) DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL DRM_XB_SetExtendedHeader(
    __inout_ecount( 1 )                         DRM_XB_BUILDER_CONTEXT    *f_pcontextBuilder,
    __in_ecount( 1 )                      const DRM_VOID                  *f_pvObject ) DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL DRM_XB_AddEntry(
    __inout_ecount( 1 )                         DRM_XB_BUILDER_CONTEXT    *f_contextBuilder,
    __in                                        DRM_WORD                   f_wObjectType,
    __in_ecount( 1 )                      const DRM_VOID                  *f_pvObject ) DRM_NO_INLINE_ATTRIBUTE;

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL  DRM_XB_AddUnknownHierarchy(
    __inout_ecount( 1 )                         DRM_XB_BUILDER_CONTEXT_INTERNAL
                                                                          *f_pcontextBuilder,
    __inout_ecount( 1 )                         DRM_XB_BUILDER_UNKNOWNOBJECT_LISTNODE
                                                                          *f_plistnode ) DRM_NO_INLINE_ATTRIBUTE;
DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL DRM_XB_AddUnknownObject(
    __inout_ecount( 1 )                         DRM_XB_BUILDER_CONTEXT    *f_pcontextBuilder,
    __in                                        DRM_WORD                   f_wObjectType,
    __in                                        DRM_BOOL                   f_fDuplicateAllowed,
    __in                                        DRM_WORD                   f_wParent,
    __in                                        DRM_WORD                   f_wFlags,
    __in                                        DRM_DWORD                  f_cbObject,
    __in_bcount_opt( f_cbObject )         const DRM_BYTE                  *f_pbObject ) DRM_NO_INLINE_ATTRIBUTE;

EXIT_PK_NAMESPACE;

#endif  /* __DRM_XB_BUILDER_H__ */

