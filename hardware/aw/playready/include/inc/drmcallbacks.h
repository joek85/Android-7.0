/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#ifndef __DRM_CALLBACKS_H__
#define __DRM_CALLBACKS_H__

#include <drmnamespace.h>   /* defining proper namespace (if used) */
#include <drmcallbacktypes.h>
#include <drmtoken.h>

ENTER_PK_NAMESPACE;

typedef enum
{
    DRM_STORE_LICENSE_NOTIFY_KID            = 1, /* Callbacks with this reason will have pvData as a DRM_CONST_STRING */
    DRM_STORE_LICENSE_NOTIFY_RESERVED_2     = 2, /* Reserved - Not Supported. */
    DRM_STORE_LICENSE_NOTIFY_RESERVED_3     = 3, /* Reserved - Not Supported. */
    DRM_STORE_LICENSE_NOTIFY_RESERVED_4     = 4  /* Reserved - Not Supported. */

} DRM_STORE_LICENSE_CALLBACK_ENUM;

typedef DRM_RESULT (DRM_CALL *pfnLicenseResponseCallback)(
    __in DRM_VOID                        *pvData,
    __in DRM_STORE_LICENSE_CALLBACK_ENUM  eReason,
    __in DRM_VOID                        *pvContext );

typedef struct
{
    DRM_CONST_STRING *pdstrAttributeName;

    /* On input contains the token data that now exists on disk, on output the
    ** callback function should update it to the value that should be stored */
    TOKEN            *pOnDisk;

    /* Contains the data that was originally intended to be stored during Bind */
    TOKEN            *pOriginallyIntendedValue;

    /* If TRUE, indicates that during Bind the token didn't exist on disk
    ** so another instance must have created this attribute as well */
    DRM_BOOL          fNewAttribute;

    /*
    ** Right ID and restriction ID the secure store entry is associated with
    */
    DRM_WORD          wRightID;
    DRM_WORD          wRestrictionID;

} DRM_SECURE_STATE_TOKEN_RESOLVE_DATA;

EXIT_PK_NAMESPACE;

#endif
