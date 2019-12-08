/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#ifndef __DRM_MOVE_CONSTANTS_H
#define __DRM_MOVE_CONSTANTS_H

#include <oemeccp256.h>

ENTER_PK_NAMESPACE;

extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrTrue;
extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrMovMoveRootTag;
extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrMovMoveTag;
extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrMovTransactionTag;
extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrMovTransactionIdTag;
extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrMovMoveVersionTag;
extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrMovMoveChallenge1Tag;
extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrMovMoveChallenge2Tag;
extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrMovMoveDataTag;
extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrMovMoveLidTag;
extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrMovMoveCertificateChainTag;
extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrMovMoveRootAttribName;
extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrMovMoveRootAttribValue;
extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrMovMoveAttrib1Name;
extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrMovMoveAttrib1Value;
extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrMovMoveAttrib2Name;
extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrMovMoveAttrib2Value;
extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrMovMoveChallenge2AttribName;
extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrMovMoveChallenge2AttribValue;
extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrMovMoveCertificateChainAttribName;
extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrMovMoveCertificateChainAttribValue;
extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrMovMoveVersion;
extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrMovMoveNonceTag;
extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrMovCopyRootTag;
extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrMovCopyTag;
extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrMovCopyLidInfoTag;
extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrMovOriginalCopyCountTag;
extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrMovCopyVersionPath;
extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrMovCopyNoncePath;
extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrMovCopyTransactionIdPath;
extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrMovCopyAllowedPath;
extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrMovMoveDataPath;
extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrMovMoveRevisionPath;
extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrMovMoveNoncePath;
extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrMovMoveVersionPath;
extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrMovMoveAllowedPath;
extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrMovMovePageIndexPath;
extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrMovMoveCertificateChainPath;
extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrMovRefreshCurIndexTag;
extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrMovRefreshPageTag;
extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrMovRefreshRootTag;
extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrMovRefreshTag;
extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrMovRefreshVersionPath;
extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrMovRefreshNoncePath;
extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrMovRefreshPageIndexPath;
extern DRM_GLOBAL_CONST  DRM_ANSI_CONST_STRING g_dastrTagMovePage;

EXIT_PK_NAMESPACE;

#endif /* __DRM_MOVE_CONSTANTS_H */

