/*****************************************************************
|
|   Wasabi - Types
|
|   $Id: WsbTypes.h 10793 2014-06-25 12:46:08Z jebaseelir $
|   Original author:  Julien Boeuf
|
|   This software is provided to you pursuant to your agreement
|   with Intertrust Technologies Corporation ("Intertrust").
|   This software may be used only in accordance with the terms
|   of the agreement.
|
|   Copyright (c) 2003-2014 by Intertrust. All rights reserved.
|
****************************************************************/
/** 
* @file
* @brief Wasabi Result
*/
 
 #ifndef _WSB_TYPES_H_
 #define _WSB_TYPES_H_
 
/*----------------------------------------------------------------------
|   includes
+---------------------------------------------------------------------*/
#include "WsbConfig.h"

/*----------------------------------------------------------------------
|   generic types
+---------------------------------------------------------------------*/
/** 
 * @{
 */

typedef unsigned int   WSB_UInt32; /**< 32-bit (or more) unsigned integer */
typedef signed int     WSB_Int32;  /**< 32-bit (or more) signed integer   */
typedef unsigned short WSB_UInt16; /**< 16-bit unsigned integer           */
typedef signed short   WSB_Int16;  /**< 16-bit signed integer             */
typedef unsigned char  WSB_UInt8;  /**< 8-bit unsigned integer            */
typedef signed char    WSB_Int8;   /**< 8-bit signed integer              */
typedef float          WSB_Float;  /**< IEEE float                        */

/* 64-bit types */
#if !defined(WSB_CONFIG_HAVE_INT64)
#error "64 bit types are not supported on this target"
#endif
typedef WSB_CONFIG_INT64_TYPE          WSB_Int64;
typedef unsigned WSB_CONFIG_INT64_TYPE WSB_UInt64;
typedef unsigned WSB_CONFIG_INT64_TYPE WSB_LargeSize;
typedef unsigned WSB_CONFIG_INT64_TYPE WSB_Position;

/*----------------------------------------------------------------------
|   macros
+---------------------------------------------------------------------*/
#if !defined(NULL)
#if defined (__cplusplus)
#define NULL 0
#else
#define NULL ((void*) 0)
#endif
#endif

/*----------------------------------------------------------------------
|   types
+---------------------------------------------------------------------*/
/**
* Boolean type used for variables that can be true (WSB_TRUE) or false
* (WSB_FALSE).
*/
typedef enum {
    WSB_FALSE = 0, 
    WSB_TRUE  = 1
} WSB_Boolean;

/**
* Signed integer value representing a function or method result (return value).
*
* When a function or method call succeeds, the return value is always 
* WSB_SUCCESS unless otherwise documented. Error conditions are always
* negative values, defined in ShiResults.h.
*/
typedef int WSB_Result;

/**
* Type used to represent a bit pattern signifying a combination of flags
* that can be on or off. Bits set to 1 indicate that the corresponding flag
* is on, bits set to 0 indicate that the corresponding flag is off. The
* position and meaning of flags is specific to each method, function, variable
* or data structure that uses this type, and the corresponding header file
* specifies symbolic constants to represent individual flag bits.
*/
typedef WSB_UInt32 WSB_Flags;

/**
* An unsigned integer used to represent a bit mask.
*/
typedef WSB_UInt32 WSB_Mask;

/**
* An unsigned integer used to represent a measurable quantity (e.g., the 
* size of a file).
*/
typedef WSB_UInt32 WSB_Size;

/**
* A signed integer used to represent an offset from a base position value.
*/
typedef WSB_Int32 WSB_Offset;

/**
* An unsigned integer used to represent the difference between a maximum 
* value and a minimum value.
*/
typedef WSB_UInt32 WSB_Range;

/**
* An unsigned integer used to represent a quantity that can be counted (such
* as a number of elements in a list).
*/
typedef WSB_UInt32 WSB_Cardinal;

/**
* An unsigned integer that represents a position in a sequence (such as an
* index into a list of elements).
*/
typedef WSB_UInt32  WSB_Ordinal;

/**
* An 8-bit byte.
*/
typedef WSB_UInt8 WSB_Byte;

/**
* A date and time.
*/
typedef struct {
    WSB_Int32 year;
    WSB_Int32 month;
    WSB_Int32 day;
    WSB_Int32 hours;
    WSB_Int32 minutes;
    WSB_Int32 seconds;
    WSB_Int32 milliseconds;
} WSB_DateTime;

/**
* A timecode suitable for media stream position
*/
typedef struct {
    WSB_UInt8 h;
    WSB_UInt8 m;
    WSB_UInt8 s;
    WSB_UInt8 f;
} WSB_TimeCode;

typedef struct {
    WSB_LargeSize offset;       /**< Offset from start (between 0 and range)*/
    WSB_LargeSize range;        /**< Range of possible offsets              */
} WSB_StreamPosition;

typedef struct {
    WSB_Mask    mask;             /**< Mask indicating which fields are valid */
    WSB_UInt32  nominal_bitrate;  /**< Nominal bitrate                        */
    WSB_UInt32  average_bitrate;  /**< Average bitrate                        */
    WSB_UInt32  instant_bitrate;  /**< Instant bitrate                        */
    WSB_UInt64  size;             /**< Size in bytes                          */
    WSB_UInt64  duration;         /**< Duration in milliseconds               */
    WSB_UInt32  sample_rate;      /**< Sample rate in Hz                      */
    WSB_UInt16  channel_count;    /**< Number of channels                     */
    WSB_Flags   flags;            /**< Stream Flags                           */
   /* WSB_CString data_type;        *//**< Human-readable data type               */
} WSB_StreamInfo;

/**
 * An untyped array of bytes.
 */
typedef struct {
    const unsigned char* data;      /**< Pointer to the bytes in the array */
    WSB_Size             data_size; /**< Number of bytes in the array      */
} WSB_ByteArray;

/** @} */

#endif /* _WSB_TYPES_H_ */
