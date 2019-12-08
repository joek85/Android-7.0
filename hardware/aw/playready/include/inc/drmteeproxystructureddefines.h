/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <drmteetypes.h>
#include <oemteetypes.h>

#ifndef __DRMTEEPROXYSTRUCTUREDDEFINES_H__
#define __DRMTEEPROXYSTRUCTUREDDEFINES_H__ 1

ENTER_PK_NAMESPACE;

#define DRM_TEE_PROXY_STRUCTURED_SERIALIZER_VERSION 1

#define DRM_TEE_PROXY_STRUCTURED_MARKER() XB_DEFINE_FOUR_BYTE_FORMAT_ID('P','R','X','Y')

typedef enum __tagDRM_TEE_PROXY_PARAM_TYPE
{
    DRM_TEE_PROXY_TYPE_CONTEXT     = 1,
    DRM_TEE_PROXY_TYPE_DWORD       = 2,
    DRM_TEE_PROXY_TYPE_QWORD       = 3,
    DRM_TEE_PROXY_TYPE_ID          = 4,
    DRM_TEE_PROXY_TYPE_BLOB        = 5,
    DRM_TEE_PROXY_TYPE_DWORDLIST   = 6,
} DRM_TEE_PROXY_PARAM_TYPE;

/* 
** The serialized message has a header with the following data
**      DRM_DWORD dwMarker;     // This is a marker used to indicate this buffer is a valid serialized buffer.
**      DRM_DWORD dwVersion;    // The version of the serializer that created the message buffer.
**      DRM_DWORD dwMethodID;   // The OEM defined method ID of the PRITEE method invocation.
**      DRM_DWORD dwResult;     // This will hold the result of the PRITEE method invocation.
**      DRM_DWORD cParamters;   // This is the count of parameters included in the response/request message.
**      DRM_DWORD dwReserved;   // This is used for internal use.
*/
#define DRM_TEE_PROXY_METHOD_REQUEST_HEADER_SIZE       (sizeof(DRM_DWORD) * 6)
#define DRM_TEE_PROXY_METHOD_REQUEST__MARKER_OFFSET    (sizeof(DRM_DWORD) * 0)
#define DRM_TEE_PROXY_METHOD_REQUEST__VERSION_OFFSET   (sizeof(DRM_DWORD) * 1)
#define DRM_TEE_PROXY_METHOD_REQUEST__METHOD_ID_OFFSET (sizeof(DRM_DWORD) * 2)
#define DRM_TEE_PROXY_METHOD_REQUEST__RESULT_OFFSET    (sizeof(DRM_DWORD) * 3)
#define DRM_TEE_PROXY_METHOD_REQUEST__CPARAMS_OFFSET   (sizeof(DRM_DWORD) * 4)
#define DRM_TEE_PROXY_METHOD_REQUEST__RESERVED_OFFSET  (sizeof(DRM_DWORD) * 5)

/*
** All serialized parameter types have the following structure.  The TEE Context and Blob parameters will have 
** additional data at the end of the message.  This data is referenced by the ibData/cbData fields.
**
**   struct
**   {
**      DRM_DWORD  dwParamType;    // The type of paramter used.
**      DRM_DWORD  dwParamIdx;     // The index of the parameter in the PRITE method call.
**
**      union
**      {
**          DRM_DWORD dwValue;
**          DRM_QWORD qwValue;
**          DRM_ID    idValue;
**
**          struct
**          {
**              DRM_DWORD cbData;
**              DRM_DWORD ibData;
**          } oTeeCtx;
**
**          struct
**          {
**              DRM_DWORD dwType;
**              DRM_DWORD dwSubType;
**              DRM_DWORD cbData;
**              DRM_DWORD ibData;
**          } oBlob;
**
**          struct
**          {
**              DRM_DWORD cdwData;
**              DRM_DWORD ibData;
**              DRM_DWORD dwFlags;
**          } oDwordList;
**    
**      } oData;
**   } DRM_TEE_PROXY_STRUCTURE_PARAMETER;
*/
#define DRM_TEE_PROXY_PARAMETER_TYPE_SIZE                   (sizeof(DRM_DWORD) * 6)
#define DRM_TEE_PROXY_PARAMETER_TYPE__TYPE_OFFSET           (sizeof(DRM_DWORD) * 0)
#define DRM_TEE_PROXY_PARAMETER_TYPE__PARAMINDEX_OFFSET     (sizeof(DRM_DWORD) * 1)
#define DRM_TEE_PROXY_PARAMETER_TYPE__DWVALUE_OFFSET        (sizeof(DRM_DWORD) * 2)
#define DRM_TEE_PROXY_PARAMETER_TYPE__QWVALUE_OFFSET        (sizeof(DRM_DWORD) * 2)
#define DRM_TEE_PROXY_PARAMETER_TYPE__IDVALUE_OFFSET        (sizeof(DRM_DWORD) * 2)
#define DRM_TEE_PROXY_PARAMETER_TYPE__TEECTX_CB_OFFSET      (sizeof(DRM_DWORD) * 2)
#define DRM_TEE_PROXY_PARAMETER_TYPE__TEECTX_IB_OFFSET      (sizeof(DRM_DWORD) * 3)
#define DRM_TEE_PROXY_PARAMETER_TYPE__BLOB_TYPE_OFFSET      (sizeof(DRM_DWORD) * 2)
#define DRM_TEE_PROXY_PARAMETER_TYPE__BLOB_SUBTYPE_OFFSET   (sizeof(DRM_DWORD) * 3)
#define DRM_TEE_PROXY_PARAMETER_TYPE__BLOB_CB_OFFSET        (sizeof(DRM_DWORD) * 4)
#define DRM_TEE_PROXY_PARAMETER_TYPE__BLOB_IB_OFFSET        (sizeof(DRM_DWORD) * 5)
#define DRM_TEE_PROXY_PARAMETER_TYPE__DWORDLIST_CDW_OFFSET  (sizeof(DRM_DWORD) * 2)
#define DRM_TEE_PROXY_PARAMETER_TYPE__DWORDLIST_IB_OFFSET   (sizeof(DRM_DWORD) * 3)
#define DRM_TEE_PROXY_PARAMETER_TYPE__DWORDLIST_FLAG_OFFSET (sizeof(DRM_DWORD) * 4)

typedef enum __tagDRM_TEE_PROXY_DWORDLIST_FLAG
{
    DRM_TEE_PROXY_DWORDLIST_FLAG_NONE      = 0,
    DRM_TEE_PROXY_DWORDLIST_FLAG_BIGENDIAN = 1,
} DRM_TEE_PROXY_DWORDLIST_FLAG;

#define DRM_TEE_PROXY_IS_VALID_MESSAGE( cb, pb )                                                                         \
    do {                                                                                                                 \
        DRM_DWORD _dwTmp = 0;                                                                                            \
        ChkBOOL( (pb) != NULL, DRM_E_TEE_PROXY_INVALID_SERIALIZATION_MESSAGE );                                          \
        ChkBOOL( (cb) > DRM_TEE_PROXY_METHOD_REQUEST_HEADER_SIZE, DRM_E_TEE_PROXY_INVALID_SERIALIZATION_MESSAGE );       \
        NETWORKBYTES_TO_DWORD( _dwTmp, (pb), DRM_TEE_PROXY_METHOD_REQUEST__MARKER_OFFSET);                               \
        ChkBOOL( _dwTmp == DRM_TEE_PROXY_STRUCTURED_MARKER(), DRM_E_TEE_PROXY_INVALID_SERIALIZATION_MESSAGE );           \
        NETWORKBYTES_TO_DWORD( _dwTmp, (pb), DRM_TEE_PROXY_METHOD_REQUEST__VERSION_OFFSET);                              \
        ChkBOOL( _dwTmp == DRM_TEE_PROXY_STRUCTURED_SERIALIZER_VERSION, DRM_E_TEE_PROXY_INVALID_SERIALIZATION_MESSAGE ); \
    } while( FALSE )

EXIT_PK_NAMESPACE;

#endif /* __DRMTEEPROXYSTRUCTUREDDEFINES_H__ */
