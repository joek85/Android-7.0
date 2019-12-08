/**
 * This file is in PUBLIC DOMAIN. You can use it freely. No guarantee.
 * Author: Fan Hongtao <fanhongtao@gmail.com>
 * Date: 2011-06-28
 */

#define LOG_TAG "HelloWorldService"

#include <binder/Parcel.h>
#include <utils/Log.h>
#include <string.h>
#include "BpHelloWorldService.h"
namespace android{

//class BpHelloWorldService : public BpInterface<IHelloWorldService> {
//public:
    BpHelloWorldService::BpHelloWorldService(const sp<IBinder>& impl) : BpInterface<IHelloWorldService>(impl) {} 

    void BpHelloWorldService::setInputGain(float gain) {
       Parcel data, reply;
       data.writeInterfaceToken(IHelloWorldService::getInterfaceDescriptor());
       data.writeFloat(gain);
       remote()->transact(SETINPUTGAIN, data, &reply, 0);
    }
    void BpHelloWorldService::setPlayerGain(float gain) {
       Parcel data, reply;
       data.writeInterfaceToken(IHelloWorldService::getInterfaceDescriptor());
       data.writeFloat(gain);
       remote()->transact(SETPLAYERGAIN, data, &reply, 0);
    }
    float BpHelloWorldService::readinpeakL() {
       Parcel data, reply;
       data.writeInterfaceToken(IHelloWorldService::getInterfaceDescriptor());
       remote()->transact(READINPEAKL, data, &reply, 0);
       reply.readExceptionCode();
       return reply.readFloat();
    }
    float BpHelloWorldService::readinpeakR() {
       Parcel data, reply;
       data.writeInterfaceToken(IHelloWorldService::getInterfaceDescriptor());
       remote()->transact(READINPEAKR, data, &reply, 0);
       reply.readExceptionCode();
       return reply.readFloat();
    }
    float BpHelloWorldService::readplayerpeakL() {
       Parcel data, reply;
       data.writeInterfaceToken(IHelloWorldService::getInterfaceDescriptor());
       remote()->transact(READPLAYERPEAKL, data, &reply, 0);
       reply.readExceptionCode();
       return reply.readFloat();
    }
    float BpHelloWorldService::readplayerpeakR() {
       Parcel data, reply;
       data.writeInterfaceToken(IHelloWorldService::getInterfaceDescriptor());
       remote()->transact(READPLAYERPEAKR, data, &reply, 0);
       reply.readExceptionCode();
       return reply.readFloat();
    }
    float BpHelloWorldService::readoutpeak1L() {
       Parcel data, reply;
       data.writeInterfaceToken(IHelloWorldService::getInterfaceDescriptor());
       remote()->transact(READOUTPEAK1L, data, &reply, 0);
       reply.readExceptionCode();
       return reply.readFloat();
    }
    float BpHelloWorldService::readoutpeak1R() {
       Parcel data, reply;
       data.writeInterfaceToken(IHelloWorldService::getInterfaceDescriptor());
       remote()->transact(READOUTPEAK1R, data, &reply, 0);
       reply.readExceptionCode();
       return reply.readFloat();
    } 
    float BpHelloWorldService::readoutpeak2L() {
       Parcel data, reply;
       data.writeInterfaceToken(IHelloWorldService::getInterfaceDescriptor());
       remote()->transact(READOUTPEAK2L, data, &reply, 0);
       reply.readExceptionCode();
       return reply.readFloat();
    } 
    float BpHelloWorldService::readoutpeak2R() {
       Parcel data, reply;
       data.writeInterfaceToken(IHelloWorldService::getInterfaceDescriptor());
       remote()->transact(READOUTPEAK2R, data, &reply, 0);
       reply.readExceptionCode();
       return reply.readFloat();
    } 
    float BpHelloWorldService::readoutpeak3L() {
       Parcel data, reply;
       data.writeInterfaceToken(IHelloWorldService::getInterfaceDescriptor());
       remote()->transact(READOUTPEAK3L, data, &reply, 0);
       reply.readExceptionCode();
       return reply.readFloat();
    } 
    float BpHelloWorldService::readoutpeak3R() {
       Parcel data, reply;
       data.writeInterfaceToken(IHelloWorldService::getInterfaceDescriptor());
       remote()->transact(READOUTPEAK3R, data, &reply, 0);
       reply.readExceptionCode();
       return reply.readFloat();
    }
    void BpHelloWorldService::setMixerGain1(float gainL, float gainR) {
       Parcel data, reply;
       data.writeInterfaceToken(IHelloWorldService::getInterfaceDescriptor());
       data.writeFloat(gainL);
       data.writeFloat(gainR);
       remote()->transact(SETMIXERGAIN1, data, &reply, 0);
    }
    void BpHelloWorldService::setMixerGain2(float gainL, float gainR) {
       Parcel data, reply;
       data.writeInterfaceToken(IHelloWorldService::getInterfaceDescriptor());
       data.writeFloat(gainL);
       data.writeFloat(gainR);
       remote()->transact(SETMIXERGAIN2, data, &reply, 0);
    }
    void BpHelloWorldService::setMixerGain3(float gainL, float gainR) {
       Parcel data, reply;
       data.writeInterfaceToken(IHelloWorldService::getInterfaceDescriptor());
       data.writeFloat(gainL);
       data.writeFloat(gainR);
       remote()->transact(SETMIXERGAIN3, data, &reply, 0);
    }
    float BpHelloWorldService::readEqualizerinpeakL(int index) {
       Parcel data, reply;
       data.writeInterfaceToken(IHelloWorldService::getInterfaceDescriptor());
       data.writeInt32(index);
       remote()->transact(READEQINPEAKL, data, &reply, 0);
       reply.readExceptionCode();
       return reply.readFloat();
    }
    float BpHelloWorldService::readEqualizerinpeakR(int index) {
       Parcel data, reply;
       data.writeInterfaceToken(IHelloWorldService::getInterfaceDescriptor());
       data.writeInt32(index);
       remote()->transact(READEQINPEAKR, data, &reply, 0);
       reply.readExceptionCode();
       return reply.readFloat();
    }
    float BpHelloWorldService::readEqualizeroutpeakL(int index) {
       Parcel data, reply;
       data.writeInterfaceToken(IHelloWorldService::getInterfaceDescriptor());
       data.writeInt32(index);
       remote()->transact(READEQOUTPEAKL, data, &reply, 0);
       reply.readExceptionCode();
       return reply.readFloat();
    }
    float BpHelloWorldService::readEqualizeroutpeakR(int index) {
       Parcel data, reply;
       data.writeInterfaceToken(IHelloWorldService::getInterfaceDescriptor());
       data.writeInt32(index);
       remote()->transact(READEQOUTPEAKR, data, &reply, 0);
       reply.readExceptionCode();
       return reply.readFloat();
    }
    void BpHelloWorldService::setEqualizerGain(int index, float gainL, float gainR) {
       Parcel data, reply;
       data.writeInterfaceToken(IHelloWorldService::getInterfaceDescriptor());
       data.writeInt32(index);
       data.writeFloat(gainL);
       data.writeFloat(gainR);
       remote()->transact(SETEQGAIN, data, &reply, 0);
    }
    void BpHelloWorldService::setEqualizerParameters(int index, int bandindex, float Freq, float Db) {
       Parcel data, reply;
       data.writeInterfaceToken(IHelloWorldService::getInterfaceDescriptor());
       data.writeInt32(index);
       data.writeInt32(bandindex);
       data.writeFloat(Freq);
       data.writeFloat(Db);
       remote()->transact(SETEQPARAMETERS, data, &reply, 0);
    }
    void BpHelloWorldService::setEqualizerEnable(int index, int enable) {
       Parcel data, reply;
       data.writeInterfaceToken(IHelloWorldService::getInterfaceDescriptor());
       data.writeInt32(index); 
       data.writeInt32(enable); 
       remote()->transact(SETEQENABLED, data, &reply, 0);
    }
    float BpHelloWorldService::readFilterinpeakL(int index) {
       Parcel data, reply;
       data.writeInterfaceToken(IHelloWorldService::getInterfaceDescriptor());
       data.writeInt32(index);
       remote()->transact(READFILTERINPEAKL, data, &reply, 0);
       reply.readExceptionCode();
       return reply.readFloat();
    }
    float BpHelloWorldService::readFilterinpeakR(int index) {
       Parcel data, reply;
       data.writeInterfaceToken(IHelloWorldService::getInterfaceDescriptor());
       data.writeInt32(index);
       remote()->transact(READFILTERINPEAKR, data, &reply, 0);
       reply.readExceptionCode();
       return reply.readFloat();
    }
    float BpHelloWorldService::readFilteroutpeakL(int index) {
       Parcel data, reply;
       data.writeInterfaceToken(IHelloWorldService::getInterfaceDescriptor());
       data.writeInt32(index);
       remote()->transact(READFILTEROUTPEAKL, data, &reply, 0);
       reply.readExceptionCode();
       return reply.readFloat();
    }
    float BpHelloWorldService::readFilteroutpeakR(int index) {
       Parcel data, reply;
       data.writeInterfaceToken(IHelloWorldService::getInterfaceDescriptor());
       data.writeInt32(index);
       remote()->transact(READFILTEROUTPEAKR, data, &reply, 0);
       reply.readExceptionCode();
       return reply.readFloat();
    }
    void BpHelloWorldService::setFilterParameters(int index, int bandindex, float Freq, float Db) {
       Parcel data, reply;
       data.writeInterfaceToken(IHelloWorldService::getInterfaceDescriptor());
       data.writeInt32(index);
       data.writeInt32(bandindex);
       data.writeFloat(Freq);
       data.writeFloat(Db);
       remote()->transact(SETFILTERPARAMETERS, data, &reply, 0);
    }
    void BpHelloWorldService::setFilterEnable(int index, int enable) {
       Parcel data, reply;
       data.writeInterfaceToken(IHelloWorldService::getInterfaceDescriptor());
       data.writeInt32(index); 
       data.writeInt32(enable); 
       remote()->transact(SETFILTERENABLED, data, &reply, 0);
    }
    void BpHelloWorldService::setFilterType(int index, int bandindex, int filterType) {
       Parcel data, reply;
       data.writeInterfaceToken(IHelloWorldService::getInterfaceDescriptor());
       data.writeInt32(index); 
       data.writeInt32(bandindex); 
       data.writeInt32(filterType); 
       remote()->transact(SETFILTERTYPE, data, &reply, 0);
    }
    float BpHelloWorldService::readStereoEnhancerinpeakL(int index) {
       Parcel data, reply;
       data.writeInterfaceToken(IHelloWorldService::getInterfaceDescriptor());
       data.writeInt32(index);
       remote()->transact(READSTEREOENHANCERINPEAKL, data, &reply, 0);
       reply.readExceptionCode();
       return reply.readFloat();
    }
    float BpHelloWorldService::readStereoEnhancerinpeakR(int index) {
       Parcel data, reply;
       data.writeInterfaceToken(IHelloWorldService::getInterfaceDescriptor());
       data.writeInt32(index);
       remote()->transact(READSTEREOENHANCERINPEAKR, data, &reply, 0);
       reply.readExceptionCode();
       return reply.readFloat();
    }
    float BpHelloWorldService::readStereoEnhanceroutpeakL(int index) {
       Parcel data, reply;
       data.writeInterfaceToken(IHelloWorldService::getInterfaceDescriptor());
       data.writeInt32(index);
       remote()->transact(READSTEREOENHANCEROUTPEAKL, data, &reply, 0);
       reply.readExceptionCode();
       return reply.readFloat();
    }
    float BpHelloWorldService::readStereoEnhanceroutpeakR(int index) {
       Parcel data, reply;
       data.writeInterfaceToken(IHelloWorldService::getInterfaceDescriptor());
       data.writeInt32(index);
       remote()->transact(READSTEREOENHANCEROUTPEAKR, data, &reply, 0);
       reply.readExceptionCode();
       return reply.readFloat();
    }
    void BpHelloWorldService::setStereoEnhancerEnable(int index, int enable) {
       Parcel data, reply;
       data.writeInterfaceToken(IHelloWorldService::getInterfaceDescriptor());
       data.writeInt32(index); 
       data.writeInt32(enable); 
       remote()->transact(SETSTEREOENHANCERENABLED, data, &reply, 0);
    }
    void BpHelloWorldService::setStereoEnhancerParameters(int index, int bandindex, float Freq) {
       Parcel data, reply;
       data.writeInterfaceToken(IHelloWorldService::getInterfaceDescriptor());
       data.writeInt32(index);
       data.writeInt32(bandindex);
       data.writeFloat(Freq);
       remote()->transact(SETSTEREOENHANCERPARAMETERS, data, &reply, 0);
    }
    float BpHelloWorldService::readLimiterinpeakL(int index) {
       Parcel data, reply;
       data.writeInterfaceToken(IHelloWorldService::getInterfaceDescriptor());
       data.writeInt32(index);
       remote()->transact(READLIMITERINPEAKL, data, &reply, 0);
       reply.readExceptionCode();
       return reply.readFloat();
    }
    float BpHelloWorldService::readLimiterinpeakR(int index) {
       Parcel data, reply;
       data.writeInterfaceToken(IHelloWorldService::getInterfaceDescriptor());
       data.writeInt32(index);
       remote()->transact(READLIMITERINPEAKR, data, &reply, 0);
       reply.readExceptionCode();
       return reply.readFloat();
    }
    float BpHelloWorldService::readLimiteroutpeakL(int index) {
       Parcel data, reply;
       data.writeInterfaceToken(IHelloWorldService::getInterfaceDescriptor());
       data.writeInt32(index);
       remote()->transact(READLIMITEROUTPEAKL, data, &reply, 0);
       reply.readExceptionCode();
       return reply.readFloat();
    }
    float BpHelloWorldService::readLimiteroutpeakR(int index) {
       Parcel data, reply;
       data.writeInterfaceToken(IHelloWorldService::getInterfaceDescriptor());
       data.writeInt32(index);
       remote()->transact(READLIMITEROUTPEAKR, data, &reply, 0);
       reply.readExceptionCode();
       return reply.readFloat();
    }
    void BpHelloWorldService::setLimiterEnable(int index, int enable) {
       Parcel data, reply;
       data.writeInterfaceToken(IHelloWorldService::getInterfaceDescriptor());
       data.writeInt32(index); 
       data.writeInt32(enable); 
       remote()->transact(SETLIMITERENABLED, data, &reply, 0);
    }
    void BpHelloWorldService::setLimiterGain(int index, float gain) {
       Parcel data, reply;
       data.writeInterfaceToken(IHelloWorldService::getInterfaceDescriptor());
       data.writeInt32(index);
       data.writeFloat(gain);
       remote()->transact(SETLIMITERGAIN, data, &reply, 0);
    }
    float BpHelloWorldService::getLimiterMeterGain(int index) {
       Parcel data, reply;
       data.writeInterfaceToken(IHelloWorldService::getInterfaceDescriptor());
       data.writeInt32(index);
       remote()->transact(GETLIMITERMETERGAIN, data, &reply, 0);
       reply.readExceptionCode();
       return reply.readFloat();
    }
    float BpHelloWorldService::readHeadphonesinpeakL() {
       Parcel data, reply;
       data.writeInterfaceToken(IHelloWorldService::getInterfaceDescriptor());
       remote()->transact(READHEADPHONESINPEAKL, data, &reply, 0);
       return reply.readFloat();
    }
    float BpHelloWorldService::readHeadphonesinpeakR() {
       Parcel data, reply;
       data.writeInterfaceToken(IHelloWorldService::getInterfaceDescriptor());
       remote()->transact(READHEADPHONESINPEAKR, data, &reply, 0);
       reply.readExceptionCode();
       return reply.readFloat();
    }
    void BpHelloWorldService::setHeadPhonesGain(float gainL, float gainR) {
       Parcel data, reply;
       data.writeInterfaceToken(IHelloWorldService::getInterfaceDescriptor());
       data.writeFloat(gainL);
       data.writeFloat(gainR);
       remote()->transact(SETHEADPHONESGAIN, data, &reply, 0);
    }
    float BpHelloWorldService::readMicinpeakL() {
       Parcel data, reply;
       data.writeInterfaceToken(IHelloWorldService::getInterfaceDescriptor());
       remote()->transact(READMICINPEAKL, data, &reply, 0);
       return reply.readFloat();
    }
    float BpHelloWorldService::readMicinpeakR() {
       Parcel data, reply;
       data.writeInterfaceToken(IHelloWorldService::getInterfaceDescriptor());
       remote()->transact(READMICINPEAKR, data, &reply, 0);
       reply.readExceptionCode();
       return reply.readFloat();
    }
    void BpHelloWorldService::setMicGain(float gain) {
       Parcel data, reply;
       data.writeInterfaceToken(IHelloWorldService::getInterfaceDescriptor());
       data.writeFloat(gain);
       remote()->transact(SETMICGAIN, data, &reply, 0);
    }
    float *BpHelloWorldService::getRecBuff() {
       Parcel data, reply;
	float *pizza;// = (float *)malloc(sizeof(float)* 300);;
       data.writeInterfaceToken(IHelloWorldService::getInterfaceDescriptor());
       remote()->transact(GETRECBUFFER, data, &reply, 0);
       //pizza = reply.createFloatArray();
	reply.readExceptionCode();
	reply.readFloat(pizza);
       return pizza;
    }
    void BpHelloWorldService::ConnectPorts(int mixerchannel, int index) {
       Parcel data, reply;
       data.writeInterfaceToken(IHelloWorldService::getInterfaceDescriptor());
       data.writeInt32(mixerchannel);
       data.writeInt32(index);
       remote()->transact(CONNECTPORTS, data, &reply, 0);
    }
    void BpHelloWorldService::setInputVol(float gainL, float gainR) {
       Parcel data, reply;
       data.writeInterfaceToken(IHelloWorldService::getInterfaceDescriptor());
       data.writeFloat(gainL);
       data.writeFloat(gainR);
       remote()->transact(SETINPUTVOL, data, &reply, 0);
    }
}; 




