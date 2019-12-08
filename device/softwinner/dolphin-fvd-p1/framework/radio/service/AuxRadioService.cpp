#include <binder/IServiceManager.h>
#include <binder/IPCThreadState.h>
#include <utils/Log.h>

#include "BnAuxRadioService.h"
#include "AuxRadioService.h"
namespace android {

int AuxRadioService::instantiate() {
    ALOGI("AuxRadioService Added!");
    android::defaultServiceManager()->addService(String16("com.joek.AuxRadioService.IAuxRadioService"), new AuxRadioService());
    
    return 0;
}
AuxRadioService::~AuxRadioService(){
	ALOGI("~BnAuxRadioService destroyed");
	radio.powerOff();
}
AuxRadioService::AuxRadioService(){
	ALOGI("BnAuxRadioService created");	
	
}

void AuxRadioService::Set_Volume(int vol){
	radio.setVolume(vol);
}
int AuxRadioService::powerOn(){
	return radio.powerOn();
}
void AuxRadioService::setChannel(int chan){
	radio.setChannel(chan);
}
int AuxRadioService::seekUp(){
	return radio.seekUp();
}
int AuxRadioService::seekDown(){
	return radio.seekDown();
}
void AuxRadioService::powerOff(){
	radio.powerOff();
}
int AuxRadioService::getChannel(){
	return radio.getChannel();
}
const char* AuxRadioService::readRDS(){
//	char RdsBuffer[100] = {0};
	char *RdsBuffer = (char *)malloc(sizeof (char) * 100);
//        radio.readRDS(RdsBuffer, 1000);
	RdsBuffer[0] = 'h';
	return RdsBuffer;
}
};
