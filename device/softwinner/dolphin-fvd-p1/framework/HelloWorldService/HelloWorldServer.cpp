/**
 * This file is in PUBLIC DOMAIN. You can use it freely. No guarantee.
 * Author: Fan Hongtao <fanhongtao@gmail.com>
 * Date: 2011-06-28
 */

#define LOG_TAG "HelloWorldServer"

#include <binder/IPCThreadState.h>
#include <binder/IServiceManager.h>
#include <binder/ProcessState.h>
#include <utils/Log.h>

#include "HelloWorldService.h"
using namespace android;
int main(int argc, char *argv[]) {
	ALOGI("HelloWorldService is starting now");
 	sp<ProcessState> proc(ProcessState::self());
    	sp<IServiceManager> sm = defaultServiceManager();
	HelloWorldService::instantiate();
	ProcessState::self()->startThreadPool();
	//ALOGI("HelloWorldService is starting now");
	IPCThreadState::self()->joinThreadPool();

    //BnHelloWorldService::instantiate();
    //android::ProcessState::self()->startThreadPool();
    //android::IPCThreadState::self()->joinThreadPool();

    return 0 ;
}

