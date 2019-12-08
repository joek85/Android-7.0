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

#ifndef _NETLINKHANDLER_H
#define _NETLINKHANDLER_H

#include <sysutils/NetlinkListener.h>
#include <sysutils/SocketListener.h>

typedef void (*HOTPLUG_CHANGE_CALLBACK)(const char *name, bool plugin);

class hotplugListener: public NetlinkListener {
	HOTPLUG_CHANGE_CALLBACK mHotplugCallback;
	int mSocket;
	SocketListener *mFrameworkListener;

public:
	hotplugListener(int listenerSocket);
	virtual ~hotplugListener();
	void registerHotplugCallback(void *pfn);
	void registerFrameworkListener(SocketListener *l);

	static int createUeventSocket(void);
	int start(void);
	int stop(void);

protected:
	virtual void onEvent(NetlinkEvent *evt);
	void notifyHotplugChanged(const char * name, bool isPlugIn);
	void sendBroadcast(const char *msg);
	void broadcastHotplugMsg(const char *name, const char *state);
};

#endif
