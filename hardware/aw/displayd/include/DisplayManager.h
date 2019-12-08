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

#ifndef _DISPLAYMANAGER_H
#define _DISPLAYMANAGER_H

#include <vector>
#include <string.h>
#include "HWC1Proxy.h"
#include "deviceControler.h"
#include "sunxi_display2.h"

class DisplayManager {
	static DisplayManager *instance;
	HWC1Proxy *mProxy;
	deviceControler* mControler;

	DisplayManager();

public:

	/* Interface sub command api */
	int listInterface(int display, std::vector<int> *interfaces);
	int getCurrentInterface(int display, int *interface);
	int setCurrentInterface(int display, int interface, int enable);

	/* Mode sub command api */
	int listMode(int display, int interface, std::vector<int> *modes);
	int getCurrentMode(int display, int interface, int *mode);
	int setCurrentMode(int display, int interface, int mode);
	int setCurrentMode_FORCE(int display, int interface, int mode);

	int getOutputFormat(int display);
	int setOutputFormat(int display, int format);
	int getOutputType(int display);
	int getOutputMode(int display);
	int setOutputMode(int display, int mode);
	int listSupportModes(int display, std::vector<int> *modes);
	int isSupportMode(int display, int mode);
	int isSupport3DMode(int display, int mode);
	int isCommonMode(int mode);

	int getOutputPixelFormat(int display);
	int setOutputPixelFormat(int display, int value);
	int getOutputCurDataspaceMode(int display);
	int getOutputDataspaceMode(int display);
	int setOutputDataspaceMode(int display, int value);
	int getDeviceConfig(int display, struct disp_device_config *config);
	int setDeviceConfig(int display, struct disp_device_config *config);

	int getDisplayMargin(int display, int *margin_x, int *margin_y);
	int setDisplayMargin(int display, int  margin_x, int  margin_y);
	int getDisplayOffset(int display, int *offset_x, int *offset_y);
	int setDisplayOffset(int display, int  offset_x, int  offset_y);

	int set3DLayerMode(int display, int mode, int videoCropHeight);

	/* Enhance mode setting */
	int getDisplayEdge(int display);
	int setDisplayEdge(int display, int target);
	int getDisplayDetail(int display);
	int setDisplayDetail(int display, int target);
	int getDisplayBright(int display);
	int setDisplayBright(int display, int target);
	int getDisplayDenoise(int display);
	int setDisplayDenoise(int display, int target);
	int getDisplayContrast(int display);
	int setDisplayContrast(int display, int target);
	int getDisplaySaturation(int display);
	int setDisplaySaturation(int display, int target);
	int getDisplayEnhanceMode(int display);
	int setDisplayEnhanceMode(int display, int mode);

	/* read/write display parameter api */

	static DisplayManager* getInstance();
	deviceControler* getControler();
};

#endif
