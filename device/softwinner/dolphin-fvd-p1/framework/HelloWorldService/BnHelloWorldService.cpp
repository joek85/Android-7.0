#define LOG_TAG "HelloWorldService"


#include "BnHelloWorldService.h"
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

status_t BnHelloWorldService::onTransact(uint32_t code,const Parcel &data,Parcel *reply,uint32_t flags) {

        //IPCThreadState* self = IPCThreadState::self();
        //ALOGE("Calling MSG: PID=%d, UID=%d",self->getCallingPid(),self->getCallingUid());
    reply->writeInt32(0);
    switch(code) {

	case SETINPUTGAIN: {
            CHECK_INTERFACE(IHelloWorldService, data, reply);
	    float gain;
	    data.readFloat(&gain);
            setInputGain(gain);
            return android::NO_ERROR;
        }break;
	case SETPLAYERGAIN: {
            CHECK_INTERFACE(IHelloWorldService, data, reply);
	    float gain;
	    data.readFloat(&gain);
            setPlayerGain(gain);
            return android::NO_ERROR;
        }break;
	case READINPEAKL: {
            CHECK_INTERFACE(IHelloWorldService, data, reply);
            float levelL = readinpeakL();
	    reply->writeFloat(levelL);
            return android::NO_ERROR;
        }break;
	case READINPEAKR: {
            CHECK_INTERFACE(IHelloWorldService, data, reply);
            float levelR = readinpeakR();
	    reply->writeFloat(levelR);
            return android::NO_ERROR;
        }break;
	case READPLAYERPEAKL: {
            CHECK_INTERFACE(IHelloWorldService, data, reply);
            float levelL = readplayerpeakL();
	    reply->writeFloat(levelL);
            return android::NO_ERROR;
        }break;
	case READPLAYERPEAKR: {
            CHECK_INTERFACE(IHelloWorldService, data, reply);
            float levelR = readplayerpeakR();
	    reply->writeFloat(levelR);
            return android::NO_ERROR;
        }break;
	case READOUTPEAK1L: {
             CHECK_INTERFACE(IHelloWorldService, data, reply);
            float levelL = readoutpeak1L();
	    reply->writeFloat(levelL);
            return android::NO_ERROR;
        }break;
	case READOUTPEAK1R: {
             CHECK_INTERFACE(IHelloWorldService, data, reply);
            float levelR = readoutpeak1R();
	    reply->writeFloat(levelR);
            return android::NO_ERROR;
        }break;
	case READOUTPEAK2L: {
             CHECK_INTERFACE(IHelloWorldService, data, reply);
            float levelL = readoutpeak2L();
	    reply->writeFloat(levelL);
            return android::NO_ERROR;
        }break;
	case READOUTPEAK2R: {
             CHECK_INTERFACE(IHelloWorldService, data, reply);
            float levelR = readoutpeak2R();
	    reply->writeFloat(levelR);
            return android::NO_ERROR;
        }break;
	case READOUTPEAK3L: {
             CHECK_INTERFACE(IHelloWorldService, data, reply);
            float levelL = readoutpeak3L();
	    reply->writeFloat(levelL);
            return android::NO_ERROR;
        }break;
	case READOUTPEAK3R: {
             CHECK_INTERFACE(IHelloWorldService, data, reply);
            float levelR = readoutpeak3R();
	    reply->writeFloat(levelR);
            return android::NO_ERROR;
        }break;
	case SETMIXERGAIN1: {
            CHECK_INTERFACE(IHelloWorldService, data, reply);
	    float gainL;
	    float gainR;
	    data.readFloat(&gainL);
	    data.readFloat(&gainR);
            setMixerGain1(gainL,gainR);
            return android::NO_ERROR;
        }break;
	case SETMIXERGAIN2: {
            CHECK_INTERFACE(IHelloWorldService, data, reply);
	    float gainL;
	    float gainR;
	    data.readFloat(&gainL);
	    data.readFloat(&gainR);
            setMixerGain2(gainL,gainR);
            return android::NO_ERROR;
        }break;
	case SETMIXERGAIN3: {
            CHECK_INTERFACE(IHelloWorldService, data, reply);
	    float gainL;
	    float gainR;
	    data.readFloat(&gainL);
	    data.readFloat(&gainR);
            setMixerGain3(gainL,gainR);
            return android::NO_ERROR;
        }break;
	case READEQINPEAKL: {
             CHECK_INTERFACE(IHelloWorldService, data, reply);
	    int index = data.readInt32();
            float levelL = readEqualizerinpeakL(index);
	    reply->writeFloat(levelL);
            return android::NO_ERROR;
        }break;
	case READEQINPEAKR: {
             CHECK_INTERFACE(IHelloWorldService, data, reply);
	    int index = data.readInt32();
            float levelR = readEqualizerinpeakR(index);
	    reply->writeFloat(levelR);
            return android::NO_ERROR;
        }break;
	case READEQOUTPEAKL: {
             CHECK_INTERFACE(IHelloWorldService, data, reply);
	    int index = data.readInt32();
            float levelL = readEqualizeroutpeakL(index);
	    reply->writeFloat(levelL);
            return android::NO_ERROR;
        }break;
	case READEQOUTPEAKR: {
             CHECK_INTERFACE(IHelloWorldService, data, reply);
	    int index = data.readInt32();
            float levelR = readEqualizeroutpeakR(index);
	    reply->writeFloat(levelR);
            return android::NO_ERROR;
        }break;
	case SETEQGAIN: {
            CHECK_INTERFACE(IHelloWorldService, data, reply);
	    int index = data.readInt32();
	    float gainL;
	    float gainR;
	    data.readFloat(&gainL);
	    data.readFloat(&gainR);
            setEqualizerGain(index,gainL,gainR);
            return android::NO_ERROR;
        }break;
	case SETEQPARAMETERS: {
            CHECK_INTERFACE(IHelloWorldService, data, reply);
	    int index = data.readInt32();
	    int bandindex = data.readInt32();
	    float freq;
	    float db;
	    data.readFloat(&freq);
	    data.readFloat(&db);
            setEqualizerParameters(index,bandindex,freq,db);
            return android::NO_ERROR;
        }break;
	case SETEQENABLED: {
            CHECK_INTERFACE(IHelloWorldService, data, reply);
	    int index = data.readInt32();  
	    int enable = data.readInt32();  
            setEqualizerEnable(index,enable);
            return android::NO_ERROR;
        }break;
	case READFILTERINPEAKL: {
             CHECK_INTERFACE(IHelloWorldService, data, reply);
	    int index = data.readInt32();
            float levelL = readFilterinpeakL(index);
	    reply->writeFloat(levelL);
            return android::NO_ERROR;
        }break;
	case READFILTERINPEAKR: {
             CHECK_INTERFACE(IHelloWorldService, data, reply);
	    int index = data.readInt32();
            float levelR = readFilterinpeakR(index);
	    reply->writeFloat(levelR);
            return android::NO_ERROR;
        }break;
	case READFILTEROUTPEAKL: {
             CHECK_INTERFACE(IHelloWorldService, data, reply);
	    int index = data.readInt32();
            float levelL = readFilteroutpeakL(index);
	    reply->writeFloat(levelL);
            return android::NO_ERROR;
        }break;
	case READFILTEROUTPEAKR: {
             CHECK_INTERFACE(IHelloWorldService, data, reply);
	    int index = data.readInt32();
            float levelR = readFilteroutpeakR(index);
	    reply->writeFloat(levelR);
            return android::NO_ERROR;
        }break;
	case SETFILTERPARAMETERS: {
            CHECK_INTERFACE(IHelloWorldService, data, reply);
	    int index = data.readInt32();
	    int bandindex = data.readInt32();
	    float freq;
	    float db;
	    data.readFloat(&freq);
	    data.readFloat(&db);
            setFilterParameters(index,bandindex,freq,db);
            return android::NO_ERROR;
        }break;
	case SETFILTERENABLED: {
            CHECK_INTERFACE(IHelloWorldService, data, reply);
	    int index = data.readInt32();  
	    int enable = data.readInt32();  
            setFilterEnable(index,enable);
            return android::NO_ERROR;
        }break;
	case SETFILTERTYPE: {
            CHECK_INTERFACE(IHelloWorldService, data, reply);
	    int index = data.readInt32();  
	    int bandindex= data.readInt32();  
	    int filterType = data.readInt32();  
            setFilterType(index,bandindex,filterType);
            return android::NO_ERROR;
        }break;
	case READSTEREOENHANCERINPEAKL: {
             CHECK_INTERFACE(IHelloWorldService, data, reply);
	    int index = data.readInt32();
            float levelL = readStereoEnhancerinpeakL(index);
	    reply->writeFloat(levelL);
            return android::NO_ERROR;
        }break;
	case READSTEREOENHANCERINPEAKR: {
             CHECK_INTERFACE(IHelloWorldService, data, reply);
	    int index = data.readInt32();
            float levelR = readStereoEnhancerinpeakR(index);
	    reply->writeFloat(levelR);
            return android::NO_ERROR;
        }break;
	case READSTEREOENHANCEROUTPEAKL: {
             CHECK_INTERFACE(IHelloWorldService, data, reply);
	    int index = data.readInt32();
            float levelL = readStereoEnhanceroutpeakL(index);
	    reply->writeFloat(levelL);
            return android::NO_ERROR;
        }break;
	case READSTEREOENHANCEROUTPEAKR: {
             CHECK_INTERFACE(IHelloWorldService, data, reply);
	    int index = data.readInt32();
            float levelR = readStereoEnhanceroutpeakR(index);
	    reply->writeFloat(levelR);
            return android::NO_ERROR;
        }break;
	case SETSTEREOENHANCERENABLED: {
            CHECK_INTERFACE(IHelloWorldService, data, reply);
	    int index = data.readInt32();  
	    int enable = data.readInt32();  
            setStereoEnhancerEnable(index,enable);
            return android::NO_ERROR;
        }break;
	case SETSTEREOENHANCERPARAMETERS: {
            CHECK_INTERFACE(IHelloWorldService, data, reply);
	    int index = data.readInt32();
	    int bandindex = data.readInt32();
	    float freq;
	    data.readFloat(&freq);
            setStereoEnhancerParameters(index,bandindex,freq);
            return android::NO_ERROR;
        }break;
	case READLIMITERINPEAKL: {
             CHECK_INTERFACE(IHelloWorldService, data, reply);
	    int index = data.readInt32();
            float levelL = readLimiterinpeakL(index);
	    reply->writeFloat(levelL);
            return android::NO_ERROR;
        }break;
	case READLIMITERINPEAKR: {
             CHECK_INTERFACE(IHelloWorldService, data, reply);
	    int index = data.readInt32();
            float levelR = readLimiterinpeakR(index);
	    reply->writeFloat(levelR);
            return android::NO_ERROR;
        }break;
	case READLIMITEROUTPEAKL: {
             CHECK_INTERFACE(IHelloWorldService, data, reply);
	    int index = data.readInt32();
            float levelL = readLimiteroutpeakL(index);
	    reply->writeFloat(levelL);
            return android::NO_ERROR;
        }break;
	case READLIMITEROUTPEAKR: {
             CHECK_INTERFACE(IHelloWorldService, data, reply);
	    int index = data.readInt32();
            float levelR = readLimiteroutpeakR(index);
	    reply->writeFloat(levelR);
            return android::NO_ERROR;
        }break;
	case SETLIMITERENABLED: {
            CHECK_INTERFACE(IHelloWorldService, data, reply);
	    int index = data.readInt32();  
	    int enable = data.readInt32();  
            setLimiterEnable(index,enable);
            return android::NO_ERROR;
        }break;
	case SETLIMITERGAIN: {
            CHECK_INTERFACE(IHelloWorldService, data, reply);
	    int index = data.readInt32();
	    float gain;
	    data.readFloat(&gain);
            setLimiterGain(index,gain);
            return android::NO_ERROR;
        }break;
	case GETLIMITERMETERGAIN: {
            CHECK_INTERFACE(IHelloWorldService, data, reply);
	    int index = data.readInt32();
            float metergain = getLimiterMeterGain(index);
	    reply->writeFloat(metergain);
            return android::NO_ERROR;
        }break;
	case READHEADPHONESINPEAKL: {
             CHECK_INTERFACE(IHelloWorldService, data, reply);
            float levelL = readHeadphonesinpeakL();
	    reply->writeFloat(levelL);
            return android::NO_ERROR;
        }break;
	case READHEADPHONESINPEAKR: {
             CHECK_INTERFACE(IHelloWorldService, data, reply);
            float levelR = readHeadphonesinpeakR();
	    reply->writeFloat(levelR);
            return android::NO_ERROR;
        }break;
	case SETHEADPHONESGAIN: {
            CHECK_INTERFACE(IHelloWorldService, data, reply);
	    float gainL;
	    float gainR;
	    data.readFloat(&gainL);
	    data.readFloat(&gainR);
            setHeadPhonesGain(gainL,gainR);
            return android::NO_ERROR;
        }break;
	case READMICINPEAKL: {
             CHECK_INTERFACE(IHelloWorldService, data, reply);
            float levelL = readMicinpeakL();
	    reply->writeFloat(levelL);
            return android::NO_ERROR;
        }break;
	case READMICINPEAKR: {
             CHECK_INTERFACE(IHelloWorldService, data, reply);
            float levelR = readMicinpeakR();
	    reply->writeFloat(levelR);
            return android::NO_ERROR;
        }break;
	case SETMICGAIN: {
            CHECK_INTERFACE(IHelloWorldService, data, reply);
	    float gain;
	    data.readFloat(&gain);
            setMicGain(gain);
            return android::NO_ERROR;
        }break;
	case GETRECBUFFER: {
             CHECK_INTERFACE(IHelloWorldService, data, reply);
            float *levelR = getRecBuff();
		//ALOGE("Calling MSG: PID=%f",levelR[0]);
	    reply->writeInt32Array(300,(int32_t *)levelR);
            return android::NO_ERROR;
        }break;
	case CONNECTPORTS: {
            CHECK_INTERFACE(IHelloWorldService, data, reply);
	    int mixerchannel = data.readInt32();  
	    int index = data.readInt32(); 
            ConnectPorts(mixerchannel, index);
            return android::NO_ERROR;
        }break;
	case SETINPUTVOL: {
            CHECK_INTERFACE(IHelloWorldService, data, reply);
	    float gainL;
	    float gainR;
	    data.readFloat(&gainL);
	    data.readFloat(&gainR);
            setInputVol(gainL,gainR);
            return android::NO_ERROR;
        }break;
        default:
                return BBinder::onTransact(code, data, reply, flags);
    }   
}
}



