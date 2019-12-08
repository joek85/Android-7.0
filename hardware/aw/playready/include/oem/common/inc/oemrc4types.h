/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#ifndef __OEMRC4TYPES_H__
#define __OEMRC4TYPES_H__

#include <drmerr.h>

#define RC4_TABLESIZE 256

ENTER_EXTERN_C_NAMESPACE;

/* Key structure */
typedef struct __tagRC4_KEYSTRUCT
{
    DRM_BYTE S[ RC4_TABLESIZE ];     /* State table */
    DRM_BYTE i, j;        /* Indices */
} RC4_KEYSTRUCT;

EXIT_EXTERN_C_NAMESPACE;

#endif /* __OEMRC4TYPES_H__ */

