
#include "ParametricEQ.h"
#include "JackServerGlobals.h"
#include "JackEngineControl.h"
#include "JackLockedEngine.h"
#include "JackArgParser.h"
#include <assert.h>
#include <string>
#include <math.h>

//                               JackMixerChannel
//                                 .----------.
// 	  Mixer_Channel_Input_1 -->|          |--> Mixer_Channel_Output_1
// 	  Mixer_Channel_Input_2 -->|          |--> Mixer_Channel_Output_2
//                                 |          |    
//                                 |          |
//    		                   |          |
//    		 		   |          |
//    		 		   |          |
//                                 |          |
//                                 |          |   
//                                 '----------'

namespace Jack{
float peakL = 0.0f;
float peakR = 0.0f;
float ParametricEQ::read_peakL() {
    float tmpL = peakL;
    peakL = 0.0f;
    return tmpL;
}
float ParametricEQ::read_peakR() {
    float tmpR = peakR;
    peakR = 0.0f;
    return tmpR;
}


    ParametricEQ::~ParametricEQ()
    {
        jack_log("JackMixerChannel::~JackMixerChannel");
    }

    int ParametricEQ::Setup(jack_client_t* client)
    {
        jack_log("JackMixerChannel::Setup");

        //refer to system ports and create sapaproxy ports
        unsigned int i = 0, j = 0;
        const char **ports_system_capture;
        const char **ports_system_playback;
        unsigned int ports_system_capture_cnt = 0;
        unsigned int ports_system_playback_cnt = 0;
        char port_name[JACK_PORT_NAME_SIZE] = {0,};
	
        //ports_system_capture = jack_get_ports(client, "__system_capture.*", NULL, JackPortIsPhysical | JackPortIsOutput);
        //if (ports_system_capture != NULL) {
            for (i = 0; i < 2; i++) {
                sprintf(port_name, "Parametriceq_input%d", i + 1);
                fInputPorts[i] = jack_port_register(client, port_name, JACK_DEFAULT_AUDIO_TYPE, JackPortIsInput, 0);
                //sprintf(port_name, "capture_%d", i + 1);
                //fOutputPorts[i] = jack_port_register(client, port_name, JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0);
                ports_system_capture_cnt++;
            }
            //jack_free(ports_system_capture);
        //}

        //ports_system_playback = jack_get_ports(client, "__system_playback.*", NULL, JackPortIsPhysical | JackPortIsInput);
        //if (ports_system_playback != NULL) {
            for (j = 0; j < 2; j++) {
                //sprintf(port_name, "playback_%d", j + 1);
                //fInputPorts[i] = jack_port_register(client, port_name, JACK_DEFAULT_AUDIO_TYPE, JackPortIsInput, 0);
                sprintf(port_name, "Parametriceq_output%d", j + 1);
                fOutputPorts[j] = jack_port_register(client, port_name, JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0);
                ports_system_playback_cnt++;
            }
            //jack_free(ports_system_playback);
        //}

        //store actual number of system ports
        fCapturePorts = ports_system_capture_cnt;
        fPlaybackPorts = ports_system_playback_cnt;
	mGain = 1.0f;

        jack_set_process_callback(client, Process, this);
        jack_activate(client);

        //conenct between sapaproxy and system ports
       for (unsigned int i = 0; i < ports_system_capture_cnt; i++) {
            sprintf(port_name, "helloworldservice:capture_%d", i + 1);
            jack_connect(client, port_name, jack_port_name(fInputPorts[i]));
        }

        for (unsigned int i = 0; i < ports_system_playback_cnt; i++) {
            sprintf(port_name, "helloworldservice:playback_%d", i + 1);
            jack_connect(client, jack_port_name(fOutputPorts[i]), port_name);
        }

	//jack_free(ports_system_capture);
	//jack_free(ports_system_playback);
        return 0;
    }

    int ParametricEQ::Process(jack_nframes_t nframes, void* arg)
    {
        ParametricEQ* parametriceq = static_cast<ParametricEQ*>(arg);
        jack_default_audio_sample_t *inL, *outL, *inR, *outR;

        inL  = (jack_default_audio_sample_t*)jack_port_get_buffer(parametriceq->fInputPorts[0] , nframes);
        inR  = (jack_default_audio_sample_t*)jack_port_get_buffer(parametriceq->fInputPorts[1] , nframes);
        outL = (jack_default_audio_sample_t*)jack_port_get_buffer(parametriceq->fOutputPorts[0], nframes);
        outR = (jack_default_audio_sample_t*)jack_port_get_buffer(parametriceq->fOutputPorts[1], nframes);

        for (unsigned int i = 0; i < nframes; i++) {
	    outL[i] *= parametriceq->mGain;
	    outR[i] *= parametriceq->mGain;
            const float sL = (const float) fabs(outL[i]);
            const float sR = (const float) fabs(outR[i]);
            if (sL > peakL) {
                peakL = sL;
            }
            if (sR > peakR) {
                peakR = sR;
            }
        }

        memcpy(outL, inL, sizeof(jack_default_audio_sample_t) * nframes);
        memcpy(outR, inR, sizeof(jack_default_audio_sample_t) * nframes);

        return 0;
    }

} // namespace Jack
