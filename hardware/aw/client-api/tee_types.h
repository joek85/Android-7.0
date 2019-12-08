/*
 * Copyright (c) 2013 NVIDIA Corporation. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#ifndef __TEE_TYPES_H__
#define __TEE_TYPES_H__
#include <stdint.h>
/*
 * Return Codes
 */
typedef enum TEEC_Result{
/*!The operation succeeded. \n*/
    TEEC_SUCCESS = 0x0,
/*!Non-specific cause.*/
    TEEC_ERROR_GENERIC = 0xFFFF0000,
/*!Access privileges are not sufficient.*/
    TEEC_ERROR_ACCESS_DENIED = 0xFFFF0001 ,
/*!The operation was cancelled.*/
    TEEC_ERROR_CANCEL = 0xFFFF0002 ,
/*!Concurrent accesses caused conflict.*/
    TEEC_ERROR_ACCESS_CONFLICT = 0xFFFF0003 ,
/*!Too much data for the requested operation was passed.*/
    TEEC_ERROR_EXCESS_DATA = 0xFFFF0004 ,
/*!Input data was of invalid format.*/
    TEEC_ERROR_BAD_FORMAT = 0xFFFF0005 ,
/*!Input parameters were invalid.*/
    TEEC_ERROR_BAD_PARAMETERS = 0xFFFF0006 ,
/*!Operation is not valid in the current state.*/
    TEEC_ERROR_BAD_STATE = 0xFFFF0007,
/*!The requested data item is not found.*/
    TEEC_ERROR_ITEM_NOT_FOUND = 0xFFFF0008,
/*!The requested operation should exist but is not yet implemented.*/
    TEEC_ERROR_NOT_IMPLEMENTED = 0xFFFF0009,
/*!The requested operation is valid but is not supported in this
* Implementation.*/
    TEEC_ERROR_NOT_SUPPORTED = 0xFFFF000A,
/*!Expected data was missing.*/
    TEEC_ERROR_NO_DATA = 0xFFFF000B,
/*!System ran out of resources.*/
    TEEC_ERROR_OUT_OF_MEMORY = 0xFFFF000C,
/*!The system is busy working on something else. */
    TEEC_ERROR_BUSY = 0xFFFF000D,
/*!Communication with a remote party failed.*/
    TEEC_ERROR_COMMUNICATION = 0xFFFF000E,
/*!A security fault was detected.*/
    TEEC_ERROR_SECURITY = 0xFFFF000F,
/*!The supplied buffer is too short for the generated output.*/
    TEEC_ERROR_SHORT_BUFFER = 0xFFFF0010,
/*! The MAC value supplied is different from the one calculated */
    TEEC_ERROR_MAC_INVALID = 0xFFFF3071,
}TEEC_Result;


#ifdef __OS_LINUX
#ifndef LOG_NDEBUG
#define LOG_NDEBUG 1
#endif

#if LOG_NDEBUG
#define ALOGV(...)   ((void)0)
#else
#ifndef ALOGV
#define ALOGV(...) ((void)printf("V/" LOG_TAG ": "));         \
		((void)printf("(%d) ",__LINE__));      \
		((void)printf(__VA_ARGS__));          \
		((void)printf("\n"))

#endif
#endif

#ifndef ALOGD
#define ALOGD(...) ((void)printf("D/" LOG_TAG ": "));         \
		((void)printf("(%d) ",__LINE__));      \
		((void)printf(__VA_ARGS__));          \
		((void)printf("\n"))
#endif

#ifndef ALOGI
#define ALOGI(...) ((void)printf("I/" LOG_TAG ": "));         \
		((void)printf("(%d) ",__LINE__));      \
		((void)printf(__VA_ARGS__));          \
		((void)printf("\n"))
#endif

#ifndef ALOGW
#define ALOGW(...) ((void)printf("W/" LOG_TAG ": "));         \
		((void)printf("(%d) ",__LINE__));      \
		((void)printf(__VA_ARGS__));          \
		((void)printf("\n"))
#endif

#ifndef ALOGE
#define ALOGE(...) ((void)printf("E/" LOG_TAG ": "));         \
		((void)printf("(%d) ",__LINE__));      \
		((void)printf(__VA_ARGS__));          \
		((void)printf("\n"))
#endif
#else
#endif

typedef uint32_t TEE_Result;

#endif
