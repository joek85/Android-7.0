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

#ifndef _POLICY_SELECTOR_H
#define _POLICY_SELECTOR_H

#include <map>
#include "config.h"
#include "DisplayManager.h"

#define declareBuilder(name)                    \
	class name##Builder: public policyBuilder { \
	public:                                     \
		name##Builder(DisplayManager* m)        \
			: policyBuilder(m) {};              \
		virtual void initializer();             \
	};                                          \

class policyBuilder {
public:
	DisplayManager& mDispManager;

	struct deviceInfo {
		int type;
		int mode;
		int active;
		int expect_type;
		int default_mode;
	};
	deviceInfo mDevice[HW_NUM_DISPLAY];

	policyBuilder(DisplayManager *m): mDispManager(*m) {};
	virtual ~policyBuilder() {};
	virtual void initializer() = 0;

	bool isValidChannel(int channel);
	int dispDeviceDiscovery();
	int dispDeviceGetOutput(int channel, int *type, int *mode);
	int dispDeviceSwitch(int channel, int type, int mode);
	int copyBootLogoLayer(void);
};

declareBuilder(default);
declareBuilder(single);
declareBuilder(dual);


#define DISP_POLICY_DEFAULT 0
#define DISP_POLICY_SINGLE  1
#define DISP_POLICY_DUAL    2
#define DISP_POLICY_ADAPT   3

class policySelector {
	DisplayManager& mDispManager;
	int mPolicy;
	std::map<int, policyBuilder *> builder;

public:
	policySelector(DisplayManager* manager);
	virtual ~policySelector();

	virtual int getPolicy();
	int setup(int policy);

private:
	int isLowMemoryDevice();

};

#endif
