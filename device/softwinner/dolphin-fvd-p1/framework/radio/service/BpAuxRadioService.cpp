/**
 * This file is in PUBLIC DOMAIN. You can use it freely. No guarantee.
 * Author: Fan Hongtao <fanhongtao@gmail.com>
 * Date: 2011-06-28
 */

#define LOG_TAG "AuxRadioService"

#include <binder/Parcel.h>
#include <utils/Log.h>
#include <string.h>
#include "BpAuxRadioService.h"
namespace android{

//class BpHelloWorldService : public BpInterface<IHelloWorldService> {
//public:
    BpAuxRadioService::BpAuxRadioService(const sp<IBinder>& impl) : BpInterface<IAuxRadioService>(impl) {} 

    void BpAuxRadioService::Set_Volume(int vol) {
       Parcel data, reply;
       data.writeInterfaceToken(IAuxRadioService::getInterfaceDescriptor());
       data.writeInt32(vol);
       remote()->transact(SETVOLUME, data, &reply, 0);
    }
     int BpAuxRadioService::powerOn() {
       Parcel data, reply;
       data.writeInterfaceToken(IAuxRadioService::getInterfaceDescriptor());
       remote()->transact(POWERON, data, &reply, 0);
       reply.readExceptionCode();
       return reply.readInt32();
    }
     void BpAuxRadioService::setChannel(int chan) {
       Parcel data, reply;
       data.writeInterfaceToken(IAuxRadioService::getInterfaceDescriptor());
       data.writeInt32(chan);
       remote()->transact(SETCHANNEL, data, &reply, 0);
    }
    int BpAuxRadioService::seekUp() {
       Parcel data, reply;
       data.writeInterfaceToken(IAuxRadioService::getInterfaceDescriptor());
       remote()->transact(SEEKUP, data, &reply, 0);
       reply.readExceptionCode();
       return reply.readInt32();
    }
    int BpAuxRadioService::seekDown() {
       Parcel data, reply;
       data.writeInterfaceToken(IAuxRadioService::getInterfaceDescriptor());
       remote()->transact(SEEKDOWN, data, &reply, 0);
       reply.readExceptionCode();
       return reply.readInt32();
    }
     void BpAuxRadioService::powerOff() {
       Parcel data, reply;
       data.writeInterfaceToken(IAuxRadioService::getInterfaceDescriptor());
       remote()->transact(POWEROFF, data, &reply, 0);
    }
    int BpAuxRadioService::getChannel() {
       Parcel data, reply;
       data.writeInterfaceToken(IAuxRadioService::getInterfaceDescriptor());
       remote()->transact(GETCHANNEL, data, &reply, 0);
       reply.readExceptionCode();
       return reply.readInt32();
    }
    const char* BpAuxRadioService::readRDS() {
       Parcel data, reply;
	const char* rdsBuffer;
       data.writeInterfaceToken(IAuxRadioService::getInterfaceDescriptor());
       remote()->transact(READRDS, data, &reply, 0);
	reply.readExceptionCode();
	rdsBuffer = reply.readCString();
	return rdsBuffer;
    }
}; 




