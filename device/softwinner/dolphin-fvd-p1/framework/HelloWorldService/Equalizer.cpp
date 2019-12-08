//
// Created by joe on 12/20/18.
//

#include <stdio.h>
#include "Equalizer.h"
#include "LogHelper.h"
#include "Biquad.h"
#include <math.h>
#include <string.h>
#include <stdlib.h>
float eqFreqs[] = {40,100,1000,8000,16000};
Equalizer::Equalizer() {
    Equalizer_GainL = 1.0f;
    Equalizer_GainR = 1.0f;
    Equalizer_Input_PeakL = 0.0f;
    Equalizer_Input_PeakR = 0.0f;
    Equalizer_Output_PeakL = 0.0f;
    Equalizer_Output_PeakR = 0.0f;
    Equalizerenabled = true;
}
void Equalizer::setEnabled(bool enabled) {
    Equalizer::Equalizerenabled = enabled;
}
void Equalizer::setEqualizer_GainL(float Equalizer_Gain1L) {
    Equalizer::Equalizer_GainL = Equalizer_Gain1L;
}
void Equalizer::setEqualizer_GainR(float Equalizer_Gain1R) {
    Equalizer::Equalizer_GainR = Equalizer_Gain1R;
}
void Equalizer::seteqFreq(int index, float freq){
    Equalizer::eq[index * 2]->setFc(freq);
    Equalizer::eq[index * 2 + 1]->setFc(freq);
}
void Equalizer::seteqGain(int index, float gain){
    Equalizer::eq[index * 2]->setPeakGain(gain);
    Equalizer::eq[index * 2 + 1]->setPeakGain(gain);
}
void Equalizer::seteqQ(int index, float Q) {
    Equalizer::eq[index * 2]->setQ(Q);
    Equalizer::eq[index * 2 + 1]->setQ(Q);
}
float Equalizer::getEqualizer_Input_PeakL() {
    float tmpL = Equalizer_Input_PeakL;
    Equalizer_Input_PeakL = 0.0f;
    return tmpL;
}

float Equalizer::getEqualizer_Input_PeakR() {
    float tmpR = Equalizer_Input_PeakR;
    Equalizer_Input_PeakR = 0.0f;
    return tmpR;
}

float Equalizer::getEqualizer_Output_PeakL() {
    float tmpL = Equalizer_Output_PeakL;
    Equalizer_Output_PeakL = 0.0f;
    return tmpL;
}

float Equalizer::getEqualizer_Output_PeakR() {
    float tmpR = Equalizer_Output_PeakR;
    Equalizer_Output_PeakR = 0.0f;
    return tmpR;
}

int Equalizer::processeq (jack_nframes_t nframes, void *arg) {
    Equalizer *equalizer = static_cast<Equalizer*>(arg);
    jack_default_audio_sample_t *inL, *inR, *outL, *outR;

    inL = static_cast<jack_default_audio_sample_t *>(jack_port_get_buffer (equalizer->Equalizer_Input_PortL, nframes));
    inR = static_cast<jack_default_audio_sample_t *>(jack_port_get_buffer (equalizer->Equalizer_Input_PortR, nframes));

    outL = static_cast<jack_default_audio_sample_t *>(jack_port_get_buffer (equalizer->Equalizer_Output_PortL, nframes));
    outR = static_cast<jack_default_audio_sample_t *>(jack_port_get_buffer (equalizer->Equalizer_Output_PortR, nframes));

    for (unsigned int i = 0; i < nframes; i++) {

        const float L1L = (const float) fabs(inL[i]);
        const float L1R = (const float) fabs(inR[i]);
        const float L2L = (const float) fabs(outL[i]);
        const float L2R = (const float) fabs(outR[i]);

        if (L1L > equalizer->Equalizer_Input_PeakL) {
            equalizer->Equalizer_Input_PeakL = L1L;
        }
        if (L1R > equalizer->Equalizer_Input_PeakR) {
            equalizer->Equalizer_Input_PeakR = L1R;
        }
        if (L2L > equalizer->Equalizer_Output_PeakL) {
            equalizer->Equalizer_Output_PeakL = L2L;
        }
        if (L2R > equalizer->Equalizer_Output_PeakR) {
            equalizer->Equalizer_Output_PeakR = L2R;
        }

        if(equalizer->Equalizerenabled){
            inL[i] = (jack_default_audio_sample_t) equalizer->eq[0]->process(inL[i]);
            inR[i] = (jack_default_audio_sample_t) equalizer->eq[1]->process(inR[i]);
            inL[i] = (jack_default_audio_sample_t) equalizer->eq[2]->process(inL[i]);
            inR[i] = (jack_default_audio_sample_t) equalizer->eq[3]->process(inR[i]);
            inL[i] = (jack_default_audio_sample_t) equalizer->eq[4]->process(inL[i]);
            inR[i] = (jack_default_audio_sample_t) equalizer->eq[5]->process(inR[i]);
            inL[i] = (jack_default_audio_sample_t) equalizer->eq[6]->process(inL[i]);
            inR[i] = (jack_default_audio_sample_t) equalizer->eq[7]->process(inR[i]);
            inL[i] = (jack_default_audio_sample_t) equalizer->eq[8]->process(inL[i]);
            inR[i] = (jack_default_audio_sample_t) equalizer->eq[9]->process(inR[i]);
        }

        inL[i] *= equalizer->Equalizer_GainL;
        inR[i] *= equalizer->Equalizer_GainR;
    }
    memcpy(outL, inL, sizeof(jack_default_audio_sample_t) * nframes);
    memcpy(outR, inR, sizeof(jack_default_audio_sample_t) * nframes);
    return 0;
}
void Equalizer::Setup_Equalizer(int index) {
    jack_status_t status;
    char port_name[120] = {0,};
    sprintf(port_name, "Equalizer%d", index);
    jackEqualizerClient = jack_client_open (port_name, JackNullOption, &status, NULL);
    if (jackEqualizerClient == NULL) {
        LOGD ( "jack_client_open() failed, ""status = 0x%2.0x\n", status);

        if (status & JackServerFailed) {
            LOGD ( "Unable to connect to JACK server\n");
        }
    }

    jack_set_process_callback (jackEqualizerClient, processeq, this);
    sprintf(port_name, "Equalizer_in%s","L");
    Equalizer_Input_PortL = jack_port_register (jackEqualizerClient, port_name, JACK_DEFAULT_AUDIO_TYPE, JackPortIsInput, 0);
    sprintf(port_name, "Equalizer_in%s","R");
    Equalizer_Input_PortR = jack_port_register (jackEqualizerClient, port_name, JACK_DEFAULT_AUDIO_TYPE, JackPortIsInput, 0);

    sprintf(port_name, "Equalizer_out%s","L");
    Equalizer_Output_PortL = jack_port_register (jackEqualizerClient, port_name, JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0);
    sprintf(port_name, "Equalizer_out%s","R");
    Equalizer_Output_PortR = jack_port_register (jackEqualizerClient, port_name, JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0);

    eq[0] = new Biquad(bq_type_lowshelf, eqFreqs[0], 1.0, 0.0);
    eq[1] = new Biquad(bq_type_lowshelf, eqFreqs[0], 1.0, 0.0);
    eq[2] = new Biquad(bq_type_peak, eqFreqs[1], 1.0, 0.0);
    eq[3] = new Biquad(bq_type_peak, eqFreqs[1], 1.0, 0.0);
    eq[4] = new Biquad(bq_type_peak, eqFreqs[2], 1.0, 0.0);
    eq[5] = new Biquad(bq_type_peak, eqFreqs[2], 1.0, 0.0);
    eq[6] = new Biquad(bq_type_peak, eqFreqs[3], 1.0, 0.0);
    eq[7] = new Biquad(bq_type_peak, eqFreqs[3], 1.0, 0.0);
    eq[8] = new Biquad(bq_type_highshelf, eqFreqs[4], 1.0, 0.0);
    eq[9] = new Biquad(bq_type_highshelf, eqFreqs[4], 1.0, 0.0);
    jack_activate(jackEqualizerClient);

}
void Equalizer::ConnectPorts(int index){
    char port_name[120] = {0,};
//    sprintf(port_name, "Filter%d:Filter_out%d%s",index, index,"L");
//    jack_connect (jackEqualizerClient, port_name, jack_port_name (Equalizer_Input_PortL));
//    sprintf(port_name, "Filter%d:Filter_out%d%s",index, index,"R");
//    jack_connect (jackEqualizerClient, port_name, jack_port_name (Equalizer_Input_PortR));

    sprintf(port_name, "StereoEnhancer%d:StereoEnhancer_in%s", index,"L");
    jack_connect (jackEqualizerClient, jack_port_name (Equalizer_Output_PortL), port_name);
    sprintf(port_name, "StereoEnhancer%d:StereoEnhancer_in%s", index,"R");
    jack_connect (jackEqualizerClient, jack_port_name (Equalizer_Output_PortR), port_name);
}
