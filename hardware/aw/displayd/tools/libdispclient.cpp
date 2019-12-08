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

int
displaydClient::displaydResponse::getPayload_bool(unsigned int index) {
	if (mArgs.size() < (3 + index))
		return -1;
	return strcmp(mArgs[2+index], "true") == 0 ? 1 : 0;
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
displaydClient::setOutputMode(int display, int mode) {
	android::Mutex::Autolock _l(mEntryLock);
	displaydClient::displaydResponse response;
	char request[512];

	sprintf(request, "%s %d %d", "interface SetMode", display, mode);
	if (sendRequest(request, &response) == 0) {
		response.dump();
		return (response.getResult() == 200) ? 0 : -1;
	}
	return -1;
}

int
displaydClient::isSupportMode(int display, int mode) {
	android::Mutex::Autolock _l(mEntryLock);
	displaydClient::displaydResponse response;
	char request[512];

	sprintf(request, "%s %d %d", "interface CheckMode", display, mode);
	if (sendRequest(request, &response) == 0) {
		response.dump();
		return (response.getResult() == 200) ? response.getPayload_bool(0) : -1;
	}
	return -1;
}

int
displaydClient::set3DLayerMode(int display, int mode) {
	android::Mutex::Autolock _l(mEntryLock);
	displaydClient::displaydResponse response;
	char request[512];

	sprintf(request, "%s %d %d", "utils Set3DLayerMode", display, mode);
	sendRequest(request, &response);
	response.dump();
	return (response.getResult() == 200) ? 0 : -1;
}

int
displaydClient::getSupport3DMode(int display) {
	android::Mutex::Autolock _l(mEntryLock);
	displaydClient::displaydResponse response;
	char request[512];

	sprintf(request, "%s %d", "utils GetSupport3DMode", display);
	if (sendRequest(request, &response) == 0) {
		response.dump();
		return (response.getResult() == 200) ? response.getPayload(0) : -1;
	}
	return -1;
}


