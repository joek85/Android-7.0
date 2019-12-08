/*****************************************************************
|
|   Wasabi - Wasabi AOSP Util
|
|   $Id: WasabiAospUtil.h 10793 2014-06-25 12:46:08Z jebaseelir $
|   Original author: Edin Hodzic (dino@concisoft.com)
|
|   This software is provided to you pursuant to your agreement 
|   with Intertrust Technologies Corporation ("Intertrust").
|   This software may be used only in accordance with the terms 
|   of the agreement.
|
|   Copyright (c) 2013-2014 by Intertrust. All rights reserved. 
|
****************************************************************/

#ifndef _WASABI_AOSP_UTIL_H_
#define _WASABI_AOSP_UTIL_H_

/*----------------------------------------------------------------------
|   includes
+---------------------------------------------------------------------*/
#include <string.h>
#include <utils/Errors.h>

/*----------------------------------------------------------------------
|   namespace
+---------------------------------------------------------------------*/
namespace android {

/*----------------------------------------------------------------------
|   IsWasabiSourceUrl
+---------------------------------------------------------------------*/
static inline bool IsWasabiSourceUrl(const char* url) {
    return
        strncmp(url, "dash://",  sizeof("dash://")-1)  == 0 ||
        strncmp(url, "dashs://", sizeof("dashs://")-1) == 0 ||
        strncmp(url, "ms3://",   sizeof("ms3://")-1)   == 0;
};

/*----------------------------------------------------------------------
|   ReformatArray
+---------------------------------------------------------------------*/
template<typename from_type, typename to_type>
to_type* ReformatArray(const from_type* arr, uint32_t bytes)
{
    uint32_t count = bytes / sizeof(arr[0]);
    to_type* ret_arr = new to_type[count];
    for (uint32_t i = 0; i < count; ++i)
    {
        ret_arr[i] = to_type(arr[i]);
    }
    return ret_arr;
}

/*----------------------------------------------------------------------
|   WSB_PrefixNalus
+---------------------------------------------------------------------*/
// WSB_PrefixAvcNalus takes a buffer of length-payload packetized NAL
// units and produces a buffer of start code prefixed NAL units.
// @param nal_len_size the size of the NAL length, in bytes
// @param src pointer to the first byte of the input buffer
// @param src_len length of the input buffer in bytes
// @param dst pointer to the first byte of the output buffer
// @param dst_len the output buffer capacity in bytes
// @param prefix_len pointer to the output buffer final data length
status_t WSB_PrefixAvcNalus(uint32_t nal_len_size,
                            uint8_t* src, uint32_t src_len, 
                            uint8_t* dst, uint32_t dst_len,
                            uint32_t* prefix_len);

};

#endif // _WASABI_AOSP_UTIL_H_
