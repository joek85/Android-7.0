#ifndef BNHELLOWORLDSERVICE
#define BNHELLOWORLDSERVICE

#include <binder/Parcel.h>
#include "IAuxRadioService.h"

namespace android {
    class BnAuxRadioService : public BnInterface<IAuxRadioService> {
        public:
        virtual status_t onTransact( uint32_t code,
                const Parcel& data,
                Parcel* reply,
                uint32_t flags = 0);
    };
};
#endif

