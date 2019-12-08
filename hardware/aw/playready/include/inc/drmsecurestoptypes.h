/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#ifndef _DRMSECURESTOPTYPES_H_
#define _DRMSECURESTOPTYPES_H_ 1

ENTER_PK_NAMESPACE;

#define DRM_SECURESTOP_MAX_MESSAGES     100
#define DRM_SECURESTOP_MAX_TRANSACTIONS 100
#define DRM_SECURESTOP_UPDATE_INTERVAL  600

typedef struct __tag_DRM_SECURESTOP_MESSAGE
{
    DRM_ID      idSession;
    DRM_ID      idPublisher;
    DRM_DWORD   dwStartTime;
    DRM_DWORD   dwUpdateTime;
    DRM_BOOL    fStopped;
    DRM_BOOL    fSuspended;
    DRM_BOOL    fLocked;
} DRM_SECURESTOP_MESSAGE;

typedef struct __tag_DRM_SECURESTOP_DECRYPTOR
{
    DRM_CIPHER_CONTEXT                    *pCipherContext;
    struct __tag_DRM_SECURESTOP_DECRYPTOR *pNext;
} DRM_SECURESTOP_DECRYPTOR;

typedef struct __tag_DRM_SECURESTOP_SESSION
{
    DRM_SECURESTOP_MESSAGE    oMessage;
    DRM_DWORD                 cidLicenses;
    DRM_LID                  *pidLicenses;
    DRM_SECURESTOP_DECRYPTOR *pDecryptors;
} DRM_SECURESTOP_SESSION;

typedef struct __tag_DRM_SECURESTOP_TRANSACTION
{
    DRM_ID   idTransaction;
    DRM_ID   idPublisher;
    DRM_BYTE rgbKey[DRM_AES_KEYSIZE_128];
} DRM_SECURESTOP_TRANSACTION;

typedef struct __tag_DRM_SECURESTOP_DATA
{
    DRM_ID                     idEnvironment;
    DRM_DWORD                  dwGenerationID;
    DRM_DWORD                  cMessages;
    DRM_SECURESTOP_MESSAGE     rgMessages[DRM_SECURESTOP_MAX_MESSAGES];
    DRM_DWORD                  cTransactions;
    DRM_SECURESTOP_TRANSACTION rgTransactions[DRM_SECURESTOP_MAX_TRANSACTIONS];
} DRM_SECURESTOP_DATA;

typedef struct __tag_DRM_SECURESTOP_CONTEXT
{
    DRM_DWORD              cSessions;
    DRM_SECURESTOP_SESSION rgSessions[DRM_SECURESTOP_MAX_MESSAGES];
} DRM_SECURESTOP_CONTEXT;

EXIT_PK_NAMESPACE;

#endif /* _DRMSECURESTOPTYPES_H_ */
