/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#ifndef __OEMCOMMONMEM_H__
#define __OEMCOMMONMEM_H__

#include <drmtypes.h>

ENTER_PK_NAMESPACE;

#undef OEM_SECURE_ZERO_MEMORY

/* OEM special implementation functions (oemimpl.c). */
#if defined(DRM_MSC_VER) || defined(__GNUC__)

DRM_VOID * DRM_CALL DRMCRT_ScrubMemory(
    __out_bcount( f_cbCount ) DRM_VOID  *f_ptr,
    __in DRM_DWORD  f_cbCount  );


#define OEM_SECURE_ZERO_MEMORY DRMCRT_ScrubMemory

#endif /* defined(DRM_MSC_VER) || defined(__GNUC__) */

/*
** OEM_MANDATORY:
** For all devices, this macro MUST be implemented by the OEM.
*/
#ifndef OEM_SECURE_ZERO_MEMORY
#error "Please provide implementation for OEM_SECURE_ZERO_MEMORY macro.\
 OEM_SECURE_ZERO_MEMORY is called to scrub memory on critical pieces of data before freeing buffers or exiting local scope.\
 Using of memset function on these buffers is not sufficient, since compiler\
 may optimize out zeroing of buffers that are not used afterwards.\
 Please verify the implementation will not be optimized out by your compiler.\
 Consult the documentation for your compiler to see how this can be done. \
 The Microsoft supplied example is only guaranteed to work on Microsoft compilers. "

#endif /* ifndef OEM_SECURE_ZERO_MEMORY */

#undef OEM_SECURE_MEMCPY
#undef OEM_SECURE_MEMCPY_IDX
#undef OEM_SECURE_MEMSET
#undef OEM_SECURE_ARE_EQUAL
#undef OEM_SECURE_DWORDCPY
#undef OEM_SECURE_DIGITTCPY

#if defined(DRM_MSC_VER) || defined(__GNUC__)

DRM_VOID * DRM_CALL DRMCRT_LocalMemcpy(
    __out_bcount_full( f_cbCount )  DRM_VOID  *f_pOut,
    __in_bcount( f_cbCount ) const  DRM_VOID  *f_pIn,
    __in                            DRM_DWORD  f_cbCount );

DRM_VOID * DRM_CALL DRMCRT_LocalMemset(
    __out_bcount_full( f_cbCount )  DRM_VOID  *f_pOut,
    __in                            DRM_BYTE   f_cbValue,
    __in                            DRM_DWORD  f_cbCount );

DRM_VOID * DRM_CALL DRMCRT_LocalDWORDSetZero(
    __out_bcount_full( f_cdwCount ) DRM_DWORD  *f_pdwOut,
    __in                            DRM_DWORD   f_cdwCount );

DRM_BOOL DRM_CALL DRMCRT_LocalAreEqual(
    __in_bcount( f_cbCount ) const  DRM_VOID  *f_pLHS,
    __in_bcount( f_cbCount ) const  DRM_VOID  *f_pRHS,
    __in                            DRM_DWORD  f_cbCount );


#if DRM_SUPPORT_INLINEDWORDCPY

DRM_VOID * DRM_CALL DRMCRT_LocalDWORDcpy(
    __out_ecount_full( f_cdwCount )  DRM_DWORD  *f_pdwOut,
    __in_ecount( f_cdwCount ) const  DRM_DWORD  *f_pdwIn,
    __in                             DRM_DWORD  f_cdwCount );

#define OEM_SECURE_DWORDCPY(out, in, ecount) DRMCRT_LocalDWORDcpy((DRM_DWORD*)(out), (const DRM_DWORD*)(in), (ecount))
#else   /* DRM_SUPPORT_INLINEDWORDCPY */
#define OEM_SECURE_DWORDCPY(out, in, ecount) OEM_SECURE_MEMCPY((out), (in), (ecount)*sizeof(DRM_DWORD))
#endif  /* DRM_SUPPORT_INLINEDWORDCPY */

#if DRM_DWORDS_PER_DIGIT == 1
#define OEM_SECURE_DIGITTCPY OEM_SECURE_DWORDCPY
#define OEM_SECURE_DIGITZEROMEMORY DRMCRT_LocalDWORDSetZero
#endif  /* DRM_DWORDS_PER_DIGIT == 1 */

#define OEM_SECURE_MEMCPY DRMCRT_LocalMemcpy

#define OEM_SECURE_MEMCPY_IDX(dest, dest_offset, source, source_offset, count) do {                           \
         DRM_BYTE *pbDest   = (DRM_BYTE*)( dest );                                                            \
         DRM_BYTE *pbSource = (DRM_BYTE*)( source );                                                          \
         ChkArg( pbDest != NULL && pbSource != NULL );                                                        \
         ChkDR( DRM_DWordPtrAdd( (DRM_DWORD_PTR)pbDest,   ( dest_offset ),   (DRM_DWORD_PTR*)&pbDest ) );     \
         ChkDR( DRM_DWordPtrAdd( (DRM_DWORD_PTR)pbSource, ( source_offset ), (DRM_DWORD_PTR*)&pbSource ) );   \
         OEM_SECURE_MEMCPY( (char*)pbDest, (const char*)pbSource, (count) );                                  \
    } while( FALSE )

#define OEM_SECURE_MEMSET DRMCRT_LocalMemset

#define OEM_SECURE_ARE_EQUAL DRMCRT_LocalAreEqual

#endif /* defined(DRM_MSC_VER) || defined(__GNUC__) */

/*
** OEM_MANDATORY:
** For all devices, these macros MUST be implemented by the OEM.
*/
#if !defined(OEM_SECURE_MEMCPY) || !defined(OEM_SECURE_MEMCPY_IDX) || !defined(OEM_SECURE_DWORDCPY) || !defined(OEM_SECURE_DIGITTCPY) || !defined(OEM_SECURE_DIGITZEROMEMORY)

#error "Please provide implementation for OEM_SECURE_* macros.\
 OEM_SECURE_* is used for copying or setting bytes where calls to CRT implementation of \
 mem* functions cannot be used."

#endif /* !defined(OEM_SECURE_MEMCPY) || !defined(OEM_SECURE_MEMCPY_IDX) || !defined(OEM_SECURE_DWORDCPY) || !defined(OEM_SECURE_DIGITTCPY) || !defined(OEM_SECURE_DIGITZEROMEMORY) */

EXIT_PK_NAMESPACE;

#endif /* __OEMCOMMONMEM_H__ */

