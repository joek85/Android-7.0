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

#ifndef _DISPLAYPOLICY_H_
#define _DISPLAYPOLICY_H_

#include "DispOutputState.h"
#include <utils/Mutex.h>
#include <vector>

using android::Mutex;
using std::vector;

class DisplayManager;

class DisplayPolicy {
private:
	DisplayManager& mDisplayManager;

	int mPolicy;
	DispOutputState *mDispState;
	DispOutputState *mMain2Dev0In;
	DispOutputState *mMain2Dev0InExt;
	DispOutputState *mMain2Dev0Out;
	DispOutputState *mMain2Dev1In;
	DispOutputState *mMain2Dev1Out;
	DispOutputState *mDualDisplay;

	class DispDevice {
	public:
		int priority;
		int type;
		int revertPlugStateType;
		bool hotplugSupport;
	};

	vector<DispDevice> mDispDevices;
	Mutex mMutex;

	static DisplayPolicy *instance;

public:
	DisplayPolicy(DisplayManager *dm);
	~DisplayPolicy() {}
	DisplayPolicy *getInstance() { return instance; };

	void notifyDispDevicePlugChange(const char *name, bool isPlugIn);
	void notifyDispDevicePlugChange(int type, bool isPlugIn);

	DispOutputState *getMainDispToDev0PlugIn();
	DispOutputState *getMainDispToDev0PlugInExt();
	DispOutputState *getMainDispToDev0PlugOut();
	DispOutputState *getMainDispToDev1PlugIn();
	DispOutputState *getMainDispToDev1PlugOut();
	DispOutputState *getDualDisplayOutput();
	void setOutputState(DispOutputState *state);

	int getPolicy() {return mPolicy;}
	int getDispOutputType(int display);
	int setDisplayOutput(int display, int type, int mode);

	static void hotplugCallbakcEntry(const char *name, bool connect);

private:
	void getDispDevices();
	bool getHotplugSupport(int displayType);
	int getRevertHotplugType(int displayType);
	int initOutputState();
	DispDevice &getDispDeviceByType(int dispType);
	bool getPlugState(int dispType);
	int getStringsFromFile(char const *fileName, char *values, unsigned int size);
	void dispDevicePlugChangeLocked(DispDevice &dispDevice, bool isPlugIn);
};

 #endif //ifndef _DISPLAYPOLICY_H_

