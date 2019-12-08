#ifndef __ParametricEQ__
#define __ParametricEQ__

#include "JackConstants.h"
#include "JackPlatformPlug.h"
#include "jack.h"
#include "jslist.h"
#include <map>
#include <string>

#define SAPAPROXY_PORT_NUM_FOR_CLIENT  16

namespace Jack
{

class ParametricEQ
{

    private:

        jack_client_t* fClient;
        jack_port_t *fInputPorts[SAPAPROXY_PORT_NUM_FOR_CLIENT];
        jack_port_t *fOutputPorts[SAPAPROXY_PORT_NUM_FOR_CLIENT];
	float mGain;

    public:

        unsigned int fCapturePorts;
        unsigned int fPlaybackPorts;

        ParametricEQ(jack_client_t* jack_client, const JSList* params);
        ~ParametricEQ();

        int Setup(jack_client_t* jack_client);
        static int Process(jack_nframes_t nframes, void* arg);

	float read_peakL();
	float read_peakR();

};

}

#endif
