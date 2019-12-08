/*
 * Copyright (c) 2008-2015 Allwinner Technology Co. Ltd.
 * All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef __OSAL_MUTEX__
#define __OSAL_MUTEX__

#include <OMX_Types.h>
#include <OMX_Core.h>

#ifdef __cplusplus
extern "C" {
#endif

OMX_ERRORTYPE OSAL_MutexCreate(OMX_HANDLETYPE *mutexHandle);
OMX_ERRORTYPE OSAL_MutexTerminate(OMX_HANDLETYPE mutexHandle);
OMX_ERRORTYPE OSAL_MutexLock(OMX_HANDLETYPE mutexHandle);
OMX_ERRORTYPE OSAL_MutexUnlock(OMX_HANDLETYPE mutexHandle);

#ifdef __cplusplus
}
#endif


#endif

