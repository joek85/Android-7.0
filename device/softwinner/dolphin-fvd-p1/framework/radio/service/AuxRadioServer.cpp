/**
 * This file is in PUBLIC DOMAIN. You can use it freely. No guarantee.
 * Author: Fan Hongtao <fanhongtao@gmail.com>
 * Date: 2011-06-28
 */

#define LOG_TAG "AuxRadioServer"

#include <binder/IPCThreadState.h>
#include <binder/IServiceManager.h>
#include <binder/ProcessState.h>
#include <utils/Log.h>

#include "AuxRadioService.h"
using namespace android;
int main(int argc, char *argv[]) {
	ALOGI("AuxRadioServer is starting now");
 	sp<ProcessState> proc(ProcessState::self());
    	sp<IServiceManager> sm = defaultServiceManager();
	AuxRadioService::instantiate();
	ProcessState::self()->startThreadPool();
	IPCThreadState::self()->joinThreadPool();

    return 0 ;
}

