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

#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <cutils/log.h>
#include "debug.h"
#include "sunxi_display2.h"
#include "deviceControler.h"

deviceControler::deviceControler() {
	dispfd = open("/dev/disp", O_RDWR);
	if (dispfd < 0)
		log_error("can't open disp device: %s(%d)", strerror(errno), errno);
}

deviceControler::~deviceControler(void) {
	if (dispfd > 0)
		close(dispfd);
}

int deviceControler::disp_ioctl(int cmd, unsigned long args) {
	android::Mutex::Autolock _l(mLock);
	if (dispfd >= 0)
		return ioctl(dispfd, cmd, args);
	return 0;
}

int
deviceControler::getIdxByOutputType(int type) {
	unsigned long args[4] = {0};
	for (int i = 0; i < 2; i++) {
		args[0] = i;
		if (ioctl(dispfd, DISP_GET_OUTPUT_TYPE, (unsigned long)args) == type)
			return i;
	}
	return -1;
}

int
deviceControler::isSupportHdmiMode(int display, int mode) {
	unsigned long args[4] = {0};
	int hwid = getIdxByOutputType(DISP_OUTPUT_TYPE_HDMI);

	if (hwid >= 0) {
		args[0] = hwid;
		args[1] = mode;
		return ioctl(dispfd, DISP_HDMI_SUPPORT_MODE, (unsigned long)args);
	}
	log_error("can't find hdmi output(%d)", display);
	return 0;
}

int
deviceControler::isSupport3DMode(int display, int mode) {
	/* only support DISP_TV_MOD_1080P_24HZ_3D_FP now */
	if (mode != DISP_TV_MOD_1080P_24HZ_3D_FP)
		return 0;
	return isSupportHdmiMode(display, mode);
}
