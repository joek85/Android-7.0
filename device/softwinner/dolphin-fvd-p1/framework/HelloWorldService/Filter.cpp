//
// Created by joe on 2/7/19.
//

#include <stdio.h>
#include "Filter.h"
#include "LogHelper.h"
#include <math.h>
#include <string.h>
#include <stdlib.h>
float filterFreqs[] = {30, 1000, 16000};
float Filter::getFilter_Input_PeakL() {
    float tmpL = Filter_Input_PeakL;
    Filter_Input_PeakL = 0.0f;
    return tmpL;
}

float Filter::getFilter_Input_PeakR() {
    float tmpR = Filter_Input_PeakR;
    Filter_Input_PeakR = 0.0f;
    return tmpR;
}

float Filter::getFilter_Output_PeakL() {
    float tmpL = Filter_Output_PeakL;
    Filter_Output_PeakL = 0.0f;
    return tmpL;
}

float Filter::getFilter_Output_PeakR() {
    float tmpR = Filter_Output_PeakR;
    Filter_Output_PeakR = 0.0f;
    return tmpR;
}

void Filter::setFilter_GainL(float Filter_GainL) {
    Filter::Filter_GainL = Filter_GainL;
}

void Filter::setFilter_GainR(float Filter_GainR) {
    Filter::Filter_GainR = Filter_GainR;
}

Filter::Filter() {
    Filterenabled = true;
    filterType = 0;
    Filter_GainL = 1.0f;
    Filter_GainR = 1.0f;
}
void Filter::setfilterFreq(int index, float freq){
    filterFrequency[index * 2] = freq;
    filterFrequency[index * 2 + 1] = freq;
    Filter::filters[index * 2]->setFc(freq);
    Filter::filters[index * 2 + 1]->setFc(freq);
}
void Filter::setfilterGain(int index, float gain){
    Filter::filters[index * 2]->setPeakGain(gain);
    Filter::filters[index * 2 + 1]->setPeakGain(gain);
}
void Filter::setFilterenabled(bool Filterenabled) {
    Filter::Filterenabled = Filterenabled;
}

void Filter::setFilterType(int index, int filterType) {
    Filter::filters[index * 2]->setFc(Filter::filterFrequency[index * 2]);
    Filter::filters[index * 2 + 1]->setFc(Filter::filterFrequency[index * 2 + 1]);
    Filter::filters[index * 2]->setQ(1.0);
    Filter::filters[index * 2 + 1]->setQ(1.0);
    Filter::filters[index * 2]->setType(filterType);
    Filter::filters[index * 2 + 1]->setType(filterType);
}

int Filter::processFilter(jack_nframes_t nframes, void *arg) {
    Filter *filter = static_cast<Filter*>(arg);
    jack_default_audio_sample_t *inL, *inR, *outL, *outR;

    inL = static_cast<jack_default_audio_sample_t *>(jack_port_get_buffer (filter->Filter_Input_PortL, nframes));
    inR = static_cast<jack_default_audio_sample_t *>(jack_port_get_buffer (filter->Filter_Input_PortR, nframes));

    outL = static_cast<jack_default_audio_sample_t *>(jack_port_get_buffer (filter->Filter_Output_PortL, nframes));
    outR = static_cast<jack_default_audio_sample_t *>(jack_port_get_buffer (filter->Filter_Output_PortR, nframes));

    for (unsigned int i = 0; i < nframes; i++) {
        const float L1L = (const float) fabs(inL[i]);
        const float L1R = (const float) fabs(inR[i]);
        const float L2L = (const float) fabs(outL[i]);
        const float L2R = (const float) fabs(outR[i]);

        if (L1L > filter->Filter_Input_PeakL) {
            filter->Filter_Input_PeakL = L1L;
        }
        if (L1R > filter->Filter_Input_PeakR) {
            filter->Filter_Input_PeakR = L1R;
        }
        if (L2L > filter->Filter_Output_PeakL) {
            filter->Filter_Output_PeakL = L2L;
        }
        if (L2R > filter->Filter_Output_PeakR) {
            filter->Filter_Output_PeakR = L2R;
        }

        inL[i] *= filter->Filter_GainL;
        inR[i] *= filter->Filter_GainR;

        if(filter->Filterenabled){
            inL[i] = (jack_default_audio_sample_t) filter->filters[0]->process(inL[i]);
            inR[i] = (jack_default_audio_sample_t) filter->filters[1]->process(inR[i]);
            inL[i] = (jack_default_audio_sample_t) filter->filters[2]->process(inL[i]);
            inR[i] = (jack_default_audio_sample_t) filter->filters[3]->process(inR[i]);
            inL[i] = (jack_default_audio_sample_t) filter->filters[4]->process(inL[i]);
            inR[i] = (jack_default_audio_sample_t) filter->filters[5]->process(inR[i]);
        }
    }
    memcpy(outL, inL, sizeof(jack_default_audio_sample_t) * nframes);
    memcpy(outR, inR, sizeof(jack_default_audio_sample_t) * nframes);
    return 0;
}

void Filter::Setup_Filter(int index) {
    jack_status_t status;
    char port_name[120] = {0,};
    sprintf(port_name, "Filter%d", index);
    jackFilterClient = jack_client_open (port_name, JackNullOption, &status, NULL);
    if (jackFilterClient == NULL) {
        LOGD ( "jack_client_open() failed, ""status = 0x%2.0x\n", status);

        if (status & JackServerFailed) {
            LOGD ( "Unable to connect to JACK server\n");
        }
    }
    jack_set_process_callback (jackFilterClient, processFilter, this);
    sprintf(port_name, "Filter_in%s","L");
    Filter_Input_PortL = jack_port_register (jackFilterClient, port_name, JACK_DEFAULT_AUDIO_TYPE, JackPortIsInput, 0);
    sprintf(port_name, "Filter_in%s","R");
    Filter_Input_PortR = jack_port_register (jackFilterClient, port_name, JACK_DEFAULT_AUDIO_TYPE, JackPortIsInput, 0);

    sprintf(port_name, "Filter_out%s","L");
    Filter_Output_PortL = jack_port_register (jackFilterClient, port_name, JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0);
    sprintf(port_name, "Filter_out%s","R");
    Filter_Output_PortR = jack_port_register (jackFilterClient, port_name, JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0);

    filters[0] = new Biquad(bq_type_lowshelf, filterFreqs[0], 1.0, 0.0);
    filters[1] = new Biquad(bq_type_lowshelf, filterFreqs[0], 1.0, 0.0);
    filters[2] = new Biquad(bq_type_peak, filterFreqs[1], 1.0, 0.0);
    filters[3] = new Biquad(bq_type_peak, filterFreqs[1], 1.0, 0.0);
    filters[4] = new Biquad(bq_type_highshelf, filterFreqs[2], 1.0, 0.0);
    filters[5] = new Biquad(bq_type_highshelf, filterFreqs[2], 1.0, 0.0);

    jack_activate(jackFilterClient);
}

void Filter::ConnectPorts(int index) {
    char port_name[120] = {0,};
//    sprintf(port_name, "Mixer_Main:Mixer_Systemout_%d%s", index,"L");
//    jack_connect (jackFilterClient, port_name, jack_port_name (Filter_Input_PortL));
//    sprintf(port_name, "Mixer_Main:Mixer_Systemout_%d%s", index,"R");
//    jack_connect (jackFilterClient, port_name, jack_port_name (Filter_Input_PortR));

//    sprintf(port_name, "system:playback_%d", 1);
//    jack_connect (jackFilterClient, port_name, jack_port_name (Filter_Input_PortL));
//    sprintf(port_name, "system:playback_%d", 2);
//    jack_connect (jackFilterClient, port_name, jack_port_name (Filter_Input_PortR));

    sprintf(port_name, "Equalizer%d:Equalizer_in%s",index,"L");
    jack_connect (jackFilterClient, jack_port_name (Filter_Output_PortL), port_name);
    sprintf(port_name, "Equalizer%d:Equalizer_in%s",index,"R");
    jack_connect (jackFilterClient, jack_port_name (Filter_Output_PortR), port_name);
}

