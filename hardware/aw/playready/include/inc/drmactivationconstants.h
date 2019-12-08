/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#ifndef __DRM_ACTIVATION_CONSTANTS_H__
#define __DRM_ACTIVATION_CONSTANTS_H__

ENTER_PK_NAMESPACE;

/*
** ---------------------------------------------------------------
** XML strings used in the construction of an activation challenge
** ---------------------------------------------------------------
*/

extern DRM_GLOBAL_CONST  DRM_EXPORT_VAR DRM_ANSI_CONST_STRING g_dastrActivationResponsePrimaryPayloadPath;
extern DRM_GLOBAL_CONST  DRM_EXPORT_VAR DRM_ANSI_CONST_STRING g_dastrActivationResponseAuxPayloadUrlPath;
extern DRM_GLOBAL_CONST  DRM_EXPORT_VAR DRM_ANSI_CONST_STRING g_dastrActivationResponseTag;
extern DRM_GLOBAL_CONST  DRM_EXPORT_VAR DRM_ANSI_CONST_STRING g_dastrActivationResultTag;
extern DRM_GLOBAL_CONST  DRM_EXPORT_VAR DRM_ANSI_CONST_STRING g_dastrauxilaryPayloadUrlTag;
extern DRM_GLOBAL_CONST  DRM_EXPORT_VAR DRM_ANSI_CONST_STRING g_dastrActivationRequestTag;
extern DRM_GLOBAL_CONST  DRM_EXPORT_VAR DRM_ANSI_CONST_STRING g_dastrActivationServiceNamespaceV1AttribValue;
extern DRM_GLOBAL_CONST  DRM_EXPORT_VAR DRM_ANSI_CONST_STRING g_dastrclientSdkVersionTag;
extern DRM_GLOBAL_CONST  DRM_EXPORT_VAR DRM_ANSI_CONST_STRING g_dastrclientSdkTypeTag;
extern DRM_GLOBAL_CONST  DRM_EXPORT_VAR DRM_ANSI_CONST_STRING g_dastrplatformTag;
extern DRM_GLOBAL_CONST  DRM_EXPORT_VAR DRM_ANSI_CONST_STRING g_dastrtidTag;
extern DRM_GLOBAL_CONST  DRM_EXPORT_VAR DRM_ANSI_CONST_STRING g_dastrpayloadTag;
extern DRM_GLOBAL_CONST  DRM_EXPORT_VAR DRM_ANSI_CONST_STRING g_dastrprimaryPayloadTag;
extern DRM_GLOBAL_CONST  DRM_EXPORT_VAR DRM_ANSI_CONST_STRING g_dastrauxilaryPayloadTag;
extern DRM_GLOBAL_CONST  DRM_EXPORT_VAR DRM_ANSI_CONST_STRING g_dastrwsdlServiceTag;
extern DRM_GLOBAL_CONST  DRM_EXPORT_VAR DRM_ANSI_CONST_STRING g_dastrnameAttribute;
extern DRM_GLOBAL_CONST  DRM_EXPORT_VAR DRM_ANSI_CONST_STRING g_dastrwsdlServiceNameAttributeValue;
extern DRM_GLOBAL_CONST  DRM_EXPORT_VAR DRM_ANSI_CONST_STRING g_dastrwsdlPortTag;
extern DRM_GLOBAL_CONST  DRM_EXPORT_VAR DRM_ANSI_CONST_STRING g_dastrwsdlPortNameAttributeValue;
extern DRM_GLOBAL_CONST  DRM_EXPORT_VAR DRM_ANSI_CONST_STRING g_dastrhttpAddressTag;
extern DRM_GLOBAL_CONST  DRM_EXPORT_VAR DRM_ANSI_CONST_STRING g_dastrlocationAttributeName;

EXIT_PK_NAMESPACE;

#endif /* __DRM_ACTIVATION_CONSTANTS_H__ */
