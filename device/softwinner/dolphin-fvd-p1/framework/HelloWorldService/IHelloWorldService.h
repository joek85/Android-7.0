/**
 * This file is in PUBLIC DOMAIN. You can use it freely. No guarantee.
 * Author: Fan Hongtao <fanhongtao@gmail.com>
 * Date: 2011-06-28
 */

#ifndef IHELLOWORLDSERVICE
#define IHELLOWORLDSERVICE

#include <binder/IInterface.h>
#include "jack.h"
#include "Mixer_Main.h"
#include "Mixer_Channel.h"
#include "Equalizer.h"
#include "Filter.h"
#include "StereoEnhancer.h"
#include "Limiter.h"
#define SERVICE_NAME "com.joek.helloworldservice.IHelloWorldService"
namespace android {
enum {
    	SETINPUTGAIN = IBinder::FIRST_CALL_TRANSACTION,
	SETPLAYERGAIN,
	READINPEAKL,
	READINPEAKR,
	READPLAYERPEAKL,
	READPLAYERPEAKR,
	READOUTPEAK1L,
	READOUTPEAK1R,
	READOUTPEAK2L,
	READOUTPEAK2R,
	READOUTPEAK3L,
	READOUTPEAK3R,
	SETMIXERGAIN1,
	SETMIXERGAIN2,
	SETMIXERGAIN3,
	READEQINPEAKL,
	READEQINPEAKR,
	READEQOUTPEAKL,
	READEQOUTPEAKR,
	SETEQGAIN,
	SETEQPARAMETERS,
	SETEQENABLED,
	READFILTERINPEAKL,
	READFILTERINPEAKR,
	READFILTEROUTPEAKL,
	READFILTEROUTPEAKR,
	SETFILTERPARAMETERS,
	SETFILTERENABLED,
	SETFILTERTYPE,
	READSTEREOENHANCERINPEAKL,
	READSTEREOENHANCERINPEAKR,
	READSTEREOENHANCEROUTPEAKL,
	READSTEREOENHANCEROUTPEAKR,
	SETSTEREOENHANCERENABLED,
	SETSTEREOENHANCERPARAMETERS,
	READLIMITERINPEAKL,
	READLIMITERINPEAKR,
	READLIMITEROUTPEAKL,
	READLIMITEROUTPEAKR,
	SETLIMITERENABLED,
	SETLIMITERGAIN,
	GETLIMITERMETERGAIN,
	READHEADPHONESINPEAKL,
	READHEADPHONESINPEAKR,
	SETHEADPHONESGAIN,
	READMICINPEAKL,
	READMICINPEAKR,
	SETMICGAIN,
	GETRECBUFFER,
	CONNECTPORTS,
	SETINPUTVOL
};

class IHelloWorldService : public IInterface {
public:
    DECLARE_META_INTERFACE(HelloWorldService);

    	virtual void setInputGain(float gain) = 0;
	virtual void setPlayerGain(float gain) = 0;
    	virtual float readinpeakL() = 0;
    	virtual float readinpeakR() = 0;
	virtual float readplayerpeakL() = 0;
    	virtual float readplayerpeakR() = 0;
	virtual float readoutpeak1L() = 0;
        virtual float readoutpeak1R() = 0;
        virtual float readoutpeak2L() = 0;
        virtual float readoutpeak2R() = 0;
        virtual float readoutpeak3L() = 0;
        virtual float readoutpeak3R() = 0;
    	virtual void setMixerGain1(float gainL,float gainR) = 0;
    	virtual void setMixerGain2(float gainL,float gainR) = 0;
    	virtual void setMixerGain3(float gainL,float gainR) = 0;
        virtual float readEqualizerinpeakL(int index) = 0;
        virtual float readEqualizerinpeakR(int index) = 0;
        virtual float readEqualizeroutpeakL(int index) = 0;
        virtual float readEqualizeroutpeakR(int index) = 0;
        virtual void setEqualizerGain(int index,float gainL,float gainR) = 0;
	virtual void setEqualizerParameters(int index, int bandindex, float Freq, float Db) = 0;
	virtual void setEqualizerEnable(int index, int enable) = 0;
	virtual float readFilterinpeakL(int index) = 0;
    	virtual float readFilterinpeakR(int index) = 0;
    	virtual float readFilteroutpeakL(int index) = 0;
    	virtual float readFilteroutpeakR(int index) = 0;
    	virtual void setFilterParameters(int index, int bandindex, float Freq, float Db) = 0;
    	virtual void setFilterEnable(int index, int enable) = 0;
	virtual void setFilterType(int index, int bandindex, int filterType) = 0;
    	virtual float readStereoEnhancerinpeakL(int index) = 0;
    	virtual float readStereoEnhancerinpeakR(int index) = 0;
    	virtual float readStereoEnhanceroutpeakL(int index) = 0;
    	virtual float readStereoEnhanceroutpeakR(int index) = 0;
    	virtual void setStereoEnhancerEnable(int index, int enable) = 0;
    	virtual void setStereoEnhancerParameters(int index, int bandindex, float Freq) = 0;
    	virtual float readLimiterinpeakL(int index) = 0;
    	virtual float readLimiterinpeakR(int index) = 0;
    	virtual float readLimiteroutpeakL(int index) = 0;
    	virtual float readLimiteroutpeakR(int index) = 0;
    	virtual void setLimiterEnable(int index, int enable) = 0;
    	virtual void setLimiterGain(int index, float gain) = 0;
    	virtual float getLimiterMeterGain(int index) = 0;
	virtual float readHeadphonesinpeakL() = 0;
    	virtual float readHeadphonesinpeakR() = 0;
	virtual void setHeadPhonesGain(float gainL,float gainR) = 0;
	virtual float readMicinpeakL() = 0;
    	virtual float readMicinpeakR() = 0;
	virtual void setMicGain(float gain) = 0;
	virtual float *getRecBuff() = 0;
	virtual void ConnectPorts(int mixerchannel, int index) = 0;
	virtual void setInputVol(float volL, float volR) = 0;
};
 
};
#endif  // I_HELLOWORLD_SERVICE_H

