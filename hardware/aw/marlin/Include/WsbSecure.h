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

#ifndef _WASABI_SECURE_H_
#define _WASABI_SECURE_H_

/*----------------------------------------------------------------------
|   includes
+---------------------------------------------------------------------*/
#include <string.h>
#include <utils/Errors.h>

#include "WsbTypes.h"
#include "WsbResults.h"
#include "Ap4Config.h"
#include "Ap4Types.h"
#include "Ap4Protection.h"
#include "Ap4CommonEncryption.h"
//#include "Ap4StreamCipher.h"

/*----------------------------------------------------------------------
|   namespace
+---------------------------------------------------------------------*/
namespace android {


WSB_Result WSBSecure_init();
WSB_Result WSBSecure_term();
WSB_Result WSBSecure_SkbAp4BlockCipherFactory_Create(AP4_BlockCipherFactory*& factory);
WSB_Result WSBSecure_SkbAp4BlockCipherFactory_Destroy(AP4_BlockCipherFactory* factory);

AP4_Result WSBSecure_AP4_CencSingleSampleDecrypter_Create(AP4_UI32               algorithm_id,
                                      const AP4_UI08*                 key,
                                      AP4_Size                        key_size,
                                      AP4_BlockCipherFactory*         block_cipher_factory,
                                      AP4_CencSingleSampleDecrypter*& decrypter);
AP4_Result WSBSecure_AP4_CencSingleSampleDecrypter_Destroy(AP4_CencSingleSampleDecrypter* decrypter);
/* data_out: in secure memory */
AP4_Result WSBSecure_AP4_CencSingleSampleDecrypter_DecryptSampleData(AP4_CencSingleSampleDecrypter *dec,
                                                 AP4_DataBuffer& data_in,
                                                 AP4_DataBuffer& data_out,
                                                 const AP4_UI08* iv,
                                                 unsigned int    subsample_count,
                                                 const AP4_UI16* bytes_of_cleartext_data,
                                                 const AP4_UI32* bytes_of_encrypted_data);
int WSBSecure_PrefixAvcNalus(uint32_t nal_len_size,
                   uint8_t* src, uint32_t src_len, 
                   uint8_t* dst, uint32_t dst_len,
                   uint32_t* prefixed_len);
};

#endif // _WASABI_SECURE_H_
