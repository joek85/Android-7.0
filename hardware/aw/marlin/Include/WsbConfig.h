/*****************************************************************
|
|   Wasabi - Types
|
|   $Id: WsbConfig.h 10793 2014-06-25 12:46:08Z jebaseelir $
|   Original author:  Gilles Boccon-Gibod
|
|   This software is provided to you pursuant to your agreement
|   with Intertrust Technologies Corporation ("Intertrust").
|   This software may be used only in accordance with the terms
|   of the agreement.
|
|   Copyright (c) 2003-2014 by Intertrust. All rights reserved.
|
****************************************************************/

#ifndef _WSB_CONFIG_H_
#define _WSB_CONFIG_H_

/*----------------------------------------------------------------------
|   defaults
+---------------------------------------------------------------------*/
#define WSB_CONFIG_HAVE_INT64

/*----------------------------------------------------------------------
|   platform specifics
+---------------------------------------------------------------------*/

/* Microsoft Platforms */
#if defined(_MSC_VER)
#define WSB_CONFIG_INT64_TYPE __int64
#endif

/*----------------------------------------------------------------------
|    defaults
+---------------------------------------------------------------------*/
#if !defined(WSB_CONFIG_INT64_TYPE)
#define WSB_CONFIG_INT64_TYPE long long
#endif

/*----------------------------------------------------------------------
|    WSB_EXPORT
+---------------------------------------------------------------------*/
#if defined(__APPLE__) || defined(GCC_SYMBOL_VISIBILITY_SUPPORT)
#define WSB_EXPORT __attribute__((visibility("default")))
#elif defined(WIN32)
#define WSB_EXPORT __declspec(dllexport)
#else
#define WSB_EXPORT /* empty */
#endif /* WSB_EXPORT */

#endif /* _WSB_CONFIG_H_ */
