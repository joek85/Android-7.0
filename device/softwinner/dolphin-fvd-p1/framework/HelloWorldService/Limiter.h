//
// Created by joe on 2/7/19.
//

#ifndef LIMITER_H
#define LIMITER_H

#include <jack.h>
extern "C"{
#include "compressor.h"
}

class Limiter{
private:
    jack_client_t *jackLimiterClient;
    jack_port_t *Limiter_Input_PortR;
    jack_port_t *Limiter_Input_PortL;
    jack_port_t *Limiter_Output_PortL;
    jack_port_t *Limiter_Output_PortR;

    float Limiter_Input_PeakL;
    float Limiter_Input_PeakR;

    float Limiter_Output_PeakL;
    float Limiter_Output_PeakR;
    bool Limiterenabled;

    sf_compressor_state_st simplecomp;

    float pregain, attack, release, threshold, ratio, meterGain;
    float *buffL, *buffR;
public:
    Limiter();
    void Setup_Limiter(int index);
    void ConnectPorts(int index);
    static int processLimiter(jack_nframes_t nframes, void *arg);

    float getLimiter_Input_PeakL();

    float getLimiter_Input_PeakR();

    float getLimiter_Output_PeakL();

    float getLimiter_Output_PeakR();

    void setPregain(float pregain);

    void setAttack(float attack);

    void setRelease(float release);

    void setThreshold(float threshold);

    void setRatio(float ratio);

    void setLimiterenabled(bool Limiterenabled);

    float getMeterGain() const;
};
#endif //SERVICETEST_LIMITER_H
