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

#ifndef _LIBDISPCLIENT_H
#define _LIBDISPCLIENT_H

#include <utils/Thread.h>
#include <sysutils/FrameworkListener.h>
#include <sysutils/FrameworkCommand.h>
#include "connector.h"

class displaydClient {
	mutable android::Mutex mEntryLock;
	connector* mConnector;

	class displaydResponse : public Response {
	public:
		int getSeqnum (void);
		int getResult (void);
		int getPayload(unsigned int index);
		int getPayload_bool(unsigned int index);
	};

public:
	displaydClient();
	virtual ~displaydClient();

	int getOutputType(int display);
	int getOutputMode(int display);

	int set3DLayerMode(int display, int mode);
	int getSupport3DMode(int display);
	int setOutputMode(int display, int mode);
	int isSupportMode(int display, int mode);

private:
	int sendRequest(char *request, displaydClient::displaydResponse *response);
};

#endif
