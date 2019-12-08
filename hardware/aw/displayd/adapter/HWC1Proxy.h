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

#ifndef _HWC1PROXY_H
#define _HWC1PROXY_H

#include <utils/Thread.h>
#include <sysutils/FrameworkListener.h>
#include <sysutils/FrameworkCommand.h>
#include "socketConnector.h"

class HWC1Proxy {
	mutable android::Mutex mEntryLock;
	socketConnector* mConnector;

	class HWC1Response : public Response {
	public:
		int getSeqnum (void);
		int getResult (void);
		int getPayload(unsigned int index);
	};

public:
	HWC1Proxy();
	virtual ~HWC1Proxy() {};

	int getOutputType(int display);
	int getOutputMode(int display);
	int setOutputMode(int display, int type, int mode);
	int getOutputFormat(int display, int *type, int *mode);
	int setOutputFormat(int display, int type, int mode);

	int getOutputPixelFormat(int display, int *value);
	int setOutputPixelFormat(int display, int value);
	int getOutputCurDataspaceMode(int display, int *value);
	int getOutputDataspaceMode(int display, int *value);
	int setOutputDataspaceMode(int display, int value);
	int setDeviceConfig(int display, struct disp_device_config *config);

	int getScreenMargin(int display, int *margin_x, int *margin_y);
	int getScreenOffset(int display, int *offset_x, int *offset_y);
	int setScreenMargin(int display, int margin_x, int margin_y);
	int setScreenOffset(int display, int offset_x, int offset_y);
	int setScreenRadio(int display, int radio);

	int get3DLayerMode(int display);
	int set3DLayerMode(int display, int mode, int videoCropHeight);

	int getDisplayEnhanceMode(int display);
	int setDisplayEnhanceMode(int display, int mode);
	int getDisplayEnhanceComponent(int display, int component);
	int setDisplayEnhanceComponent(int display, int component, int target);
	int getDisplayFPS(int display);

private:
	int sendRequest(char *request, HWC1Proxy::HWC1Response *response);
	int getOverscan(int display, int *margin_x, int *margin_y, int *offset_x, int *offset_y);
	int setOverscan(int display, int margin_x, int margin_y, int offset_x, int offset_y);
};

#endif
