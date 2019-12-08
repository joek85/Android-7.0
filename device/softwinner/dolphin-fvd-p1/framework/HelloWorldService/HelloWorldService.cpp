#include <binder/IServiceManager.h>
#include <binder/IPCThreadState.h>
#include <utils/Log.h>

#include "BnHelloWorldService.h"
#include "HelloWorldService.h"
namespace android {

int HelloWorldService::instantiate() {
    ALOGI("HelloWorldService Added!");
    android::defaultServiceManager()->addService(String16("com.joek.helloworldservice.IHelloWorldService"), new HelloWorldService());
    return 0;
}
HelloWorldService::~HelloWorldService(){
	ALOGI("~BnHelloWorldService()");
}
HelloWorldService::HelloWorldService(){
	ALOGI("BnHelloWorldService()");
	Setup_Mixer_Main();
	//Setup_Mixer_Channel();
	SetupEqualizer(0);
	SetupEqualizer(1);
	SetupEqualizer(2);
	SetupFilter(0);
	SetupFilter(1);
	SetupFilter(2);
	SetupStereoEnhancer(0);
	SetupStereoEnhancer(1);
	SetupStereoEnhancer(2);
	SetupLimiter(0);
	SetupLimiter(1);
	SetupLimiter(2);


	ConnectEqualizer(0);
	ConnectEqualizer(1);
	ConnectEqualizer(2);
	ConnectFilter(0);
	ConnectFilter(1);
	ConnectFilter(2);
	ConnectStereoEnhancer(0);
	ConnectStereoEnhancer(1);
	ConnectStereoEnhancer(2);
	ConnectLimiter(0);
	ConnectLimiter(1);
	ConnectLimiter(2);

	Connect_Mixer_Main();
	//Connect_Mixer_Channel();

}
void HelloWorldService::Setup_Mixer_Main(){
	mixer_main.Setup_Mixer_Main();
}
void HelloWorldService::Release_Mixer_Main(){
	//mixer_main.Release_Mixer_Main();
}
void HelloWorldService::Setup_Mixer_Channel(){
	//mixer_channel.Setup_Mixer_Channel();
}
void HelloWorldService::Release_Mixer_Channel(){
	//mixer_channel.Release_Mixer_Channel();
}

float HelloWorldService::readinpeakL(){
	return mixer_main.getInput_PeakL();
}
float HelloWorldService::readinpeakR(){
	return mixer_main.getInput_PeakR();
}
void HelloWorldService::setInputGain(float gain){
	mixer_main.setInput_GainL(gain);
        mixer_main.setInput_GainR(gain);
}

void HelloWorldService::SetupFilter(int index){
	filter[index].Setup_Filter(index);
}
void HelloWorldService::SetupEqualizer(int index){
	equalizer[index].Setup_Equalizer(index);
}

void HelloWorldService::Connect_Mixer_Main(){
	mixer_main.ConnectPorts();
}
void HelloWorldService::Connect_Mixer_Channel(){
	//mixer_channel.ConnectPorts();
}
void HelloWorldService::ConnectEqualizer(int index){
	equalizer[index].ConnectPorts(index);
}
void HelloWorldService::ConnectFilter(int index){
	filter[index].ConnectPorts(index);
}
void HelloWorldService::SetupLimiter(int index){
	limiter[index].Setup_Limiter(index);
}
void HelloWorldService::ConnectLimiter(int index){
	limiter[index].ConnectPorts(index);
}

float HelloWorldService::readoutpeak1L(){
	return limiter[0].getLimiter_Output_PeakL();
}
float HelloWorldService::readoutpeak1R(){
	return limiter[0].getLimiter_Output_PeakR();
}
float HelloWorldService::readoutpeak2L(){
	return limiter[1].getLimiter_Output_PeakL();
}
float HelloWorldService::readoutpeak2R(){
	return limiter[1].getLimiter_Output_PeakR();
}
float HelloWorldService::readoutpeak3L(){
	return limiter[2].getLimiter_Output_PeakL();
}
float HelloWorldService::readoutpeak3R(){
	return limiter[2].getLimiter_Output_PeakR();
}
float HelloWorldService::readplayerpeakL(){
	return mixer_main.getPlayer_PeakL();
}
float HelloWorldService::readplayerpeakR(){
	return mixer_main.getPlayer_PeakR();
}

void HelloWorldService::setPlayerGain(float gain){
	mixer_main.setPlayer_GainL(gain);
        mixer_main.setPlayer_GainR(gain);
}
void HelloWorldService::setMixerGain1(float gainL, float gainR){
	//mixer_channel.setMixerChannel_Gain1L(gainL);
	//mixer_channel.setMixerChannel_Gain1R(gainR);
	filter[0].setFilter_GainL(gainL);
	filter[0].setFilter_GainR(gainR);
}
void HelloWorldService::setMixerGain2(float gainL, float gainR){
	//mixer_channel.setMixerChannel_Gain2L(gainL);
	//mixer_channel.setMixerChannel_Gain2R(gainR);
	filter[1].setFilter_GainL(gainL);
	filter[1].setFilter_GainR(gainR);
}//
void HelloWorldService::setMixerGain3(float gainL, float gainR){
	//mixer_channel.setMixerChannel_Gain3L(gainL);
	//mixer_channel.setMixerChannel_Gain3R(gainR);
	filter[2].setFilter_GainL(gainL);
	filter[2].setFilter_GainR(gainR);
}
float HelloWorldService::readEqualizerinpeakL(int index){
	return equalizer[index].getEqualizer_Input_PeakL();
}
float HelloWorldService::readEqualizerinpeakR(int index){
	return equalizer[index].getEqualizer_Input_PeakR();
}
float HelloWorldService::readEqualizeroutpeakL(int index){
	return equalizer[index].getEqualizer_Output_PeakL();
}
float HelloWorldService::readEqualizeroutpeakR(int index){
	return equalizer[index].getEqualizer_Output_PeakR();
}
void HelloWorldService::setEqualizerGain(int index,float gainL,float gainR){
	equalizer[index].setEqualizer_GainL(gainL);
	equalizer[index].setEqualizer_GainR(gainR);
}
void HelloWorldService::setEqualizerParameters(int index, int bandindex, float Freq, float Db){
	equalizer[index].seteqFreq(bandindex,Freq);
	equalizer[index].seteqGain(bandindex,Db);
}
void HelloWorldService::setEqualizerEnable(int index, int enable){
	equalizer[index].setEnabled(enable ? 1 : 0);
}
float HelloWorldService::readFilterinpeakL(int index){
	return filter[index].getFilter_Input_PeakL();
}
float HelloWorldService::readFilterinpeakR(int index){
	return filter[index].getFilter_Input_PeakR();
}
float HelloWorldService::readFilteroutpeakL(int index){
	return filter[index].getFilter_Output_PeakL();
}
float HelloWorldService::readFilteroutpeakR(int index){
	return filter[index].getFilter_Output_PeakR();
}
void HelloWorldService::setFilterParameters(int index, int bandindex, float Freq, float Db){
	filter[index].setfilterFreq(bandindex,Freq);
	filter[index].setfilterGain(bandindex,Db);
}
void HelloWorldService::setFilterEnable(int index, int enable){
	filter[index].setFilterenabled(enable ? 1 : 0);
}
void HelloWorldService::setFilterType(int index, int bandindex, int filterType){
	filter[index].setFilterType(bandindex, filterType);
}
void HelloWorldService::SetupStereoEnhancer(int index){
	stereoEnhancer[index].Setup_StereoEnhancer(index);
}
void HelloWorldService::ConnectStereoEnhancer(int index){
	stereoEnhancer[index].ConnectPorts(index);
}
float HelloWorldService::readStereoEnhancerinpeakL(int index){
	return stereoEnhancer[index].getStereoEnhancer_Input_PeakL();
}
float HelloWorldService::readStereoEnhancerinpeakR(int index){
	return stereoEnhancer[index].getStereoEnhancer_Input_PeakR();
}
float HelloWorldService::readStereoEnhanceroutpeakL(int index){
	return stereoEnhancer[index].getStereoEnhancer_Output_PeakL();
}
float HelloWorldService::readStereoEnhanceroutpeakR(int index){
	return stereoEnhancer[index].getStereoEnhancer_Output_PeakR();
}
void HelloWorldService::setStereoEnhancerEnable(int index, int enable){
	stereoEnhancer[index].setStereoEnhancerenabled(enable ? 1 : 0);
}
void HelloWorldService::setStereoEnhancerParameters(int index, int bandindex, float Freq){
	stereoEnhancer[index].setStereoEnhancerFreq(bandindex,Freq);
}
float HelloWorldService::readLimiterinpeakL(int index){
	return limiter[index].getLimiter_Input_PeakL();
}
float HelloWorldService::readLimiterinpeakR(int index){
	return limiter[index].getLimiter_Input_PeakR();
}
float HelloWorldService::readLimiteroutpeakL(int index){
	return limiter[index].getLimiter_Output_PeakL();
}
float HelloWorldService::readLimiteroutpeakR(int index){
	return limiter[index].getLimiter_Output_PeakR();
}
void HelloWorldService::setLimiterEnable(int index, int enable){
	limiter[index].setLimiterenabled(enable ? 1 : 0);
}
void HelloWorldService::setLimiterGain(int index, float gain){
	limiter[index].setPregain(gain);
}
float HelloWorldService::getLimiterMeterGain(int index){
	return limiter[index].getMeterGain();
}
float HelloWorldService::readHeadphonesinpeakL(){
	return mixer_main.getHeadPhones_PeakL();
}
float HelloWorldService::readHeadphonesinpeakR(){
	return mixer_main.getHeadPhones_PeakR();
}
void HelloWorldService::setHeadPhonesGain(float gainL,float gainR){
	mixer_main.setHeadPhones_GainL(gainL);
	mixer_main.setHeadPhones_GainR(gainR);
}
float HelloWorldService::readMicinpeakL(){
	return mixer_main.getMic_PeakL();
}
float HelloWorldService::readMicinpeakR(){
	return mixer_main.getMic_PeakR();
}
void HelloWorldService::setMicGain(float gain){
	mixer_main.setMic_GainL(gain);
	mixer_main.setMic_GainR(gain);
}
float *HelloWorldService::getRecBuff(){
	return mixer_main.getRecBuffer();
}
void HelloWorldService::ConnectPorts(int mixerchannel, int index){
	mixer_main.ConnectPorts(mixerchannel, index);
}
void HelloWorldService::setInputVol(float volL, float volR){
	mixer_main.setInput_VolL(volL);
	mixer_main.setInput_VolR(volR);	
}


};
