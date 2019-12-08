/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#ifndef __DRM_BYTEMANIP_H__
#define __DRM_BYTEMANIP_H__

#include <drmcrt.h>
#include <byteorder.h>

ENTER_PK_NAMESPACE;

DRM_API_VOID DRM_VOID DRM_CALL DRM_BYT_ReverseBytes(
    __inout_bcount(cbData) DRM_BYTE  *pbData,
    __in                   DRM_DWORD  cbData );

#define DRM_BYT_CopyBytes(to,tooffset,from,fromoffset,count) DRMCRT_memcpy(&((to)[(tooffset)]),&((from)[(fromoffset)]),(count))
#define DRM_BYT_MoveBytes(to,tooffset,from,fromoffset,count) DRMCRT_memmove(&((to)[(tooffset)]),&((from)[(fromoffset)]),(count))
#define DRM_BYT_SetBytes(pb,ib,cb,b) DRMCRT_memset(&((pb)[(ib)]),b,cb)
#define DRM_BYT_CompareBytes(pbA,ibA,pbB,ibB,cb) DRMCRT_memcmp(&((pbA)[(ibA)]),&((pbB)[(ibB)]),(cb))

#define MEMSET(pb,ch,cb) DRM_BYT_SetBytes(((DRM_BYTE*)(pb)),0,(cb),(ch))
#define ZEROMEM(pb,cb)   DRM_BYT_SetBytes(((DRM_BYTE*)(pb)),0,(cb),0)
#define MEMCPY(pbTo,pbFrom,cb)  DRM_BYT_CopyBytes(   ((DRM_BYTE*)(pbTo)),0,((DRM_BYTE*)(pbFrom)),0,(cb))
#define MEMMOVE(pbTo,pbFrom,cb) DRM_BYT_MoveBytes(   ((DRM_BYTE*)(pbTo)),0,((DRM_BYTE*)(pbFrom)),0,(cb))
#define MEMCMP(pbA,pbB,cb)      DRMCRT_memcmp((pbA) ,(pbB), (cb))

#define DRM_ID_ARE_EQUAL(idA,idB) (0==DRMCRT_memcmp(&(idA),&(idB),sizeof(idA)))

EXIT_PK_NAMESPACE;

#endif /* __DRM_BYTEMANIP_H__ */

