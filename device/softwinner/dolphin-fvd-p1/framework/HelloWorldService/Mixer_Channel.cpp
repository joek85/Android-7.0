//
// Created by joe on 12/18/18.
//

#include <stdio.h>
#include <math.h>
#include "Mixer_Channel.h"
#include "LogHelper.h"

Mixer_Channel::Mixer_Channel() {
    MixerChannel_Gain1R = 1.0f;
    MixerChannel_Gain1L = 1.0f;
    MixerChannel_Gain2R = 1.0f;
    MixerChannel_Gain2L = 1.0f;
    MixerChannel_Gain3R = 1.0f;
    MixerChannel_Gain3L = 1.0f;
//    MixerChannel_Gain4R = 1.0f;
//    MixerChannel_Gain4L = 1.0f;

    MixerChannel_Input_Peak1L = 0.0f;
    MixerChannel_Input_Peak1R = 0.0f;
    MixerChannel_Input_Peak2L = 0.0f;
    MixerChannel_Input_Peak2R = 0.0f;
    MixerChannel_Input_Peak3L = 0.0f;
    MixerChannel_Input_Peak3R = 0.0f;
//    MixerChannel_Input_Peak4L = 0.0f;
//    MixerChannel_Input_Peak4R = 0.0f;
}

float Mixer_Channel::getMixerChannel_Input_Peak1L() {
    float tmpL = MixerChannel_Input_Peak1L;
    MixerChannel_Input_Peak1L = 0.0f;
    return tmpL;
}

float Mixer_Channel::getMixerChannel_Input_Peak1R() {
    float tmpR = MixerChannel_Input_Peak1R;
    MixerChannel_Input_Peak1R = 0.0f;
    return tmpR;
}

float Mixer_Channel::getMixerChannel_Input_Peak2L() {
    float tmpL = MixerChannel_Input_Peak2L;
    MixerChannel_Input_Peak2L = 0.0f;
    return tmpL;
}

float Mixer_Channel::getMixerChannel_Input_Peak2R() {
    float tmpR = MixerChannel_Input_Peak2R;
    MixerChannel_Input_Peak2R = 0.0f;
    return tmpR;
}

float Mixer_Channel::getMixerChannel_Input_Peak3L() {
    float tmpL = MixerChannel_Input_Peak3L;
    MixerChannel_Input_Peak3L = 0.0f;
    return tmpL;
}

float Mixer_Channel::getMixerChannel_Input_Peak3R() {
    float tmpR = MixerChannel_Input_Peak3R;
    MixerChannel_Input_Peak3R = 0.0f;
    return tmpR;
}

//float Mixer_Channel::getMixerChannel_Input_Peak4L() {
//    float tmpL = MixerChannel_Input_Peak4L;
//    MixerChannel_Input_Peak4L = 0.0f;
//    return tmpL;
//}
//
//float Mixer_Channel::getMixerChannel_Input_Peak4R() {
//    float tmpR = MixerChannel_Input_Peak4R;
//    MixerChannel_Input_Peak4R = 0.0f;
//    return tmpR;
//}

void Mixer_Channel::setMixerChannel_Gain1L(float MixerChannel_Gain1L) {
    Mixer_Channel::MixerChannel_Gain1L = MixerChannel_Gain1L;
}

void Mixer_Channel::setMixerChannel_Gain1R(float MixerChannel_Gain1R) {
    Mixer_Channel::MixerChannel_Gain1R = MixerChannel_Gain1R;
}

void Mixer_Channel::setMixerChannel_Gain2L(float MixerChannel_Gain2L) {
    Mixer_Channel::MixerChannel_Gain2L = MixerChannel_Gain2L;
}

void Mixer_Channel::setMixerChannel_Gain2R(float MixerChannel_Gain2R) {
    Mixer_Channel::MixerChannel_Gain2R = MixerChannel_Gain2R;
}

void Mixer_Channel::setMixerChannel_Gain3L(float MixerChannel_Gain3L) {
    Mixer_Channel::MixerChannel_Gain3L = MixerChannel_Gain3L;
}

void Mixer_Channel::setMixerChannel_Gain3R(float MixerChannel_Gain3R) {
    Mixer_Channel::MixerChannel_Gain3R = MixerChannel_Gain3R;
}

//void Mixer_Channel::setMixerChannel_Gain4L(float MixerChannel_Gain4L) {
//    Mixer_Channel::MixerChannel_Gain4L = MixerChannel_Gain4L;
//}
//
//void Mixer_Channel::setMixerChannel_Gain4R(float MixerChannel_Gain4R) {
//    Mixer_Channel::MixerChannel_Gain4R = MixerChannel_Gain4R;
//}

int Mixer_Channel::process (jack_nframes_t nframes, void *arg) {
    Mixer_Channel *mixer_channel = static_cast<Mixer_Channel*>(arg);
    jack_default_audio_sample_t *in1L, *in1R, *in2L, *in2R, *in3L, *in3R,
            *out1L, *out1R, *out2L, *out2R, *out3L, *out3R;

    in1L = static_cast<jack_default_audio_sample_t *>(jack_port_get_buffer (mixer_channel->MixerChannel_Input_Port1L, nframes));
    in1R = static_cast<jack_default_audio_sample_t *>(jack_port_get_buffer (mixer_channel->MixerChannel_Input_Port1R, nframes));
    in2L = static_cast<jack_default_audio_sample_t *>(jack_port_get_buffer (mixer_channel->MixerChannel_Input_Port2L, nframes));
    in2R = static_cast<jack_default_audio_sample_t *>(jack_port_get_buffer (mixer_channel->MixerChannel_Input_Port2R, nframes));
    in3L = static_cast<jack_default_audio_sample_t *>(jack_port_get_buffer (mixer_channel->MixerChannel_Input_Port3L, nframes));
    in3R = static_cast<jack_default_audio_sample_t *>(jack_port_get_buffer (mixer_channel->MixerChannel_Input_Port3R, nframes));
//    in4L = static_cast<jack_default_audio_sample_t *>(jack_port_get_buffer (mixer_channel->MixerChannel_Input_Port4L, nframes));
//    in4R = static_cast<jack_default_audio_sample_t *>(jack_port_get_buffer (mixer_channel->MixerChannel_Input_Port4R, nframes));

    out1L = static_cast<jack_default_audio_sample_t *>(jack_port_get_buffer (mixer_channel->MixerChannel_Output_Port1L, nframes));
    out1R = static_cast<jack_default_audio_sample_t *>(jack_port_get_buffer (mixer_channel->MixerChannel_Output_Port1R, nframes));
    out2L = static_cast<jack_default_audio_sample_t *>(jack_port_get_buffer (mixer_channel->MixerChannel_Output_Port2L, nframes));
    out2R = static_cast<jack_default_audio_sample_t *>(jack_port_get_buffer (mixer_channel->MixerChannel_Output_Port2R, nframes));
    out3L = static_cast<jack_default_audio_sample_t *>(jack_port_get_buffer (mixer_channel->MixerChannel_Output_Port3L, nframes));
    out3R = static_cast<jack_default_audio_sample_t *>(jack_port_get_buffer (mixer_channel->MixerChannel_Output_Port3R, nframes));
//    out4L = static_cast<jack_default_audio_sample_t *>(jack_port_get_buffer (mixer_channel->MixerChannel_Output_Port4L, nframes));
//    out4R = static_cast<jack_default_audio_sample_t *>(jack_port_get_buffer (mixer_channel->MixerChannel_Output_Port4R, nframes));


    for (unsigned int i = 0; i < nframes; i++) {
        const float l1L = (const float) (out1L[i]);
        const float l1R = (const float) (out1R[i]);
        const float l2L = (const float) (out2L[i]);
        const float l2R = (const float) (out2R[i]);
        const float l3L = (const float) (out3L[i]);
        const float l3R = (const float) (out3R[i]);
//        const float l4L = (const float) fabs(out4L[i]);
//        const float l4R = (const float) fabs(out4R[i]);

        if (l1L > mixer_channel->MixerChannel_Input_Peak1L) {
            mixer_channel->MixerChannel_Input_Peak1L = l1L;
        }
        if (l1R > mixer_channel->MixerChannel_Input_Peak1R) {
            mixer_channel->MixerChannel_Input_Peak1R = l1R;
        }
        if (l2L > mixer_channel->MixerChannel_Input_Peak2L) {
            mixer_channel->MixerChannel_Input_Peak2L = l2L;
        }
        if (l2R > mixer_channel->MixerChannel_Input_Peak2R) {
            mixer_channel->MixerChannel_Input_Peak2R = l2R;
        }
        if (l3L > mixer_channel->MixerChannel_Input_Peak3L) {
            mixer_channel->MixerChannel_Input_Peak3L = l3L;
        }
        if (l3R > mixer_channel->MixerChannel_Input_Peak3R) {
            mixer_channel->MixerChannel_Input_Peak3R = l3R;
        }
//        if (l4L > mixer_channel->MixerChannel_Input_Peak4L) {
//            mixer_channel->MixerChannel_Input_Peak4L = l4L;
//        }
//        if (l4R > mixer_channel->MixerChannel_Input_Peak4R) {
//            mixer_channel->MixerChannel_Input_Peak4R = l4R;
//        }

        in1L[i] *= mixer_channel->MixerChannel_Gain1L;
        in1R[i] *= mixer_channel->MixerChannel_Gain1R;
        in2L[i] *= mixer_channel->MixerChannel_Gain2L;
        in2R[i] *= mixer_channel->MixerChannel_Gain2R;
        in3L[i] *= mixer_channel->MixerChannel_Gain3L;
        in3R[i] *= mixer_channel->MixerChannel_Gain3R;
//        in4L[i] *= mixer_channel->MixerChannel_Gain4L;
//        in4R[i] *= mixer_channel->MixerChannel_Gain4R;
    }
//    sf_compressor_process(&mixer_channel->simplecomp, nframes, in1L, in1R,out1L,out1R);
    memcpy(out1L, in1L, sizeof(jack_default_audio_sample_t) * nframes);
    memcpy(out1R, in1R, sizeof(jack_default_audio_sample_t) * nframes);
    memcpy(out2L, in2L, sizeof(jack_default_audio_sample_t) * nframes);
    memcpy(out2R, in2R, sizeof(jack_default_audio_sample_t) * nframes);
    memcpy(out3L, in3L, sizeof(jack_default_audio_sample_t) * nframes);
    memcpy(out3R, in3R, sizeof(jack_default_audio_sample_t) * nframes);
//    memcpy(out4L, in4L, sizeof(jack_default_audio_sample_t) * nframes);
//    memcpy(out4R, in4R, sizeof(jack_default_audio_sample_t) * nframes);

    return 0;
}
void Mixer_Channel::portregcallback(jack_port_id_t port, int register, void *arg){
    //LOGE("port = %i",port);
}
void Mixer_Channel::Setup_Mixer_Channel(){
    jack_status_t status;

    jackMixerChannelClient = jack_client_open ("Mixer_Channel", JackNullOption, &status, NULL);
    if (jackMixerChannelClient == NULL) {
        LOGD ( "jack_client_open() failed, ""status = 0x%2.0x\n", status);

        if (status & JackServerFailed) {
            LOGD ( "Unable to connect to JACK server\n");
        }
    }

    jack_set_process_callback (jackMixerChannelClient, process, this);
    jack_set_port_registration_callback(jackMixerChannelClient,portregcallback,this);
    MixerChannel_Input_Port1L = jack_port_register (jackMixerChannelClient, "MixerChannel_in1L", JACK_DEFAULT_AUDIO_TYPE, JackPortIsInput, 0);
    MixerChannel_Input_Port1R = jack_port_register (jackMixerChannelClient, "MixerChannel_in1R", JACK_DEFAULT_AUDIO_TYPE, JackPortIsInput, 0);
    MixerChannel_Input_Port2L = jack_port_register (jackMixerChannelClient, "MixerChannel_in2L", JACK_DEFAULT_AUDIO_TYPE, JackPortIsInput, 0);
    MixerChannel_Input_Port2R = jack_port_register (jackMixerChannelClient, "MixerChannel_in2R", JACK_DEFAULT_AUDIO_TYPE, JackPortIsInput, 0);
    MixerChannel_Input_Port3L = jack_port_register (jackMixerChannelClient, "MixerChannel_in3L", JACK_DEFAULT_AUDIO_TYPE, JackPortIsInput, 0);
    MixerChannel_Input_Port3R = jack_port_register (jackMixerChannelClient, "MixerChannel_in3R", JACK_DEFAULT_AUDIO_TYPE, JackPortIsInput, 0);
//    MixerChannel_Input_Port4L = jack_port_register (jackMixerChannelClient, "MixerChannel_in4L", JACK_DEFAULT_AUDIO_TYPE, JackPortIsInput, 0);
//    MixerChannel_Input_Port4R = jack_port_register (jackMixerChannelClient, "MixerChannel_in4R", JACK_DEFAULT_AUDIO_TYPE, JackPortIsInput, 0);

    MixerChannel_Output_Port1L = jack_port_register (jackMixerChannelClient, "MixerChannel_out1L", JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0);
    MixerChannel_Output_Port1R = jack_port_register (jackMixerChannelClient, "MixerChannel_out1R", JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0);
    MixerChannel_Output_Port2L = jack_port_register (jackMixerChannelClient, "MixerChannel_out2L", JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0);
    MixerChannel_Output_Port2R = jack_port_register (jackMixerChannelClient, "MixerChannel_out2R", JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0);
    MixerChannel_Output_Port3L = jack_port_register (jackMixerChannelClient, "MixerChannel_out3L", JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0);
    MixerChannel_Output_Port3R = jack_port_register (jackMixerChannelClient, "MixerChannel_out3R", JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0);
//    MixerChannel_Output_Port4L = jack_port_register (jackMixerChannelClient, "MixerChannel_out4L", JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0);
//    MixerChannel_Output_Port4R = jack_port_register (jackMixerChannelClient, "MixerChannel_out4R", JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0);

    jack_activate(jackMixerChannelClient);
}

void Mixer_Channel::Deactivate_Mixer_Channel() {
    jack_deactivate(jackMixerChannelClient);
}

void Mixer_Channel::Activate_Mixer_Channel() {
    jack_activate(jackMixerChannelClient);
}
void Mixer_Channel::ConnectPorts(){
//    jack_connect (jackMixerChannelClient, "Limiter_out1L", jack_port_name (MixerChannel_Input_Port1L));
//    jack_connect (jackMixerChannelClient, "Limiter_out1R", jack_port_name (MixerChannel_Input_Port1R));
    jack_connect (jackMixerChannelClient, "Mixer_Main:Mixer_out1L", jack_port_name (MixerChannel_Input_Port1L));
    jack_connect (jackMixerChannelClient, "Mixer_Main:Mixer_out1L", jack_port_name (MixerChannel_Input_Port1R));
    jack_connect (jackMixerChannelClient, "Mixer_Main:Mixer_out1L", jack_port_name (MixerChannel_Input_Port2L));
    jack_connect (jackMixerChannelClient, "Mixer_Main:Mixer_out1L", jack_port_name (MixerChannel_Input_Port2R));
    jack_connect (jackMixerChannelClient, "Mixer_Main:Mixer_out1L", jack_port_name (MixerChannel_Input_Port3L));
    jack_connect (jackMixerChannelClient, "Mixer_Main:Mixer_out1L", jack_port_name (MixerChannel_Input_Port3R));

//    jack_connect (jackMixerChannelClient, "Mixer_Main:Mixer_out1L", jack_port_name (MixerChannel_Input_Port1L));
//    jack_connect (jackMixerChannelClient, "Mixer_Main:Mixer_out1R", jack_port_name (MixerChannel_Input_Port1R));
//    jack_connect (jackMixerChannelClient, "Mixer_Main:Mixer_out1L", jack_port_name (MixerChannel_Input_Port2L));
//    jack_connect (jackMixerChannelClient, "Mixer_Main:Mixer_out1R", jack_port_name (MixerChannel_Input_Port2R));
//    jack_connect (jackMixerChannelClient, "Mixer_Main:Mixer_out3L", jack_port_name (MixerChannel_Input_Port3L));
//    jack_connect (jackMixerChannelClient, "Mixer_Main:Mixer_out3R", jack_port_name (MixerChannel_Input_Port3R));
//    jack_connect (jackMixerChannelClient, "Mixer_Main:Mixer_in2L", jack_port_name (MixerChannel_Input_Port4L));
//    jack_connect (jackMixerChannelClient, "Mixer_Main:Mixer_in2R", jack_port_name (MixerChannel_Input_Port4R));
//    jack_connect (jackMixerChannelClient, "Mixer_Main:Mixer_in1L", jack_port_name (MixerChannel_Input_Port4L));
//    jack_connect (jackMixerChannelClient, "Mixer_Main:Mixer_in1R", jack_port_name (MixerChannel_Input_Port4R));

    jack_connect (jackMixerChannelClient, jack_port_name (MixerChannel_Output_Port1L), "system:playback_1");
    jack_connect (jackMixerChannelClient, jack_port_name (MixerChannel_Output_Port1R), "system:playback_2");
    jack_connect (jackMixerChannelClient, jack_port_name (MixerChannel_Output_Port2L), "system:playback_3");
    jack_connect (jackMixerChannelClient, jack_port_name (MixerChannel_Output_Port2R), "system:playback_4");
    jack_connect (jackMixerChannelClient, jack_port_name (MixerChannel_Output_Port3L), "system:playback_7");
    jack_connect (jackMixerChannelClient, jack_port_name (MixerChannel_Output_Port3R), "system:playback_8");
//    jack_connect (jackMixerChannelClient, jack_port_name (MixerChannel_Output_Port4L), "Mixer_Main:Mixer_out4L");
//    jack_connect (jackMixerChannelClient, jack_port_name (MixerChannel_Output_Port4R), "Mixer_Main:Mixer_out4R");
}
