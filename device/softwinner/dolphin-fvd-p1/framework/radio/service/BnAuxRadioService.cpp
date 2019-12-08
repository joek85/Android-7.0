#define LOG_TAG "AuxRadioService"


#include "BnAuxRadioService.h"
#include <utils/String16.h>
#include <utils/Log.h>
#include <string.h>
#include <sys/types.h>      /* needed to use pid_t, etc. */
#include <sys/wait.h>       /* needed to use wait() */  
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>         /* LINUX constants and functions (fork(), etc.) */
#include <err.h>
#include <binder/Parcel.h>

namespace android {

status_t BnAuxRadioService::onTransact(uint32_t code,const Parcel &data,Parcel *reply,uint32_t flags) {

        //IPCThreadState* self = IPCThreadState::self();
        //ALOGE("Calling MSG: PID=%d, UID=%d",self->getCallingPid(),self->getCallingUid());
    reply->writeInt32(0);
    switch(code) {
	case SETVOLUME: {
            CHECK_INTERFACE(IAuxRadioService, data, reply);
	    int vol = data.readInt32();
            Set_Volume(vol);
            return android::NO_ERROR;
        }break;
	case POWERON: {
            CHECK_INTERFACE(IAuxRadioService, data, reply);
            int power = powerOn();
	    reply->writeInt32(power);
            return android::NO_ERROR;
        }break;
	case SETCHANNEL: {
            CHECK_INTERFACE(IAuxRadioService, data, reply);
	    int chan = data.readInt32();
            setChannel(chan);
            return android::NO_ERROR;
        }break;
	case SEEKUP: {
            CHECK_INTERFACE(IAuxRadioService, data, reply);
	    int seek = seekUp();
            reply->writeInt32(seek);
            return android::NO_ERROR;
        }break;
	case SEEKDOWN: {
            CHECK_INTERFACE(IAuxRadioService, data, reply);
	    int seek = seekDown();
            reply->writeInt32(seek);
            return android::NO_ERROR;
        }break;
	case POWEROFF: {
            CHECK_INTERFACE(IAuxRadioService, data, reply);
	    powerOff();
            return android::NO_ERROR;
        }break;
	case GETCHANNEL: {
            CHECK_INTERFACE(IAuxRadioService, data, reply);
	    int chan = getChannel();
            reply->writeInt32(chan);
            return android::NO_ERROR;
        }break;
	case READRDS: {
            CHECK_INTERFACE(IAuxRadioService, data, reply);
	    const char *rdsBuffer;
	    rdsBuffer = readRDS();
	    reply->writeCString(rdsBuffer);
            return android::NO_ERROR;
        }break;
        default:
                return BBinder::onTransact(code, data, reply, flags);
    }   
}
}



