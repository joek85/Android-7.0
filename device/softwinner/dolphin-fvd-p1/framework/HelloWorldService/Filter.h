//
// Created by joe on 2/7/19.
//

#ifndef FILTER_H
#define FILTER_H
#include <jack.h>
#include "Biquad.h"

class Filter{
private:
    jack_client_t *jackFilterClient;
    jack_port_t *Filter_Input_PortR;
    jack_port_t *Filter_Input_PortL;
    jack_port_t *Filter_Output_PortL;
    jack_port_t *Filter_Output_PortR;

    float Filter_Input_PeakL;
    float Filter_Input_PeakR;

    float Filter_Output_PeakL;
    float Filter_Output_PeakR;

    float Filter_GainL;
    float Filter_GainR;

    bool Filterenabled;
    Biquad *filters[3*2];
    int filterType;
    float filterFrequency[3*2];
public:
    Filter();
    void Setup_Filter(int index);
    void ConnectPorts(int index);
    static int processFilter(jack_nframes_t nframes, void *arg);
    float getFilter_Input_PeakL();

    float getFilter_Input_PeakR();

    float getFilter_Output_PeakL();

    float getFilter_Output_PeakR();

    void setfilterFreq(int index, float freq);

    void setFilterenabled(bool Filterenabled);

    void setfilterGain(int index, float gain);

    void setFilterType(int index, int filterType);

    void setFilter_GainL(float Filter_GainL);

    void setFilter_GainR(float Filter_GainR);
};

#endif //SERVICETEST_FILTER_H
