/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#ifndef _OEMTEETYPES_H_
#define _OEMTEETYPES_H_ 1

#include <drmtypes.h>
#include <drmcrt.h>

ENTER_PK_NAMESPACE;

#define OEM_TEE_MEMCPY                  DRMCRT_memcpy

#define OEM_TEE_MEMCPY_IDX(dest, dest_offset, source, source_offset, count) do {                        \
    DRM_BYTE *pbDest   = (DRM_BYTE*)( dest );                                                           \
    DRM_BYTE *pbSource = (DRM_BYTE*)( source );                                                         \
    ChkArg( pbDest != NULL && pbSource != NULL );                                                       \
    ChkDR( DRM_DWordPtrAdd( (DRM_DWORD_PTR)pbDest, ( dest_offset ), (DRM_DWORD_PTR*)&pbDest ) );        \
    ChkDR( DRM_DWordPtrAdd( (DRM_DWORD_PTR)pbSource, ( source_offset ), (DRM_DWORD_PTR*)&pbSource ) );  \
    OEM_TEE_MEMCPY( (char*)pbDest, (const char*)pbSource, ( count ) );                                  \
} while( FALSE )

#define OEM_TEE_ZERO_MEMORY( pb, cb )   DRMCRT_memset( pb, 0, cb )

/*
** This defines a Platform ID value that can be used for the 
** the out parameter f_pdwPlatformID in the OEM TEE function 
** OEM_TEE_LPROV_GetModelSecurityVersion.  If this value is
** used, validation of the model certificate's Platform ID will 
** be skipped during local provisioning.
*/
#define OEM_TEE_LPROV_PLATFORM_ID_IGNORE       DRM_MAX_UNSIGNED_TYPE(DRM_DWORD)

/*
** This defines a Security Version value that can be used for the 
** the out parameter f_pdwSecurityVersion in the OEM TEE function 
** OEM_TEE_LPROV_GetModelSecurityVersion.  If this value is
** used, validation of the model certificate's Security Version will 
** be skipped during local provisioning.
*/
#define OEM_TEE_LPROV_SECURITY_VERSION_IGNORE  DRM_MAX_UNSIGNED_TYPE(DRM_DWORD)

/*
** This defines the maximum number of TEE sessions that will be
** cached.  The cached sessions are used to determine the validity
** of the DRM_TEE_CONTEXT for all PRITEE functions.  An overflow
** of the cache will evict the oldest item in the cache and
** therefore invalidating the DRM_TEE_CONTEXT associated with that
** session.  This value MUST be less than or equal to 252.
*/
#define OEM_TEE_CACHE_MAX_ENTRIES 32

/*
** WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING
**
** All data inside the OEM_TEE_CONTEXT is returned OUTSIDE the TEE
** in the clear and unsigned.  It is completely unprotected against
** reading, writing, and modification by code running outside the TEE.
** By placing any data inside this structure, you acknowledge this
** lack of protection and agree to ensure that any sensitive data
** you include will be protected against reading, writing, and/or
** modification using TEE-backed secure assets (such as the CTK)
** with signing and/or encryption.  You must also assume that any data
** contained within this structure, including but not limited to pointer
** values, lengths, handles, keys, and raw bytes, may be maliciously
** modified outside the TEE and cannot be trusted.
** It is a violation of the PlayReady Compliance Rules to expose
** sensitive data by placing said data inside this structure.
**
** WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING
**
** This type allows the OEM to define its own context to refer
** to a single session inside the TEE and store its associated data.
** This type is considered opaque outside TEE.
**
** The pbUnprotectedOEMData member will be passed outside the TEE
** in the clear and unsigned.
**
** You must not modify this structure declaration.
** However, you may use its contents in any way you wish.
** You are provided with NO guarantees that your data will
** not be read from, written to, or tampered with outside the TEE.
**
** It is recommended that you avoid using this structure if possible.
** If you choose to use this structure, it is recommended that you
** avoid placing any sensitive assets in it.
** If you choose to place sensitive assets in it, you MUST protect
** those assets as described in the WARNING section above.
**
** pbUnprotectedOEMData MUST be allocated with OEM_TEE_BASE_SecureMemAlloc
** and MUST be freed with OEM_TEE_BASE_SecureMemFree.
*/
typedef struct __tagOEM_TEE_CONTEXT
{
    DRM_DWORD cbUnprotectedOEMData;
    DRM_BYTE *pbUnprotectedOEMData;
} OEM_TEE_CONTEXT;

#define OEM_TEE_CONTEXT_EMPTY {0, NULL}

/*
** OEM_TEE_CONTEXT_INTERNAL is a sample implemenation of an OEM defined structure 
** for the contextual information required by the OEM layer of the TEE interface.  
** None of the current member variables are required and are only used for testing.
*/
typedef struct __tagOEM_TEE_CONTEXT_INTERNAL
{
    DRM_DWORD dwOEMContextClear;
    DRM_WORD  wOEMContextEncrypted;
} OEM_TEE_CONTEXT_INTERNAL;

/*
** This type allows the OEM to define its own type for
** for AES128 and ECC256 keys.  This type is considered
** opaque outside the OEM portion of the TEE layer.
*/

typedef struct __tagOEM_TEE_KEY
{
    DRM_VOID *pKey;
} OEM_TEE_KEY;

#define OEM_TEE_KEY_EMPTY   { NULL }

/*
** This enum defines the types of decryption modes
** the OEM supports for AES CTR content decryption.
** Note that OEM_TEE_AES128CTR_DECRYPTION_MODE_NOT_SECURE
** MUST always be *listed* in this enum and
** MUST take the value zero.
** It lists the valid values for the following.
** DRM_TEE_AES128CTR_PrepareToDecrypt (*f_pdwDecryptionMode)
** DRM_FFLICENSE.dwDecryptionOutputMode
** DRM_APP_CONTEXT_INTERNAL.dwDecryptionOutputMode
** Note: The value of zero for
** OEM_TEE_AES128CTR_DECRYPTION_MODE_NOT_SECURE
** must *NOT* be changed by the OEM.
*/
typedef enum
{
    OEM_TEE_AES128CTR_DECRYPTION_MODE_NOT_SECURE        = 0x0,
    OEM_TEE_AES128CTR_DECRYPTION_MODE_HANDLE            = 0x1,
    OEM_TEE_AES128CTR_DECRYPTION_MODE_SAMPLE_PROTECTION = 0x2,
} OEM_TEE_AES128CTR_DECRYPTION_MODE;

#define OEM_TEE_AES128CTR_DECRYPTION_MODE_IS_HANDLE_TYPE( _dwMode ) ((_dwMode) == OEM_TEE_AES128CTR_DECRYPTION_MODE_HANDLE)

/* This macro defines the length of nonce used in remote provisioning, must not be changed */
#define OEM_PROVISIONING_NONCE_LENGTH 16

/*
** If an OEM needs to have some extended secure media path capabilities data,
** an OEM can agree with MSFT what information shall reside there and populate these structures.
** Currently left empty.
*/
typedef struct __tagOEM_TEE_RPROV_SMPC_HEADER
{
    DRM_CHAR    rgbMagic[4]; /* {'S','M','P','C'}, */
} OEM_TEE_RPROV_SMPC_HEADER;

typedef struct __tagOEM_SMPC
{
    OEM_TEE_RPROV_SMPC_HEADER oHeader;
} OEM_TEE_RPROV_SMPC;

typedef enum
{
    OEM_TEE_RPROV_BOOTSTRAP_ID_MESSAGE_INVALID       = 0x0,
    OEM_TEE_RPROV_BOOTSTRAP_ID_MESSAGE_S1            = 0x1,
    OEM_TEE_RPROV_BOOTSTRAP_ID_MESSAGE_S2            = 0x2,
    OEM_TEE_RPROV_BOOTSTRAP_ID_MESSAGE_S3            = 0x3,
    OEM_TEE_RPROV_BOOTSTRAP_ID_MESSAGE_S4            = 0x4,
    OEM_TEE_RPROV_BOOTSTRAP_ID_MESSAGE_LAST          = 0x4,
} OEM_TEE_RPROV_BOOTSTRAP_ID_MESSAGE;

#define RPROV_KEYPAIR_COUNT ((DRM_DWORD)3)
#define RPROV_KEYPAIR_SIGNING_IDX ((DRM_DWORD)0)

typedef enum
{
    OEM_TEE_XB_SIGNATURE_TYPE_AES_OMAC1_TKD = 0x1,
} OEM_TEE_XB_SIGNATURE_TYPE;

typedef DRM_VOID* OEM_TEE_MEMORY_HANDLE;
#define OEM_TEE_MEMORY_HANDLE_INVALID NULL

/*
** If TEE Supports secure clock that could accumulate a drift over time,
** TEE will force a periodic resync of the secure clock.
** The resync period is defined as the duration overwhich the TEE can
** accumulate 2 minutes of clock drift
** For example: a 3 seconds per day drift results in two minutes drift after 40 days
** 60 Seconds * 60 Minutes * 24 Hours * 40 Days = 3456000
**
** For Perf, MAX_ALLOWED_TIME_BEFORE_CLOCK_RESYNC was pre-calculated.
** If C_TICS_PER_SECOND is changed, make sure this expression is also updated.
**
** MAX_ALLOWED_TIME_BEFORE_CLOCK_RESYNC =
**              3456000 * C_TICS_PER_SECOND
*/
#define MAX_ALLOWED_TIME_BEFORE_CLOCK_RESYNC ( (DRM_UINT64)DRM_UI64LITERAL( 0x1F6E, 0xA0860000 ) )

EXIT_PK_NAMESPACE;

#endif /* _OEMTEETYPES_H_ */

