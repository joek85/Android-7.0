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
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "config.h"
#include "../log.h"

#define declareBuilder(name)                    \
	class name##Builder: public policyBuilder { \
	public:                                     \
		name##Builder(DisplayManager* m)        \
			: policyBuilder(m) {};              \
		virtual void initializer();             \
	};                                          \

class DisplayManager {
public:
	struct deviceControler {
		deviceControler() {
			dispfd = open("/dev/disp", O_RDWR);
			if (dispfd < 0)
				ERROR("can't open disp device: %s(%d)\n", strerror(errno), errno);
		};
		~deviceControler(void) {
			if (dispfd > 0)
				close(dispfd);
		};
		int disp_ioctl(int cmd, unsigned long args) {
			if (dispfd >= 0)
				return ioctl(dispfd, cmd, args);
			return 0;
		};

		int exit(void) {
			if (dispfd >= 0) {
				close(dispfd);
				dispfd = -1;
				NOTICE("close disp_device fd\n");
			}
			return 0;
		};

	private:
		int dispfd;
	};

	DisplayManager() {
		mControler = new deviceControler();
	}
	~DisplayManager() {
		delete mControler;
	}

	deviceControler* getControler() {
		return mControler;
	};

private:
	deviceControler* mControler;
};

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
	int primaryChannel;
	int externalChannel;

	policyBuilder(DisplayManager *m)
		: mDispManager(*m), primaryChannel(-1), externalChannel(-1) {};
	virtual ~policyBuilder() {};
	virtual void initializer() = 0;

	bool isValidChannel(int channel);
	int dispDeviceDiscovery();
	int dispDeviceGetOutput(int channel, int *type, int *mode);
	int dispDeviceSwitch(int channel, int type, int mode);
	int copyBootLogoLayer(void);
	int dispCheckHdmiOutput();
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
