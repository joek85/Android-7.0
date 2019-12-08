/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/


#ifndef __OEMCRYPTOCTX_H__
#define __OEMCRYPTOCTX_H__

#include <drmstkalloc.h>

ENTER_PK_NAMESPACE;

#define PKSYMM_ALG_TYPE_RC4        1

/*
** The largest single allocation in the crypto code is in mod_exp
** which allocates up to this many bytes: (1 << MAX_BUCKET_WIDTH) * (max digits) * sizeof(digit)
** MAX_BUCKET_WIDTH from modexp.c is 6, so (1 << MAX_BUCKET_WIDTH) == 0x40
** Meanwhile, max digits is 0x80 used by 4096-bit-RSA.
** So the largest single allocation is 0x40 * 0x80 * 4 == 0x8000 bytes.
** This allocation dwarfs all the other buffer allocations that might be
** used in conjunction with it by a factor of more than 0x10, and no
** codepath that doesn't use mod_exp comes anywhere close to using this
** large a buffer. Therefore, set the size to 0x8800:
** 0x8000 for that one allocation and an additional
** 0x0800 for all other allocations on that codepath.
*/
#define DRM_PKCRYPTO_CONTEXT_BUFFER_INTERNAL_SIZE 0x8800

/* This is size for PK_DATA structure defined in oemeccp256.h.
   This size of structure on x86 is 3036.
   use 3200 to allow larger size for different architecture.
*/
#define DRM_PK_DATA_SIZE                          3200


#define LNGQDW 5
#define LNGQ DRM_DWORDS_TO_DIGITS(LNGQDW)
#define QBITS (LNGQDW * DWORD_BITS)

/*
** Defines the buffer size for DRMBIGNUM_CONTEXT_STRUCT structure defined in oemeccp256.h
** It is possible that the structure can be defined smaller by removing PK_DATA size, 
** but it is used in various places 
** with various assumptions. Because of that we choose to keep 
** this size as it is currently and decrease only when necessary.
*/
#define DRM_PKCRYPTO_CONTEXT_BUFFER_SIZE  ( DRM_PKCRYPTO_CONTEXT_BUFFER_INTERNAL_SIZE + sizeof( DRM_STACK_ALLOCATOR_CONTEXT ) + DRM_PK_DATA_SIZE + sizeof( DRM_DWORD ) + sizeof(DRM_VOID*) + DRM_OBFUS_PTR_WRAP_SIZE )

typedef DRM_OBFUS_FIXED_ALIGN struct __tagDRM_CRYPTO_CONTEXT
{
    DRM_BYTE rgbCryptoContext[ DRM_PKCRYPTO_CONTEXT_BUFFER_SIZE ];   /* size of the buffer pkcrypto code needs. Note this may have alignment problems. */
} DRM_CRYPTO_CONTEXT;

EXIT_PK_NAMESPACE;

#endif /* __OEMCRYPTOCTX_H__ */

