/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <drmteetypes.h>
#include <oemteetypes.h>

#ifndef __DRMTEEPROXYSTRUCTURED_H__
#define __DRMTEEPROXYSTRUCTURED_H__ 1

PREFAST_PUSH_DISABLE_EXPLAINED( __WARNING_NONCONST_BUFFER_PARAM_25033, "Out params can't be const" )
PREFAST_PUSH_DISABLE_EXPLAINED( __WARNING_NONCONST_PARAM_25004, "Out params can't be const" )

ENTER_PK_NAMESPACE;

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL DRM_TEE_PROXY_StructuredMethodInvoke(
    __in_opt                                DRM_TEE_PROXY_CONTEXT       *f_pTeeContext,
    __in                                    DRM_DWORD                    f_dwFunctionMapOEMValue,
    __in_ecount( 1 )                        XB_DRM_TEE_PROXY_METHOD_REQ *f_pMethodRequest,
    __inout_ecount( 1 )                     XB_DRM_TEE_PROXY_METHOD_REQ *f_pMethodResponse,
    __in                                    DRM_DWORD                    f_cbMethodResponse,
    __out_bcount( f_cbMethodResponse )      DRM_BYTE                    *f_pbMethodResponse ) DRM_NO_INLINE_ATTRIBUTE;

EXIT_PK_NAMESPACE;

PREFAST_POP /* __WARNING_NONCONST_PARAM_25004 */
PREFAST_POP /* __WARNING_NONCONST_BUFFER_PARAM_25033 */

#endif /* __DRMTEEPROXYSTRUCTURED_H__ */
