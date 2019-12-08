/*
 * Copyright (C) 2008 The Android Open Source Project
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

#ifndef _DEVICECONTROLER_H
#define _DEVICECONTROLER_H

#include <utils/Thread.h>

class deviceControler {
	mutable android::Mutex mLock;
	int dispfd;

public:
	deviceControler();
	~deviceControler();

	int disp_ioctl(int cmd, unsigned long args);
	int isSupportHdmiMode(int display, int mode);
	int isSupport3DMode(int display, int mode);

private:
	int getIdxByOutputType(int type);
};

#endif
