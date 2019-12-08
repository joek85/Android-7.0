/**
 * This file is in PUBLIC DOMAIN. You can use it freely. No guarantee.
 * Author: Fan Hongtao <fanhongtao@gmail.com>
 * Date: 2011-06-28
 */

#ifndef IAUXRADIOSERVICE
#define IAUXRADIOSERVICE

#include <binder/IInterface.h>
#include "Si4703_Breakout.h"
#define SERVICE_NAME "com.joek.AuxRadioService.IAuxRadioService"
namespace android {
enum {
	SETVOLUME = IBinder::FIRST_CALL_TRANSACTION,
	POWERON,
	SETCHANNEL,
	SEEKUP,
	SEEKDOWN,
	POWEROFF,
	GETCHANNEL,
	READRDS
};

class IAuxRadioService : public IInterface {
public:
    DECLARE_META_INTERFACE(AuxRadioService);
	virtual void Set_Volume(int vol) = 0;
	virtual int powerOn() = 0;
	virtual void setChannel(int chan) = 0;
	virtual int seekUp() = 0;
	virtual int seekDown() = 0;
	virtual void powerOff() = 0;
	virtual int getChannel() = 0;
	virtual const char* readRDS() = 0;
};
 
};
#endif 

