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
#include "libdispclient.h"
#include "sunxi_display2.h"

int
displaydClient::displaydResponse::getSeqnum(void) {
	if (mArgs.size() < 3)
		return -1;
	return 0;
}

int
displaydClient::displaydResponse::getResult(void) {
	if (mArgs.size() < 1)
		return -1;
	return strtoul(mArgs[0], 0 ,0);
}

int
displaydClient::displaydResponse::getPayload(unsigned int index) {
	if (mArgs.size() < (3 + index))
		return -1;
	return strtoul(mArgs[2+index], 0 ,0);
}

const char *
displaydClient::displaydResponse::getRawData(unsigned int index) {
	if (mArgs.size() < (3 + index))
		return 0;
	return mArgs[2+index];
}

displaydClient::displaydClient()
	: mConnector(new connector()) {
	}

displaydClient::~displaydClient() {
	if (mConnector)
		delete mConnector;
}

int displaydClient::sendRequest(char *request, displaydClient::displaydResponse *response) {
	return mConnector->sendRequest(request, response);
}

int
displaydClient::getOutputType(int display) {
	android::Mutex::Autolock _l(mEntryLock);
	displaydClient::displaydResponse response;
	char request[512];

	sprintf(request, "%s %d", "interface GetType", display);
	if (sendRequest(request, &response) == 0) {
		response.dump();
		return (response.getResult() == 200) ? response.getPayload(0) : -1;
	}
	return -1;
}

int
displaydClient::getOutputMode(int display) {
	android::Mutex::Autolock _l(mEntryLock);
	displaydClient::displaydResponse response;
	char request[512];

	sprintf(request, "%s %d", "interface GetMode", display);
	if (sendRequest(request, &response) == 0) {
		response.dump();
		return (response.getResult() == 200) ? response.getPayload(0) : -1;
	}
	return -1;
}

int
displaydClient::setDeviceConfig(int display, void *config) {
	android::Mutex::Autolock _l(mEntryLock);
	displaydClient::displaydResponse response;
	char request[512];
	struct disp_device_config *dc = 0;

	ALOGD("displaydClient::setDeviceConfig on disp.%d", display);
	if (!config) {
		ALOGE("displaydClient::setDeviceConfig input param error");
		return -1;
	}
	dc = static_cast<struct disp_device_config *>(config);
	sprintf(request, "%s %d %d,%d,%d,%d,%d,%d,%d,%d,%d,%d",
		"interface SetDeviceConfig", display,
		dc->type, dc->mode, dc->format, dc->bits,
		dc->eotf, dc->cs, dc->dvi_hdmi, dc->range,
		dc->scan, dc->aspect_ratio);
	sendRequest(request, &response);
	response.dump();
	return (response.getResult() == 200) ? 0 : -1;
}

int
displaydClient::getDeviceConfig(int display, void *config) {
	android::Mutex::Autolock _l(mEntryLock);
	displaydClient::displaydResponse response;
	char request[512];
	struct disp_device_config *dc = 0;

	ALOGD("displaydClient::setDeviceConfig on disp.%d", display);
	if (!config) {
		ALOGE("displaydClient::setDeviceConfig input param error");
		return -1;
	}
	dc = static_cast<struct disp_device_config *>(config);
	sprintf(request, "%s %d", "interface GetDeviceConfig", display);
	if (sendRequest(request, &response) == 0
			&& response.getResult() == 200) {
		response.dump();
		const char *retval = response.getRawData(1);
		if (retval && sscanf(retval, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d",
				&dc->type, &dc->mode, &dc->format, &dc->bits,
				&dc->eotf, &dc->cs, &dc->dvi_hdmi, &dc->range,
				&dc->scan, &dc->aspect_ratio) == 10) {
			return 0;
		}
	}
	return -1;
}

int
displaydClient::set3DLayerMode(int display, int mode) {
	android::Mutex::Autolock _l(mEntryLock);
	displaydClient::displaydResponse response;
	char request[512];

	ALOGD("displaydClient::Set3DLayerMode %d", mode);
	sprintf(request, "%s %d %d %d", "utils Set3DLayerMode", display, mode, 0);
	sendRequest(request, &response);
	response.dump();
	return (response.getResult() == 200) ? 0 : -1;
}

int
displaydClient::getSupport3DMode(int display) {
	android::Mutex::Autolock _l(mEntryLock);
	displaydClient::displaydResponse response;
	char request[512];

	ALOGD("displaydClient::GetSupport3DMode");
	sprintf(request, "%s %d", "utils GetSupport3DMode", display);
	if (sendRequest(request, &response) == 0) {
		response.dump();
		return (response.getResult() == 200) ? response.getPayload(0) : -1;
	}
	return -1;
}


