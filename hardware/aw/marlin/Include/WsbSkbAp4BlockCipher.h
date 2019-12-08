/*****************************************************************
|
|   Wasabi Example - AP4_BlockCipherFactory implemention using SKB
|
|   $Id: WsbSkbAp4BlockCipher.h 10793 2014-06-25 12:46:08Z jebaseelir $
|   Original author: Edin Hodzic (dino@concisoft.com)
|
|   This software is provided to you pursuant to your agreement
|   with Intertrust Technologies Corporation ("Intertrust").
|   This software may be used only in accordance with the terms
|   of the agreement.
|
|   Copyright (c) 2009-2014 by Intertrust. All rights reserved.
|
****************************************************************/

#ifndef WSB_BLOCK_CIPHER_H
#define WSB_BLOCK_CIPHER_H

/*----------------------------------------------------------------------
|   includes
+---------------------------------------------------------------------*/

#include "Ap4.h"
#include "WsbTypes.h"
#include "WsbResults.h"

/*----------------------------------------------------------------------
|   declarations
+---------------------------------------------------------------------*/

#ifdef __cplusplus
extern "C" {
#endif

// The caller has to delete the returned object when done.
WSB_EXPORT WSB_Result 
WSB_SkbAp4BlockCipherFactory_Create(AP4_BlockCipherFactory*& factory);

#ifdef __cplusplus
}
#endif

#endif // WSB_BLOCK_CIPHER_H
