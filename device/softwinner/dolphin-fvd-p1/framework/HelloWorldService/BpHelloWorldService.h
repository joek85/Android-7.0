#ifndef BPHELLOWORLDSERVICE
#define BPHELLOWORLDSERVICE

#include <binder/Parcel.h>
#include "IHelloWorldService.h"

namespace android {
	class BpHelloWorldService: public BpInterface<IHelloWorldService> {
	public:
	BpHelloWorldService( const sp<IBinder> & impl);
	//virtual ~BpHelloWorldService(); 
	virtual void setInputGain(float gain);
	virtual void setPlayerGain(float gain);
	virtual float readinpeakL();
	virtual float readinpeakR();
	virtual float readplayerpeakL();
    	virtual float readplayerpeakR();
	virtual float readoutpeak1L();
        virtual float readoutpeak1R();
        virtual float readoutpeak2L();
        virtual float readoutpeak2R();
        virtual float readoutpeak3L();
        virtual float readoutpeak3R();
    	virtual void setMixerGain1(float gainL,float gainR);
    	virtual void setMixerGain2(float gainL,float gainR);
    	virtual void setMixerGain3(float gainL,float gainR);
        virtual float readEqualizerinpeakL(int index);
        virtual float readEqualizerinpeakR(int index);
        virtual float readEqualizeroutpeakL(int index);
        virtual float readEqualizeroutpeakR(int index);
        virtual void setEqualizerGain(int index,float gainL,float gainR);
	virtual void setEqualizerParameters(int index, int bandindex, float Freq, float Db);
	virtual void setEqualizerEnable(int index, int enable);
	virtual float readFilterinpeakL(int index);
    	virtual float readFilterinpeakR(int index);
    	virtual float readFilteroutpeakL(int index);
    	virtual float readFilteroutpeakR(int index);
    	virtual void setFilterParameters(int index, int bandindex, float Freq, float Db);
    	virtual void setFilterEnable(int index, int enable);
	virtual void setFilterType(int index, int bandindex, int filterType);
    	virtual float readStereoEnhancerinpeakL(int index);
    	virtual float readStereoEnhancerinpeakR(int index);
    	virtual float readStereoEnhanceroutpeakL(int index);
    	virtual float readStereoEnhanceroutpeakR(int index);
    	virtual void setStereoEnhancerEnable(int index, int enable);
    	virtual void setStereoEnhancerParameters(int index, int bandindex, float Freq);
    	virtual float readLimiterinpeakL(int index);
    	virtual float readLimiterinpeakR(int index);
    	virtual float readLimiteroutpeakL(int index);
    	virtual float readLimiteroutpeakR(int index);
    	virtual void setLimiterEnable(int index, int enable);
    	virtual void setLimiterGain(int index, float gain);
    	virtual float getLimiterMeterGain(int index);
	virtual float readHeadphonesinpeakL();
    	virtual float readHeadphonesinpeakR();
	virtual void setHeadPhonesGain(float gainL,float gainR);
	virtual float readMicinpeakL();
    	virtual float readMicinpeakR();
	virtual void setMicGain(float gain);
	virtual float *getRecBuff();
	virtual void ConnectPorts(int mixerchannel, int index);
	virtual void setInputVol(float volL, float volR);
	};
};

#endif
