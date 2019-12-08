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

#include <string>
#include <cutils/log.h>
#include "HWC1Proxy.h"
#include "displaydCommand.h"
#include "sunxi_display2.h"

int
HWC1Proxy::HWC1Response::getSeqnum(void) {
	if (mArgs.size() < 3)
		return -1;
	return 0;
}

int
HWC1Proxy::HWC1Response::getResult(void) {
	if (mArgs.size() < 3)
		return -1;
	return strtoul(mArgs[1], 0 ,0);
}

int
HWC1Proxy::HWC1Response::getPayload(unsigned int index) {
	if (mArgs.size() < (3 + index))
		return -1;
	return strtoul(mArgs[2+index], 0 ,0);
}

HWC1Proxy::HWC1Proxy()
	: mConnector(new socketConnector()) {
	}

int HWC1Proxy::sendRequest(char *request, HWC1Proxy::HWC1Response *response) {
	return mConnector->sendRequest(request, response);
}

int
HWC1Proxy::getOutputFormat(int display, int *type, int *mode) {
	android::Mutex::Autolock _l(mEntryLock);
	HWC1Proxy::HWC1Response response;
	char request[512];

	sprintf(request, "%s %d", "interface GetFormat", display);
	if (sendRequest(request, &response) == 0) {
		*type = response.getPayload(0);
		*mode = response.getPayload(1);
		return 0;
	}
	*type = 0;
	*mode = 0;
	return -1;
}

int
HWC1Proxy::getOutputType(int display) {
	int type, mode;
	if (getOutputFormat(display, &type, &mode) == 0)
		return type;
	return -1;
}

int
HWC1Proxy::getOutputMode(int display) {
	int type, mode;
	if (getOutputFormat(display, &type, &mode) == 0)
		return mode;
	return -1;
}

int
HWC1Proxy::setOutputFormat(int display, int type, int mode) {
	android::Mutex::Autolock _l(mEntryLock);
	HWC1Proxy::HWC1Response response;
	char request[512];

	sprintf(request, "%s %d %d %d", "interface SetFormat", display, type, mode);
	sendRequest(request, &response);
	return response.getResult();
}

int
HWC1Proxy::setOutputMode(int display, int type, int mode) {
	return setOutputFormat(display, type, mode);
}

int
HWC1Proxy::getOutputPixelFormat(int display, int *value) {
	android::Mutex::Autolock _l(mEntryLock);
	HWC1Proxy::HWC1Response response;
	char request[512];

	sprintf(request, "%s %d", "interface GetPixelFormat", display);
	if (sendRequest(request, &response) == 0) {
		*value = response.getPayload(0);
		return 0;
	}
	return -1;
}

int
HWC1Proxy::setOutputPixelFormat(int display, int value) {
	android::Mutex::Autolock _l(mEntryLock);
	HWC1Proxy::HWC1Response response;
	char request[512];

	sprintf(request, "%s %d %d", "interface SetPixelFormat", display, value);
	sendRequest(request, &response);
	return response.getResult();
}

int
HWC1Proxy::getOutputCurDataspaceMode(int display, int *value) {
	android::Mutex::Autolock _l(mEntryLock);
	HWC1Proxy::HWC1Response response;
	char request[512];

	sprintf(request, "%s %d", "interface GetCurDataspace", display);
	if (sendRequest(request, &response) == 0) {
		*value = response.getPayload(0);
		return 0;
	}
	return -1;
}

int
HWC1Proxy::getOutputDataspaceMode(int display, int *value) {
	android::Mutex::Autolock _l(mEntryLock);
	HWC1Proxy::HWC1Response response;
	char request[512];

	sprintf(request, "%s %d", "interface GetDataspace", display);
	if (sendRequest(request, &response) == 0) {
		*value = response.getPayload(0);
		return 0;
	}
	return -1;
}

int
HWC1Proxy::setOutputDataspaceMode(int display, int value) {
	android::Mutex::Autolock _l(mEntryLock);
	HWC1Proxy::HWC1Response response;
	char request[512];

	sprintf(request, "%s %d %d", "interface SetDataspace", display, value);
	sendRequest(request, &response);
	return response.getResult();
}

int
HWC1Proxy::getOverscan(int display,
		int *margin_x, int *margin_y, int *offset_x, int *offset_y) {
	android::Mutex::Autolock _l(mEntryLock);
	HWC1Proxy::HWC1Response response;
	char request[512];

	sprintf(request, "%s %d", "overscan GetOverscan", display);
	if (sendRequest(request, &response) == 0) {
		if (margin_x) *margin_x = response.getPayload(0);
		if (margin_y) *margin_y = response.getPayload(1);
		if (offset_x) *offset_x = response.getPayload(2);
		if (offset_y) *offset_y = response.getPayload(3);
		return 0;
	}
	return -1;
}

int
HWC1Proxy::getScreenMargin(int display, int *margin_x, int *margin_y) {
	return getOverscan(display, margin_x, margin_y, NULL, NULL);
}

int
HWC1Proxy::getScreenOffset(int display, int *offset_x, int *offset_y) {
	return getOverscan(display, NULL, NULL, offset_x, offset_y);
}

int
HWC1Proxy::setOverscan(int display,
		int margin_x, int margin_y, int offset_x, int offset_y) {
	android::Mutex::Autolock _l(mEntryLock);
	HWC1Proxy::HWC1Response response;
	char request[512];

	sprintf(request, "%s %d %d %d %d %d", "overscan SetOverscan",
					display, margin_x, margin_y, offset_x, offset_y);
	return sendRequest(request, &response);
}

int
HWC1Proxy::setScreenMargin(int display, int margin_x, int margin_y) {
	int tmp[4];
	int ret = getOverscan(display, &tmp[0], &tmp[1], &tmp[2], &tmp[3]);

	if (!ret && (margin_x != tmp[0] || margin_y != tmp[1]))
		return setOverscan(display, margin_x, margin_y, tmp[2], tmp[3]);
	return 0;
}

int
HWC1Proxy::setScreenOffset(int display, int offset_x, int offset_y) {
	int tmp[4];
	int ret = getOverscan(display, &tmp[0], &tmp[1], &tmp[2], &tmp[3]);

	if (!ret && (offset_x != tmp[2] || offset_y != tmp[3]))
		return setOverscan(display, tmp[0], tmp[1], offset_x, offset_y);
	return 0;
}

int
HWC1Proxy::setScreenRadio(int display, int radio) {
	android::Mutex::Autolock _l(mEntryLock);
	HWC1Proxy::HWC1Response response;
	char request[512];

	sprintf(request, "%s %d %d", "overscan SetScreenRadio", display, radio);
	return sendRequest(request, &response);
}

int
HWC1Proxy::get3DLayerMode(int display) {
	android::Mutex::Autolock _l(mEntryLock);
	HWC1Proxy::HWC1Response response;
	char request[512];

	sprintf(request, "%s %d", "utils Get3Dmode", display);
	if (sendRequest(request, &response) == 0) {
		return response.getPayload(0);
	}
	return -1;
}

int
HWC1Proxy::set3DLayerMode(int display, int mode, int videoCropHeight) {
	android::Mutex::Autolock _l(mEntryLock);
	HWC1Proxy::HWC1Response response;
	char request[512];

	sprintf(request, "%s %d %d %d", "utils Set3Dmode",
		display, mode, videoCropHeight);
	sendRequest(request, &response);
	return response.getResult();
}

int
HWC1Proxy::getDisplayEnhanceMode(int display) {
	android::Mutex::Autolock _l(mEntryLock);
	HWC1Proxy::HWC1Response response;
	char request[512];

	sprintf(request, "%s %d", "enhance GetMode", display);
	if (sendRequest(request, &response) == 0) {
		return response.getPayload(0);
	}
	return -1;
}

int
HWC1Proxy::setDisplayEnhanceMode(int display, int mode) {
	android::Mutex::Autolock _l(mEntryLock);
	HWC1Proxy::HWC1Response response;
	char request[512];

	sprintf(request, "%s %d %d", "enhance SetMode", display, mode);
	sendRequest(request, &response);
	return response.getResult();
}

int
HWC1Proxy::getDisplayEnhanceComponent(int display, int component) {
	android::Mutex::Autolock _l(mEntryLock);
	HWC1Proxy::HWC1Response response;
	char request[512];

	sprintf(request, "%s %d %d", "enhance GetComponent", display, component);
	if (sendRequest(request, &response) == 0) {
		return response.getPayload(0);
	}
	return -1;
}

int
HWC1Proxy::setDisplayEnhanceComponent(int display, int component, int target) {
	android::Mutex::Autolock _l(mEntryLock);
	HWC1Proxy::HWC1Response response;
	char request[512];

	sprintf(request, "%s %d %d %d", "enhance SetComponent", display, component, target);
	sendRequest(request, &response);
	return response.getResult();
}

int
HWC1Proxy::getDisplayFPS(int /* display */) {
	return 0;
}

int
HWC1Proxy::setDeviceConfig(int display, struct disp_device_config *config) {
	android::Mutex::Autolock _l(mEntryLock);
	HWC1Proxy::HWC1Response response;
	char request[512];

	sprintf(request, "%s %d %d,%d,%d,%d,%d,%d,%d,%d,%d,%d",
			"interface SetDeviceConfig", display,
			config->type, config->mode, config->format, config->bits,
			config->eotf, config->cs, config->dvi_hdmi, config->range,
			config->scan, config->aspect_ratio);
	sendRequest(request, &response);
	return response.getResult();
}

