//
// Created by joe on 2/15/19.
//

#ifndef STEREOENHANCER_H
#define STEREOENHANCER_H

#include "Biquad.h"
#include <jack.h>
class StereoEnhancer{
    private:
    jack_client_t *jackStereoEnhancerClient;
    jack_port_t *StereoEnhancer_Input_PortR;
    jack_port_t *StereoEnhancer_Input_PortL;
    jack_port_t *StereoEnhancer_Output_PortL;
    jack_port_t *StereoEnhancer_Output_PortR;

    float StereoEnhancer_Input_PeakL;
    float StereoEnhancer_Input_PeakR;

    float StereoEnhancer_Output_PeakL;
    float StereoEnhancer_Output_PeakR;
    bool StereoEnhancerenabled;
    Biquad *filterStereoEnhancers[4*2];
    bool solo[4*2];
    int soloIndex;

    public:
    StereoEnhancer();
    void Setup_StereoEnhancer(int index);
    void ConnectPorts(int index);
    static int processStereoEnhancer(jack_nframes_t nframes, void *arg);

    float getStereoEnhancer_Input_PeakL();

    float getStereoEnhancer_Input_PeakR();

    float getStereoEnhancer_Output_PeakL();

    float getStereoEnhancer_Output_PeakR();

    void setStereoEnhancerenabled(bool StereoEnhancerenabled);

    void setSolo(bool solo);

    void setSoloIndex(int soloIndex);

    void setStereoEnhancerFreq(int index, float freq);
};
#endif //SERVICETEST_STEREOENHANCER_H
