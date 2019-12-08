#ifndef HELLOWORLDSERVICE
#define HELLOWORLDSERVICE

#include <binder/Parcel.h>
#include "BnHelloWorldService.h"
#include <utils/Log.h>

namespace android {

class HelloWorldService : public BnHelloWorldService {
public:
    static int instantiate();
    
    virtual ~HelloWorldService();
   
    virtual void Setup_Mixer_Main();
    virtual void Release_Mixer_Main();
    virtual void Connect_Mixer_Main();

    virtual void Setup_Mixer_Channel();
    virtual void Release_Mixer_Channel();
    virtual void Connect_Mixer_Channel();
    
    virtual void setInputGain(float gain);
    virtual float readinpeakL();
    virtual float readinpeakR();
    virtual void SetupEqualizer(int index);
    virtual void ConnectEqualizer(int index);

    virtual void SetupFilter(int index);
    virtual void ConnectFilter(int index);

    virtual void SetupStereoEnhancer(int index);
    virtual void ConnectStereoEnhancer(int index);

    virtual void SetupLimiter(int index);
    virtual void ConnectLimiter(int index);

    virtual float readoutpeak1L();
        virtual float readoutpeak1R();
        virtual float readoutpeak2L();
        virtual float readoutpeak2R();
        virtual float readoutpeak3L();
        virtual float readoutpeak3R();
	virtual float readplayerpeakL();
    	virtual float readplayerpeakR();

	virtual void setPlayerGain(float gain);
    	virtual void setMixerGain1(float gainL,float gainR);
    	virtual void setMixerGain2(float gainL,float gainR);
    	virtual void setMixerGain3(float gainL,float gainR);

    float readEqualizerinpeakL(int index);
    float readEqualizerinpeakR(int index);
    float readEqualizeroutpeakL(int index);
    float readEqualizeroutpeakR(int index);
    void setEqualizerGain(int index,float gainL,float gainR);
    void setEqualizerParameters(int index, int bandindex, float Freq, float Db);
    void setEqualizerEnable(int index, int enable);

    float readFilterinpeakL(int index);
    float readFilterinpeakR(int index);
    float readFilteroutpeakL(int index);
    float readFilteroutpeakR(int index);
    void setFilterParameters(int index, int bandindex, float Freq, float Db);
    void setFilterEnable(int index, int enable);
    void setFilterType(int index, int bandindex, int filterType);

    float readStereoEnhancerinpeakL(int index);
    float readStereoEnhancerinpeakR(int index);
    float readStereoEnhanceroutpeakL(int index);
    float readStereoEnhanceroutpeakR(int index);
    void setStereoEnhancerEnable(int index, int enable);
    void setStereoEnhancerParameters(int index, int bandindex, float Freq);

    float readLimiterinpeakL(int index);
    float readLimiterinpeakR(int index);
    float readLimiteroutpeakL(int index);
    float readLimiteroutpeakR(int index);
    void setLimiterEnable(int index, int enable);
    void setLimiterGain(int index, float gain);
    float getLimiterMeterGain(int index);

    float readHeadphonesinpeakL();
    float readHeadphonesinpeakR();
    void setHeadPhonesGain(float gainL,float gainR);

    float readMicinpeakL();
    float readMicinpeakR();
    void setMicGain(float gain);
    float *getRecBuff();
    void ConnectPorts(int mixerchannel, int index);
    void setInputVol(float volL, float volR);
private:
    HelloWorldService();
    Mixer_Main mixer_main;
    //Mixer_Channel mixer_channel;
    Filter filter[3];
    Equalizer equalizer[3];
    StereoEnhancer stereoEnhancer[3];
    Limiter limiter[3];
};

};

#endif
