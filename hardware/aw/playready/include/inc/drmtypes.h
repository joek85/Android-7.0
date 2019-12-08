/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#ifndef __DRMTYPES_H__
#define __DRMTYPES_H__

#include <drmnamespace.h>   /* defining proper namespace (if used) */
#include <drmcompiler.h>
#include <drmint64.h>
#include <drmsal.h>

ENTER_PK_NAMESPACE;

#if DRM_BUILD_PROFILE == DRM_BUILD_PROFILE_IOS || DRM_BUILD_PROFILE == DRM_BUILD_PROFILE_LINUX || DRM_BUILD_PROFILE == DRM_BUILD_PROFILE_PRND_TX_1 || DRM_BUILD_PROFILE == DRM_BUILD_PROFILE_ANDROID || DRM_BUILD_PROFILE == DRM_BUILD_PROFILE_MAC
#define DRM_VOID    void
#else
typedef void      DRM_VOID;
#endif /* DRM_BUILD_PROFILE == DRM_BUILD_PROFILE_IOS || DRM_BUILD_PROFILE == DRM_BUILD_PROFILE_LINUX || DRM_BUILD_PROFILE == DRM_BUILD_PROFILE_PRND_TX_1 || DRM_BUILD_PROFILE == DRM_BUILD_PROFILE_ANDROID || DRM_BUILD_PROFILE == DRM_BUILD_PROFILE_MAC */

#ifndef DRM_RESULT_DEFINED
#define DRM_RESULT_DEFINED
typedef __success(return>=0) DRM_LONG  DRM_RESULT;
#endif /*DRM_RESULT_DEFINED*/

#ifndef DRM_CHAR_BIT
#define DRM_CHAR_BIT      8                /* number of bits in a char    */
#endif

#ifndef DRM_ISODD
#define DRM_ISODD(x) ((x)&1)
#endif

/*
** Using sizeof(DRM_UINT64) can return different values on different platforms
** because it could be implemented as a built in type or as a struct with
** possibly varying packing.  Some places need to use a value for a serialized
** QWORD size so just define it here.
*/
#define DRM_SIZEOFQWORD 8

#ifndef DRM_BITS_PER_BYTE
#define DRM_BITS_PER_BYTE (8)
#endif

#if DRM_64BIT_TARGET
typedef DRM_UINT64 DRM_DWORD_PTR;
typedef DRM_UINT64 DRM_SIZE_T;
#else  /* DRM_64BIT_TARGET */
typedef DRM_DWORD DRM_DWORD_PTR;
typedef DRM_DWORD DRM_SIZE_T;
#endif /* DRM_64BIT_TARGET */

#ifndef DRM_NO_OF
#if defined(__cplusplus) && !defined(_PREFAST_) && ( DRM_MSC_VER >= 1400 )
/*
** Use a template to ensure that DRM_NO_OF is never called with a pointer.
** Templates cannot be declared to have 'C' linkage, so extern "C++".
*/
extern "C++" template <typename T, DRM_SIZE_T N> char (*DRM_NUMBER_OF_FUNCTION(T(&)[N]))[N];
#define DRM_NO_OF( x )  ( sizeof(*DRM_NUMBER_OF_FUNCTION(x)) )
#else
#define DRM_NO_OF( x )  ( sizeof( (x) ) / sizeof( (x)[0] ) )
#endif
#endif /* #ifndef DRM_NO_OF */

#define DRM_IS_DWORD_ALIGNED( x ) ( ( DRM_DWORD_PTR )( x ) % sizeof( DRM_DWORD_PTR ) == 0 )
#define DRM_DWORD_ALIGNED_SIZE( x ) ( ( (x) % sizeof( DRM_DWORD_PTR ) == 0 ) ? (x) : (x) + ( sizeof( DRM_DWORD_PTR ) - ( (x) % sizeof( DRM_DWORD_PTR ) ) ) )

typedef struct __tagDRM_GUID {
    DRM_DWORD Data1;
    DRM_WORD  Data2;
    DRM_WORD  Data3;
    DRM_BYTE  Data4 [ 8 ];
} DRM_GUID;

#define DRM_EMPTY_DRM_GUID  { 0, 0, 0, { 0, 0, 0, 0, 0, 0, 0, 0 } }

#define DRM_IDENTICAL_GUIDS(p0,p1) \
    (OEM_SECURE_ARE_EQUAL((p0), (p1), sizeof(DRM_GUID)))


#if TARGET_LITTLE_ENDIAN

/*
** The following macro should be used to define GUIDs that are local to a single code file.
*/
#define DRM_DEFINE_LOCAL_GUID( name, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8 )    \
        static const DRM_GUID name                                                  \
                = { l, w1, w2, { b1, b2, b3, b4, \
                                 b5, b6, b7, b8 } }

/*
** The following two macros should be used to declare and define GUIDs that are
** shared among multiple code files.
*/
#define DRM_DEFINE_GUID( name, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8 )          \
        const DRM_EXPORT_VAR DRM_GUID name                                          \
                = { l, w1, w2, { b1, b2, b3, b4, \
                                 b5, b6, b7, b8 } }
#else


/*
** The following macro should be used to define GUIDs that are local to a single code file.
*/
#define DRM_DEFINE_LOCAL_GUID( name, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8 )    \
        static const DRM_GUID name                                                  \
                = { FLIP_DWORD_BYTES(l), FLIP_WORD_BYTES(w1), FLIP_WORD_BYTES(w2),  \
                  { b1, b2, b3, b4, \
                    b5, b6, b7, b8 } }

/*
** The following two macros should be used to declare and define GUIDs that are
** shared among multiple code files.
*/
#define DRM_DEFINE_GUID( name, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8 )          \
        const DRM_EXPORT_VAR DRM_GUID name                                          \
                = { FLIP_DWORD_BYTES(l), FLIP_WORD_BYTES(w1), FLIP_WORD_BYTES(w2),  \
                  { b1, b2, b3, b4, \
                    b5, b6, b7, b8 } }

#endif

#define DRM_DECLARE_GUID( name )    \
        extern const DRM_EXPORT_VAR DRM_GUID name

/*
** This macro will calculate the maximum value of an unsigned data type
** where the type is passed in as x.
** For example DRM_MAX_UNSIGNED_TYPE( DRM_BYTE  ) will evaluate to 0xFF
**             DRM_MAX_UNSIGNED_TYPE( DRM_DWORD ) will evaluate to 0xFFFFFFFF
*/
#define DRM_MAX_UNSIGNED_TYPE(x) ( (x)~((x)0) )

/*
** This macro will calculate the maximum value of an signed data type
** where the type is passed in as x.
** For example DRM_MAX_SIGNED_TYPE( DRM_CHAR) will evaluate to 0x7F
**             DRM_MAX_SIGNED_TYPE( DRM_LONG ) will evaluate to 0x7FFFFFFF
*/
#define DRM_MAX_SIGNED_TYPE(x) ( (x)~((x)1 << (sizeof(x)*8 - 1)) )

/*
** This macro will calculate the minimum value of an signed data type
** where the type is passed in as x.
** For example DRM_MIN_SIGNED_TYPE( DRM_CHAR  ) will evaluate to 0x80
**             DRM_MIN_SIGNED_TYPE( DRM_LONG ) will evaluate to 0x80000000
*/
#define DRM_MIN_SIGNED_TYPE(x) ( (x)((x)1 << (sizeof(x)*8 - 1)) )

/*
**
*/
typedef struct tagDRM_CONST_STRING
{
DRM_OBFUS_PTR_TOP
    const DRM_WCHAR *pwszString;
DRM_OBFUS_PTR_BTM
    DRM_DWORD        cchString;
DRM_OBFUS_FILL_BYTES(4)
} DRM_CONST_STRING;

/*
**
*/
typedef struct tagDRM_STRING
{
DRM_OBFUS_PTR_TOP
    DRM_WCHAR *pwszString;
DRM_OBFUS_PTR_BTM
    DRM_DWORD  cchString;
DRM_OBFUS_FILL_BYTES(4)
} DRM_STRING;

typedef struct tagDRM_ANSI_CONST_STRING
{
DRM_OBFUS_PTR_TOP
    const DRM_CHAR *pszString;
DRM_OBFUS_PTR_BTM
    DRM_DWORD       cchString;
DRM_OBFUS_FILL_BYTES(4)
} DRM_ANSI_CONST_STRING;

typedef struct tagDRM_ANSI_STRING
{
DRM_OBFUS_PTR_TOP
    DRM_CHAR  *pszString;
DRM_OBFUS_PTR_BTM
    DRM_DWORD  cchString;
DRM_OBFUS_FILL_BYTES(4)
} DRM_ANSI_STRING;

typedef struct __tagSubString
{
    DRM_DWORD m_ich;
    DRM_DWORD m_cch;
} DRM_SUBSTRING;

typedef struct tagDRM_STRING_WINDOW
{
    DRM_DWORD m_ichMin; /* inclusive */
    DRM_DWORD m_ichMaxExclusive; /* exclusive */
} DRM_STRING_WINDOW;

#define DRM_ASSIGN_DRM_ANSI_STRING(x,y) do {    \
        (x).pszString = (y);                    \
        (x).cchString = DRM_NO_OF(y)-1;         \
    } while( FALSE )

#define DRM_ASSIGN_DRM_STRING(x,y) do {         \
        (x).pwszString = (y).pwszString;        \
        (x).cchString  = (y).cchString;         \
    } while( FALSE )

#define DRM_INIT_DRM_STRING(x) do {             \
        (x).pwszString = NULL;                  \
        (x).cchString  = 0;                     \
    } while( FALSE )

#define DRM_INIT_DRM_ANSI_STRING(x) do {        \
        (x).pszString = NULL;                   \
        (x).cchString = 0;                      \
    } while( FALSE )

/*
** This macro can only be used at the declaration of DRM_STRING.
** It initailizes DRM string from DRM_WCHAR array.
** Wide strings should always be declared using the DRM_ONE_WCHAR macro.
** Using the L"" syntax will generate strings like "S\0t\0r\0i\0n\0g\0"
** (i.e. Big-Endian UTF16), but this porting kit assumes ALL wide strings
** are Little-Endian UTF16.
*/
#define DRM_CREATE_DRM_STRING(x) { DRM_OBFUS_INIT_PTR_TOP ((const DRM_WCHAR *) (x)), DRM_OBFUS_INIT_PTR_BTM (DRM_NO_OF(x)) - 1 }

#if TARGET_LITTLE_ENDIAN
#define DRM_WCHAR_CAST(x)   ((DRM_WCHAR)(x))
#define DRM_NATIVE_WCHAR(x) (x)
#define DRM_MAKE_MACHINE_WCHAR(ch0,ch1)  ( ((ch1)<<8) | ((ch0) & 0x00FF) )
#else
#define DRM_WCHAR_CAST(x)   ((DRM_WCHAR)((x)<<8))
#define DRM_NATIVE_WCHAR(x) ((DRM_WCHAR)((x)>>8))
#define DRM_MAKE_MACHINE_WCHAR(ch0,ch1)  ( ((ch0)<<8) | ((ch1) & 0x00FF) )
#endif

#define DRM_ONE_WCHAR(ch0,ch1)           ((DRM_WCHAR)(DRM_MAKE_MACHINE_WCHAR(ch0,ch1)))

/*
** In case these have been already defined, don't redefine.
** If you use these macros for string declarations, you have to
** add those strings to drmallstrings.c
*/
#ifndef DRM_INIT_CHAR_OBFUS
#define DRM_INIT_CHAR_OBFUS(x) x
#endif
#ifndef DRM_INIT_WCHAR_OBFUS
#define DRM_INIT_WCHAR_OBFUS(x) DRM_WCHAR_CAST(x)
#endif
#ifndef DRM_STR_CONST
#define DRM_STR_CONST const
#endif
#ifndef DRM_INIT_ALL_STRINGS
#define DRM_INIT_ALL_STRINGS do {} while(FALSE)
#endif

/*
** Note: in drmconstants.c DRM_CREATE_DRM_ANSI_STRING should be used
** with caution. When we create a const ansi string from const char[] arrays
** an array may have one or two trailing zero chars for alignment reasons,
** but this macro just always sets ansi string's char size into a size of array - 1.
*/
#define DRM_CREATE_DRM_ANSI_STRING(x) { DRM_OBFUS_INIT_PTR_TOP (x), DRM_OBFUS_INIT_PTR_BTM ((DRM_NO_OF(x))-1)}
#define DRM_CREATE_DRM_ANSI_STRING_EX(x,s) { DRM_OBFUS_INIT_PTR_TOP (x), DRM_OBFUS_INIT_PTR_BTM (s) }

#define DRM_EMPTY_DRM_STRING        { DRM_OBFUS_INIT_PTR_TOP NULL, DRM_OBFUS_INIT_PTR_BTM 0 }
#define DRM_EMPTY_DRM_SUBSTRING     { 0, 0 }
#define DRM_EMPTY_DRM_STRING_WINDOW { 0, 0 }

/*
**  Assert if the pointer is not DRM_WCHAR aligned
*/

#define DRM_DSTR_FROM_PB(pdstr,pb,cb) do {                                  \
        (pdstr)->cchString=(cb)/sizeof(DRM_WCHAR);                          \
        (pdstr)->pwszString=(DRM_WCHAR*)(pb);                               \
        DRMASSERT( (DRM_DWORD_PTR)(unsigned long)(pb) % sizeof( DRM_WCHAR ) == 0 );        \
    } while( FALSE )

#define DRM_DASTR_FROM_PB(pdastr,pb,cb) do {                                \
        (pdastr)->cchString=(cb)/( sizeof( DRM_CHAR ) );                    \
        (pdastr)->pszString=(DRM_CHAR*)(pb);                                \
    } while( FALSE )

/* for frequent uses of DRM_CONST_STRINGs' char counts and
  wide-char buffers as byte counts and byte buffers */

#define DRM_PB_DSTR(pdstr) (DRM_BYTE*)((pdstr)->pwszString)
#define DRM_CB_DSTR(pdstr) ((pdstr)->cchString*sizeof(DRM_WCHAR))

#define DRM_PB_DASTR(pdstr) (DRM_BYTE*)((pdstr)->pszString)
#define DRM_CB_DASTR(pdstr) ((pdstr)->cchString)


/*
** PLEASE NOTE: If DRM_C_TICS_PER_SECOND is changed, make sure
** the following are updated accordingly:
**
** 1. DRM_H264_ERROR_WINDOW_IN_TICS
** 2. MAX_ALLOWED_TIME_BEFORE_CLOCK_RESYNC
** 3. MAX_REVOCATION_EXPIRE_TICS
** 4. PRND_MAX_REGISTRATION_AGE_IN_TICKS
**
*/
#define DRM_C_TICS_PER_SECOND 10000000

#define DRM_C_SECONDS_FROM_1601_TO_1970 ( (DRM_UINT64)DRM_UI64LITERAL( 0x2, 0xB6109100 ) )

#define DRM_CREATE_FILE_TIME(dw, ft) do {                                                       \
        DRM_UINT64 ui64Tics = DRM_UI64Add( DRM_UI64( dw ), DRM_C_SECONDS_FROM_1601_TO_1970 );   \
        ui64Tics = DRM_UI64Mul( ui64Tics, DRM_UI64( DRM_C_TICS_PER_SECOND ) );                  \
        UI64_TO_FILETIME( ui64Tics, ft );                                                       \
    } while( FALSE )

#define DRM_FILE_TIME_TO_DWORD(ft, dw) do {                                                 \
        DRM_UINT64 ui64Tics;                                                                \
        FILETIME_TO_UI64( ft, ui64Tics );                                                   \
        ui64Tics = DRM_UI64Div( ui64Tics, DRM_UI64( DRM_C_TICS_PER_SECOND ) );              \
        dw = DRM_UI64Low32( DRM_UI64Sub( ui64Tics, DRM_C_SECONDS_FROM_1601_TO_1970 ) );     \
    } while( FALSE )

typedef struct _tagDRMSYSTEMTIME
{
    DRM_WORD wYear;
    DRM_WORD wMonth;
    DRM_WORD wDayOfWeek;
    DRM_WORD wDay;
    DRM_WORD wHour;
    DRM_WORD wMinute;
    DRM_WORD wSecond;
    DRM_WORD wMilliseconds;
} DRMSYSTEMTIME;

/* Contains a 64-bit value representing the number of 100-nanosecond intervals since January 1, 1601 (UTC). */
#if TARGET_LITTLE_ENDIAN
typedef struct _tagDRMFILETIME
{
    DRM_DWORD dwLowDateTime;
    DRM_DWORD dwHighDateTime;
} DRMFILETIME;
#else
typedef struct _tagDRMFILETIME
{
    DRM_DWORD dwHighDateTime;
    DRM_DWORD dwLowDateTime;
} DRMFILETIME;
#endif
/* generic ID type, currently all the same size */

#define DRM_ID_SIZE  16

typedef struct _tagDRM_ID
{
    DRM_BYTE rgb [ DRM_ID_SIZE ];
} DRM_ID;

typedef DRM_ID DRM_MID;
typedef DRM_ID DRM_KID;
typedef DRM_ID DRM_LID;
typedef DRM_ID DRM_TID;

/* Structure that contains the definition of a domain ID. */
typedef struct
{
    /* Service ID. */
    DRM_GUID    m_oServiceID;

    /* Account ID. */
    DRM_GUID    m_oAccountID;

    /* Revision. */
    DRM_DWORD   m_dwRevision;
} DRM_DOMAIN_ID;

#define DRM_EMPTY_DRM_DOMAIN_ID { DRM_EMPTY_DRM_GUID, DRM_EMPTY_DRM_GUID, 0 }

/*
** Structure that encapsulates the information returned by
** domain cert enumeration APIs.
*/
typedef struct __tagDRM_DOMAINCERT_INFO
{
    /* Domain ID. */
    DRM_DOMAIN_ID    m_oDomainID;

    /* Point to a URL in the domain cert. (not used at the moment) */
    DRM_ANSI_STRING  m_dastrURL;

} DRM_DOMAINCERT_INFO;

typedef struct _tagDRM_BYTEBLOB
{
    DRM_BYTE *pbBlob;
    DRM_DWORD cbBlob;
} DRM_BYTEBLOB;

#ifdef __cplusplus
#define DRM_REINTERPRET_CAST( type, ptr ) (reinterpret_cast<type*>(reinterpret_cast<DRM_VOID*>(ptr)))
#define DRM_REINTERPRET_CONST_CAST( type, ptr ) (reinterpret_cast<type*>(reinterpret_cast<const DRM_VOID*>(ptr)))
#else   /* __cplusplus */
#define DRM_REINTERPRET_CAST( type, ptr ) ((type*)((DRM_VOID*)(ptr)))
#define DRM_REINTERPRET_CONST_CAST DRM_REINTERPRET_CAST
#endif  /* __cplusplus */

EXIT_PK_NAMESPACE;

/******* The following definitions are pulled from bigdefs.h and are for use by bignum *******/
#define DWORD_BITS 32
#define DWORD_LEFT_BIT 0x80000000UL

/*      Multiple-precision data is normally represented
**      in radix 2^DRM_RADIX_BITS, with DRM_RADIX_BITS bits per word.
**      Here ``word'' means type digit_t.  DRM_RADIX_BITS
**      should be 32 for all supported architectures */
#define DRM_RADIX_BITS      32
#define DRM_RADIX_BYTES     4

/* Datatypes used by bignum */
typedef DRM_DWORD       digit_t;
typedef DRM_LONG        sdigit_t;
struct bigctx_t;

#define DRM_DIGIT_ZERO ((digit_t)0)
#define DRM_DIGIT_ONE  ((digit_t)1)

#define DRM_RADIX_HALF (DRM_DIGIT_ONE << (DRM_RADIX_BITS - 1))
#define DRM_RADIXM1 (DRM_DIGIT_ZERO - DRM_DIGIT_ONE)

#define DRM_DWORDS_PER_DIGIT (DRM_RADIX_BITS/DWORD_BITS)

/* DWORDS_TO_DIGITS(lng_dwords) computes the number of digit_t
** elements required to store an array with -lng_dwords- DWORDs.
** DRM_DIGITS_TO_DWORDS converts in the opposite direction. */
#define DRM_DWORDS_TO_DIGITS(lng_dwords) \
                ( ((lng_dwords) + DRM_DWORDS_PER_DIGIT - 1)/DRM_DWORDS_PER_DIGIT)
#define DRM_DIGITS_TO_DWORDS(lng_digits) ((lng_digits) * DRM_DWORDS_PER_DIGIT)
#define DRM_BITS_TO_DIGITS(nb) (((nb) + DRM_RADIX_BITS - 1)/DRM_RADIX_BITS)
#define DRM_digit_getbit(iword, ibit) (((iword) >> (ibit)) & 1)

/*
**      Test whether a (possibly negative) number is odd or even.
*/
#define DRM_IS_EVEN(n) (~(DRM_DWORD)(n) & 1)
#define DRM_IS_ODD(n) ((DRM_DWORD)(n) & 1)

/*
        Maximum and minimum of two arguments
        (no side effects in arguments)
*/
#define DRM_MAX(x, y) ((x) > (y) ? (x) : (y))
#define DRM_MIN(x, y) ((x) > (y) ? (y) : (x))

#ifndef TRUE
    #define TRUE  1
#endif

#ifndef FALSE
    #define FALSE 0
#endif

#ifndef NULL
    #define NULL  0
#endif

#define DRM_VERSION_LEN          4
#define DRM_VER_STRING_MAX_LEN   20
#define DRM_MAX_PATH             256

/*
** This is the root-most public key for all PlayReady binary certificates.
**
** DO NOT change this value unless you are absolutely certain it is required
** Verify with both your lead and your skip-level before you change this value.
**
** Changing this value for any reason will cause an enormous number of compatibily issues.
**
** Here is the base-64 encoded value for this public key:
** hk1hz/IlbkIsVos8KAAc+z4VJ2WFhLoFIbebGCjZNt4dgmqPw+bn+nqQ1copRvH2Si77n13P/n5DTrRCk/rFqw==
*/
#define DRM_ECC256_MS_PLAYREADY_ROOT_ISSUER_PUBKEY                                                         \
    {                                                                                                      \
        0x86, 0x4D, 0x61, 0xCF, 0xF2, 0x25, 0x6E, 0x42, 0x2C, 0x56, 0x8B, 0x3C, 0x28, 0x00, 0x1C, 0xFB,    \
        0x3E, 0x15, 0x27, 0x65, 0x85, 0x84, 0xBA, 0x05, 0x21, 0xB7, 0x9B, 0x18, 0x28, 0xD9, 0x36, 0xDE,    \
        0x1D, 0x82, 0x6A, 0x8F, 0xC3, 0xE6, 0xE7, 0xFA, 0x7A, 0x90, 0xD5, 0xCA, 0x29, 0x46, 0xF1, 0xF6,    \
        0x4A, 0x2E, 0xFB, 0x9F, 0x5D, 0xCF, 0xFE, 0x7E, 0x43, 0x4E, 0xB4, 0x42, 0x93, 0xFA, 0xC5, 0xAB     \
    }


#endif  /* __DRMTYPES_H__ */

