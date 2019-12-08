//
// Created by joe on 12/18/18.
//

#ifndef MIXER_MAIN_H
#define MIXER_MAIN_H
#include <jack.h>


class Mixer_Main{
private:
    jack_port_t
            *Mixer_System_Input_PortL, *Mixer_System_Input_PortR,
            *Mixer_Player_Input_PortL, *Mixer_Player_Input_PortR,
            *Mixer_System_Output_Port1L, *Mixer_System_Output_Port1R,
            *Mixer_Player_Output_Port1L, *Mixer_Player_Output_Port1R,
            *Mixer_HeadPhones_Input_PortL, *Mixer_HeadPhones_Input_PortR,
            *Mixer_HeadPhones_Output_PortL, *Mixer_HeadPhones_Output_PortR,
            *Mixer_Mic_Input_PortL, *Mixer_Mic_Input_PortR,
            *Mixer_Mic_Output_PortL, *Mixer_Mic_Output_PortR;

    float Input_GainL;
    float Input_GainR;
    float Input_VolL;
    float Input_VolR;
    float Input_PeakL;
    float Input_PeakR;

    float Player_GainL;
    float Player_GainR;
    float Player_VolL;
    float Player_VolR;
    float Player_PeakL;
    float Player_PeakR;

    float HeadPhones_GainL;
    float HeadPhones_GainR;
    float HeadPhones_PeakL;
    float HeadPhones_PeakR;

    float mic_GainL;
    float mic_GainR;
    float mic_PeakL;
    float mic_PeakR;

    jack_client_t *jackMixerMainClient;

    float LinL;
    float LinR;
    float PinL;
    float PinR;
    float HinL;
    float HinR;
    float minL;
    float minR;

public:
    Mixer_Main();

    void Setup_Mixer_Main();
    void Release_Mixer_Main();
    static int process(jack_nframes_t nframes, void *arg);

    void setInput_GainL(float Input_GainL);

    void setInput_GainR(float Input_GainR);

    float getInput_PeakL();

    float getInput_PeakR();

    void ConnectPorts();
	void ConnectPorts(int mixerchannel, int index);

    void setPlayer_GainL(float player_GainL);

    void setPlayer_GainR(float player_GainR);

    float getPlayer_PeakL();

    float getPlayer_PeakR();

    void setHeadPhones_GainL(float HeadPhones_GainL);

    void setHeadPhones_GainR(float HeadPhones_GainR);

    float getHeadPhones_PeakL();

    float getHeadPhones_PeakR();

    void setMic_GainL(float mic_GainL);

    void setMic_GainR(float mic_GainR);

    float getMic_PeakR();

    float getMic_PeakL();

	float *recBuffer;
	float *getRecBuffer();

    void setInput_VolL(float Input_VolL);

    void setInput_VolR(float Input_VolR);

    void setPlayer_VolL(float Player_VolL);

    void setPlayer_VolR(float Player_VolR);
};



#endif //SERVICETEST_MIXER_MAIN_H
