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

#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <string>
#include <linux/if.h>
#include <cutils/log.h>
#include <cutils/sockets.h>

#include "connector.h"
#include "debug.h"

responsePackt::responsePackt(int seqnum, int argc, char **argv) {
	mSeqNum = seqnum;
	for (int i = 0; i < argc; i++)
		mArguments.push_back(argv[i]);
}

responsePackt::~responsePackt() {
	for (unsigned int i = 0; i < mArguments.size(); i++)
		if (mArguments[i]) free(mArguments[i]);
	mArguments.clear();
}

void Response::dup(responsePackt *in) {
	char *p = mBuff;
	for (auto& item : in->mArguments) {
		mArgs.push_back(p);
		p += sprintf(p, "%s", item);
		*p++ = 0;
	}
	mSeqNum = in->mSeqNum;
}

void Response::dump() {
	char buffer[512];
	char *p = buffer;
	for (auto& item : mArgs)
		p += sprintf(p, "%s ", item);
	ALOGD("response: %s\n", buffer);
}

connector::Client::Client(int fd)
: SocketListener(fd, false), mSocket(fd),
  mActive(true), mSocketClient(fd, false, false) {
}

connector::Client::~Client() {
	resetResponse();
	close(mSocket);
}

bool
connector::Client::onDataAvailable(SocketClient *client) {
	char buffer[1024];
	int len;

	len = read(client->getSocket(), buffer, sizeof(buffer));
	if (len < 0) {
		ALOGE("read() failed (%s)", strerror(errno));
		return false;
	} else if (!len)
		return false;
	if(buffer[len-1] != '\0')
		ALOGW("String is not zero-terminated");

	_variable("displayd --> client: [%s]", buffer);

	int offset = 0;
	int i;
	for (i = 0; i < len; i++) {
		if (buffer[i] == 0) {
			dispatchCommand(client, buffer + offset);
			offset = i + 1;
		}
	}
	return true;
}

void
connector::Client::dispatchCommand(SocketClient *client, char *data) {
	char temp[512];
	char *p = data;
	char *q = temp;
	char *argv[CMD_ARGS_MAX];
	int argc = 0;
	bool hasCmdNum = true;
	int cmdNum;

	memset(temp, 0, sizeof(temp));
	memset(argv, 0, sizeof(argv));
	while (*p) {
		*q = *p++;
		if (*q == ' ') {
			*q = 0;
			if (!hasCmdNum) {
				char *endPtr;
				cmdNum = strtol(temp, &endPtr, 0);
				if (!endPtr || *endPtr != 0) {
					client->sendMsg(500, "Invalid sequence number", false);
					goto out;
				}
				client->setCmdNum(cmdNum);
				hasCmdNum = true;
			} else {
				if (argc >= CMD_ARGS_MAX)
					goto overflow;
				argv[argc++] = strdup(temp);
			}

			memset(temp, 0, sizeof(temp));
			q = temp;
			continue;
		}
		q++;
	}
	if (argc >= CMD_ARGS_MAX)
		goto overflow;
	argv[argc++] = strdup(temp);

	updateResponse(argc > 1 ? strtol(argv[1], NULL, 0) : 0, argc, argv);
	mCondition.broadcast();

out:
	return;

overflow:
	ALOGD("Command too long");
	client->sendMsg(500, "Command too long", false);
	goto out;
}

int
connector::Client::sendRequest(char *request, int timeout) {
	android::Mutex::Autolock _l(mLock);

	resetResponse();
	if (mSocketClient.sendMsg(request)) {
		ALOGE("Error on sending message");
		mActive = false;
		return -1;
	}

	if (mCondition.waitRelative(mLock, ms2ns(timeout)) == android::TIMED_OUT) {
		mActive = false;
		ALOGE("wait response timeout");
		return -1;
	}
	return 0;
}

void
connector::Client::resetResponse(void) {
	for (auto& r : mResponse)
		delete r;
	mResponse.clear();
}

int
connector::Client::updateResponse(int seqnum, int argc, char **argv) {
	android::Mutex::Autolock _l(mLock);
	mResponse.push_back(new responsePackt(seqnum, argc, argv));
	return 0;
}

int
connector::Client::getResponse(std::list<responsePackt *>& out) {
	android::Mutex::Autolock _l(mLock);
	out.splice(out.begin(), mResponse);
	return out.size();
}

bool
connector::Client::isActive(void) {
	android::Mutex::Autolock _l(mLock);
	return mActive;
}

connector::connector() : mClient(0), mSequenceNumber(0) {
	reconnect();
}

connector::~connector() {
	if (mClient) {
		mClient->stopListener();
		delete mClient;
		mClient = NULL;
	}
}

void
connector::reconnect(void) {
	if (mClient && mClient->isActive()) {
		ALOGE("Connected, just return");
		return;
	}

	if (mClient) {
		mClient->stopListener();
		delete mClient;
		mClient = NULL;
	}

	int sockfd = getClientSocket();
	if (sockfd > 0) {
		mClient = new Client(sockfd);
		if (mClient->startListener()) {
			ALOGE("unable to start displayd listen (%s)", strerror(errno));
			mClient->stopListener();
			delete mClient;
			mClient = NULL;
		}
	}
}

int
connector::sendRequest(char *request, Response *out) {
	if (!mClient || !mClient->isActive()) {
		ALOGE("lose connection, try to reconnect");
		reconnect();
		if (!mClient) return -1;
	}

	char *buf;
	int seqnum = generateSeqNum();
	if (asprintf(&buf, "%d %s", seqnum, request) == -1) {
		ALOGE("%s: unable alloc memory", __func__);
		return -1;
	}

	if (mClient->sendRequest(buf) != 0) {
		/* send error, just return */
		free(buf);
		return -1;
	}

	std::list<responsePackt *> packets;
	mClient->getResponse(packets);

	/* carefully: free the argvs alloc by listener thread */
	for (auto& p : packets) {
		if (p->mSeqNum == seqnum)
			out->dup(p);
		delete p;
	}

	free(buf);
	return 0;
}

int
connector::getClientSocket(void) {
	int sockfd = socket_local_client("displayd", ANDROID_SOCKET_NAMESPACE_RESERVED, SOCK_STREAM);
	if (sockfd < 0) {
		ALOGE("connect error: %s(errno: %d)", strerror(errno), errno);
		return -1;
	}
	return sockfd;
}

