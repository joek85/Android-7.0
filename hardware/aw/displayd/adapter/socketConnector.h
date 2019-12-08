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

#ifndef _SOCKETCONNECTOR_H
#define _SOCKETCONNECTOR_H

#include <vector>
#include <list>
#include <utils/Thread.h>
#include <sysutils/FrameworkListener.h>
#include <sysutils/FrameworkCommand.h>

class responsePackt {
public:
	int mSeqNum;
	std::vector<char *> mArguments;
	responsePackt(int seqnum, int argc, char **argv);
	~responsePackt();
};

class Response {
public:
	char mBuff[512];
	std::vector<char *> mArgs;
	int mSeqNum;
	void dup(responsePackt *in);
};

class socketConnector {

	class Client : public SocketListener {
		static const int CMD_ARGS_MAX = 8;
		mutable android::Mutex mLock;
		mutable android::Condition mCondition;
		int mSocket;
		bool mActive;
		SocketClient mSocketClient;
		std::list<responsePackt *> mResponse;

	public:
		Client(int fd);
		virtual ~Client();
		virtual bool onDataAvailable(SocketClient *client);
		bool isActive(void);
		int sendRequest(char *request, int timeout = 2000);
		int getResponse(std::list<responsePackt *>& out);

	private:
		void resetResponse(void);
		int updateResponse(int seqnum, int argc, char **argv);
		void dispatchCommand(SocketClient *client, char *data);
	};

	mutable android::Mutex mEntryLock;
	Client *mClient;
	int mSequenceNumber;

public:
	socketConnector();
	virtual ~socketConnector() {};
	int sendRequest(char *request, Response *out);

private:
	int generateSeqNum() { return mSequenceNumber++; };
	int getClientSocket(void);
	void reconnect(void);
};

#endif
