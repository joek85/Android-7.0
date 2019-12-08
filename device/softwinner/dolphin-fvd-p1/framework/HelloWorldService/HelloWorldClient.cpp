/**
 * This file is in PUBLIC DOMAIN. You can use it freely. No guarantee.
 * Author: Fan Hongtao <fanhongtao@gmail.com>
 * Date: 2011-06-28
 */

#define LOG_TAG "HelloWorldClient"

#include <binder/IPCThreadState.h>
#include <binder/IServiceManager.h>
#include <binder/ProcessState.h>
#include <utils/Log.h>

#include "IHelloWorldService.h"
namespace android {
sp<IHelloWorldService> getAndroidShmService() {
  sp<IHelloWorldService> shm = 0;

  /* Get the buffer service */
  if (shm == NULL) {
    sp<IServiceManager> sm = defaultServiceManager();
    sp<IBinder> binder;
    binder = sm->getService(String16("com.joek.helloworldservice.IHelloWorldService"));
    if (binder != 0) {
        shm = IHelloWorldService::asInterface(binder);
        //shm = interface_cast<IAndroidShm>(binder);
    }
  }
  return shm;
}
}
using namespace android;
int main(int argc, char *argv[]) {

    sp<IHelloWorldService> shm = getAndroidShmService();

    if(shm == NULL) {
        printf("shm service is not available\n");
       
    }

    printf("<<<<<<<<<<< show registry index >>>>>>>>>>\n");

    //printf("index [%3d]\n",shm->getRegistryIndex());
    shm->Setup_Mixer_Main();
    shm->Setup_Mixer_Channel();

    return 0;
}
