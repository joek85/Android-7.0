/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#ifndef __DRM_REVOCATIONSTORE_H__
#define __DRM_REVOCATIONSTORE_H__

#include <drmdatastoretypes.h>
#include <drmsecurestoretypes.h>
#include <drmsecurecoretypes.h>

ENTER_PK_NAMESPACE;

typedef struct _tag_RevocationStoreContext
{
    DRM_DST                      *pDatastore;
    DRM_SECSTORE_CONTEXT         *pContextSST;
    DRM_SECURECORE_CONTEXT       *pSecureCoreCtx;
    DRM_BOOL                      fInitedPassword;
    DRM_BYTE                      rgbPassword[ DRM_SHA1_DIGEST_LEN ];
} _RevocationStoreContext;

typedef struct __tagDRM_REVOCATIONSTORE_CONTEXT{
    DRM_BYTE rgbOpaqueBuffer[ sizeof( _RevocationStoreContext ) ];
} DRM_REVOCATIONSTORE_CONTEXT;

/**********************************************************************
**
** Function:    DRM_RVS_InitRevocationStore
**
** Synopsis:    Initializes a revocation store context
**
** Arguments:   [f_pDatastore]         -- DST context
**              [f_pContextSST]        -- secure store context
**              [f_pContextBB]         -- blackbox context
**              [f_pContextRev]        -- revocation store context
**
** Returns:     DRM_SUCCESS on success or failed with,
**              DRM_E_INVALIDARG if a parameter is incorrect or
**              other errors found in drmresults.h
**
** Notes:
**
**********************************************************************/

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL DRM_RVS_InitRevocationStore(
    __in  const DRM_DST                      *f_pDatastore,
    __in  const DRM_SECSTORE_CONTEXT         *f_pContextSST,
    __in  const DRM_SECURECORE_CONTEXT       *f_pSecureCoreCtx,
    __out       DRM_REVOCATIONSTORE_CONTEXT  *f_pContextRev );

/**********************************************************************
**
** Function:    DRM_RVS_StoreRevocationData
**
** Synopsis:    Stores data in the revocation store
**
** Arguments:   [f_pContextRev]        -- revocation store context
**              [f_pRevGUID]           -- GUID of data to store
**              [f_pbRevocationData]   -- pointer to data to store
**              [f_cbRevocationData]   -- size of data to store
**
** Returns:     DRM_SUCCESS on success or failed with,
**              DRM_E_INVALIDARG if a parameter is incorrect or
**              other errors found in drmresults.h
**
** Notes:
**
**********************************************************************/

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL DRM_RVS_StoreRevocationData(
    __inout                               DRM_REVOCATIONSTORE_CONTEXT *f_pContextRev,
    __in                            const DRM_GUID                    *f_pRevGUID,
    __in_bcount(f_cbRevocationData) const DRM_BYTE                    *f_pbRevocationData,
    __in                            const DRM_DWORD                    f_cbRevocationData );

/**********************************************************************
**
** Function:    DRM_RVS_GetRevocationData
**
** Synopsis:    Retrieves data from the revocation store
**
** Arguments:   [f_pContextRev]        -- revocation store context
**              [f_pRevGUID]           -- GUID of data to retrieve
**              [f_pbRevocationData]   -- pointer to buffer to hold returned data
**              [f_cbRevocationData]   -- size of buffer
**
** Returns:     DRM_SUCCESS on success or failed with,
**              DRM_E_INVALIDARG if a parameter is incorrect or
**              other errors found in drmresults.h
**
** Notes:
**
**********************************************************************/

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL DRM_RVS_GetRevocationData(
    __inout                                            DRM_REVOCATIONSTORE_CONTEXT *f_pContextRev,
    __in                                         const DRM_GUID                    *f_pRevGUID,
    __out_bcount_opt( *f_pcbRevocationData )           DRM_BYTE                    *f_pbRevocationData,
    __inout                                            DRM_DWORD                   *f_pcbRevocationData );

DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL DRM_RVS_LoopkupRevocationLIDFromGUID(
    __in  const DRM_GUID  *f_pRevGUID,
    __out const DRM_ID   **f_pLID );

EXIT_PK_NAMESPACE;

#endif  /* __DRM_REVOCATIONSTORE_H__ */


