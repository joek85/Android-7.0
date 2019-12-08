//
// Created by joe on 12/20/18.
//

#ifndef EQUALIZER_H
#define EQUALIZER_H
#include <jack.h>
#include "Biquad.h"

class Equalizer{
private:
    jack_client_t *jackEqualizerClient;
    jack_port_t *Equalizer_Input_PortR;
    jack_port_t *Equalizer_Input_PortL;
    jack_port_t *Equalizer_Output_PortL;
    jack_port_t *Equalizer_Output_PortR;
    float Equalizer_GainL;
    float Equalizer_GainR;

    float Equalizer_Input_PeakL;
    float Equalizer_Input_PeakR;

    float Equalizer_Output_PeakL;
    float Equalizer_Output_PeakR;
    bool Equalizerenabled;
    Biquad *eq[5*2];

public:
    Equalizer();
    void Setup_Equalizer(int index);
    static int processeq(jack_nframes_t nframes, void *arg);

    void setEqualizer_GainL(float Equalizer_Gain1L);

    void setEqualizer_GainR(float Equalizer_Gain1R);

    void seteqFreq(int index, float freq);

    void seteqQ(int index, float Q);

    void seteqGain(int index, float gain);

    float getEqualizer_Input_PeakL();

    float getEqualizer_Input_PeakR();

    float getEqualizer_Output_PeakL();

    float getEqualizer_Output_PeakR();

    void ConnectPorts(int index);

    void setEnabled(bool enabled);
};
#endif //SERVICETEST_EQUALIZER_H
