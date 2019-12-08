/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#ifndef __DRM_TOOLS_ASF_PARSER_H__
#define __DRM_TOOLS_ASF_PARSER_H__

ENTER_PK_NAMESPACE;

/*******************************************************
** Holds info on which header objects have been found **
*******************************************************/
typedef struct _tagDRM_ASFHEADERFLAGS
{
    DRM_BOOL fFoundV1;
    DRM_BOOL fFoundV2;
    DRM_BOOL fFoundV4;
    DRM_BOOL fFoundHdrExt;
    DRM_BOOL fFoundFileProperties;
    DRM_BOOL fFoundDigitalSignature;
} DRM_ASFHEADERFLAGS;

#define EMPTY_DRM_ASFHEADERFLAGS  { FALSE, FALSE, FALSE, FALSE, FALSE, FALSE }

/*
** Holds information about an encrypted stream
*/
typedef struct _tagDRM_TOOLS_ASF_ENCRYPTED_STREAM
{
    DRM_BYTE    bStreamID;
    DRM_GUID    guidEncryptedStreamType;
    DRM_WORD    wFormatTag;
} DRM_TOOLS_ASF_ENCRYPTED_STREAM;

#define EMPTY_DRM_TOOLS_ASF_ENCRYPTED_STREAM {                        \
                                             0,                       \
                                             DRM_EMPTY_DRM_GUID,      \
                                             0 }

#define DRMTOOLS_MAX_ENCRYPTED_STREAMS 5

/*
** Holds all the info used by the asf parser
*/
typedef struct _tagDRM_TOOLS_ASF_CONTEXT
{
    DRM_BOOL                fVerbose;
    OEM_FILEHDL             hFileIn;
    DRM_UINT64              qwReadTotal;
    DRM_DWORD               cbDRMHeader;
    DRM_DWORD               ibDRMHeaderOffsetInFile;
    DRM_CSP_V1_HEADER_DATA  oV1Data;
    DRM_ASFHEADERFLAGS      oFoundObjects;
    DRM_GUID                guidCurObjectID;
    DRM_UINT64              qwCurObjSize;
    DRM_GUID                guidFileID;
    DRM_UINT64              qwFileSize;
    DRM_UINT64              qwPacketCount;
    DRM_DWORD               cbPacketSize;
    DRM_DWORD               dwNumEncryptedStreams;
    DRM_TOOLS_ASF_ENCRYPTED_STREAM rgoEncryptedStreams[ DRMTOOLS_MAX_ENCRYPTED_STREAMS ];
} DRM_TOOLS_ASF_CONTEXT;

#define EMPTY_DRM_TOOLS_ASF_CONTEXT {                            \
                                    FALSE,                       \
                                    OEM_INVALID_HANDLE_VALUE,    \
                                    DRM_UI64LITERAL( 0, 0 ),     \
                                    0,                           \
                                    0,                           \
                                    {NULL, 0, NULL, 0, NULL, 0}, \
                                    EMPTY_DRM_ASFHEADERFLAGS,    \
                                    DRM_EMPTY_DRM_GUID,          \
                                    DRM_UI64LITERAL( 0, 0 ),     \
                                    DRM_EMPTY_DRM_GUID,          \
                                    DRM_UI64LITERAL( 0, 0 ),     \
                                    DRM_UI64LITERAL( 0, 0 ),     \
                                    0,                           \
                                    0,                           \
                                    { EMPTY_DRM_TOOLS_ASF_ENCRYPTED_STREAM } }
/*****************************
** ASF Header Object Reader **
*****************************/
DRM_API DRM_RESULT DRM_CALL DRMTOOLS_OpenASFContext(
    __in                                OEM_FILEHDL            f_hFile,
    __out                               DRM_TOOLS_ASF_CONTEXT *f_poAsfContext,
    __out_bcount( f_cbDRMHeaderData )   DRM_BYTE              *f_pbDRMHeaderData,
    __in                                DRM_DWORD              f_cbDRMHeaderData );

DRM_API DRM_RESULT DRM_CALL DRMTOOLS_CloseASFContext(
    __inout DRM_TOOLS_ASF_CONTEXT *f_poAsfContext);

EXIT_PK_NAMESPACE;

#endif /* __DRM_TOOLS_ASF_PARSER_H__ */
