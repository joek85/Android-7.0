/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#ifndef __DRM_CALLBACKTYPES_H__
#define __DRM_CALLBACKTYPES_H__

#include <drmerr.h>
#include <drmoutputleveltypes.h>
#include <drmxmrformattypes.h>

ENTER_PK_NAMESPACE;

typedef enum
{
    DRM_PLAY_OPL_CALLBACK                       = 0x1,  /* DRM_PLAY_OPL_EX2                                 */
    RESERVED_CALLBACK2                          = 0x2,  /* Never called, enum value maintained for compat   */
    RESERVED_CALLBACK3                          = 0x3,  /* Never called, enum value maintained for compat   */
    DRM_EXTENDED_RESTRICTION_CONDITION_CALLBACK = 0x4,  /* DRM_EXTENDED_RESTRICTION_CALLBACK_STRUCT         */
    DRM_EXTENDED_RESTRICTION_ACTION_CALLBACK    = 0x5,  /* DRM_EXTENDED_RESTRICTION_CALLBACK_STRUCT         */
    DRM_EXTENDED_RESTRICTION_QUERY_CALLBACK     = 0x6,  /* DRM_EXTENDED_RESTRICTION_CALLBACK_STRUCT         */
    DRM_SECURE_STATE_TOKEN_RESOLVE_CALLBACK     = 0x7,  /* DRM_SECURE_STATE_TOKEN_RESOLVE_DATA              */
    DRM_RESTRICTED_SOURCEID_CALLBACK            = 0x8,  /* DRM_RESTRICTED_SOURCEID_CALLBACK_STRUCT          */
    DRM_OEM_KEY_INFO_CALLBACK                   = 0x9,  /* DRM_OEM_KEY_INFO_CALLBACK_STRUCT                 */
} DRM_POLICY_CALLBACK_TYPE;

typedef DRM_RESULT (DRM_CALL* DRMPFNPOLICYCALLBACK)(
    __in     const DRM_VOID                 *f_pvCallbackData,
    __in           DRM_POLICY_CALLBACK_TYPE  f_dwCallbackType,
    __in_opt const DRM_KID                  *f_pKID,            /* KID that is being enumerated, i.e. the KID of the leaf-most license in a chain.  Will be NULL for callbacks not dealing with a license. */
    __in_opt const DRM_LID                  *f_pLID,            /* LID of the actual license being called upon, i.e. may be leaf or root license in a chain.  Will be NULL for callbacks not dealing with a license. */
    __in     const DRM_VOID                 *f_pv );            /* Void pointer to opaque data passed in alongside the DRMPFNPOLICYCALLBACK parameter which is then passed to the callback, e.g. in Drm_Reader_Bind */

typedef struct
{
    DRM_WORD                             wRightID;
    DRM_XB_UNKNOWN_OBJECT               *pRestriction;
    DRM_XMRFORMAT                       *pXMRLicense;
    DRM_VOID                            *pContextSST;   /* DRM_SECSTORE_CONTEXT */
} DRM_EXTENDED_RESTRICTION_CALLBACK_STRUCT;

typedef struct
{
    DRM_DWORD dwSourceID; /* global requirement, no need to pass right ID */
} DRM_RESTRICTED_SOURCEID_CALLBACK_STRUCT;

typedef struct
{
          DRM_DWORD  cb;
    const DRM_BYTE  *pb;
} DRM_OEM_KEY_INFO_CALLBACK_STRUCT;

EXIT_PK_NAMESPACE;

#endif /* __DRMCALLBACKTYPES_H__ */
