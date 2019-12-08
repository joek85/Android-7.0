//
// Created by joe on 2/15/19.
//

#include <stdio.h>
#include "StereoEnhancer.h"
#include "LogHelper.h"
#include <math.h>
#include <string.h>
#include <stdlib.h>
float stereoFreqs[] = {100,1000,16000};
float StereoEnhancer::getStereoEnhancer_Input_PeakL() {
    float tmpL = StereoEnhancer_Input_PeakL;
    StereoEnhancer_Input_PeakL = 0.0f;
    return tmpL;
}

float StereoEnhancer::getStereoEnhancer_Input_PeakR() {
    float tmpR = StereoEnhancer_Input_PeakR;
    StereoEnhancer_Input_PeakR = 0.0f;
    return tmpR;
}

float StereoEnhancer::getStereoEnhancer_Output_PeakL() {
    float tmpL = StereoEnhancer_Output_PeakL;
    StereoEnhancer_Output_PeakL = 0.0f;
    return tmpL;
}

float StereoEnhancer::getStereoEnhancer_Output_PeakR() {
    float tmpR = StereoEnhancer_Output_PeakR;
    StereoEnhancer_Output_PeakR = 0.0f;
    return tmpR;
}
void StereoEnhancer::setStereoEnhancerFreq(int index, float freq){
    StereoEnhancer::filterStereoEnhancers[index * 2]->setFc(freq);
    StereoEnhancer::filterStereoEnhancers[index * 2 + 1]->setFc(freq);
}

void StereoEnhancer::setStereoEnhancerenabled(bool StereoEnhancerenabled) {
    StereoEnhancer::StereoEnhancerenabled = StereoEnhancerenabled;
}

void StereoEnhancer::setSolo(bool solo) {
    StereoEnhancer::solo[soloIndex] = solo;
}

void StereoEnhancer::setSoloIndex(int soloIndex) {
    StereoEnhancer::soloIndex = soloIndex;
}

StereoEnhancer::StereoEnhancer() {
    StereoEnhancerenabled = false;
    soloIndex = 0;
    solo[0] = false;
}

int StereoEnhancer::processStereoEnhancer(jack_nframes_t nframes, void *arg) {
    StereoEnhancer *stereoEnhancer = static_cast<StereoEnhancer*>(arg);
    jack_default_audio_sample_t *inL, *inR, *outL, *outR;

    inL = static_cast<jack_default_audio_sample_t *>(jack_port_get_buffer (stereoEnhancer->StereoEnhancer_Input_PortL, nframes));
    inR = static_cast<jack_default_audio_sample_t *>(jack_port_get_buffer (stereoEnhancer->StereoEnhancer_Input_PortR, nframes));

    outL = static_cast<jack_default_audio_sample_t *>(jack_port_get_buffer (stereoEnhancer->StereoEnhancer_Output_PortL, nframes));
    outR = static_cast<jack_default_audio_sample_t *>(jack_port_get_buffer (stereoEnhancer->StereoEnhancer_Output_PortR, nframes));
    float left, right;
    for (unsigned int i = 0; i < nframes; i++) {
        const float L1L = (const float) fabs(inL[i]);
        const float L1R = (const float) fabs(inR[i]);
        const float L2L = (const float) fabs(outL[i]);
        const float L2R = (const float) fabs(outR[i]);

        if (L1L > stereoEnhancer->StereoEnhancer_Input_PeakL) {
            stereoEnhancer->StereoEnhancer_Input_PeakL = L1L;
        }
        if (L1R > stereoEnhancer->StereoEnhancer_Input_PeakR) {
            stereoEnhancer->StereoEnhancer_Input_PeakR = L1R;
        }
        if (L2L > stereoEnhancer->StereoEnhancer_Output_PeakL) {
            stereoEnhancer->StereoEnhancer_Output_PeakL = L2L;
        }
        if (L2R > stereoEnhancer->StereoEnhancer_Output_PeakR) {
            stereoEnhancer->StereoEnhancer_Output_PeakR = L2R;
        }

        if(stereoEnhancer->StereoEnhancerenabled){
//            if (stereoEnhancer->solo){

//            }
            switch (stereoEnhancer->soloIndex){
                case 0:
                    break;
                case 1:
                    break;
                case 2:
                    break;
                default:break;
            }
            left = right = 0.0f;
            left += stereoEnhancer->filterStereoEnhancers[0]->process(inL[i]);
            left += stereoEnhancer->filterStereoEnhancers[2]->process(inL[i]);
            left += stereoEnhancer->filterStereoEnhancers[4]->process(inL[i]);
            inL[i] = left;

            right += stereoEnhancer->filterStereoEnhancers[1]->process(inR[i]);
            right += stereoEnhancer->filterStereoEnhancers[3]->process(inR[i]);
            right += stereoEnhancer->filterStereoEnhancers[5]->process(inR[i]);
            inR[i] = right;
//            inL[i] = (jack_default_audio_sample_t) stereoEnhancer->filterStereoEnhancers[0]->process(inL[i]);
//            inR[i] = (jack_default_audio_sample_t) stereoEnhancer->filterStereoEnhancers[1]->process(inR[i]);
//            inL[i] = (jack_default_audio_sample_t) stereoEnhancer->filterStereoEnhancers[2]->process(inL[i]);
//            inR[i] = (jack_default_audio_sample_t) stereoEnhancer->filterStereoEnhancers[3]->process(inR[i]);
//            inL[i] = (jack_default_audio_sample_t) stereoEnhancer->filterStereoEnhancers[4]->process(inL[i]);
//            inR[i] = (jack_default_audio_sample_t) stereoEnhancer->filterStereoEnhancers[5]->process(inR[i]);
            //inL[i] = (jack_default_audio_sample_t) stereoEnhancer->filterStereoEnhancers[6]->process(inL[i]);
            //inR[i] = (jack_default_audio_sample_t) stereoEnhancer->filterStereoEnhancers[7]->process(inR[i]);
        }
    }
    memcpy(outL, inL, sizeof(jack_default_audio_sample_t) * nframes);
    memcpy(outR, inR, sizeof(jack_default_audio_sample_t) * nframes);
    return 0;
}

void StereoEnhancer::Setup_StereoEnhancer(int index) {
    jack_status_t status;
    char port_name[120] = {0,};
    sprintf(port_name, "StereoEnhancer%d", index);
    jackStereoEnhancerClient = jack_client_open (port_name, JackNullOption, &status, NULL);
    if (jackStereoEnhancerClient == NULL) {
        LOGD ( "jack_client_open() failed, ""status = 0x%2.0x\n", status);

        if (status & JackServerFailed) {
            LOGD ( "Unable to connect to JACK server\n");
        }
    }
    jack_set_process_callback (jackStereoEnhancerClient, processStereoEnhancer, this);
    sprintf(port_name, "StereoEnhancer_in%s","L");
    StereoEnhancer_Input_PortL = jack_port_register (jackStereoEnhancerClient, port_name, JACK_DEFAULT_AUDIO_TYPE, JackPortIsInput, 0);
    sprintf(port_name, "StereoEnhancer_in%s","R");
    StereoEnhancer_Input_PortR = jack_port_register (jackStereoEnhancerClient, port_name, JACK_DEFAULT_AUDIO_TYPE, JackPortIsInput, 0);

    sprintf(port_name, "StereoEnhancer_out%s","L");
    StereoEnhancer_Output_PortL = jack_port_register (jackStereoEnhancerClient, port_name, JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0);
    sprintf(port_name, "StereoEnhancer_out%s","R");
    StereoEnhancer_Output_PortR = jack_port_register (jackStereoEnhancerClient, port_name, JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0);

    filterStereoEnhancers[0] = new Biquad(bq_type_lowpass, stereoFreqs[0], 1.0, 0.0);
    filterStereoEnhancers[1] = new Biquad(bq_type_lowpass, stereoFreqs[0], 1.0, 0.0);
    filterStereoEnhancers[2] = new Biquad(bq_type_bandpass, stereoFreqs[1], 1.0, 0.0);
    filterStereoEnhancers[3] = new Biquad(bq_type_bandpass, stereoFreqs[1], 1.0, 0.0);
    filterStereoEnhancers[4] = new Biquad(bq_type_highpass, stereoFreqs[2], 1.0, 0.0);
    filterStereoEnhancers[5] = new Biquad(bq_type_highpass, stereoFreqs[2], 1.0, 0.0);

    jack_activate(jackStereoEnhancerClient);

}

void StereoEnhancer::ConnectPorts(int index) {
    char port_name[120] = {0,};
//    sprintf(port_name, "Equalizer%d:Equalizer_out%d%s",index, index,"L");
//    jack_connect (jackStereoEnhancerClient, port_name, jack_port_name (StereoEnhancer_Input_PortL));
//    sprintf(port_name, "Equalizer%d:Equalizer_out%d%s",index, index,"R");
//    jack_connect (jackStereoEnhancerClient, port_name, jack_port_name (StereoEnhancer_Input_PortR));

    sprintf(port_name, "Limiter%d:Limiter_in%s", index,"L");
    jack_connect (jackStereoEnhancerClient, jack_port_name (StereoEnhancer_Output_PortL), port_name);
    sprintf(port_name, "Limiter%d:Limiter_in%s", index,"R");
    jack_connect (jackStereoEnhancerClient, jack_port_name (StereoEnhancer_Output_PortR), port_name);
}

