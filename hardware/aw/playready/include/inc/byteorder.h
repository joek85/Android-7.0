/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#ifndef __BYTEORDER_H__
#define __BYTEORDER_H__

#include <oembyteorder.h>

#define COPY_FROMBUFFER(to, from, index, size, buffersize)          \
do {                                                                \
    DRM_DWORD __dwSpaceRequired=0;                                  \
    ChkDR(DRM_DWordAdd(index,size,&__dwSpaceRequired));             \
    ChkBOOL(__dwSpaceRequired<=(buffersize),DRM_E_BUFFERTOOSMALL);  \
    DRM_BYT_CopyBytes((DRM_BYTE*)(to),0,(from),(index),(size));     \
    (index)=(__dwSpaceRequired);                                    \
    } while( FALSE )

#if TARGET_LITTLE_ENDIAN
#define NETWORKBYTES_FROMBUFFER(to, from, index, size, buffersize)       do {COPY_FROMBUFFER(to,from,index,size,buffersize);DRM_BYT_ReverseBytes((DRM_BYTE*)(to),(size));} while( FALSE )
#else /* TARGET_LITTLE_ENDIAN */
#define NETWORKBYTES_FROMBUFFER(to, from, index, size, buffersize)       do {COPY_FROMBUFFER(to,from,index,size,buffersize);DRM_BYT_ReverseBytes((DRM_BYTE*)(to),(size));} while( FALSE )
#endif /* TARGET_LITTLE_ENDIAN */

#endif /* __BYTEORDER_H__ */

