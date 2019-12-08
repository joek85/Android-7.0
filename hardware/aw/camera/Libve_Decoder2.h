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

#ifndef __LIBVE_DECORDER2_H__
#define __LIBVE_DECORDER2_H__

#include <sys/mman.h>
#include <sys/ioctl.h>
#include <fcntl.h>

#include <pthread.h>
#include "vencoder.h"  //* video encode library in "LIBRARY/CODEC/VIDEO/ENCODER"
//#include "memoryAdapter.h"
#include "vdecoder.h"

//add by zhengjiangwei for Android N extern void AddVDPlugin(void);
#include <string.h>
#include <dlfcn.h>
#include <dirent.h>

#define DBG_ENABLE 0

#ifdef __cplusplus
extern "C" {
#endif


//add by zhengjiangwei for Android N
extern void AddVDPlugin(void);
extern void AddVDPluginSingle(char *lib);
//extern static int GetLocalPathFromProcessMaps(char *localPath, int len);

void Libve_dec2(VideoDecoder** mVideoDecoder,
                const void *in,
                void *out,
                VideoStreamInfo* pVideoInfo,
                VideoStreamDataInfo* dataInfo,
                VConfig* pVconfig);
int  Libve_init2(VideoDecoder** mVideoDecoder,
                 VideoStreamInfo* pVideoInfo,
                 VConfig* pVconfig);
int  Libve_exit2(VideoDecoder** mVideoDecoder);

#ifdef __cplusplus
}
#endif


#endif  /* __LIBVE_DECORDER2_H__ */

