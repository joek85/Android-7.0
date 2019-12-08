/*
 * Copyright (C) 2011 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef _KARAOKE_AUDIO_UTILS_H
#define _KARAOKE_AUDIO_UTILS_H

#ifdef __cplusplus
extern "C" {
#endif
#include <tinyalsa/asoundlib.h>

/**
 * ALSA pcm id 
 *
 * used mainly for get_audio_card_dev()' param audio_device_name
 **/
#define INFOTMIC_PCM_ID_DEFAULT_CODEC     "audiocodec"
//"SUNXI-TDM0 rt3261-aif1-0"
#define INFOTMIC_PCM_ID_HDMI_AUDIO        "sndhdmi"
//"SUNXI-HDMIAUDIO sndhdmi-0"
#define INFOTMIC_PCM_ID_BUILD_AUDIO       "audiocodec"
//"SUNXI-CODEC sndcodec-0"
#define INFOTMIC_PCM_ID_SPDIF              "sndspdif"
//"SUNXI-SPDIF sndspdif-0"
#define INFOTMIC_PCM_ID_DEFAULT_RT     		"rt3261"
#define SND_SYS_SOUND_PATH                 "/sys/class/sound/card%i/id"
#define CARD_NUM_MAX                         8


void get_audio_card_dev(const char *audio_device_name, int stream, int *_card, int *_device);

void get_audio_card_num(const char *audio_device_name, int *_card);

void pcm_params_get_max_min(int card, int dev, enum pcm_stream stream, enum pcm_param param,
        unsigned int *rmax, unsigned int *rmin);

void pcm_params_get_rate_max_min(int card, int dev, enum pcm_stream stream,
        unsigned int *rmax, unsigned int *rmin);

void pcm_params_get_channel_max_min(int card, int dev, enum pcm_stream stream,
        unsigned int *rmax, unsigned int *rmin);

void pcm_params_get_format_max_min(int card, int dev, enum pcm_stream stream,
        unsigned int *rmax, unsigned int *rmin);

/* period size */
void pcm_params_get_ps_max_min(int card, int dev, enum pcm_stream stream,
        unsigned int *rmax, unsigned int *rmin);

/* period count */
void pcm_params_get_pc_max_min(int card, int dev, enum pcm_stream stream,
        unsigned int *rmax, unsigned int *rmin);

#ifdef __cplusplus
}
#endif

#endif
