/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#ifndef __DRM_TOOLS_MEDIA_FILE_H__
#define __DRM_TOOLS_MEDIA_FILE_H__ 1

#include <drmtoolsasfparser.h>

ENTER_PK_NAMESPACE;

typedef enum _tageDRM_MEDIA_FILE_TYPE
{
    eDRM_MEDIA_FILE_TYPE_UNKNOWN  = 0,
    eDRM_MEDIA_FILE_TYPE_ASF      = 1,
    eDRM_MEDIA_FILE_TYPE_ENVELOPE = 2
} eDRM_MEDIA_FILE_TYPE;

typedef struct _tagDRM_MEDIA_FILE_CONTEXT
{
    DRM_BOOL             fOpened;
    DRM_BOOL             fReadOnly;
    eDRM_MEDIA_FILE_TYPE eFileType;
    DRM_CONST_STRING     dstrFileName;
    DRM_BYTE             rgbDRMHeaderData[ DRM_STANDARD_PLAYREADYOBJSIZE ];
    DRM_DWORD            cbDRMHeaderData;
    DRM_DWORD            ibDRMHeaderOffsetInFile;  /* Offset into the file where the DRM Header is */
    DRM_DECRYPT_CONTEXT  oDecryptContext;
    union
    {
        DRM_ENVELOPED_FILE_CONTEXT oEnvFile;
        DRM_TOOLS_ASF_CONTEXT      oAsfFile;
    } FormatContext;
} DRM_MEDIA_FILE_CONTEXT;

#define EMPTY_DRM_MEDIA_FILE_CONTEXT {                                \
                                        FALSE,                        \
                                        FALSE,                        \
                                        eDRM_MEDIA_FILE_TYPE_UNKNOWN, \
                                        DRM_EMPTY_DRM_STRING,         \
                                        { 0 },                        \
                                        0,                            \
                                        { 0 },                        \
                                        { 0 } }

DRM_API DRM_RESULT DRM_CALL DRMTOOLS_OpenMediaFile(
    __in    const DRM_CONST_STRING       *f_dstrFileName,
    __inout       DRM_MEDIA_FILE_CONTEXT *f_poMediaFile );

DRM_API DRM_RESULT DRM_CALL DRMTOOLS_CloseMediaFile(
    __inout DRM_MEDIA_FILE_CONTEXT *f_poMediaFile );

DRM_API DRM_RESULT DRM_CALL DRMTOOLS_SaveContentHeaderToFile(
    __in const DRM_MEDIA_FILE_CONTEXT *f_poMediaFile,
    __in const DRM_CONST_STRING       *f_dstrOutFileName );

DRM_API DRM_RESULT DRM_CALL DRMTOOLS_UpdatePlayReadyObject(
    __in DRM_MEDIA_FILE_CONTEXT *f_poMediaFile );

EXIT_PK_NAMESPACE;

#endif /* __DRM_TOOLS_MEDIA_FILE_H__ */
