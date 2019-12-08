//
// Created by joe on 12/18/18.
//

#include "Mixer_Main.h"
#include "LogHelper.h"
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
//float Output_Peak1L = 0.0f;
//float Output_Peak1R = 0.0f;
//float Output_Peak2L = 0.0f;
//float Output_Peak2R = 0.0f;
//float Output_Peak3L = 0.0f;
//float Output_Peak3R = 0.0f;
//float Output_Peak4L = 0.0f;
//float Output_Peak4R = 0.0f;


//float filterFreqs[] = {30, 100, 1000, 16000};
//float eqFreqs[] = {40,60,125,500,1000,6000,8000,16000};
//float stereoFreqs[] = {100,1000,16000};
Mixer_Main::Mixer_Main() {

    Input_GainL = 1.0f;
    Input_GainR = 1.0f;
    Input_VolL = 1.0f;
    Input_VolR = 1.0f;
    Input_PeakL = 0.0f;
    Input_PeakR = 0.0f;

    Player_GainL = 1.0f;
    Player_GainR = 1.0f;
    Player_VolL = 1.0f;
    Player_VolR = 1.0f;
    Player_PeakL = 0.0f;
    Player_PeakR = 0.0f;

    HeadPhones_GainL = 1.0f;
    HeadPhones_GainR = 1.0f;

    mic_GainL = 1.0f;
    mic_GainR = 1.0f;
    mic_PeakL = 0.0f;
    mic_PeakR = 0.0f;

    LinL = 0;
    LinR = 0;
    PinL = 0;
    PinR = 0;
    HinL = 0;
    HinR = 0;
    minL = 0;
    minR = 0;
}

void Mixer_Main::setInput_GainL(float input_GainL) {
    Input_GainL = input_GainL;
}

void Mixer_Main::setInput_GainR(float input_GainR) {
    Input_GainR = input_GainR;
}
void Mixer_Main::setPlayer_GainL(float player_GainL) {
    Player_GainL = player_GainL;
}

void Mixer_Main::setPlayer_GainR(float player_GainR) {
    Player_GainR = player_GainR;
}

float Mixer_Main::getInput_PeakL() {
    float tmpL = Input_PeakL;
    Input_PeakL = 0.0f;
    return tmpL;
}

float Mixer_Main::getInput_PeakR() {
    float tmpR = Input_PeakR;
    Input_PeakR = 0.0f;
    return tmpR;
}
float Mixer_Main::getPlayer_PeakL() {
    float tmpL = Player_PeakL;
    Player_PeakL = 0.0f;
    return tmpL;
}

float Mixer_Main::getPlayer_PeakR() {
    float tmpR = Player_PeakR;
    Player_PeakR = 0.0f;
    return tmpR;
}

void Mixer_Main::setHeadPhones_GainL(float HeadPhonesGainL) {
    HeadPhones_GainL = HeadPhonesGainL;
}

void Mixer_Main::setHeadPhones_GainR(float HeadPhonesGainR) {
    HeadPhones_GainR = HeadPhonesGainR;
}

float Mixer_Main::getHeadPhones_PeakL() {
    float tmpL = HeadPhones_PeakL;
    HeadPhones_PeakL = 0.0f;
    return tmpL;
}

float Mixer_Main::getHeadPhones_PeakR() {
    float tmpR = HeadPhones_PeakR;
    HeadPhones_PeakR = 0.0f;
    return tmpR;
}

void Mixer_Main::setMic_GainL(float mic_GainL) {
    Mixer_Main::mic_GainL = mic_GainL;
}

void Mixer_Main::setMic_GainR(float mic_GainR) {
    Mixer_Main::mic_GainR = mic_GainR;
}

float Mixer_Main::getMic_PeakL() {
    float tmpL = mic_PeakL;
    mic_PeakL = 0.0f;
    return tmpL;
}

float Mixer_Main::getMic_PeakR() {
    float tmpR = mic_PeakR;
    mic_PeakR = 0.0f;
    return tmpR;
}
float *Mixer_Main::getRecBuffer() {
    return recBuffer;
}

void Mixer_Main::setInput_VolL(float Input_VL) {
    Input_VolL = Input_VL;
}

void Mixer_Main::setInput_VolR(float Input_VR) {
    Input_VolR = Input_VR;
}

void Mixer_Main::setPlayer_VolL(float Player_VL) {
    Player_VolL = Player_VL;
}

void Mixer_Main::setPlayer_VolR(float Player_VR) {
    Player_VolR = Player_VR;
}

int Mixer_Main::process (jack_nframes_t nframes, void *arg) {

    Mixer_Main *mixer_main = static_cast<Mixer_Main*>(arg);
    jack_default_audio_sample_t
            *systeminL, *systeminR,*systemout1L, *systemout1R,
            *playerinL, *playerinR, *playerout1L, *playerout1R;
//            *headphonesinL, *headphonesinR, *headphonesout1L, *headphonesout1R,
//            *micinL, *micinR, *micout1L, *micout1R;

    systeminL = static_cast<jack_default_audio_sample_t *>(jack_port_get_buffer (mixer_main->Mixer_System_Input_PortL, nframes));
    systeminR = static_cast<jack_default_audio_sample_t *>(jack_port_get_buffer (mixer_main->Mixer_System_Input_PortR, nframes));
//    systemin2L = static_cast<jack_default_audio_sample_t *>(jack_port_get_buffer (mixer_main->Mixer_System_Input_Port2L, nframes));
//    systemin2R = static_cast<jack_default_audio_sample_t *>(jack_port_get_buffer (mixer_main->Mixer_System_Input_Port2R, nframes));
//    systemin3L = static_cast<jack_default_audio_sample_t *>(jack_port_get_buffer (mixer_main->Mixer_System_Input_Port3L, nframes));
//    systemin3R = static_cast<jack_default_audio_sample_t *>(jack_port_get_buffer (mixer_main->Mixer_System_Input_Port3R, nframes));
    playerinL = static_cast<jack_default_audio_sample_t *>(jack_port_get_buffer (mixer_main->Mixer_Player_Input_PortL, nframes));
    playerinR = static_cast<jack_default_audio_sample_t *>(jack_port_get_buffer (mixer_main->Mixer_Player_Input_PortR, nframes));

//    headphonesinL = static_cast<jack_default_audio_sample_t *>(jack_port_get_buffer (mixer_main->Mixer_HeadPhones_Input_PortL, nframes));
//    headphonesinR = static_cast<jack_default_audio_sample_t *>(jack_port_get_buffer (mixer_main->Mixer_HeadPhones_Input_PortR, nframes));

//    micinL = static_cast<jack_default_audio_sample_t *>(jack_port_get_buffer (mixer_main->Mixer_Mic_Input_PortL, nframes));
//    micinR = static_cast<jack_default_audio_sample_t *>(jack_port_get_buffer (mixer_main->Mixer_Mic_Input_PortR, nframes));
//    in2L = static_cast<jack_default_audio_sample_t *>(jack_port_get_buffer (mixer_main->Mixer_Input_Port2L, nframes));
//    in2R = static_cast<jack_default_audio_sample_t *>(jack_port_get_buffer (mixer_main->Mixer_Input_Port2R, nframes));
//    in3L = static_cast<jack_default_audio_sample_t *>(jack_port_get_buffer (mixer_main->Mixer_Input_Port3L, nframes));
//    in3R = static_cast<jack_default_audio_sample_t *>(jack_port_get_buffer (mixer_main->Mixer_Input_Port3R, nframes));
//    in4L = static_cast<jack_default_audio_sample_t *>(jack_port_get_buffer (mixer_main->Mixer_Input_Port4L, nframes));
//    in4R = static_cast<jack_default_audio_sample_t *>(jack_port_get_buffer (mixer_main->Mixer_Input_Port4R, nframes));

    systemout1L = static_cast<jack_default_audio_sample_t *>(jack_port_get_buffer (mixer_main->Mixer_System_Output_Port1L, nframes));
    systemout1R = static_cast<jack_default_audio_sample_t *>(jack_port_get_buffer (mixer_main->Mixer_System_Output_Port1R, nframes));
//    systemout2L = static_cast<jack_default_audio_sample_t *>(jack_port_get_buffer (mixer_main->Mixer_System_Output_Port2L, nframes));
//    systemout2R = static_cast<jack_default_audio_sample_t *>(jack_port_get_buffer (mixer_main->Mixer_System_Output_Port2R, nframes));
//    systemout3L = static_cast<jack_default_audio_sample_t *>(jack_port_get_buffer (mixer_main->Mixer_System_Output_Port3L, nframes));
//    systemout3R = static_cast<jack_default_audio_sample_t *>(jack_port_get_buffer (mixer_main->Mixer_System_Output_Port3R, nframes));
//    systemout4L = static_cast<jack_default_audio_sample_t *>(jack_port_get_buffer (mixer_main->Mixer_System_Output_Port4L, nframes));
//    systemout4R = static_cast<jack_default_audio_sample_t *>(jack_port_get_buffer (mixer_main->Mixer_System_Output_Port4R, nframes));

    playerout1L = static_cast<jack_default_audio_sample_t *>(jack_port_get_buffer (mixer_main->Mixer_Player_Output_Port1L, nframes));
    playerout1R = static_cast<jack_default_audio_sample_t *>(jack_port_get_buffer (mixer_main->Mixer_Player_Output_Port1R, nframes));

//    headphonesout1L = static_cast<jack_default_audio_sample_t *>(jack_port_get_buffer (mixer_main->Mixer_HeadPhones_Output_PortL, nframes));
//    headphonesout1R = static_cast<jack_default_audio_sample_t *>(jack_port_get_buffer (mixer_main->Mixer_HeadPhones_Output_PortR, nframes));

//    micout1L = static_cast<jack_default_audio_sample_t *>(jack_port_get_buffer (mixer_main->Mixer_Mic_Output_PortL, nframes));
//    micout1R = static_cast<jack_default_audio_sample_t *>(jack_port_get_buffer (mixer_main->Mixer_Mic_Output_PortR, nframes));

//    out3L = static_cast<jack_default_audio_sample_t *>(jack_port_get_buffer (mixer_main->Mixer_Output_Port3L, nframes));
//    out3R = static_cast<jack_default_audio_sample_t *>(jack_port_get_buffer (mixer_main->Mixer_Output_Port3R, nframes));
//    out4L = static_cast<jack_default_audio_sample_t *>(jack_port_get_buffer (mixer_main->Mixer_Output_Port4L, nframes));
//    out4R = static_cast<jack_default_audio_sample_t *>(jack_port_get_buffer (mixer_main->Mixer_Output_Port4R, nframes));
//    float left, right;
    for (unsigned int i = 0; i < nframes; i++) {

        mixer_main->LinL = (float) fabs(systeminL[i]);
        mixer_main->LinR = (float) fabs(systeminR[i]);

        mixer_main->PinL = (float) fabs(playerinL[i]);
        mixer_main->PinR = (float) fabs(playerinR[i]);

//        mixer_main->HinL = (float) fabs(headphonesout1L[i]);
//        mixer_main->HinR = (float) fabs(headphonesout1R[i]);
//        const float Lout1L = (const float) fabs(out1L[i]);
//        const float Lout1R = (const float) fabs(out1R[i]);
//        const float Lout2L = (const float) fabs(out2L[i]);
//        const float Lout2R = (const float) fabs(out2R[i]);
//        const float Lout3L = (const float) fabs(out3L[i]);
//        const float Lout3R = (const float) fabs(out3R[i]);
//        const float Lout4L = (const float) fabs(out4L[i]);
//        const float Lout4R = (const float) fabs(out4R[i]);
//        mixer_main->minL = (float) fabs(micinL[i]);
//        mixer_main->minR = (float) fabs(micinR[i]);

        if (mixer_main->LinL > mixer_main->Input_PeakL) {
            mixer_main->Input_PeakL = mixer_main->LinL;
        }
        if (mixer_main->LinR > mixer_main->Input_PeakR) {
            mixer_main->Input_PeakR = mixer_main->LinR;
        }
        if (mixer_main->PinL > mixer_main->Player_PeakL) {
            mixer_main->Player_PeakL = mixer_main->PinL;
        }
        if (mixer_main->PinR > mixer_main->Player_PeakR) {
            mixer_main->Player_PeakR = mixer_main->PinR;
        }
//        if (mixer_main->HinL > mixer_main->HeadPhones_PeakL) {
//            mixer_main->HeadPhones_PeakL = mixer_main->HinL;
//        }
//        if (mixer_main->HinR > mixer_main->HeadPhones_PeakR) {
//            mixer_main->HeadPhones_PeakR = mixer_main->HinR;
//        }
//        if (mixer_main->minL > mixer_main->mic_PeakL) {
//            mixer_main->mic_PeakL = mixer_main->minL;
//        }
//        if (mixer_main->minR > mixer_main->mic_PeakR) {
//            mixer_main->mic_PeakR = mixer_main->minR;
//        }
//        if (Lout1L > Output_Peak1L) {
//            Output_Peak1L = Lout1L;
//        }
//        if (Lout1R > Output_Peak1R) {
//            Output_Peak1R = Lout1R;
//        }
//        if (Lout2L > Output_Peak2L) {
//            Output_Peak2L = Lout2L;
//        }
//        if (Lout2R > Output_Peak2R) {
//            Output_Peak2R = Lout2R;
//        }
//        if (Lout3L > Output_Peak3L) {
//            Output_Peak3L = Lout3L;
//        }
//        if (Lout3R > Output_Peak3R) {
//            Output_Peak3R = Lout3R;
//        }
//        if (Lout4L > Output_Peak4L) {
//            Output_Peak4L = Lout4L;
//        }
//        if (Lout4R > Output_Peak4R) {
//            Output_Peak4R = Lout4R;
//        }

        systeminL[i] *= mixer_main->Input_GainL * mixer_main->Input_VolL;
        systeminR[i] *= mixer_main->Input_GainR * mixer_main->Input_VolR;

//        systeminL[i] = (jack_default_audio_sample_t) mixer_main->filters[0]->process(systeminL[i]);
//        systeminR[i] = (jack_default_audio_sample_t) mixer_main->filters[1]->process(systeminR[i]);
//        systeminL[i] = (jack_default_audio_sample_t) mixer_main->filters[2]->process(systeminL[i]);
//        systeminR[i] = (jack_default_audio_sample_t) mixer_main->filters[3]->process(systeminR[i]);
//        systeminL[i] = (jack_default_audio_sample_t) mixer_main->filters[4]->process(systeminL[i]);
//        systeminR[i] = (jack_default_audio_sample_t) mixer_main->filters[5]->process(systeminR[i]);
//        systeminL[i] = (jack_default_audio_sample_t) mixer_main->filters[6]->process(systeminL[i]);
//        systeminR[i] = (jack_default_audio_sample_t) mixer_main->filters[7]->process(systeminR[i]);
//
//        systemin2L[i] = (jack_default_audio_sample_t) mixer_main->filters[8]->process(systemin2L[i]);
//        systemin2R[i] = (jack_default_audio_sample_t) mixer_main->filters[9]->process(systemin2R[i]);
//        systemin2L[i] = (jack_default_audio_sample_t) mixer_main->filters[10]->process(systemin2L[i]);
//        systemin2R[i] = (jack_default_audio_sample_t) mixer_main->filters[11]->process(systemin2R[i]);
//        systemin2L[i] = (jack_default_audio_sample_t) mixer_main->filters[12]->process(systemin2L[i]);
//        systemin2R[i] = (jack_default_audio_sample_t) mixer_main->filters[13]->process(systemin2R[i]);
//        systemin2L[i] = (jack_default_audio_sample_t) mixer_main->filters[14]->process(systemin2L[i]);
//        systemin2R[i] = (jack_default_audio_sample_t) mixer_main->filters[15]->process(systemin2R[i]);
//
//        systemin3L[i] = (jack_default_audio_sample_t) mixer_main->filters[16]->process(systemin3L[i]);
//        systemin3R[i] = (jack_default_audio_sample_t) mixer_main->filters[17]->process(systemin3R[i]);
//        systemin3L[i] = (jack_default_audio_sample_t) mixer_main->filters[18]->process(systemin3L[i]);
//        systemin3R[i] = (jack_default_audio_sample_t) mixer_main->filters[19]->process(systemin3R[i]);
//        systemin3L[i] = (jack_default_audio_sample_t) mixer_main->filters[20]->process(systemin3L[i]);
//        systemin3R[i] = (jack_default_audio_sample_t) mixer_main->filters[21]->process(systemin3R[i]);
//        systemin3L[i] = (jack_default_audio_sample_t) mixer_main->filters[22]->process(systemin3L[i]);
//        systemin3R[i] = (jack_default_audio_sample_t) mixer_main->filters[23]->process(systemin3R[i]);
//
//
//
//        systeminL[i] = (jack_default_audio_sample_t) mixer_main->eq[0]->process(systeminL[i]);
//        systeminR[i] = (jack_default_audio_sample_t) mixer_main->eq[1]->process(systeminR[i]);
//        systeminL[i] = (jack_default_audio_sample_t) mixer_main->eq[2]->process(systeminL[i]);
//        systeminR[i] = (jack_default_audio_sample_t) mixer_main->eq[3]->process(systeminR[i]);
//        systeminL[i] = (jack_default_audio_sample_t) mixer_main->eq[4]->process(systeminL[i]);
//        systeminR[i] = (jack_default_audio_sample_t) mixer_main->eq[5]->process(systeminR[i]);
//        systeminL[i] = (jack_default_audio_sample_t) mixer_main->eq[6]->process(systeminL[i]);
//        systeminR[i] = (jack_default_audio_sample_t) mixer_main->eq[7]->process(systeminR[i]);
//        systeminL[i] = (jack_default_audio_sample_t) mixer_main->eq[8]->process(systeminL[i]);
//        systeminR[i] = (jack_default_audio_sample_t) mixer_main->eq[9]->process(systeminR[i]);
//        systeminL[i] = (jack_default_audio_sample_t) mixer_main->eq[10]->process(systeminL[i]);
//        systeminR[i] = (jack_default_audio_sample_t) mixer_main->eq[11]->process(systeminR[i]);
//        systeminL[i] = (jack_default_audio_sample_t) mixer_main->eq[12]->process(systeminL[i]);
//        systeminR[i] = (jack_default_audio_sample_t) mixer_main->eq[13]->process(systeminR[i]);
//        systeminL[i] = (jack_default_audio_sample_t) mixer_main->eq[14]->process(systeminL[i]);
//        systeminR[i] = (jack_default_audio_sample_t) mixer_main->eq[15]->process(systeminR[i]);
//
//        systemin3L[i] = (jack_default_audio_sample_t) mixer_main->eq[16]->process(systemin3L[i]);
//        systemin3R[i] = (jack_default_audio_sample_t) mixer_main->eq[17]->process(systemin3R[i]);
//        systemin3L[i] = (jack_default_audio_sample_t) mixer_main->eq[18]->process(systemin3L[i]);
//        systemin3R[i] = (jack_default_audio_sample_t) mixer_main->eq[19]->process(systemin3R[i]);
//        systemin3L[i] = (jack_default_audio_sample_t) mixer_main->eq[20]->process(systemin3L[i]);
//        systemin3R[i] = (jack_default_audio_sample_t) mixer_main->eq[5]->process(systemin3R[i]);
//        systemin3L[i] = (jack_default_audio_sample_t) mixer_main->eq[6]->process(systemin3L[i]);
//        systemin3R[i] = (jack_default_audio_sample_t) mixer_main->eq[7]->process(systemin3R[i]);
//        systemin3L[i] = (jack_default_audio_sample_t) mixer_main->eq[8]->process(systemin3L[i]);
//        systemin3R[i] = (jack_default_audio_sample_t) mixer_main->eq[9]->process(systemin3R[i]);
//        systemin3L[i] = (jack_default_audio_sample_t) mixer_main->eq[10]->process(systemin3L[i]);
//        systemin3R[i] = (jack_default_audio_sample_t) mixer_main->eq[11]->process(systemin3R[i]);
//        systemin3L[i] = (jack_default_audio_sample_t) mixer_main->eq[12]->process(systemin3L[i]);
//        systemin3R[i] = (jack_default_audio_sample_t) mixer_main->eq[13]->process(systemin3R[i]);
//        systemin3L[i] = (jack_default_audio_sample_t) mixer_main->eq[14]->process(systemin3L[i]);
//        systemin3R[i] = (jack_default_audio_sample_t) mixer_main->eq[15]->process(systemin3R[i]);
//
//        left = right = 0.0f;
//        left += mixer_main->filterStereoEnhancers[0]->process(systeminL[i]);
//        left += mixer_main->filterStereoEnhancers[2]->process(systeminL[i]);
//        left += mixer_main->filterStereoEnhancers[4]->process(systeminL[i]);
//        systeminL[i] = left;
//
//        right += mixer_main->filterStereoEnhancers[1]->process(systeminR[i]);
//        right += mixer_main->filterStereoEnhancers[3]->process(systeminR[i]);
//        right += mixer_main->filterStereoEnhancers[5]->process(systeminR[i]);
//        systeminR[i] = right;
        playerinL[i] *= mixer_main->Player_GainL * mixer_main->Player_VolL;
        playerinR[i] *= mixer_main->Player_GainR * mixer_main->Player_VolR;

//        headphonesinL[i] *= mixer_main->HeadPhones_GainL;
//        headphonesinR[i] *= mixer_main->HeadPhones_GainR;

//        micinL[i] *= mixer_main->mic_GainL;
//        micinR[i] *= mixer_main->mic_GainR;
    }
    //sf_compressor_process(&mixer_main->simplecomp, nframes, systeminL, systeminR,out1L,out1R);
//    memcpy(in1L, playerinL, sizeof(jack_default_audio_sample_t) * nframes);
//    memcpy(in1R, playerinR, sizeof(jack_default_audio_sample_t) * nframes);

    memcpy(systemout1L, systeminL, sizeof(jack_default_audio_sample_t) * nframes);
    memcpy(systemout1R, systeminR, sizeof(jack_default_audio_sample_t) * nframes);

	memcpy(mixer_main->recBuffer, systeminL, sizeof(jack_default_audio_sample_t) * nframes);
//    memcpy(systemout2L, systeminL, sizeof(jack_default_audio_sample_t) * nframes);
//    memcpy(systemout2R, systeminR, sizeof(jack_default_audio_sample_t) * nframes);
//    memcpy(systemout3L, systeminL, sizeof(jack_default_audio_sample_t) * nframes);
//    memcpy(systemout3R, systeminR, sizeof(jack_default_audio_sample_t) * nframes);
//    memcpy(in4L, playerinL, sizeof(jack_default_audio_sample_t) * nframes);
//    memcpy(in4R, playerinR, sizeof(jack_default_audio_sample_t) * nframes);

    memcpy(playerout1L,playerinL, sizeof(jack_default_audio_sample_t) * nframes);
    memcpy(playerout1R,playerinR, sizeof(jack_default_audio_sample_t) * nframes);

//    memcpy(headphonesout1L,headphonesinL, sizeof(jack_default_audio_sample_t) * nframes);
//    memcpy(headphonesout1R,headphonesinR, sizeof(jack_default_audio_sample_t) * nframes);

//    memcpy(micout1L,micinL, sizeof(jack_default_audio_sample_t) * nframes);
//    memcpy(micout1R,micinR, sizeof(jack_default_audio_sample_t) * nframes);
//    memcpy(out2L,playerinL,  sizeof(jack_default_audio_sample_t) * nframes);
//    memcpy(out2R,playerinR,  sizeof(jack_default_audio_sample_t) * nframes);
//    memcpy(out3L,playerinL,  sizeof(jack_default_audio_sample_t) * nframes);
//    memcpy(out3R,playerinR,  sizeof(jack_default_audio_sample_t) * nframes);
//    memcpy(systemout4L, out4L, sizeof(jack_default_audio_sample_t) * nframes);
//    memcpy(systemout4R, out4R, sizeof(jack_default_audio_sample_t) * nframes);

    return 0;
}
void Mixer_Main::Setup_Mixer_Main(){
    jack_status_t status;
    jackMixerMainClient = jack_client_open ("Mixer_Main", JackNullOption, &status, NULL);
    if (jackMixerMainClient == NULL) {
        LOGD ( "jack_client_open() failed, ""status = 0x%2.0x\n", status);

        if (status & JackServerFailed) {
            LOGD ( "Unable to connect to JACK server\n");
        }
    }

    jack_set_process_callback (jackMixerMainClient, process, this);
		recBuffer = (jack_default_audio_sample_t *)malloc(sizeof(jack_default_audio_sample_t)* 300);
    Mixer_System_Input_PortL = jack_port_register (jackMixerMainClient, "Mixer_System_inL", JACK_DEFAULT_AUDIO_TYPE, JackPortIsInput, 0);
    Mixer_System_Input_PortR = jack_port_register (jackMixerMainClient, "Mixer_System_inR", JACK_DEFAULT_AUDIO_TYPE, JackPortIsInput, 0);
//    Mixer_System_Input_Port2L = jack_port_register (jackMixerMainClient, "Mixer_System_in2L", JACK_DEFAULT_AUDIO_TYPE, JackPortIsInput, 0);
//    Mixer_System_Input_Port2R = jack_port_register (jackMixerMainClient, "Mixer_System_in2R", JACK_DEFAULT_AUDIO_TYPE, JackPortIsInput, 0);
//    Mixer_System_Input_Port3L = jack_port_register (jackMixerMainClient, "Mixer_System_in3L", JACK_DEFAULT_AUDIO_TYPE, JackPortIsInput, 0);
//    Mixer_System_Input_Port3R = jack_port_register (jackMixerMainClient, "Mixer_System_in3R", JACK_DEFAULT_AUDIO_TYPE, JackPortIsInput, 0);

    Mixer_Player_Input_PortL = jack_port_register (jackMixerMainClient, "Mixer_Player_inL", JACK_DEFAULT_AUDIO_TYPE, JackPortIsInput, 0);
    Mixer_Player_Input_PortR = jack_port_register (jackMixerMainClient, "Mixer_Player_inR", JACK_DEFAULT_AUDIO_TYPE, JackPortIsInput, 0);
//    Mixer_Player_Input_Port2L = jack_port_register (jackMixerMainClient, "Mixer_Player_in2L", JACK_DEFAULT_AUDIO_TYPE, JackPortIsInput, 0);
//    Mixer_Player_Input_Port2R = jack_port_register (jackMixerMainClient, "Mixer_Player_in2R", JACK_DEFAULT_AUDIO_TYPE, JackPortIsInput, 0);
//    Mixer_Player_Input_Port3L = jack_port_register (jackMixerMainClient, "Mixer_Player_in3L", JACK_DEFAULT_AUDIO_TYPE, JackPortIsInput, 0);
//    Mixer_Player_Input_Port3R = jack_port_register (jackMixerMainClient, "Mixer_Player_in3R", JACK_DEFAULT_AUDIO_TYPE, JackPortIsInput, 0);

//    Mixer_Input_Port1L = jack_port_register (jackMixerMainClient, "Mixer_in1L", JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0);
//    Mixer_Input_Port1R = jack_port_register (jackMixerMainClient, "Mixer_in1R", JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0);
//    Mixer_Input_Port2L = jack_port_register (jackMixerMainClient, "Mixer_in2L", JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0);
//    Mixer_Input_Port2R = jack_port_register (jackMixerMainClient, "Mixer_in2R", JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0);
//    Mixer_Input_Port3L = jack_port_register (jackMixerMainClient, "Mixer_in3L", JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0);
//    Mixer_Input_Port3R = jack_port_register (jackMixerMainClient, "Mixer_in3R", JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0);
//    Mixer_Input_Port4L = jack_port_register (jackMixerMainClient, "Mixer_in4L", JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0);
//    Mixer_Input_Port4R = jack_port_register (jackMixerMainClient, "Mixer_in4R", JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0);
//
    Mixer_System_Output_Port1L = jack_port_register (jackMixerMainClient, "Mixer_Systemout_1L", JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0);
    Mixer_System_Output_Port1R = jack_port_register (jackMixerMainClient, "Mixer_Systemout_1R", JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0);
//    Mixer_System_Output_Port2L = jack_port_register (jackMixerMainClient, "Mixer_Systemout_2L", JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0);
//    Mixer_System_Output_Port2R = jack_port_register (jackMixerMainClient, "Mixer_Systemout_2R", JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0);
//    Mixer_System_Output_Port3L = jack_port_register (jackMixerMainClient, "Mixer_Systemout_3L", JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0);
//    Mixer_System_Output_Port3R = jack_port_register (jackMixerMainClient, "Mixer_Systemout_3R", JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0);
//    Mixer_System_Output_Port4L = jack_port_register (jackMixerMainClient, "Mixer_Systemout_4L", JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0);
//    Mixer_System_Output_Port4R = jack_port_register (jackMixerMainClient, "Mixer_Systemout_4R", JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0);

    Mixer_Player_Output_Port1L = jack_port_register (jackMixerMainClient, "Mixer_Player_out_1L", JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0);
    Mixer_Player_Output_Port1R = jack_port_register (jackMixerMainClient, "Mixer_Player_out_1R", JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0);
//    Mixer_Output_Port2L = jack_port_register (jackMixerMainClient, "Mixer_out2L", JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0);
//    Mixer_Output_Port2R = jack_port_register (jackMixerMainClient, "Mixer_out2R", JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0);
//    Mixer_Output_Port3L = jack_port_register (jackMixerMainClient, "Mixer_out3L", JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0);
//    Mixer_Output_Port3R = jack_port_register (jackMixerMainClient, "Mixer_out3R", JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0);

//    Mixer_HeadPhones_Input_PortL = jack_port_register (jackMixerMainClient, "Mixer_HeadPhones_inL", JACK_DEFAULT_AUDIO_TYPE, JackPortIsInput, 0);
//    Mixer_HeadPhones_Input_PortR = jack_port_register (jackMixerMainClient, "Mixer_HeadPhones_inR", JACK_DEFAULT_AUDIO_TYPE, JackPortIsInput, 0);

//    Mixer_HeadPhones_Output_PortL = jack_port_register (jackMixerMainClient, "Mixer_HeadPhones_outL", JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0);
//    Mixer_HeadPhones_Output_PortR = jack_port_register (jackMixerMainClient, "Mixer_HeadPhones_outR", JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0);

//    Mixer_Mic_Input_PortL = jack_port_register (jackMixerMainClient, "Mixer_Mic_inL", JACK_DEFAULT_AUDIO_TYPE, JackPortIsInput, 0);
//    Mixer_Mic_Input_PortR = jack_port_register (jackMixerMainClient, "Mixer_Mic_inR", JACK_DEFAULT_AUDIO_TYPE, JackPortIsInput, 0);

//    Mixer_Mic_Output_PortL = jack_port_register (jackMixerMainClient, "Mixer_Mic_outL", JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0);
//    Mixer_Mic_Output_PortR = jack_port_register (jackMixerMainClient, "Mixer_Mic_outR", JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0);

    jack_activate(jackMixerMainClient);

    jack_connect (jackMixerMainClient, "system:capture_1", jack_port_name (Mixer_System_Input_PortL));
    jack_connect (jackMixerMainClient, "system:capture_2", jack_port_name (Mixer_System_Input_PortR));

    jack_connect (jackMixerMainClient, "alsaplayer:capture_1", jack_port_name (Mixer_Player_Input_PortL));
    jack_connect (jackMixerMainClient, "alsaplayer:capture_2", jack_port_name (Mixer_Player_Input_PortR));

//    jack_connect (jackMixerMainClient, "alsamic:capture_1", jack_port_name (Mixer_Mic_Input_PortL));
//    jack_connect (jackMixerMainClient, "alsamic:capture_2", jack_port_name (Mixer_Mic_Input_PortR));

//    jack_connect (jackMixerMainClient, jack_port_name (Mixer_HeadPhones_Output_PortL), "alsahph:playback_1");
//    jack_connect (jackMixerMainClient, jack_port_name (Mixer_HeadPhones_Output_PortR), "alsahph:playback_2");
}

void Mixer_Main::Release_Mixer_Main() {
    jack_deactivate(jackMixerMainClient);
}
void Mixer_Main::ConnectPorts(){

    jack_connect (jackMixerMainClient, jack_port_name (Mixer_System_Output_Port1L), "Filter0:Filter_inL");
    jack_connect (jackMixerMainClient, jack_port_name (Mixer_System_Output_Port1R), "Filter0:Filter_inR");
    jack_connect (jackMixerMainClient, jack_port_name (Mixer_System_Output_Port1L), "Filter1:Filter_inL");
    jack_connect (jackMixerMainClient, jack_port_name (Mixer_System_Output_Port1R), "Filter1:Filter_inR");
    jack_connect (jackMixerMainClient, jack_port_name (Mixer_System_Output_Port1L), "Filter2:Filter_inL");
    jack_connect (jackMixerMainClient, jack_port_name (Mixer_System_Output_Port1R), "Filter2:Filter_inR");

    jack_connect (jackMixerMainClient, jack_port_name (Mixer_Player_Output_Port1L), "Filter0:Filter_inL");
    jack_connect (jackMixerMainClient, jack_port_name (Mixer_Player_Output_Port1R), "Filter0:Filter_inR");
    jack_connect (jackMixerMainClient, jack_port_name (Mixer_Player_Output_Port1L), "Filter1:Filter_inL");
    jack_connect (jackMixerMainClient, jack_port_name (Mixer_Player_Output_Port1R), "Filter1:Filter_inR");
    jack_connect (jackMixerMainClient, jack_port_name (Mixer_Player_Output_Port1L), "Filter2:Filter_inL");
    jack_connect (jackMixerMainClient, jack_port_name (Mixer_Player_Output_Port1R), "Filter2:Filter_inR");

//    jack_connect (jackMixerMainClient, jack_port_name (Mixer_System_Output_Port1L), jack_port_name(Mixer_HeadPhones_Input_PortL));
//    jack_connect (jackMixerMainClient, jack_port_name (Mixer_System_Output_Port1R), jack_port_name(Mixer_HeadPhones_Input_PortR));
//    jack_connect (jackMixerMainClient, jack_port_name (Mixer_Player_Output_Port1L), jack_port_name(Mixer_HeadPhones_Input_PortL));
//    jack_connect (jackMixerMainClient, jack_port_name (Mixer_Player_Output_Port1R), jack_port_name(Mixer_HeadPhones_Input_PortR));

//    jack_connect (jackMixerMainClient, jack_port_name (Mixer_Mic_Output_PortL), "Filter0:Filter_inL");
//    jack_connect (jackMixerMainClient, jack_port_name (Mixer_Mic_Output_PortR), "Filter0:Filter_inR");
//    jack_connect (jackMixerMainClient, jack_port_name (Mixer_Mic_Output_PortL), "Filter1:Filter_inL");
//    jack_connect (jackMixerMainClient, jack_port_name (Mixer_Mic_Output_PortR), "Filter1:Filter_inR");
//    jack_connect (jackMixerMainClient, jack_port_name (Mixer_Mic_Output_PortL), "Filter2:Filter_inL");
//    jack_connect (jackMixerMainClient, jack_port_name (Mixer_Mic_Output_PortR), "Filter2:Filter_inR");
}

void Mixer_Main::ConnectPorts(int mixerchannel, int index) {
    char portL[120] = {0,};
    char portR[120] = {0,};
    sprintf(portL, "Filter%d:Filter_inL",mixerchannel);
    sprintf(portR, "Filter%d:Filter_inR",mixerchannel);
    switch (mixerchannel){
        case 0:
        case 1:
        case 2:
        case 3:
            switch (index){
                case 0:
                    jack_connect (jackMixerMainClient, jack_port_name (Mixer_System_Output_Port1L), portL);
                    jack_connect (jackMixerMainClient, jack_port_name (Mixer_System_Output_Port1R), portR);
                    jack_connect (jackMixerMainClient, jack_port_name (Mixer_Player_Output_Port1L), portL);
                    jack_connect (jackMixerMainClient, jack_port_name (Mixer_Player_Output_Port1R), portR);
//                    jack_connect (jackMixerMainClient, jack_port_name (Mixer_Mic_Output_PortL), portL);
//                    jack_connect (jackMixerMainClient, jack_port_name (Mixer_Mic_Output_PortR), portR);
                    break;
                case 1:
                    jack_connect (jackMixerMainClient, jack_port_name (Mixer_System_Output_Port1L), portL);
                    jack_connect (jackMixerMainClient, jack_port_name (Mixer_System_Output_Port1R), portR);
                    jack_disconnect (jackMixerMainClient, jack_port_name (Mixer_Player_Output_Port1L), portL);
                    jack_disconnect (jackMixerMainClient, jack_port_name (Mixer_Player_Output_Port1R), portR);
//                    jack_disconnect (jackMixerMainClient, jack_port_name (Mixer_Mic_Output_PortL), portL);
//                    jack_disconnect (jackMixerMainClient, jack_port_name (Mixer_Mic_Output_PortR), portR);
                    break;
                case 2:
                    jack_disconnect (jackMixerMainClient, jack_port_name (Mixer_System_Output_Port1L), portL);
                    jack_disconnect (jackMixerMainClient, jack_port_name (Mixer_System_Output_Port1R), portR);
                    jack_connect (jackMixerMainClient, jack_port_name (Mixer_Player_Output_Port1L), portL);
                    jack_connect (jackMixerMainClient, jack_port_name (Mixer_Player_Output_Port1R), portR);
//                    jack_disconnect (jackMixerMainClient, jack_port_name (Mixer_Mic_Output_PortL), portL);
//                    jack_disconnect (jackMixerMainClient, jack_port_name (Mixer_Mic_Output_PortR), portR);
                    break;
                case 3:
                    jack_disconnect (jackMixerMainClient, jack_port_name (Mixer_System_Output_Port1L), portL);
                    jack_disconnect (jackMixerMainClient, jack_port_name (Mixer_System_Output_Port1R), portR);
                    jack_disconnect (jackMixerMainClient, jack_port_name (Mixer_Player_Output_Port1L), portL);
                    jack_disconnect (jackMixerMainClient, jack_port_name (Mixer_Player_Output_Port1R), portR);
//                    jack_connect (jackMixerMainClient, jack_port_name (Mixer_Mic_Output_PortL), portL);
//                    jack_connect (jackMixerMainClient, jack_port_name (Mixer_Mic_Output_PortR), portR);
                    break;
                default:break;
            }
            break;
        case 4:
            break;
        default:break;
    }

}
