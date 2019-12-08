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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/un.h>
#include <sys/socket.h>
#include <linux/netlink.h>

#include <cutils/log.h>
#include <sysutils/NetlinkEvent.h>
#include "hotplugListener.h"

hotplugListener::hotplugListener(int listenerSocket)
	: NetlinkListener(listenerSocket, NetlinkListener::NETLINK_FORMAT_ASCII),
	mHotplugCallback(0), mSocket(listenerSocket), mFrameworkListener(0) {
}

hotplugListener::~hotplugListener() {
	if (mSocket > 0)
		close(mSocket);
}

void
hotplugListener::registerHotplugCallback(void *pfn) {
	mHotplugCallback = reinterpret_cast<HOTPLUG_CHANGE_CALLBACK>(pfn);
	return;
}

void
hotplugListener::registerFrameworkListener(SocketListener *l) {
	mFrameworkListener = l;
	return;
}

void
hotplugListener::sendBroadcast(const char *msg) {
	if (mFrameworkListener)
		mFrameworkListener->sendBroadcast(618, msg, false);
	return;
}

void
hotplugListener::broadcastHotplugMsg(const char *name, const char *state) {
	if (!strcmp(name, "hdmi") || !strcmp(name, "cvbs")) {
		char msg[128];
		sprintf(msg, "hotplug %s %s", name, state);
		sendBroadcast(msg);
	}
}

int
hotplugListener::start() {
	return this->startListener();
}

int hotplugListener::stop() {
	return this->stopListener();
}

void
hotplugListener::onEvent(NetlinkEvent *event) {
	const char *subsys = event->getSubsystem();
	if (!subsys) {
		ALOGE("No subsystem found in netlink event");
		return;
	}

#if  (defined PLATFORM_SUN50IW6P1)
	if(!strcmp(subsys, "switch")) {
		if(event->getAction() == NetlinkEvent::Action::kChange) {
			const char *name = event->findParam("SWITCH_NAME");
			const char *state = event->findParam("SWITCH_STATE");
			if((NULL == name) || (NULL == state)) {
				ALOGE("null point: name=%s, state=%s.", name, state);
				return;
			}
			ALOGD("hotplug change: name=%s, state=%s ", name, state);
			notifyHotplugChanged(name, atoi(state)==1);

			broadcastHotplugMsg(name, state);
		} else {
			event->dump();
		}
	}
#elif  (defined PLATFORM_SUN8IW7P1)
	/* kernel-4.4 use extcon */
	if(!strcmp(subsys, "extcon")) {
		if(event->getAction() == NetlinkEvent::Action::kChange) {
			const char *name = event->findParam("NAME");
			const char *state = event->findParam("STATE");
			if((NULL == name) || (NULL == state)) {
				ALOGE("null point: name=%s, state=%s.", name, state);
				return;
			}
			ALOGD("hotplug change: name=%s, state=%s ", name, state);

			int connected = strstr(state, "1") ? 1 : 0;
			notifyHotplugChanged(name, connected);
			broadcastHotplugMsg(name, connected ? "1" : "0");
		} else {
			event->dump();
		}
	}
#else
#error "Please impl uevent handle for new platform"
#endif
	return;
}

void
hotplugListener::notifyHotplugChanged(const char *name, bool plugin) {
	ALOGD("hotplugListener: %s %s", name, plugin ? "connect" : "disconnect");
	if (mHotplugCallback)
		mHotplugCallback(name, plugin);
	return;
}

int
hotplugListener::createUeventSocket(void) {
	struct sockaddr_nl addr;
	int sz = 64*1024;
	int on = 1;
	int sockfd;

	memset(&addr, 0, sizeof(addr));
	addr.nl_family = AF_NETLINK;
	addr.nl_pid = getpid();
	addr.nl_groups = 0xffffffff;

	sockfd = socket(PF_NETLINK, SOCK_DGRAM, NETLINK_KOBJECT_UEVENT);
	if(sockfd < 0) {
		ALOGE("create socket error: %s(%d)", strerror(errno), errno);
		return -1;
	}
	if (setsockopt(sockfd, SOL_SOCKET, SO_RCVBUFFORCE, &sz, sizeof(sz)) < 0) {
		ALOGE("setsockopt error: %s(%d)", strerror(errno), errno);
		close(sockfd);
		return -1;
	}
	if (setsockopt(sockfd, SOL_SOCKET, SO_PASSCRED, &on, sizeof(on)) < 0) {
		ALOGE("setsockopt error: %s(%d)", strerror(errno), errno);
		close(sockfd);
		return -1;
	}
	if(bind(sockfd, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
		ALOGE("bind error: %s(%d)", strerror(errno), errno);
		close(sockfd);
		return 0;
	}

	return sockfd;
}

