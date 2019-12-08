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

#ifndef _DISPLAYDREQUEST_H
#define _DISPLAYDREQUEST_H

#include <map>
#include <string.h>
#include <sysutils/FrameworkListener.h>
#include <sysutils/FrameworkCommand.h>

#include "hwcomposerInterface.h"

#define declareCommand(name)                                         \
	class name##Cmd : public requestCommand {                        \
		hwcomposerInterface& mInterface;                             \
	public:                                                          \
		name##Cmd(hwcomposerInterface* interface);                   \
		virtual ~name##Cmd() {};                                     \
		int runCommand(SocketClient *client, int argc, char **argv); \
	};

struct stringKeyCmp {
	bool operator () (const char *a, const char *b) const {
		return strcmp(a, b) < 0;
	};
};
typedef std::map<const char *, int, stringKeyCmp> commandMaps_t;

class requestCommand : public FrameworkCommand {
public:
	commandMaps_t mCommandMaps;

	int toCommandCode(char *name);
	requestCommand(const char *cmd);
	virtual ~requestCommand() {};
};

class displaydRequest : public FrameworkListener {
	hwcomposerInterface *mInterface;

public:
	displaydRequest(hwcomposerInterface *interface);
	virtual ~displaydRequest() {};

private:
	int createSocketfd(void);
	declareCommand(interface);
	declareCommand(overscan);
	declareCommand(utils);
	declareCommand(enhance);
};

#endif
