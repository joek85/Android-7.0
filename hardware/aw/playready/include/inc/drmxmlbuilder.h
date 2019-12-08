/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/


#ifndef __DRMXMLBUILDER_H__
#define __DRMXMLBUILDER_H__

#include <drmtypes.h>
#include <drmbase64.h>
#include <oemcryptoctx.h>
#include <oemsha1.h>

ENTER_PK_NAMESPACE;

#define CB_XMB_EXTRA              100
#define DRM_XMB_OVERHEAD          (sizeof(_XMBContext) + CB_XMB_EXTRA)

extern const DRM_DWORD g_cbXMLBuilderMinimum;

typedef struct _tagXMBContext
{
    /*
    ** Usage of buffer:
    ** This context buffer is allocated by user at CreateDocument(). Its size  can be reallocated
    ** via ReallocDocument(). The XML string is built on the way when each of the API calls are
    ** made. The XML string grow from top to bottom. On the way, all opened node's open tag
    ** position is kept and saved on the buffer from bottom to top. So when wNextStringPos
    ** and wNextOpenNodePos collides, NO_MEM is returned and user should reallocate more
    ** memory to finish the operation.
    ** Note: The open nodes positions will be saved as DWORDs, so it aligns perfectly with the
    ** growing XML string within the WCHAR buffer.
    */

    /* !!! Caller is ensuring only 2 byte alignment. Never add any member variable greater than 2 bytes*/

    DRM_BOOL fInited;
    DRM_BOOL fIsSimMode;        /* TRUE to create a faked doc to calculate the length needed */
    DRM_DWORD wMaxStackUsed;    /* Max stack space used for nested nodes */
    DRM_DWORD wSize;            /* size of context */
    DRM_DWORD wNextOpenNodePos;
    DRM_DWORD wNextStringPos;   /* next insertion point */
    DRM_DWORD wBuffSize;        /* size of buffer in WCHARs*/
    DRM_WCHAR XmlString [1];
} _XMBContext;




/* state to be used for save/restore XML context during API calls */
typedef struct _tagXMBState
{
    DRM_DWORD cwchMaxStackUsed;    /* Max stack space used for nested nodes */
    DRM_DWORD iwchNextOpenNodePos;
    DRM_DWORD iwchNextStringPos;   /* next insertion point */
} XMBState;


/*
*******************************************************************************
** API methods
*******************************************************************************
*/

DRM_API DRM_RESULT DRM_CALL DRM_XMB_SimulateCreateDocument(
    __in    const DRM_DWORD         cbXmlContext,
    __inout       _XMBContext      *pbXmlContext,
    __in    const DRM_CONST_STRING *pdstrRootNodeName);

DRM_API DRM_RESULT DRM_CALL DRM_XMB_CreateDocument(
    __in    const DRM_DWORD         cbXmlContext,
    __inout       _XMBContext      *pbXmlContext,
    __in    const DRM_CONST_STRING *pdstrRootNodeName);

DRM_API DRM_RESULT DRM_CALL DRM_XMB_ReallocDocument(
    __in          _XMBContext *pbOldXmlContext,
    __in    const DRM_DWORD    cbNewXmlContext,
    __inout       _XMBContext *pbNewXmlContext );

/**********************************************************************
** Function:    DRM_XMB_CloseDocument
** Synopsis:    Pop the opened nodes and close the document, if space allowed.
** Arguments:
** Returns:     DRM_SUCCESS on success
** Notes:
***********************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_XMB_CloseDocument(
    __inout _XMBContext *pbXmlContext,
    __out   DRM_STRING  *pdstrXML   /* if opened with DRM_XMB_SimulateCreateDocument, this contains the counts of WCHAR's needed for the real string */
    );

DRM_API DRM_RESULT DRM_CALL DRM_XMB_OpenNode(
    __inout       _XMBContext      *pbXmlContext,
    __in    const DRM_CONST_STRING *pdstrNodeName);

DRM_API DRM_RESULT DRM_CALL DRM_XMB_SaveState(
    __inout const _XMBContext *pbXmlContext,
    __out         XMBState    *pState );

DRM_API DRM_RESULT DRM_CALL DRM_XMB_RestoreState(
    __inout       _XMBContext *pbXmlContext,
    __in    const XMBState    *pState );


DRM_API DRM_RESULT DRM_CALL DRM_XMB_CloseCurrNode(
    __inout   _XMBContext *pbXmlContext,
    __out_opt DRM_STRING  *pdstrXMLFragment );

DRM_API DRM_RESULT DRM_CALL DRM_XMB_GetCurrNodeName(
    __in  _XMBContext *pbXmlContext,
    __out DRM_STRING  *pdstrNodeName);

DRM_API DRM_RESULT DRM_CALL DRM_XMB_GetContextSize(
    __in  const _XMBContext *pbXmlContext,
    __out       DRM_DWORD   *pcbXmlContext );

DRM_API DRM_RESULT DRM_CALL DRM_XMB_AddAttribute(
    __inout       _XMBContext      *pXmlContext,
    __in    const DRM_CONST_STRING *pdstrAttrName,
    __in    const DRM_CONST_STRING *pdstrAttrValue );

DRM_API DRM_RESULT DRM_CALL DRM_XMB_AddData(
    __inout       _XMBContext      *pXmlContext,
    __in    const DRM_CONST_STRING *pdstrData );

DRM_API DRM_RESULT DRM_CALL DRM_XMB_ReserveSpace(
    __inout                           _XMBContext *f_poXMBContext,
    __in                              DRM_DWORD    f_cchData,
    __deref_out_ecount_opt(f_cchData) DRM_WCHAR  **f_ppwchData );

DRM_API DRM_RESULT DRM_CALL DRM_XMB_AddCData(
    __inout       _XMBContext      *pXmlContext,
    __in    const DRM_CONST_STRING *pdstrCData );

/**********************************************************************
** Function:    DRM_XMB_AppendNode
** Synopsis:    Appends a caller provided node nested from current opened node
** Arguments:   [pdwStamp] --
** Returns:     DRM_SUCCESS on success
** Notes:       The caller is responsible for the validity of the appending node
***********************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_XMB_AppendNode(
    __inout       _XMBContext      *pXmlContext,
    __in    const DRM_CONST_STRING *pdstrXmlString);

/**********************************************************************
** Function:    DRM_XMB_AddXMLNode
** Synopsis:    Appends a caller provided node nested from current opened node
** Arguments:   [dstrName]  -- Name for node tag
**              [dstrValue] -- Context of the node for node tag
** Returns:     DRM_SUCCESS on success
** Notes:       The caller is responsible for the validity of the appending node
***********************************************************************/


DRM_API DRM_RESULT DRM_CALL DRM_XMB_AddXMLNode(
    __inout       _XMBContext        *pbXMLContext,
    __in    const DRM_CONST_STRING   *dstrName,
    __in    const DRM_CONST_STRING   *dstrValue );

/*******************************************************************
 *          PUBLIC MACRO DRM_XMB_RequiredCharsForTag
 *
 * purpose: calculates the number of XML characters required for
 *          the indicated tag, optional text, and optional
 *          attribute label and text.
 * note: <TAG></TAG> + attribute, optionally 2 quotes, space and equal sign
 ******************************************************************/

#define DRM_XMB_RequiredCharsForTag( cchTag, cchData, cchAttrLabel, cchAttrText) \
     ((cchTag)*2+5 \
     +(cchData) \
     +((cchAttrLabel)!=0?(cchAttrLabel)+4+(cchAttrText):0))

/*******************************************************************
 *          PUBLIC MACRO DRM_XMB_RequiredCharsForTagNoClose
 *
 * purpose: calculates the number of XML characters required for
 *          the indicated tag, optional text, and optional
 *          attribute label and text, without considering the closing
 *          tag.
 * note: <TAG> + optionally attribute, 2 quotes, space and equal sign
 ******************************************************************/
#define DRM_XMB_RequiredCharsForTagNoClose( cchTag, cchData, cchAttrLabel, cchAttrText) \
    ((cchTag)+2 \
    +(cchData) \
    +((cchAttrLabel)!=0?(cchAttrLabel)+4+(cchAttrText):0))

/*******************************************************************
 *          PUBLIC MACRO DRM_XMB_RequiredCharsForAttribute
 *
 * purpose: calculates the number of XML characters required for
 *          the an optional attribute label and text.
 * note: optionally attribute, 2 quotes, space and equal sign
 ******************************************************************/
#define DRM_XMB_RequiredCharsForAttribute( cchAttrLabel, cchAttrText) \
    ((cchAttrLabel)!=0?(cchAttrLabel)+4+(cchAttrText):0)

typedef enum _WriteTagType
{
    wttOpen,
    wttClosed
} WriteTagType;

/*********************************************************************
 *                 PUBLIC FUNCTION DRM_XMB_WriteTag
 *
 *      function: write one or both tags of an XML pair
 *                  and optionally the enclosed data and/or
 *                  a single attribute
 *
 *      if parameter wtt is wttClosed, the tag is closed and the
 *      XML builder insertion point remains at the same scope as
 *      when the function was called
 *
 *        <TAG[ attrlabel="attrtext"]>[data][</TAG>]
 *
 ********************************************************************/

DRM_API DRM_RESULT DRM_CALL DRM_XMB_WriteTag(
    __inout        _XMBContext      *pbDataOut,
    __in     const DRM_CONST_STRING *pdstrTag,
    __in_opt const DRM_CONST_STRING *pdstrData,
    __in_opt const DRM_CONST_STRING *pdstrAttrLabel,
    __in_opt const DRM_CONST_STRING *pdstrAttrText,
    __in           WriteTagType      wtt);

/*********************************************************************
 *             PUBLIC FUNCTION DRM_XMB_WriteCDATATag
 *
 * as above but writes a CDATA tag
 ********************************************************************/

DRM_API DRM_RESULT DRM_CALL DRM_XMB_WriteCDATATag(
    __inout        _XMBContext      *pbDataOut,
    __in     const DRM_CONST_STRING *pdstrTag,
    __in_opt const DRM_CONST_STRING *pdstrCDATA,
    __in_opt const DRM_CONST_STRING *pdstrAttrLabel,
    __in_opt const DRM_CONST_STRING *pdstrAttrText,
    __in           WriteTagType      wtt);

DRM_API DRM_RESULT DRM_CALL DRM_XMB_RemainingBuffer(
    __in  const _XMBContext *f_pbXMB,
    __out       DRM_DWORD   *f_pcbRemaining );

EXIT_PK_NAMESPACE;

#endif      /* __XMLBUILDER_H__ */


