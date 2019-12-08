/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#ifndef _DRMSECURESTOP_H_
#define _DRMSECURESTOP_H_ 1

#include <oemciphertypes.h>
#include <drmbytemanip.h>
#include <drmsecurecoretypes.h>
#include <drmsecurestoretypes.h>
#include <drmsecurestoptypes.h>

ENTER_PK_NAMESPACE;

/*********************************************************************
**
** Function: DRM_SECURESTOP_Initialize
**
** Synopsis: This function initializes a secure store context for
**           secure stop.
**
** Arguments:
**
** [f_pSecureCore]          -- Pointer to a secure core context.
** [f_pDatastore]           -- Pointer to an initialized data store.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL DRM_SECURESTOP_Initialize(
    __inout                                 DRM_SECURECORE_CONTEXT     *f_pSecureCore,
    __in                                    DRM_DST                    *f_pDatastore );

/*********************************************************************
**
** Function: DRM_SECURESTOP_CreateSession
**
** Synopsis: This function creates a new secure stop session.
**
** Arguments:
**
** [f_pSecureCore]          -- Pointer to a secure core context.
** [f_pDatastore]           -- Pointer to an initialized data store.
** [f_pidSession]           -- The ID used to identify this session.
** [f_pidPublisher]         -- The ID of the application publisher.
** [f_cidLicenses]          -- The count of license ID's.
** [f_pidLicenses]          -- The ID's of the licenses.
** [f_pSession]             -- The secure stop session.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL DRM_SECURESTOP_CreateSession(
    __inout                                 DRM_SECURECORE_CONTEXT     *f_pSecureCore,
    __in                                    DRM_DST                    *f_pDatastore,
    __in_opt                          const DRM_ID                     *f_pidSession,
    __in                              const DRM_ID                     *f_pidPublisher,
    __in                              const DRM_DWORD                   f_cidLicenses,
    __in_ecount_opt(f_cidLicenses)    const DRM_LID                    *f_pidLicenses,
    __out                                   DRM_SECURESTOP_SESSION     *f_pSession );

/*********************************************************************
**
** Function: DRM_SECURESTOP_BindDecryptor
**
** Synopsis: This function binds a decryptor to a secure stop session.
**
** Arguments:
**
** [f_pSecureCore]          -- Pointer to a secure core context.
** [f_pDatastore]           -- Pointer to an initialized data store.
** [f_pSession]             -- The secure stop session.
** [f_pDecryptor]           -- A pointer to the decryptor.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL DRM_SECURESTOP_BindDecryptor(
    __inout                                 DRM_SECURECORE_CONTEXT     *f_pSecureCore,
    __in                                    DRM_DST                    *f_pDatastore,
    __inout                                 DRM_SECURESTOP_SESSION     *f_pSession,
    __in                              const DRM_CIPHER_CONTEXT         *f_pDecryptor );

/*********************************************************************
**
** Function: DRM_SECURESTOP_UpdateSession
**
** Synopsis: This function updates a secure stop session.
**
** Arguments:
**
** [f_pSecureCore]          -- Pointer to a secure core context.
** [f_pDatastore]           -- Pointer to an initialized data store.
** [f_pSession]             -- The secure stop session.
** [f_fSuspend]             -- Indicate whether playback is suspended.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL DRM_SECURESTOP_UpdateSession(
    __inout                                 DRM_SECURECORE_CONTEXT     *f_pSecureCore,
    __in                                    DRM_DST                    *f_pDatastore,
    __inout                                 DRM_SECURESTOP_SESSION     *f_pSession,
    __in                                    DRM_BOOL                    f_fSuspend );

/*********************************************************************
**
** Function: DRM_SECURESTOP_UnbindDecryptor
**
** Synopsis: This function unbinds a decryptor from a secure stop session.
**
** Arguments:
**
** [f_pSecureCore]          -- Pointer to a secure core context.
** [f_pDatastore]           -- Pointer to an initialized data store.
** [f_pSession]             -- The secure stop session.
** [f_pDecryptor]           -- A pointer to the decryptor.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL DRM_SECURESTOP_UnbindDecryptor(
    __inout                                 DRM_SECURECORE_CONTEXT     *f_pSecureCore,
    __in                                    DRM_DST                    *f_pDatastore,
    __inout                                 DRM_SECURESTOP_SESSION     *f_pSession,
    __in                              const DRM_CIPHER_CONTEXT         *f_pDecryptor );

/*********************************************************************
**
** Function: DRM_SECURESTOP_GenerateChallenge
**
** Synopsis: This function generates a message challenge.
**
** Arguments:
**
** [f_pSecureCore]          -- Pointer to a secure core context.
** [f_pDatastore]           -- Pointer to an initialized data store.
** [f_pSession]             -- The secure stop session.
** [f_pPublicKey]           -- The publisher certificate public key.
** [f_cchCustomData]        -- The challenge custom data string length.
** [f_pchCustomData]        -- The challenge custom data string buffer.
** [f_pcbChallenge]         -- The secure stop challenge size in bytes.
** [f_ppbChallenge]         -- The secure stop challenge buffer.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**                          DRM_E_NOMORE if there are no messages in the HDS.
**
** Notes:                   Caller must free f_ppbChallenge using
**                          Oem_MemFree after use.
**
*********************************************************************/
DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL DRM_SECURESTOP_GenerateChallenge(
    __inout                                 DRM_SECURECORE_CONTEXT     *f_pSecureCore,
    __in                                    DRM_DST                    *f_pDatastore,
    __inout                                 DRM_SECURESTOP_SESSION     *f_pSession,
    __in                              const PUBKEY_P256                *f_pPublicKey,
    __in                              const DRM_DWORD                   f_cchCustomData,
    __in_ecount_opt(f_cchCustomData)  const DRM_CHAR                   *f_pchCustomData,
    __out                                   DRM_DWORD                  *f_pcbChallenge,
    __deref_out_bcount(*f_pcbChallenge)     DRM_BYTE                  **f_ppbChallenge );

/*********************************************************************
**
** Function: DRM_SECURESTOP_ProcessResponse
**
** Synopsis: This function processes a message response.
**
** Arguments:
**
** [f_pSecureCore]          -- Pointer to a secure core context.
** [f_pDatastore]           -- Pointer to an initialized data store.
** [f_pSession]             -- The secure stop session.
** [f_cbResponse]           -- The secure stop response size in bytes.
** [f_pbResponse]           -- The secure stop response buffer.
** [f_pcchCustomData]       -- The response custom data string length.
** [f_ppchCustomData]       -- The response custom data string buffer.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL DRM_SECURESTOP_ProcessResponse(
    __inout                                 DRM_SECURECORE_CONTEXT     *f_pSecureCore,
    __in                                    DRM_DST                    *f_pDatastore,
    __inout                                 DRM_SECURESTOP_SESSION     *f_pSession,
    __in                              const DRM_DWORD                   f_cbResponse,
    __in_bcount(f_cbResponse)         const DRM_BYTE                   *f_pbResponse,
    __out                                   DRM_DWORD                  *f_pcchCustomData,
    __deref_out_ecount(*f_pcchCustomData)   DRM_CHAR                  **f_ppchCustomData );

/*********************************************************************
**
** Function: DRM_SECURESTOP_EnumerateSessions
**
** Synopsis: This function enumerates existing secure stop sessions.
**
** Arguments:
**
** [f_pSecureCore]          -- Pointer to a secure core context.
** [f_pDatastore]           -- Pointer to an initialized data store.
** [f_pidPublisher]         -- The ID of the application publisher.
** [f_pcidSessions]         -- The count of session ID's.
** [f_ppidSessions]         -- The enumerated session ID's.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
** Notes:                   Caller must free f_ppidSessions using
**                          Oem_MemFree after use.
**
**********************************************************************/
DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL DRM_SECURESTOP_EnumerateSessions(
    __inout                                 DRM_SECURECORE_CONTEXT     *f_pSecureCore,
    __in                                    DRM_DST                    *f_pDatastore,
    __in                              const DRM_ID                     *f_pidPublisher,
    __out                                   DRM_DWORD                  *f_pcidSessions,
    __deref_out_ecount(*f_pcidSessions)     DRM_ID                    **f_ppidSessions );

/*********************************************************************
**
** Function: DRM_SECURESTOP_LoadSession
**
** Synopsis: This function loads an existing secure stop session.
**
** Arguments:
**
** [f_pSecureCore]          -- Pointer to a secure core context.
** [f_pDatastore]           -- Pointer to an initialized data store.
** [f_pidSession]           -- The ID used to identify this session.
** [f_pidPublisher]         -- The ID of the application publisher.
** [f_pSession]             -- The secure stop session.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
DRM_NO_INLINE DRM_API DRM_RESULT DRM_CALL DRM_SECURESTOP_LoadSession(
    __inout                                 DRM_SECURECORE_CONTEXT     *f_pSecureCore,
    __in                                    DRM_DST                    *f_pDatastore,
    __in                              const DRM_ID                     *f_pidSession,
    __in                              const DRM_ID                     *f_pidPublisher,
    __out                                   DRM_SECURESTOP_SESSION     *f_pSession );

/*********************************************************************
**
** Function: DRM_SECURESTOP_DeleteSession
**
** Synopsis: This function deletes an existing secure stop session.
**
** Arguments:
**
** [f_pidSession]           -- The ID used to identify this session.
**
**********************************************************************/
DRM_NO_INLINE DRM_API_VOID DRM_VOID DRM_CALL DRM_SECURESTOP_DeleteSession(
    __inout                                 DRM_SECURESTOP_SESSION     *f_pSession );

EXIT_PK_NAMESPACE;

#endif /* _DRMSECURESTOP_H_ */
