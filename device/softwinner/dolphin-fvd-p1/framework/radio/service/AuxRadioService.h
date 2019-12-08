#ifndef AUXRADIOSERVICE
#define AUXRADIOSERVICE

#include <binder/Parcel.h>
#include "BnAuxRadioService.h"
#include <utils/Log.h>

namespace android {

class AuxRadioService : public BnAuxRadioService {
public:
    static int instantiate();
    
    virtual ~AuxRadioService();
    virtual void Set_Volume(int vol);
    virtual int powerOn();
    virtual void setChannel(int chan);
    virtual int seekUp();
    virtual int seekDown();
    virtual void powerOff();
    virtual int getChannel();
    virtual const char* readRDS();
private:
    AuxRadioService();
    Si4703_Breakout radio;
//    char *RdsBuffer = (char *)malloc(sizeof (char) * 100);
};

};

#endif
