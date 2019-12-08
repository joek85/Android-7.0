/*****************************************************************
|
|   Wasabi - Wasabi Source for the NuPlayer
|
|   $Id: WasabiSource.h 10793 2014-06-25 12:46:08Z jebaseelir $
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

#ifndef _WASABI_SOURCE_H_
#define _WASABI_SOURCE_H_

/*----------------------------------------------------------------------
|   includes
+---------------------------------------------------------------------*/
#include <media/stagefright/MetaData.h>
#include <NuPlayer.h>
#include <NuPlayerSource.h>

/*----------------------------------------------------------------------
|   namespace
+---------------------------------------------------------------------*/
namespace android {

    NuPlayer::Source* WasabiSourceCreate(
        const sp<AMessage> &notify,
        const char *url,
        const KeyedVector<String8, String8> *headers,
        bool uidValid = false,
        uid_t uid = 0);

};

#endif // _WASABI_SOURCE_H_
