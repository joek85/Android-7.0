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

#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <cutils/log.h>
#include "DisplayManager.h"
#include "CommandListener.h"
#include "hotplugListener.h"
#include "policySelector.h"
#include "DisplayPolicy.h"

void wait_for_hwcomposer(DisplayManager *manager);

int main(int /* argc */, char** /* argv */) {

	/* Setup the display policy */
	DisplayManager *dispManager = DisplayManager::getInstance();
#if 0
	/* Extenral display was open on init */
	policySelector *policy = new policySelector(dispManager);
	policy->setup(policy->getPolicy());
#endif

	/* CommandListener: listen request from framework */
	CommandListener *listener = new CommandListener(dispManager);
	if (listener->startListener()) {
		ALOGE("unable to start displayd listen (%s)", strerror(errno));
		exit(1);
	}
	wait_for_hwcomposer(dispManager);

	/* display policy mode */
	DisplayPolicy *runtime_policy = new DisplayPolicy(dispManager);

	/* hotplugListener: monitor the hotplug uevent and dispatch it */
	hotplugListener *hotplugMonitor = new hotplugListener(hotplugListener::createUeventSocket());

	hotplugMonitor->registerHotplugCallback(reinterpret_cast<void *>(runtime_policy->hotplugCallbakcEntry));
	hotplugMonitor->registerFrameworkListener(listener);
	hotplugMonitor->start();
	while (1) sleep(10);
	return 0;
}

void wait_for_hwcomposer(DisplayManager *manager) {
	int interface = 0;
__try:
	manager->getCurrentInterface(0, &interface);
	if (interface > 0)
		return;
	ALOGD("Wait for hwcomposer bootup");
	usleep(1000 * 100);
	goto __try;
}

