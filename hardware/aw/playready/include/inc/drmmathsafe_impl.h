/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#define DRM_BUILDING_DRMMATHSAFE_C

/* Declare intrinsics for Microsoft compilers */
#if (defined(_M_AMD64) && defined(DRM_MSC_VER) && !defined(__GNUC__) && DRM_SUPPORT_NATIVE_64BIT_TYPES)
#define DRMMATHSAFEUSEINTRINSICS 1
#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */
__checkReturn unsigned __int64 __cdecl _umul128(__in unsigned __int64 Multiplier, __in unsigned __int64 Multiplicand, __out unsigned __int64 *HighProduct);
#ifdef __cplusplus
} /* extern "C"*/
#endif /* __cplusplus */
PRAGMA_INTRINSIC(_umul128)
#endif  /* ((defined(_M_IX86) || defined(_M_AMD64)) && !defined(__GNUC__) && DRM_SUPPORT_NATIVE_64BIT_TYPES) */

ENTER_PK_NAMESPACE_CODE;

/* WORD -> BYTE conversion */
DRM_API DRMMATHSAFEINLINE DRM_RESULT DRM_CALL DRM_WordToByte(
    __in                      DRM_WORD       f_wOperand,
    __out_ecount(1)           DRM_BYTE      *f_pbResult )
{
    DRM_RESULT dr = DRM_SUCCESS;

    DRMASSERT( f_pbResult != NULL );
    __analysis_assume( f_pbResult != NULL );

    if( DRM_LIKELY( f_wOperand <= DRM_MAX_UNSIGNED_TYPE(DRM_BYTE) ) )
    {
        *f_pbResult = (DRM_BYTE)f_wOperand;
    }
    else
    {
        /*
        ** Initialize out value to ensure that failure
        ** to check return value doesn't result in buffer
        ** overflow if value is then used as pointer offset.
        */
        *f_pbResult = (DRM_BYTE)0;
        dr = DRM_E_ARITHMETIC_OVERFLOW;
    }

    return dr;
}

/* LONG -> BYTE conversion */
DRM_API DRMMATHSAFEINLINE DRM_RESULT DRM_CALL DRM_LongToByte(
    __in                      DRM_LONG       f_lOperand,
    __out_ecount(1)           DRM_BYTE      *f_pbResult )
{
    DRM_RESULT dr = DRM_SUCCESS;

    DRMASSERT( f_pbResult != NULL );
    __analysis_assume( f_pbResult != NULL );

    if( DRM_LIKELY( ( 0 <= f_lOperand ) && ( f_lOperand <= DRM_MAX_UNSIGNED_TYPE(DRM_BYTE) ) ) )
    {
        *f_pbResult = (DRM_BYTE)f_lOperand;
    }
    else
    {
        /*
        ** Initialize out value to ensure that failure
        ** to check return value doesn't result in buffer
        ** overflow if value is then used as pointer offset.
        */
        *f_pbResult = (DRM_BYTE)0;
        dr = DRM_E_ARITHMETIC_OVERFLOW;
    }

    return dr;
}

/* LONG -> WORD conversion */
DRM_API DRMMATHSAFEINLINE DRM_RESULT DRM_CALL DRM_LongToWord(
    __in                      DRM_LONG       f_lOperand,
    __out                     DRM_WORD      *f_pwResult )
{
    DRM_RESULT dr = DRM_SUCCESS;

    DRMASSERT( f_pwResult != NULL );
    __analysis_assume( f_pwResult != NULL );

    if( DRM_LIKELY( ( 0 <= f_lOperand ) && ( f_lOperand <= DRM_MAX_UNSIGNED_TYPE(DRM_WORD) ) ) )
    {
        *f_pwResult = (DRM_WORD)f_lOperand;
    }
    else
    {
        /*
        ** Initialize out value to ensure that failure
        ** to check return value doesn't result in buffer
        ** overflow if value is then used as pointer offset.
        */
        *f_pwResult = (DRM_WORD)0;
        dr = DRM_E_ARITHMETIC_OVERFLOW;
    }

    return dr;
}

/* LONG -> DWORD conversion */
DRM_API DRMMATHSAFEINLINE DRM_RESULT DRM_CALL DRM_LongToDWord(
    __in                      DRM_LONG       f_lOperand,
    __out                     DRM_DWORD     *f_pdwResult )
{
    DRM_RESULT dr = DRM_SUCCESS;

    DRMASSERT( f_pdwResult != NULL );
    __analysis_assume( f_pdwResult != NULL );

    if( DRM_LIKELY( f_lOperand >= 0 ) )
    {
        *f_pdwResult = (DRM_DWORD)f_lOperand;
    }
    else
    {
        /*
        ** Initialize out value to ensure that failure
        ** to check return value doesn't result in buffer
        ** overflow if value is then used as pointer offset.
        */
        *f_pdwResult = (DRM_DWORD)0;
        dr = DRM_E_ARITHMETIC_OVERFLOW;
    }

    return dr;
}

/* LONG -> DWORD_PTR conversion */
DRM_API DRMMATHSAFEINLINE DRM_RESULT DRM_CALL DRM_LongToDWordPtr(
    __in                      DRM_LONG       f_lOperand,
    __out                     DRM_DWORD_PTR *f_pdwpResult )
{
    DRM_RESULT dr = DRM_SUCCESS;

    DRMASSERT( f_pdwpResult != NULL );
    __analysis_assume( f_pdwpResult != NULL );

    if( DRM_LIKELY( f_lOperand >= 0 ) )
    {
        *f_pdwpResult = (DRM_DWORD_PTR)f_lOperand;
    }
    else
    {
        /*
        ** Initialize out value to ensure that failure
        ** to check return value doesn't result in buffer
        ** overflow if value is then used as pointer offset.
        */
        *f_pdwpResult = (DRM_DWORD_PTR)0;
        dr = DRM_E_ARITHMETIC_OVERFLOW;
    }

    return dr;
}

/* DWORD -> BYTE conversion */
DRM_API DRMMATHSAFEINLINE DRM_RESULT DRM_CALL DRM_DWordToByte(
    __in                      DRM_DWORD     f_dwOperand,
    __out_ecount(1)           DRM_BYTE     *f_pbResult )
{
    DRM_RESULT dr = DRM_SUCCESS;

    DRMASSERT( f_pbResult != NULL );
    __analysis_assume( f_pbResult != NULL );

    if( DRM_LIKELY( f_dwOperand <= DRM_MAX_UNSIGNED_TYPE(DRM_BYTE) ) )
    {
        *f_pbResult = (DRM_BYTE)f_dwOperand;
    }
    else
    {
        /*
        ** Initialize out value to ensure that failure
        ** to check return value doesn't result in buffer
        ** overflow if value is then used as pointer offset.
        */
        *f_pbResult = (DRM_BYTE)0;
        dr = DRM_E_ARITHMETIC_OVERFLOW;
    }

    return dr;
}

/* DWORD -> WORD conversion */
DRM_API DRMMATHSAFEINLINE DRM_RESULT DRM_CALL DRM_DWordToWord(
    __in                      DRM_DWORD     f_dwOperand,
    __out                     DRM_WORD     *f_pwResult )
{
    DRM_RESULT dr = DRM_SUCCESS;

    DRMASSERT( f_pwResult != NULL );
    __analysis_assume( f_pwResult != NULL );

    if( DRM_LIKELY( f_dwOperand <= DRM_MAX_UNSIGNED_TYPE(DRM_WORD) ) )
    {
        *f_pwResult = (DRM_WORD)f_dwOperand;
    }
    else
    {
        /*
        ** Initialize out value to ensure that failure
        ** to check return value doesn't result in buffer
        ** overflow if value is then used as pointer offset.
        */
        *f_pwResult = (DRM_WORD)0;
        dr = DRM_E_ARITHMETIC_OVERFLOW;
    }
    return dr;
}

/* DWORD -> LONG conversion */
DRM_API DRMMATHSAFEINLINE DRM_RESULT DRM_CALL DRM_DWordToLong(
    __in                      DRM_DWORD     f_dwOperand,
    __out                     DRM_LONG     *f_plResult )
{
    DRM_RESULT dr = DRM_SUCCESS;

    DRMASSERT( f_plResult != NULL );
    __analysis_assume( f_plResult != NULL );

    if( DRM_LIKELY( f_dwOperand <= DRM_LONG_MAX ) )
    {
        *f_plResult = (DRM_LONG)f_dwOperand;
    }
    else
    {
        /*
        ** Initialize out value to ensure that failure
        ** to check return value doesn't result in buffer
        ** overflow if value is then used as pointer offset.
        */
        *f_plResult = (DRM_LONG)0;
        dr = DRM_E_ARITHMETIC_OVERFLOW;
    }

    return dr;
}

/* DWORD_PTR -> DWORD conversion */
#ifndef DRM_64BIT_TARGET
DRM_API DRMMATHSAFEINLINE DRM_RESULT DRM_CALL DRM_DWordPtrToDWord(
    __in                      DRM_DWORD_PTR  f_dwpOperand,
    __out                     DRM_DWORD     *f_pdwResult )
{
    DRMASSERT( f_pdwResult != NULL );
    __analysis_assume( f_pdwResult != NULL );

    *f_pdwResult = (DRM_DWORD)f_dwpOperand;
    return DRM_SUCCESS;
}
#endif /* DRM_64BIT_TARGET */

/* INT64 -> WORD conversion */
DRM_API DRMMATHSAFEINLINE DRM_RESULT DRM_CALL DRM_Int64ToWord(
    __in                      DRM_INT64     f_i64Operand,
    __out                     DRM_WORD     *f_pwResult )
{
    DRM_RESULT dr = DRM_SUCCESS;

    DRMASSERT( f_pwResult != NULL );
    __analysis_assume( f_pwResult != NULL );

    if( DRM_LIKELY( DRM_I64GEq( f_i64Operand, DRM_I64Asgn( 0, 0 ) )
                 && DRM_I64LEq( f_i64Operand, DRM_I64Asgn( 0, DRM_MAX_UNSIGNED_TYPE(DRM_WORD) ) ) ) )
    {
        *f_pwResult = (DRM_WORD)DRM_I64Low32( f_i64Operand );
    }
    else
    {
        /*
        ** Initialize out value to ensure that failure
        ** to check return value doesn't result in buffer
        ** overflow if value is then used as pointer offset.
        */
        *f_pwResult = (DRM_WORD)0;
        dr = DRM_E_ARITHMETIC_OVERFLOW;
    }

    return dr;
}

/* INT64 -> BYTE conversion */
DRM_API DRMMATHSAFEINLINE DRM_RESULT DRM_CALL DRM_Int64ToByte(
    __in                      DRM_INT64     f_i64Operand,
    __out_ecount(1)           DRM_BYTE     *f_pbResult )
{
    DRM_RESULT dr = DRM_SUCCESS;

    DRMASSERT( f_pbResult != NULL );
    __analysis_assume( f_pbResult != NULL );

    if( DRM_LIKELY( DRM_I64GEq( f_i64Operand, DRM_I64Asgn( 0, 0 ) )
                 && DRM_I64LEq( f_i64Operand, DRM_I64Asgn( 0, DRM_MAX_UNSIGNED_TYPE(DRM_BYTE) ) ) ) )
    {
        *f_pbResult = (DRM_BYTE)DRM_I64Low32( f_i64Operand );
    }
    else
    {
        /*
        ** Initialize out value to ensure that failure
        ** to check return value doesn't result in buffer
        ** overflow if value is then used as pointer offset.
        */
        *f_pbResult = (DRM_BYTE)0;
        dr = DRM_E_ARITHMETIC_OVERFLOW;
    }

    return dr;
}

/* INT64 -> LONG conversion */
DRM_API DRMMATHSAFEINLINE DRM_RESULT DRM_CALL DRM_Int64ToLong(
    __in                      DRM_INT64     f_i64Operand,
    __out                     DRM_LONG     *f_plResult )
{
    DRM_RESULT dr = DRM_SUCCESS;

    DRMASSERT( f_plResult != NULL );
    __analysis_assume( f_plResult != NULL );

    if( DRM_LIKELY( DRM_I64GEq( f_i64Operand, DRM_I64( DRM_LONG_MIN ) )
                 && DRM_I64LEq( f_i64Operand, DRM_I64( DRM_LONG_MAX ) ) ) )
    {
        *f_plResult = (DRM_LONG)DRM_I64Low32( f_i64Operand );
    }
    else
    {
        /*
        ** Initialize out value to ensure that failure
        ** to check return value doesn't result in buffer
        ** overflow if value is then used as pointer offset.
        */
        *f_plResult = (DRM_LONG)0;
        dr = DRM_E_ARITHMETIC_OVERFLOW;
    }

    return dr;
}

/* INT64 -> DWORD conversion */
DRM_API DRMMATHSAFEINLINE DRM_RESULT DRM_CALL DRM_Int64ToDWord(
    __in                      DRM_INT64     f_i64Operand,
    __out                     DRM_DWORD    *f_pdwResult )
{
    DRM_RESULT dr = DRM_SUCCESS;

    DRMASSERT( f_pdwResult != NULL );
    __analysis_assume( f_pdwResult != NULL );

    if( DRM_LIKELY( DRM_I64GEq( f_i64Operand, DRM_I64Asgn( 0, 0 ) )
                 && DRM_I64LEq( f_i64Operand, DRM_I64Asgn( 0, DRM_MAX_UNSIGNED_TYPE(DRM_DWORD) ) ) ) )
    {
        *f_pdwResult = (DRM_DWORD)DRM_I64ToUI32( f_i64Operand );
    }
    else
    {
        /*
        ** Initialize out value to ensure that failure
        ** to check return value doesn't result in buffer
        ** overflow if value is then used as pointer offset.
        */
        *f_pdwResult = (DRM_DWORD)0;
        dr = DRM_E_ARITHMETIC_OVERFLOW;
    }

    return dr;
}

/* INT64 -> UINT64 conversion */
DRM_API DRMMATHSAFEINLINE DRM_RESULT DRM_CALL DRM_Int64ToUInt64(
    __in                      DRM_INT64     f_i64Operand,
    __out                     DRM_UINT64   *f_pui64Result )
{
    DRM_RESULT dr = DRM_SUCCESS;

    DRMASSERT( f_pui64Result != NULL );
    __analysis_assume( f_pui64Result != NULL );

    if( DRM_LIKELY( DRM_I64GEq( f_i64Operand, DRM_I64Asgn( 0, 0 ) ) ) )
    {
        *f_pui64Result = DRM_I2UI64( f_i64Operand );
    }
    else
    {
        *f_pui64Result = DRM_UI64HL( (DRM_DWORD)0, (DRM_DWORD)0 );
        dr = DRM_E_ARITHMETIC_OVERFLOW;
    }

    return dr;
}

/* UINT64 -> WORD conversion */
DRM_API DRMMATHSAFEINLINE DRM_RESULT DRM_CALL DRM_UInt64ToWord(
    __in                      DRM_UINT64     f_ui64Operand,
    __out                     DRM_WORD      *f_pwResult )
{
    DRM_RESULT dr = DRM_SUCCESS;

    DRMASSERT( f_pwResult != NULL );
    __analysis_assume( f_pwResult != NULL );

    if( DRM_LIKELY( DRM_UI64LEq( f_ui64Operand, DRM_UI64HL( 0, DRM_MAX_UNSIGNED_TYPE(DRM_WORD) ) ) ) )
    {
        *f_pwResult = (DRM_WORD)DRM_UI64Low32( f_ui64Operand );
    }
    else
    {
        /*
        ** Initialize out value to ensure that failure
        ** to check return value doesn't result in buffer
        ** overflow if value is then used as pointer offset.
        */
        *f_pwResult = (DRM_WORD)0;
        dr = DRM_E_ARITHMETIC_OVERFLOW;
    }

    return dr;
}

/* UINT64 -> BYTE conversion */
DRM_API DRMMATHSAFEINLINE DRM_RESULT DRM_CALL DRM_UInt64ToByte(
    __in                      DRM_UINT64     f_ui64Operand,
    __out_ecount(1)           DRM_BYTE      *f_pbResult )
{
    DRM_RESULT dr = DRM_SUCCESS;

    DRMASSERT( f_pbResult != NULL );
    __analysis_assume( f_pbResult != NULL );

    if( DRM_LIKELY( DRM_UI64LEq( f_ui64Operand, DRM_UI64HL( 0, DRM_MAX_UNSIGNED_TYPE(DRM_BYTE) ) ) ) )
    {
        *f_pbResult = (DRM_BYTE)DRM_UI64Low32( f_ui64Operand );
    }
    else
    {
        /*
        ** Initialize out value to ensure that failure
        ** to check return value doesn't result in buffer
        ** overflow if value is then used as pointer offset.
        */
        *f_pbResult = (DRM_BYTE)0;
        dr = DRM_E_ARITHMETIC_OVERFLOW;
    }

    return dr;
}

/*  UINT64 -> LONG conversion */
DRM_API DRMMATHSAFEINLINE DRM_RESULT DRM_CALL DRM_UInt64ToLong(
    __in                      DRM_UINT64     f_ui64Operand,
    __out                     DRM_LONG      *f_plResult )
{
    DRM_RESULT dr = DRM_SUCCESS;

    DRMASSERT( f_plResult != NULL );
    __analysis_assume( f_plResult != NULL );

    if( DRM_LIKELY( DRM_UI64LEq( f_ui64Operand, DRM_UI64HL( 0, DRM_LONG_MAX ) ) ) )
    {
        *f_plResult = (DRM_LONG)DRM_UI64Low32( f_ui64Operand );
    }
    else
    {
        /*
        ** Initialize out value to ensure that failure
        ** to check return value doesn't result in buffer
        ** overflow if value is then used as pointer offset.
        */
        *f_plResult = (DRM_LONG)0;
        dr = DRM_E_ARITHMETIC_OVERFLOW;
    }

    return dr;
}

/* UINT64 -> DWORD conversion */
DRM_API DRMMATHSAFEINLINE DRM_RESULT DRM_CALL DRM_UInt64ToDWord(
    __in                      DRM_UINT64     f_ui64Operand,
    __out                     DRM_DWORD     *f_pdwResult )
{
    DRMASSERT( f_pdwResult != NULL );
    __analysis_assume( f_pdwResult != NULL );

    if( DRM_LIKELY( DRM_UI64High32(f_ui64Operand) == 0 ) )
    {
        *f_pdwResult = DRM_UI64Low32( f_ui64Operand );
        return DRM_SUCCESS;
    }
    else
    {
        /*
        ** Initialize out value to ensure that failure
        ** to check return value doesn't result in buffer
        ** overflow if value is then used as pointer offset.
        */
        *f_pdwResult = (DRM_DWORD)0;
        return DRM_E_ARITHMETIC_OVERFLOW;
    }
}

/* UINT64 -> DWORD_PTR conversion */
#ifdef DRM_64BIT_TARGET
DRM_API DRMMATHSAFEINLINE DRM_RESULT DRM_CALL DRM_UInt64ToDWordPtr(
    __in                      DRM_UINT64     f_ui64Operand,
    __out                     DRM_DWORD_PTR *f_pdwpResult )
{
    DRMASSERT( f_pdwpResult != NULL );
    __analysis_assume( f_pdwpResult != NULL );

    DRMSIZEASSERT( sizeof( DRM_DWORD_PTR ), sizeof( DRM_UINT64 ) );
    *f_pdwpResult = (DRM_DWORD_PTR)f_ui64Operand;
    return DRM_SUCCESS;
}
#endif /* DRM_64BIT_TARGET */

/* UINT64 -> INT64 conversion */
DRM_API DRMMATHSAFEINLINE DRM_RESULT DRM_CALL DRM_UInt64ToInt64(
    __in                      DRM_UINT64     f_ui64Operand,
    __out                     DRM_INT64     *f_pi64Result )
{
    DRM_RESULT dr        = DRM_SUCCESS;

    DRMASSERT( f_pi64Result != NULL );
    __analysis_assume( f_pi64Result != NULL );

    if( DRM_LIKELY( DRM_UI64High32( f_ui64Operand ) <= DRM_LONG_MAX ) )
    {
        *f_pi64Result = DRM_UI2I64( f_ui64Operand );
    }
    else
    {
        *f_pi64Result = DRM_I64Asgn( 0, 0 );
        dr = DRM_E_ARITHMETIC_OVERFLOW;
    }

    return dr;
}

/*
** Addition functions
*/

/* BYTE addition */
DRM_API DRMMATHSAFEINLINE DRM_RESULT DRM_CALL DRM_ByteAdd(
    __in                      DRM_BYTE       f_bAugend,
    __in                      DRM_BYTE       f_bAddend,
    __out_ecount(1)           DRM_BYTE      *f_pbResult )
{
    DRM_DWORD  dwResult = ((DRM_DWORD)f_bAugend) + ((DRM_DWORD)f_bAddend);

    DRMASSERT( f_pbResult != NULL );
    __analysis_assume( f_pbResult != NULL );

    return DRM_DWordToByte( dwResult, f_pbResult );
}

/* WORD addition */
DRM_API DRMMATHSAFEINLINE DRM_RESULT DRM_CALL DRM_WordAdd(
    __in                      DRM_WORD       f_wAugend,
    __in                      DRM_WORD       f_wAddend,
    __out                     DRM_WORD      *f_pwResult )
{
    DRM_DWORD  dwResult = ((DRM_DWORD)f_wAugend) + ((DRM_DWORD)f_wAddend);

    DRMASSERT( f_pwResult != NULL );
    __analysis_assume( f_pwResult != NULL );

    return DRM_DWordToWord( dwResult, f_pwResult );
}

/* UINT64 addition */
DRM_API DRMMATHSAFEINLINE DRM_RESULT DRM_CALL DRM_UInt64Add(
    __in                      DRM_UINT64       f_ui64Augend,
    __in                      DRM_UINT64       f_ui64Addend,
    __out                     DRM_UINT64      *f_pui64Result )
{
    DRM_RESULT dr             = DRM_SUCCESS;
    DRM_UINT64 ui64ResultLow  = DRM_UI64Add( DRM_UI64HL( 0, DRM_UI64Low32( f_ui64Augend ) ),  DRM_UI64HL( 0, DRM_UI64Low32( f_ui64Addend ) ) );
    DRM_UINT64 ui64ResultHigh = DRM_UI64Add( DRM_UI64HL( 0, DRM_UI64High32( f_ui64Augend ) ), DRM_UI64HL( 0, DRM_UI64High32( f_ui64Addend ) ) );

    /* Fold any carry into high order dword */
    ui64ResultHigh = DRM_UI64Add( ui64ResultHigh, DRM_UI64HL( 0, DRM_UI64High32( ui64ResultLow ) ) );

    DRMASSERT( f_pui64Result != NULL );
    __analysis_assume( f_pui64Result != NULL );

    /* If the high order dword add(s) didn't carry, i.e. are zero, then we did not overflow */
    if( DRM_LIKELY( DRM_UI64High32( ui64ResultHigh ) == 0 ) )
    {
        *f_pui64Result = DRM_UI64HL( DRM_UI64Low32( ui64ResultHigh ), DRM_UI64Low32( ui64ResultLow ) );
    }
    else
    {
        *f_pui64Result = DRM_UI64HL( 0, 0 );
        dr = DRM_E_ARITHMETIC_OVERFLOW;
    }

    return dr;
}

DRM_API DRMMATHSAFEINLINE DRM_RESULT DRM_CALL DRM_UInt64AddSame(
    __inout                   DRM_UINT64      *f_pui64AugendResult,
    __in                      DRM_UINT64       f_ui64Addend )
{
    DRM_RESULT dr                = DRM_SUCCESS;
    DRM_UINT64 ui64ResultLow;   /* Initialized later by DRM_UI64Add */
    DRM_UINT64 ui64ResultHigh;  /* Initialized later by DRM_UI64Add */

    DRMASSERT( f_pui64AugendResult != NULL );
    __analysis_assume( f_pui64AugendResult != NULL );

    ui64ResultLow  = DRM_UI64Add( DRM_UI64HL( 0, DRM_UI64Low32( *f_pui64AugendResult ) ),  DRM_UI64HL( 0, DRM_UI64Low32( f_ui64Addend ) ) );
    ui64ResultHigh = DRM_UI64Add( DRM_UI64HL( 0, DRM_UI64High32( *f_pui64AugendResult ) ), DRM_UI64HL( 0, DRM_UI64High32( f_ui64Addend ) ) );

    /* Fold any carry into high order dword */
    ui64ResultHigh = DRM_UI64Add( ui64ResultHigh, DRM_UI64HL( 0, DRM_UI64High32( ui64ResultLow ) ) );

    /* If the high order dword add(s) didn't carry, i.e. are zero, then we did not overflow */
    if( DRM_LIKELY( DRM_UI64High32( ui64ResultHigh ) == 0 ) )
    {
        *f_pui64AugendResult = DRM_UI64HL( DRM_UI64Low32( ui64ResultHigh ), DRM_UI64Low32( ui64ResultLow ) );
    }
    else
    {
        *f_pui64AugendResult = DRM_UI64HL( 0, 0 );
        dr = DRM_E_ARITHMETIC_OVERFLOW;
    }

    return dr;
}

/* DWORD addition */
DRM_API DRMMATHSAFEINLINE DRM_RESULT DRM_CALL DRM_DWordAdd(
    __in                      DRM_DWORD       f_dwAugend,
    __in                      DRM_DWORD       f_dwAddend,
    __out                     DRM_DWORD      *f_pdwResult )
{
    DRM_UINT64 ui64result;
    DRM_UINT64 ui64Augend = DRM_UI64HL( 0, f_dwAugend );
    DRM_UINT64 ui64Addend = DRM_UI64HL( 0, f_dwAddend );

    DRMASSERT( f_pdwResult != NULL );
    __analysis_assume( f_pdwResult != NULL );

    ui64result = DRM_UI64Add( ui64Augend, ui64Addend );
    return DRM_UInt64ToDWord( ui64result, f_pdwResult );
}

DRM_API DRMMATHSAFEINLINE DRM_RESULT DRM_CALL DRM_DWordAddSame(
    __inout                   DRM_DWORD       *f_pdwAugendResult,
    __in                      DRM_DWORD        f_dwAddend )
{
    DRM_UINT64 ui64result;

    DRMASSERT( f_pdwAugendResult != NULL );
    __analysis_assume( f_pdwAugendResult != NULL );

    ui64result = DRM_UI64Add( DRM_UI64HL( 0, *f_pdwAugendResult ),  DRM_UI64HL( 0, f_dwAddend  ) );
    return DRM_UInt64ToDWord( ui64result, f_pdwAugendResult );
}

/*
** Subtraction functions
*/

/*BYTE subtraction */
DRM_API DRMMATHSAFEINLINE DRM_RESULT DRM_CALL DRM_ByteSub(
    __in                      DRM_BYTE       f_bMinuend,
    __in                      DRM_BYTE       f_bSubtrahend,
    __out                     DRM_BYTE      *f_pbResult )
{
    DRM_RESULT dr = DRM_SUCCESS;

    DRMASSERT( f_pbResult != NULL );
    __analysis_assume( f_pbResult != NULL );

    if( DRM_LIKELY( f_bMinuend >= f_bSubtrahend ) )
    {
        *f_pbResult = (DRM_BYTE)(f_bMinuend - f_bSubtrahend);
    }
    else
    {
        /*
        ** Initialize out value to ensure that failure
        ** to check return value doesn't result in buffer
        ** overflow if value is then used as pointer offset.
        */
        *f_pbResult = (DRM_BYTE)0;
        dr = DRM_E_ARITHMETIC_OVERFLOW;
    }

    return dr;
}

/* WORD subtraction */
DRM_API DRMMATHSAFEINLINE DRM_RESULT DRM_CALL DRM_WordSub(
    __in                      DRM_WORD       f_wMinuend,
    __in                      DRM_WORD       f_wSubtrahend,
    __out                     DRM_WORD      *f_pwResult )
{
    DRM_RESULT dr = DRM_SUCCESS;

    DRMASSERT( f_pwResult != NULL );
    __analysis_assume( f_pwResult != NULL );

    if( DRM_LIKELY( f_wMinuend >= f_wSubtrahend ) )
    {
        *f_pwResult = (DRM_WORD)(f_wMinuend - f_wSubtrahend);
    }
    else
    {
        /*
        ** Initialize out value to ensure that failure
        ** to check return value doesn't result in buffer
        ** overflow if value is then used as pointer offset.
        */
        *f_pwResult = (DRM_WORD)0;
        dr = DRM_E_ARITHMETIC_OVERFLOW;
    }

    return dr;
}

/* DWORD subtraction */
DRM_API DRMMATHSAFEINLINE DRM_RESULT DRM_CALL DRM_DWordSub(
    __in                      DRM_DWORD       f_dwMinuend,
    __in                      DRM_DWORD       f_dwSubtrahend,
    __out                     DRM_DWORD      *f_pdwResult )
{
    DRMASSERT( f_pdwResult != NULL );
    __analysis_assume( f_pdwResult != NULL );

    if( DRM_LIKELY( f_dwMinuend >= f_dwSubtrahend ) )
    {
        *f_pdwResult = (f_dwMinuend - f_dwSubtrahend);
        return DRM_SUCCESS;
    }
    else
    {
        /*
        ** Initialize out value to ensure that failure
        ** to check return value doesn't result in buffer
        ** overflow if value is then used as pointer offset.
        */
        *f_pdwResult = (DRM_DWORD)0;
         return DRM_E_ARITHMETIC_OVERFLOW;
    }
}

DRM_API DRMMATHSAFEINLINE DRM_RESULT DRM_CALL DRM_DWordSubSame(
    __in                      DRM_DWORD      *f_pdwMinuend,
    __in                      DRM_DWORD       f_dwSubtrahend )
{
    DRMASSERT( f_pdwMinuend != NULL );
    __analysis_assume( f_pdwMinuend != NULL );

    if( DRM_LIKELY( *f_pdwMinuend >= f_dwSubtrahend ) )
    {
        *f_pdwMinuend -= f_dwSubtrahend;
        return DRM_SUCCESS;
    }
    else
    {
        /*
        ** Initialize out value to ensure that failure
        ** to check return value doesn't result in buffer
        ** overflow if value is then used as pointer offset.
        */
        *f_pdwMinuend = (DRM_DWORD)0;
         return DRM_E_ARITHMETIC_OVERFLOW;
    }
}

/* UINT64 subtraction */
DRM_API DRMMATHSAFEINLINE DRM_RESULT DRM_CALL DRM_UInt64Sub(
    __in            DRM_UINT64       f_ui64Minuend,
    __in            DRM_UINT64       f_ui64Subtrahend,
    __out           DRM_UINT64      *f_pui64Result )
{
    DRM_RESULT dr = DRM_SUCCESS;

    DRMASSERT( f_pui64Result != NULL );
    __analysis_assume( f_pui64Result != NULL );

    if( DRM_LIKELY( DRM_UI64GEq( f_ui64Minuend, f_ui64Subtrahend ) ) )
    {
        *f_pui64Result = DRM_UI64Sub( f_ui64Minuend, f_ui64Subtrahend );
    }
    else
    {
        *f_pui64Result = DRM_UI64HL( 0, 0 );
        dr = DRM_E_ARITHMETIC_OVERFLOW;
    }

    return dr;
}

/*
** Multiplication functions
*/

/* WORD multiplication */
DRM_API DRMMATHSAFEINLINE DRM_RESULT DRM_CALL DRM_WordMult(
    __in                      DRM_WORD       f_wMultiplicand,
    __in                      DRM_WORD       f_wMultiplier,
    __out                     DRM_WORD      *f_pwResult )
{
    DRM_DWORD dwResult = ((DRM_DWORD)f_wMultiplicand) * ((DRM_DWORD)f_wMultiplier);

    return DRM_DWordToWord( dwResult, f_pwResult );
}

/* DWORD multiplication */
DRM_API DRMMATHSAFEINLINE DRM_RESULT DRM_CALL DRM_DWordMult(
    __in                      DRM_DWORD       f_dwMultiplicand,
    __in                      DRM_DWORD       f_dwMultiplier,
    __out                     DRM_DWORD      *f_pdwResult )
{
    DRM_UINT64 ui64Result = DRM_UI64Mul( DRM_UI64HL( 0, f_dwMultiplicand ), DRM_UI64HL( 0, f_dwMultiplier ) );

    return DRM_UInt64ToDWord( ui64Result, f_pdwResult );
}


#if DRMMATHSAFEUSEINTRINSICS

DRM_API DRMMATHSAFEINLINE DRM_RESULT DRM_UInt64Mult(
    __in                      DRM_UINT64       f_ui64Multiplicand,
    __in                      DRM_UINT64       f_ui64Multiplier,
    __out                     DRM_UINT64      *f_pui64Result )
{
    DRM_RESULT dr = DRM_SUCCESS;
    unsigned __int64 ulLow  = 0;
    unsigned __int64 ulHigh = 0;
    ulLow = _umul128( f_ui64Multiplicand, f_ui64Multiplier, &ulHigh );
    if( ulHigh == 0 )
    {
        *f_pui64Result = ulLow;
        dr = DRM_SUCCESS;
    }
    else
    {
        *f_pui64Result = 0;
        dr = DRM_E_ARITHMETIC_OVERFLOW;
    }
    return dr;
}

#else /* DRMMATHSAFEUSEINTRINSICS */

/* UINT64 multiplication */
DRM_API DRMMATHSAFEINLINE DRM_RESULT DRM_UInt64Mult(
    __in                      DRM_UINT64       f_ui64Multiplicand,
    __in                      DRM_UINT64       f_ui64Multiplier,
    __out                     DRM_UINT64      *f_pui64Result )
{
    DRM_RESULT dr = DRM_SUCCESS;

    /*
    ** 64x64 into 128 is like 32.32 x 32.32.
    **
    ** a.b * c.d = a*(c.d) + .b*(c.d) = a*c + a*.d + .b*c + .b*.d
    ** back in non-decimal notation where A=a*2^32 and C=c*2^32:
    ** A*C + A*d + b*C + b*d
    ** So there are four components to add together.
    **   result = (a*c*2^64) + (a*d*2^32) + (b*c*2^32) + (b*d)
    **
    ** a * c must be 0 or there would be bits in the high 64-bits
    ** a * d must be less than 2^32 or there would be bits in the high 64-bits
    ** b * c must be less than 2^32 or there would be bits in the high 64-bits
    ** then there must be no overflow of the resulting values summed up.
    */

    DRM_DWORD  dw_a       = 0;
    DRM_DWORD  dw_b       = 0;
    DRM_DWORD  dw_c       = 0;
    DRM_DWORD  dw_d       = 0;
    DRM_UINT64 ad         = DRM_UI64LITERAL( 0, 0 );
    DRM_UINT64 bc         = DRM_UI64LITERAL( 0, 0 );
    DRM_UINT64 bd         = DRM_UI64LITERAL( 0, 0 );
    DRM_UINT64 ui64Result = DRM_UI64LITERAL( 0, 0 );

    DRMASSERT( f_pui64Result != NULL );
    __analysis_assume( f_pui64Result != NULL );

    dr = DRM_E_ARITHMETIC_OVERFLOW;
    *f_pui64Result = DRM_UI64HL( 0, 0 );

    dw_a = DRM_UI64High32( f_ui64Multiplicand );
    dw_c = DRM_UI64High32( f_ui64Multiplier );

    /* common case -- if high dwords are both zero, no chance for overflow */
    if( DRM_LIKELY( ( dw_a == 0 ) && ( dw_c == 0 ) ) )
    {
        dw_b = DRM_UI64Low32( f_ui64Multiplicand );
        dw_d = DRM_UI64Low32( f_ui64Multiplier );

        *f_pui64Result = DRM_UI64Mul( DRM_UI64HL( 0, dw_b ), DRM_UI64HL( 0, dw_d ) );
        dr = DRM_SUCCESS;
    }
    else
    {
        /* a * c must be 0 or there would be bits set in the high 64-bits */
        if( ( dw_a == 0 )
         || ( dw_c == 0 ) )
        {
            dw_d = DRM_UI64Low32( f_ui64Multiplier );

            /* a * d must be less than 2^32 or there would be bits set in the high 64-bits */
            ad = DRM_UI64Mul( DRM_UI64HL( 0, dw_a ), DRM_UI64HL( 0, dw_d ) );

            if( DRM_UI64High32( ad ) == 0 )
            {
                dw_b = DRM_UI64Low32( f_ui64Multiplicand );

                /* b * c must be less than 2^32 or there would be bits set in the high 64-bits */
                bc = DRM_UI64Mul( DRM_UI64HL( 0, dw_b ), DRM_UI64HL( 0, dw_c ) );

                if( DRM_UI64High32( bc ) == 0 )
                {
                    /*
                    ** now sum them all up checking for overflow.
                    ** shifting is safe because we already checked for overflow above
                    */
                    if( DRM_SUCCEEDED( DRM_UInt64Add( DRM_UI64ShL( bc, 32 ),
                                                      DRM_UI64ShL( ad, 32 ), &ui64Result ) ) )
                    {
                        /* b * d */
                        bd = DRM_UI64Mul( DRM_UI64HL( 0, dw_b ), DRM_UI64HL( 0, dw_d ) );

                        if( DRM_SUCCEEDED( DRM_UInt64Add( ui64Result, bd, &ui64Result ) ) )
                        {
                             dr = DRM_SUCCESS;
                            *f_pui64Result = ui64Result;
                        }
                    }
                }
            }
        }
    }

    return dr;
}

#endif /* DRMMATHSAFEUSEINTRINSICS */

/*
** signed operations
**
** Strongly consider using unsigned numbers.
**
** Signed numbers are often used where unsigned numbers should be used.
** For example file sizes and array indices should always be unsigned.
** (File sizes should be 64bit integers; array indices should be SIZE_T.)
** Subtracting a larger positive signed number from a smaller positive
** signed number with IntSubwill succeed, producing a negative number,
** that then must not be used as an array index (but can occasionally be
** used as a pointer index.) Similarly for adding a larger magnitude
** negative number to a smaller magnitude positive number.
**
** drmmathsafe.h does not protect you from such errors. It tells you if your
** integer operations overflowed, not if you are doing the right thing
** with your non-overflowed integers.
**
** Likewise you can overflow a buffer with a non-overflowed unsigned index.
*/

/*
** Signed addition functions
*/

/* LONG Addition */
DRM_API DRMMATHSAFEINLINE DRM_RESULT DRM_CALL DRM_LongAdd(
    __in                      DRM_LONG       f_lAugend,
    __in                      DRM_LONG       f_lAddend,
    __out                     DRM_LONG      *f_plResult )
{
    DRMCASSERT( sizeof(DRM_INT64) > sizeof(DRM_LONG) );
    return DRM_Int64ToLong( DRM_I64Add( DRM_I64( f_lAugend ), DRM_I64( f_lAddend ) ), f_plResult );
}

/*
** Signed subtraction functions
*/

/* LONG Subtraction */
DRM_API DRMMATHSAFEINLINE DRM_RESULT DRM_CALL DRM_LongSub(
    __in                      DRM_LONG       f_lMinuend,
    __in                      DRM_LONG       f_lSubtrahend,
    __out                     DRM_LONG      *f_plResult )
{
    DRMCASSERT( sizeof( DRM_INT64 ) > sizeof( DRM_LONG ) );
    return DRM_Int64ToLong( DRM_I64Sub( DRM_I64( f_lMinuend ), DRM_I64( f_lSubtrahend ) ), f_plResult );
}

/*
** Signed multiplication functions
*/

/* LONG multiplication */
DRM_API DRMMATHSAFEINLINE DRM_RESULT DRM_CALL DRM_LongMult(
    __in                      DRM_LONG       f_lMultiplicand,
    __in                      DRM_LONG       f_lMultiplier,
    __out                     DRM_LONG      *f_plResult )
{
    DRMCASSERT( sizeof( DRM_INT64 ) > sizeof( DRM_LONG ) );
    return DRM_Int64ToLong( DRM_I64Mul( DRM_I64(f_lMultiplicand), DRM_I64(f_lMultiplier) ), f_plResult );
}

EXIT_PK_NAMESPACE_CODE;

