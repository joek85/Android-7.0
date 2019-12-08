//
// Created by joe on 2/7/19.
//

#include <stdio.h>
#include "Limiter.h"
#include "LogHelper.h"
#include "compressor.h"
#include <math.h>
#include <string.h>
#include <stdlib.h>
Limiter::Limiter() {
    Limiterenabled = true;
    pregain = 1.0f;
    threshold = 0.0f;
    ratio = 1000.0f;
    attack = 0.001f;
    release = 1.0f;
    meterGain = 0.0f;
}
static inline float db2lin(float db){ // dB to linear
    return powf(10.0f, 0.05f * db);
}

static inline float lin2db(float lin){ // linear to dB
    return 20.0f * log10f(lin);
}
float Limiter::getLimiter_Input_PeakL() {
    float tmpL = Limiter_Input_PeakL;
    Limiter_Input_PeakL = 0.0f;
    return tmpL;
}

float Limiter::getLimiter_Input_PeakR() {
    float tmpR = Limiter_Input_PeakR;
    Limiter_Input_PeakR = 0.0f;
    return tmpR;
}

float Limiter::getLimiter_Output_PeakL() {
    float tmpL = Limiter_Output_PeakL;
    Limiter_Output_PeakL = 0.0f;
    return tmpL;
}

float Limiter::getLimiter_Output_PeakR() {
    float tmpR = Limiter_Output_PeakR;
    Limiter_Output_PeakR = 0.0f;
    return tmpR;
}

void Limiter::setPregain(float pregain) {
    Limiter::pregain = pregain;
}

void Limiter::setAttack(float attack) {
    Limiter::attack = attack;
}

void Limiter::setRelease(float release) {
    Limiter::release = release;
}

void Limiter::setThreshold(float threshold) {
    Limiter::threshold = threshold;
}

void Limiter::setRatio(float ratio) {
    Limiter::ratio = ratio;
}

void Limiter::setLimiterenabled(bool Limiterenabled) {
    Limiter::Limiterenabled = Limiterenabled;
}

float Limiter::getMeterGain() const {
    return meterGain;
}

int Limiter::processLimiter(jack_nframes_t nframes, void *arg) {
    Limiter *limiter = static_cast<Limiter*>(arg);
    jack_default_audio_sample_t *inL, *inR, *outL, *outR;

    inL = static_cast<jack_default_audio_sample_t *>(jack_port_get_buffer (limiter->Limiter_Input_PortL, nframes));
    inR = static_cast<jack_default_audio_sample_t *>(jack_port_get_buffer (limiter->Limiter_Input_PortR, nframes));

    outL = static_cast<jack_default_audio_sample_t *>(jack_port_get_buffer (limiter->Limiter_Output_PortL, nframes));
    outR = static_cast<jack_default_audio_sample_t *>(jack_port_get_buffer (limiter->Limiter_Output_PortR, nframes));

    for (unsigned int i = 0; i < nframes; i++) {
        const float L1L = (const float) fabs(inL[i]);
        const float L1R = (const float) fabs(inR[i]);
        const float L2L = (const float) fabs(outL[i]);
        const float L2R = (const float) fabs(outR[i]);

        if (L1L > limiter->Limiter_Input_PeakL) {
            limiter->Limiter_Input_PeakL = L1L;
        }
        if (L1R > limiter->Limiter_Input_PeakR) {
            limiter->Limiter_Input_PeakR = L1R;
        }
        if (L2L > limiter->Limiter_Output_PeakL) {
            limiter->Limiter_Output_PeakL = L2L;
        }
        if (L2R > limiter->Limiter_Output_PeakR) {
            limiter->Limiter_Output_PeakR = L2R;
        }
    }
    if (limiter->Limiterenabled){
        limiter->simplecomp.linearpregain = limiter->pregain;
        limiter->meterGain = limiter->simplecomp.metergain;
//        limiter->simplecomp.attacksamplesinv = limiter->attack;
//        limiter->simplecomp.satreleasesamplesinv = limiter->release;
//        limiter->simplecomp.linearthreshold = limiter->threshold;
        sf_compressor_process(&limiter->simplecomp, nframes, inL, inR,outL,outR);
    }else{
        memcpy(outL, inL, sizeof(jack_default_audio_sample_t) * nframes);
        memcpy(outR, inR, sizeof(jack_default_audio_sample_t) * nframes);
    }
    return 0;
}

void Limiter::Setup_Limiter(int index) {
    jack_status_t status;
    char port_name[120] = {0,};
    sprintf(port_name, "Limiter%d", index);
    jackLimiterClient = jack_client_open (port_name, JackNullOption, &status, NULL);
    if (jackLimiterClient == NULL) {
        LOGD ( "jack_client_open() failed, ""status = 0x%2.0x\n", status);

        if (status & JackServerFailed) {
            LOGD ( "Unable to connect to JACK server\n");
        }
    }
    jack_set_process_callback (jackLimiterClient, processLimiter, this);
    sprintf(port_name, "Limiter_in%s","L");
    Limiter_Input_PortL = jack_port_register (jackLimiterClient, port_name, JACK_DEFAULT_AUDIO_TYPE, JackPortIsInput, 0);
    sprintf(port_name, "Limiter_in%s","R");
    Limiter_Input_PortR = jack_port_register (jackLimiterClient, port_name, JACK_DEFAULT_AUDIO_TYPE, JackPortIsInput, 0);

    sprintf(port_name, "Limiter_out%s","L");
    Limiter_Output_PortL = jack_port_register (jackLimiterClient, port_name, JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0);
    sprintf(port_name, "Limiter_out%s","R");
    Limiter_Output_PortR = jack_port_register (jackLimiterClient, port_name, JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0);

    sf_simplecomp(&simplecomp, 48000, pregain, threshold, 1.0f,ratio, attack, release);

    jack_activate(jackLimiterClient);
}

void Limiter::ConnectPorts(int index) {
    char port_name[120] = {0,};
//    sprintf(port_name, "StereoEnhancer%d:StereoEnhancer_out%d%s",index, index,"L");
//    jack_connect (jackLimiterClient, port_name, jack_port_name (Limiter_Input_PortL));
//    sprintf(port_name, "StereoEnhancer%d:StereoEnhancer_out%d%s",index, index,"R");
//    jack_connect (jackLimiterClient, port_name, jack_port_name (Limiter_Input_PortR));

    switch (index){
        case 0:
            sprintf(port_name, "system:playback_%d", 1);
            jack_connect (jackLimiterClient, jack_port_name (Limiter_Output_PortL), port_name);
            sprintf(port_name, "system:playback_%d", 2);
            jack_connect (jackLimiterClient, jack_port_name (Limiter_Output_PortR), port_name);
            break;
        case 1:
            sprintf(port_name, "system:playback_%d", 3);
            jack_connect (jackLimiterClient, jack_port_name (Limiter_Output_PortL), port_name);
            sprintf(port_name, "system:playback_%d", 4);
            jack_connect (jackLimiterClient, jack_port_name (Limiter_Output_PortR), port_name);
            break;
        case 2:
            sprintf(port_name, "system:playback_%d", 7);
            jack_connect (jackLimiterClient, jack_port_name (Limiter_Output_PortL), port_name);
            sprintf(port_name, "system:playback_%d", 8);
            jack_connect (jackLimiterClient, jack_port_name (Limiter_Output_PortR), port_name);
            break;
        default:
            break;

    }

}
