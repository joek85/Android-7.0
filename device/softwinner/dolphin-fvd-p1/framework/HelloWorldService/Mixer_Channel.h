//
// Created by joe on 12/18/18.
//

#ifndef SERVICETEST_MIXER_CHANNEL_H
#define SERVICETEST_MIXER_CHANNEL_H
#include <jack.h>
extern "C"{
#include "compressor.h"
};
class Mixer_Channel{

private:
    float MixerChannel_Gain1L;
    float MixerChannel_Gain1R;
    float MixerChannel_Gain2L;
    float MixerChannel_Gain2R;
    float MixerChannel_Gain3L;
    float MixerChannel_Gain3R;
    float MixerChannel_Gain4L;
    float MixerChannel_Gain4R;

    float MixerChannel_Input_Peak1L;
    float MixerChannel_Input_Peak1R;
    float MixerChannel_Input_Peak2L;
    float MixerChannel_Input_Peak2R;
    float MixerChannel_Input_Peak3L;
    float MixerChannel_Input_Peak3R;
    float MixerChannel_Input_Peak4L;
    float MixerChannel_Input_Peak4R;

    float MixerChannel_Output_Peak1L;
    float MixerChannel_Output_Peak1R;

    jack_port_t *MixerChannel_Input_Port1R;
    jack_port_t *MixerChannel_Input_Port1L;
    jack_port_t *MixerChannel_Input_Port2R;
    jack_port_t *MixerChannel_Input_Port2L;
    jack_port_t *MixerChannel_Input_Port3R;
    jack_port_t *MixerChannel_Input_Port3L;
    jack_port_t *MixerChannel_Input_Port4R;
    jack_port_t *MixerChannel_Input_Port4L;

    jack_port_t *MixerChannel_Output_Port1L;
    jack_port_t *MixerChannel_Output_Port1R;
    jack_port_t *MixerChannel_Output_Port2L;
    jack_port_t *MixerChannel_Output_Port2R;
    jack_port_t *MixerChannel_Output_Port3L;
    jack_port_t *MixerChannel_Output_Port3R;
    jack_port_t *MixerChannel_Output_Port4L;
    jack_port_t *MixerChannel_Output_Port4R;

    jack_client_t *jackMixerChannelClient;

public:
    Mixer_Channel();

    static int process(jack_nframes_t nframes, void *arg);

    void Setup_Mixer_Channel();
    void Deactivate_Mixer_Channel();
    void Activate_Mixer_Channel();

    void setMixerChannel_Gain1L(float MixerChannel_Gain1L);

    void setMixerChannel_Gain1R(float MixerChannel_Gain1R);

    void setMixerChannel_Gain2L(float MixerChannel_Gain2L);

    void setMixerChannel_Gain2R(float MixerChannel_Gain2R);

    void setMixerChannel_Gain3L(float MixerChannel_Gain3L);

    void setMixerChannel_Gain3R(float MixerChannel_Gain3R);

    void setMixerChannel_Gain4L(float MixerChannel_Gain4L);

    void setMixerChannel_Gain4R(float MixerChannel_Gain4R);

    float getMixerChannel_Input_Peak1L();

    float getMixerChannel_Input_Peak1R();

    float getMixerChannel_Input_Peak2L();

    float getMixerChannel_Input_Peak2R();

    float getMixerChannel_Input_Peak3L();

    float getMixerChannel_Input_Peak3R();

    float getMixerChannel_Input_Peak4L();

    float getMixerChannel_Input_Peak4R();

    static void portregcallback(jack_port_id_t port, int, void *arg);

    void ConnectPorts();
};
#endif //SERVICETEST_MIXER_CHANNEL_H
