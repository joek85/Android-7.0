/*
 * Copyright (C) 2010 The Android Open Source Project
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

#ifndef _INIT_LOG_H_
#define _INIT_LOG_H_

#define __ANDROID_LOG__
#ifdef __ANDROID_LOG__
#include <cutils/log.h>
#define TAG     "hwc-writeback"
#define logerr(fmt, args...)        ALOGE(fmt, ##args)
#define loginfo(fmt, args...)       ALOGI(fmt, ##args)
#define logdebug(fmt, args...)      ALOGD(fmt, ##args)
#else
#define TAG     "hwc-writeback"
#define logerr(fmt, args...)        printf(TAG fmt, ##args)
#define loginfo(fmt, args...)       printf(TAG fmt, ##args)
#define logdebug(fmt, args...)      printf(TAG fmt, ##args)
#endif

#endif
