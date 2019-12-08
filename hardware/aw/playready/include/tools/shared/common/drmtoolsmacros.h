/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/
#pragma once

ENTER_PK_NAMESPACE;

#define CallXmlBuilder(context, fn)   {\
    while (TRUE)\
    {\
        dr = (fn);\
        if ( dr == DRM_E_BUFFERTOOSMALL )\
        {\
            ChkDR(DRMTOOLS_ReallocXmlContext(&(context)));\
            continue;\
        }\
        ChkDR(dr);\
        break;\
    }\
}

#define MAX_FILENAME 1023
#define XML_INIT_SIZE   1024

EXIT_PK_NAMESPACE;
