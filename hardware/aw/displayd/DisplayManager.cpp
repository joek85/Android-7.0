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

#include "helper.h"
#include "DisplayManager.h"
#include "libhwcproxy/hwcomposerInterface.h"
#include <cutils/log.h>

DisplayManager *DisplayManager::instance = new DisplayManager();

DisplayManager *
DisplayManager::getInstance() {
	return instance;
}

DisplayManager::DisplayManager() {
	mProxy = new HWC1Proxy();
	mControler = new deviceControler();
}

deviceControler*
DisplayManager::getControler() {
	return mControler;
}

int
DisplayManager::listInterface(int display, std::vector<int> *interfaces) {
	interfaces->clear();
	int type = mProxy->getOutputType(display);
	interfaces->push_back(type);
	return 0;
}

int
DisplayManager::getCurrentInterface(int display, int *interface) {
	*interface = mProxy->getOutputType(display);
	return 0;
}

int
DisplayManager::setCurrentInterface(int display, int interface, int enable) {

	int current = mProxy->getOutputType(display);
	if ((current != interface) && current != DISP_OUTPUT_TYPE_NONE) {
		ALOGE("display %d not support type %d", display, interface);
		return -1;
	}
	if (enable)
		mProxy->setOutputMode(display, interface, 0xff);
	else
		mProxy->setOutputMode(display, 0, 0);

	return 0;
}

int
DisplayManager::listMode(int display, int interface, std::vector<int> *modes) {
	if (mProxy->getOutputType(display) != interface) {
		ALOGE("display %d not support type %d", display, interface);
		return -1;
	}

	if (interface == DISP_OUTPUT_TYPE_HDMI) {
		std::vector<int> maximum;
		getHdmiModes(&maximum);
		for (int m : maximum) {
			if (isSupportMode(display, m) == 1)
				modes->push_back(m);
		}
	} else if (interface == DISP_OUTPUT_TYPE_TV) {
		getCvbsModes(modes);
	}
	return 0;
}

int
DisplayManager::getCurrentMode(int display, int interface, int *mode) {
	if (mProxy->getOutputType(display) != interface) {
		ALOGE("display %d not support type %d", display, interface);
		return -1;
	}
	*mode = mProxy->getOutputMode(display);
	return 0;
}

int
DisplayManager::setCurrentMode(int display, int interface, int mode) {
	if (mProxy->getOutputType(display) != interface) {
		ALOGE("display %d not support type %d", display, interface);
		return -1;
	}
	if (interface == DISP_OUTPUT_TYPE_HDMI && isSupportMode(display, mode) != 1) {
		ALOGE("display %d not support hdmi mode %d", display, mode);
		return -1;
	}
	mProxy->setOutputMode(display, interface, mode);
	return 0;
}

int
DisplayManager::setCurrentMode_FORCE(int display, int interface, int mode) {
	ALOGD("Force: display %d type %d mode %d", display, interface, mode);
	mProxy->setOutputMode(display, interface, mode);
	return 0;
}

int
DisplayManager::getOutputFormat(int display) {
	int type, mode;
	if (mProxy->getOutputFormat(display, &type, &mode) == 0)
		return ((type << 8) | mode);
	return 0;
}

int
DisplayManager::setOutputFormat(int display, int format) {
	int type = (format >> 8) & 0xff;
	int mode = (format >> 0) & 0xff;

	if (mProxy->getOutputType(display) != type) {
		ALOGE("display %d not support type %d", display, type);
		return -1;
	}
	if (isSupportMode(display, mode) != 1) {
		ALOGW("display %d not support mode %d", display, mode);
		if (isCommonMode(mode) == 0)
			return -1;
	}
	return mProxy->setOutputMode(display, type, mode);
}

int
DisplayManager::getOutputType(int display) {
	return mProxy->getOutputType(display);
}

int
DisplayManager::getOutputMode(int display) {
	return mProxy->getOutputMode(display);
}

int
DisplayManager::setOutputMode(int display, int mode) {

	if (isSupportMode(display, mode) != 1) {
		ALOGW("#display %d not support mode %d", display, mode);
		if (isCommonMode(mode) == 0)
			return -1;
	}

	int type = mProxy->getOutputType(display);
	return mProxy->setOutputMode(display, type, mode);
}

int
DisplayManager::getOutputPixelFormat(int display) {
	int value;
	if (0 == mProxy->getOutputPixelFormat(display, &value)) {
		return value;
	} else {
		return -1;
	}
}

int
DisplayManager::setOutputPixelFormat(int display, int value) {
	return mProxy->setOutputPixelFormat(display, value);
}

int
DisplayManager::getOutputCurDataspaceMode(int display) {
	int value;
	if (0 == mProxy->getOutputCurDataspaceMode(display, &value)) {
		return value;
	} else {
		return -1;
	}
}

int
DisplayManager::getOutputDataspaceMode(int display) {
	int value;
	if (0 == mProxy->getOutputDataspaceMode(display, &value)) {
		return value;
	} else {
		return -1;
	}
}

int
DisplayManager::setOutputDataspaceMode(int display, int value) {
	return mProxy->setOutputDataspaceMode(display, value);
}

int
DisplayManager::isSupportMode(int display, int mode) {
	int type = mProxy->getOutputType(display);
	if (type == DISP_OUTPUT_TYPE_TV)
		return (mode == DISP_TV_MOD_PAL) || (mode == DISP_TV_MOD_NTSC);

	if (type == DISP_OUTPUT_TYPE_HDMI)
		return mControler->isSupportHdmiMode(display, mode);
	return 0;
}

int
DisplayManager::isCommonMode(int mode)
{
	const int commond_mods[] = {
		DISP_TV_MOD_720P_50HZ,
		DISP_TV_MOD_720P_60HZ,

		DISP_TV_MOD_1080P_50HZ,
		DISP_TV_MOD_1080P_60HZ,

		DISP_TV_MOD_NTSC,
		DISP_TV_MOD_PAL,
	};

	for (size_t i = 0; i < sizeof(commond_mods) / sizeof(commond_mods[0]); i++) {
		if (commond_mods[i] == mode)
			return 1;
	}
	return 0;
}

int
DisplayManager::listSupportModes(int display, std::vector<int> *modes) {
	int type = mProxy->getOutputType(display);
	if (type == DISP_OUTPUT_TYPE_HDMI) {
		std::vector<int> maximum;
		getHdmiModes(&maximum);
		for (int m : maximum) {
			if (isSupportMode(display, m) == 1)
				modes->push_back(m);
		}
		if (modes->size() == 0) {
			modes->push_back(DISP_TV_MOD_720P_50HZ);
			modes->push_back(DISP_TV_MOD_720P_60HZ);
			modes->push_back(DISP_TV_MOD_1080P_50HZ);
			modes->push_back(DISP_TV_MOD_1080P_60HZ);
		}
	} else if (type == DISP_OUTPUT_TYPE_TV) {
		getCvbsModes(modes);
	}
	return 0;
}

int
DisplayManager::isSupport3DMode(int display, int mode) {
	if (getOutputType(display) != DISP_OUTPUT_TYPE_HDMI)
		return 0;
	if (mode == -1)
		mode = DISP_TV_MOD_1080P_24HZ_3D_FP;
	return mControler->isSupport3DMode(display, mode);
}

int
DisplayManager::getDisplayMargin(int display, int *margin_x, int *margin_y) {
	return mProxy->getScreenMargin(display, margin_x, margin_y);
}

int
DisplayManager::setDisplayMargin(int display, int margin_x, int margin_y) {
	return mProxy->setScreenMargin(display, margin_x, margin_y);
}

int
DisplayManager::getDisplayOffset(int display, int *offset_x, int *offset_y) {
	return mProxy->getScreenOffset(display, offset_x, offset_y);
}

int
DisplayManager::setDisplayOffset(int display, int offset_x, int offset_y) {
	return mProxy->setScreenOffset(display, offset_x, offset_y);
}

int
DisplayManager::set3DLayerMode(int display, int mode, int videoCropHeight) {
	return mProxy->set3DLayerMode(display, mode, videoCropHeight);
}

int
DisplayManager::getDisplayEdge(int display) {
	return mProxy->getDisplayEnhanceComponent(display, ENHANCE_EDGE);
}

int
DisplayManager::setDisplayEdge(int display, int target) {
	return mProxy->setDisplayEnhanceComponent(display, ENHANCE_EDGE, target);
}

int
DisplayManager::getDisplayDetail(int display) {
	return mProxy->getDisplayEnhanceComponent(display, ENHANCE_DETAIL);
}

int
DisplayManager::setDisplayDetail(int display, int target) {
	return mProxy->setDisplayEnhanceComponent(display, ENHANCE_DETAIL, target);
}

int
DisplayManager::getDisplayBright(int display) {
	return mProxy->getDisplayEnhanceComponent(display, ENHANCE_BRIGHT);
}

int
DisplayManager::setDisplayBright(int display, int target) {
	return mProxy->setDisplayEnhanceComponent(display, ENHANCE_BRIGHT, target);
}

int
DisplayManager::getDisplayDenoise(int display) {
	return mProxy->getDisplayEnhanceComponent(display, ENHANCE_DENOISE);
}

int
DisplayManager::setDisplayDenoise(int display, int target) {
	return mProxy->setDisplayEnhanceComponent(display, ENHANCE_DENOISE, target);
}

int
DisplayManager::getDisplayContrast(int display) {
	return mProxy->getDisplayEnhanceComponent(display, ENHANCE_CONTRAST);
}

int
DisplayManager::setDisplayContrast(int display, int target) {
	return mProxy->setDisplayEnhanceComponent(display, ENHANCE_CONTRAST, target);
}

int
DisplayManager::getDisplaySaturation(int display) {
	return mProxy->getDisplayEnhanceComponent(display, ENHANCE_SATURATION);
}

int
DisplayManager::setDisplaySaturation(int display, int target) {
	return mProxy->setDisplayEnhanceComponent(display, ENHANCE_SATURATION, target);
}

int
DisplayManager::getDisplayEnhanceMode(int display) {
	return mProxy->getDisplayEnhanceMode(display);
}

int
DisplayManager::setDisplayEnhanceMode(int display, int mode) {
	return mProxy->setDisplayEnhanceMode(display, mode);
}

int
DisplayManager::getDeviceConfig(int display, struct disp_device_config *config) {
	int type = mProxy->getOutputType(display);
	if (type != DISP_OUTPUT_TYPE_HDMI && type != DISP_OUTPUT_TYPE_TV) {
		ALOGE("getDeviceConfig: error type '%d' on display %d", type, display);
		return -1;
	}
	unsigned long args[4] = {0};
	for (int i = 0; i < 2; ++i) {
		args[0] = i;
		args[1] = (unsigned long)config;
		if (mControler->disp_ioctl(
				DISP_DEVICE_GET_CONFIG, (unsigned long)args) == 0) {
			if (config->type == type)
				return 0;
		}
	}
	ALOGE("getDeviceConfig: can't find device of type '%d'", type);
	return -1;
}

int
DisplayManager::setDeviceConfig(int display, struct disp_device_config *config) {
	return mProxy->setDeviceConfig(display, config);
}

