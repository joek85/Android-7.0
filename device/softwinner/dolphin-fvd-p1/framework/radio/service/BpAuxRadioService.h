#ifndef BPAUXRADIOSERVICE
#define BPAUXRADIOSERVICE

#include <binder/Parcel.h>
#include "IAuxRadioService.h"

namespace android {
	class BpAuxRadioService: public BpInterface<IAuxRadioService> {
	public:
	BpAuxRadioService( const sp<IBinder> & impl);
	virtual void Set_Volume(int vol);
	virtual int powerOn();
	virtual void setChannel(int chan);
	virtual int seekUp();
	virtual int seekDown();
	virtual void powerOff();
	virtual int getChannel();
	virtual const char* readRDS();
	};
};

#endif
