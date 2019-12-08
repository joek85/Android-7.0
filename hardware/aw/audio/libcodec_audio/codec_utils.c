
#define LOG_TAG "codec_utils"
#define LOG_NDEBUG 0

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <utils/Log.h>
#include <cutils/properties.h>

#include "asoundlib.h"

#include "codec_utils.h"

struct pcm_config bp_i2s_out_config =
{
	.channels = 1,
	.rate = 8000,
	.period_size = 1024,
	.period_count = 4,
	.format = PCM_FORMAT_S16_LE,
	.start_threshold = 0,
	.stop_threshold = 0,
	.silence_threshold = 0,
};

struct pcm_config bp_i2s_in_config =
{
	.channels = 1,
	.rate = 8000,
	.period_size = 1024,
	.period_count = 4,
	.format = PCM_FORMAT_S16_LE,
	.start_threshold = 0,
	.stop_threshold = 0,
	.silence_threshold = 0,
};

struct pcm_config bt_pcm_out_config =
{
	.channels = 1,
	.rate = 8000,
	.period_size = 1024,
	.period_count =4,
	.format = PCM_FORMAT_S16_LE,
	.start_threshold = 0,
	.stop_threshold = 0,
	.silence_threshold = 0,
};

struct pcm_config bt_pcm_in_config =
{
	.channels = 1,
	.rate = 8000,
	.period_size = 1024,
	.period_count = 4,
	.format = PCM_FORMAT_S16_LE,
	.start_threshold = 0,
	.stop_threshold = 0,
	.silence_threshold = 0,
};

struct pcm_config codec_out_config =
{
	.channels = 1,
	.rate = 44100,
	.period_size = 1024,
	.period_count = 4,
	.format = PCM_FORMAT_S16_LE, 
	.start_threshold = 0,        
	.stop_threshold = 0,
	.silence_threshold = 0,

};

struct pcm_config codec_in_config =
{
	.channels = 1,
	.rate = 44100,
	.period_size = 1024,          
	.period_count = 4,           
	.format = PCM_FORMAT_S16_LE, 
	.start_threshold = 0,        
	.stop_threshold = 0,
	.silence_threshold = 0,  
};

char *audio_dev_name[3]={"audiocodec","sndpcm", "sndi2s"} ;

int init_stream(struct dev_stream *dev_stream) 
{
	enum device_type dev_type = CARD_UNKNOWN;
	int dev_direction =0;
	int dev_node = -1;

	switch (dev_stream->type){
		case BT: 
			dev_type = CARD_PCM;
			break;
		case BT_FM: 
			dev_type = CARD_PCM;
			break;
		case BP:
			dev_type = CARD_I2S;
			break;	
		case FM:
			dev_type = CARD_I2S;
			break;
		case CODEC:
			dev_type = CARD_CODEC;
			break;	
		default:
			dev_type = CARD_UNKNOWN;				
	};

	if(dev_type == CARD_UNKNOWN){
		ALOGE("unknown stream");
		return -1;
	}

	dev_node = pcm_get_node_number(audio_dev_name[dev_type]);
	if (dev_node < 0) {
		ALOGE("err: get %s node number failed ", audio_dev_name[dev_type]);
		return -1;
	}


	dev_direction = dev_stream->direction == SENDER ? PCM_IN : PCM_OUT;
ALOGD("huangxin------------dev_node:%d", dev_node);
	dev_stream->dev = pcm_open(dev_node, 0, dev_direction, &(dev_stream->config));
	if (!pcm_is_ready(dev_stream->dev )) {
		ALOGE("err: Unable to open  device (%s)", pcm_get_error(dev_stream->dev));
		goto open_failed;
	}

	if (dev_direction == PCM_IN){ //only pcm_in alloc buffer
		dev_stream->buf_size = pcm_get_buffer_size(dev_stream->dev);
		dev_stream->buf = (void *)malloc(dev_stream->buf_size);
		if (dev_stream->buf == NULL) {
			ALOGE("Unable to allocate %d bytes", dev_stream->buf_size);
			goto malloc_failed;
		}
		//ALOGD("sender stream type:%d, pcm_read buf=%d bytes",dev_stream->type, dev_stream->buf_size);
	}
	memset(dev_stream->buf, 0, dev_stream->buf_size);

	ALOGD("dev_stream dev node =%d, type=%d, direction:%s, buf size:%d", 
			dev_node, dev_stream->type, dev_stream->direction == SENDER ? "PCM_IN" : "PCM_OUT", pcm_get_buffer_size(dev_stream->dev));

    	return 0;
malloc_failed:
	if (dev_stream->dev){
		pcm_close(dev_stream->dev);	
	}

open_failed:
    	return -1;
}

void close_stream(struct dev_stream *dev_stream) 
{

	if (dev_stream->buf){
		free(dev_stream->buf);	
	}

	if (dev_stream->dev){
		pcm_close(dev_stream->dev);
	}
}

void ReduceVolume(char *buf, int size, int repeat)
{
        int i,j;
        int zhen_shu;
        signed long minData = -0x8000;
        signed long maxData = 0x7FFF;
        signed short data ;
        unsigned char low, hight;

        if(!size){
                return;
        }   

	zhen_shu = size - size%2;

        for(i=0; i<zhen_shu; i+=2){
		low = buf[i];
                hight = buf[i+1];
                data = low | (hight << 8);
                for(j=0; j< repeat; j++){
                        data = data / 1.25;    
                        if(data < minData){
                                data = minData; 
                        } /*else if (data > 0x7fff){
                                data = maxData;
                        } */ // signed short is [-0x8000, 0x7fff]
                }   
                buf[i] = (data) & 0x00ff;
                buf[i+1] = ((data)>>8) & 0xff;
        } 
}


int get_mixer(struct mixer_ctls *mixer_ctls)
{
    struct mixer *mixer;

    mixer = mixer_open(0);
    if (!mixer) {
	    ALOGE("Unable to open the mixer, aborting.");
	    return -1;
    }

	/*adc volume*/
	mixer_ctls->adc_left_chan_vol = mixer_get_ctl_by_name(mixer,
		    MIXER_AUDIO_ADC_VOL_CTRL_L);
    if (!mixer_ctls->adc_left_chan_vol) {
	    ALOGE("Unable to find '%s' mixer control", MIXER_AUDIO_ADC_VOL_CTRL_L);
		goto error_out;
    }
	mixer_ctls->adc_right_chan_vol = mixer_get_ctl_by_name(mixer,
		    MIXER_AUDIO_ADC_VOL_CTRL_R);
    if (!mixer_ctls->adc_right_chan_vol) {
	    ALOGE("Unable to find '%s' mixer control", MIXER_AUDIO_ADC_VOL_CTRL_R);
		goto error_out;
    }
	mixer_ctls->adc_left_chan_input_gain_ctrl = mixer_get_ctl_by_name(mixer,
		    MIXER_AUDIO_ADC_APC_CTRL_ADCLG);
    if (!mixer_ctls->adc_left_chan_input_gain_ctrl) {
	    ALOGE("Unable to find '%s' mixer control", MIXER_AUDIO_ADC_APC_CTRL_ADCLG);
		goto error_out;
    }
	mixer_ctls->adc_right_chan_input_gain_ctrl = mixer_get_ctl_by_name(mixer,
		    MIXER_AUDIO_ADC_APC_CTRL_ADCRG);
    if (!mixer_ctls->adc_right_chan_input_gain_ctrl) {
	    ALOGE("Unable to find '%s' mixer control", MIXER_AUDIO_ADC_APC_CTRL_ADCRG);
		goto error_out;
    }

	/*dac volume*/
	mixer_ctls->dac_left_chan_vol = mixer_get_ctl_by_name(mixer,
		    MIXER_AUDIO_DAC_VOL_CTRL_L);
    if (!mixer_ctls->dac_left_chan_vol) {
	    ALOGE("Unable to find '%s' mixer control", MIXER_AUDIO_DAC_VOL_CTRL_L);
		goto error_out;
    }
	mixer_ctls->dac_right_chan_vol = mixer_get_ctl_by_name(mixer,
		    MIXER_AUDIO_DAC_VOL_CTRL_R);
    if (!mixer_ctls->dac_right_chan_vol) {
	    ALOGE("Unable to find '%s' mixer control", MIXER_AUDIO_DAC_VOL_CTRL_R);
		goto error_out;
    }
	mixer_ctls->dac_left_chan_mixer_gain_ctrl = mixer_get_ctl_by_name(mixer,
		    MIXER_AUDIO_DAC_MXR_GAIN_L);
    if (!mixer_ctls->dac_left_chan_mixer_gain_ctrl) {
	    ALOGE("Unable to find '%s' mixer control", MIXER_AUDIO_DAC_MXR_GAIN_L);
		goto error_out;
    }
	mixer_ctls->dac_right_chan_mixer_gain_ctrl = mixer_get_ctl_by_name(mixer,
		    MIXER_AUDIO_DAC_MXR_GAIN_R);
    if (!mixer_ctls->dac_right_chan_mixer_gain_ctrl) {
	    ALOGE("Unable to find '%s' mixer control", MIXER_AUDIO_DAC_MXR_GAIN_R);
		goto error_out;
    }    
    
    mixer_ctls->mic1_boost_amp_gain_ctrl = mixer_get_ctl_by_name(mixer,
		    MIXER_AUDIO_ADC_SRCBST_CTRL_MIC1G);
    if (!mixer_ctls->mic1_boost_amp_gain_ctrl) {
	    ALOGE("Unable to find '%s' mixer control", MIXER_AUDIO_ADC_SRCBST_CTRL_MIC1G);
		goto error_out;
    }
    mixer_ctls->mic2_boost_amp_gain_ctrl = mixer_get_ctl_by_name(mixer,
		    MIXER_AUDIO_ADC_SRCBST_CTRL_MIC2G);
    if (!mixer_ctls->mic2_boost_amp_gain_ctrl) {
	    ALOGE("Unable to find '%s' mixer control", MIXER_AUDIO_ADC_SRCBST_CTRL_MIC2G);
		goto error_out;
    }
    mixer_ctls->linein_l_r_boost_amp_gain_ctrl = mixer_get_ctl_by_name(mixer,
		    MIXER_AUDIO_ADC_SRCBST_CTRL_LINEING);
    if (!mixer_ctls->linein_l_r_boost_amp_gain_ctrl) {
	    ALOGE("Unable to find '%s' mixer control", MIXER_AUDIO_ADC_SRCBST_CTRL_LINEING);
		goto error_out;
    }
    mixer_ctls->auxi_boost_amp_gain_ctrl = mixer_get_ctl_by_name(mixer,
		    MIXER_AUDIO_ADC_SRCBST_CTRL_AUXIG);
    if (!mixer_ctls->auxi_boost_amp_gain_ctrl) {
	    ALOGE("Unable to find '%s' mixer control", MIXER_AUDIO_ADC_SRCBST_CTRL_AUXIG);
		goto error_out;
    }

    mixer_ctls->axin_to_l_r_output_mixer_gain_ctrl = mixer_get_ctl_by_name(mixer,
		    MIXER_AUDIO_OMIXER_BST1_CTRL_AXG);
    if (!mixer_ctls->axin_to_l_r_output_mixer_gain_ctrl) {
	    ALOGE("Unable to find '%s' mixer control", MIXER_AUDIO_OMIXER_BST1_CTRL_AXG);
		goto error_out;
    }
    mixer_ctls->mic1_to_l_r_output_mixer_gain_ctrl = mixer_get_ctl_by_name(mixer,
		    MIXER_AUDIO_OMIXER_BST1_CTRL_MIC1G);
    if (!mixer_ctls->mic1_to_l_r_output_mixer_gain_ctrl) {
	    ALOGE("Unable to find '%s' mixer control", MIXER_AUDIO_OMIXER_BST1_CTRL_MIC1G);
		goto error_out;
    }
    mixer_ctls->mic2_to_l_r_output_mixer_gain_ctrl = mixer_get_ctl_by_name(mixer,
		    MIXER_AUDIO_OMIXER_BST1_CTRL_MIC2G);
    if (!mixer_ctls->mic2_to_l_r_output_mixer_gain_ctrl) {
	    ALOGE("Unable to find '%s' mixer control", MIXER_AUDIO_OMIXER_BST1_CTRL_MIC2G);
		goto error_out;
    }
    mixer_ctls->linein_to_l_r_output_mixer_gain_ctrl = mixer_get_ctl_by_name(mixer,
		    MIXER_AUDIO_OMIXER_BST1_CTRL_LINEING);
    if (!mixer_ctls->linein_to_l_r_output_mixer_gain_ctrl) {
	    ALOGE("Unable to find '%s' mixer control", MIXER_AUDIO_OMIXER_BST1_CTRL_LINEING);
		goto error_out;
    }

    mixer_ctls->headphone_vol_ctrl = mixer_get_ctl_by_name(mixer,
		    MIXER_AUDIO_HEADPHONE_VOL_CTRL);
    if (!mixer_ctls->headphone_vol_ctrl) {
	    ALOGE("Unable to find '%s' mixer control", MIXER_AUDIO_HEADPHONE_VOL_CTRL);
		goto error_out;
    }
    mixer_ctls->earpiece_vol_ctrl = mixer_get_ctl_by_name(mixer,
		    MIXER_AUDIO_EARPIECE_VOL_CTRL);
    if (!mixer_ctls->earpiece_vol_ctrl) {
	    ALOGE("Unable to find '%s' mixer control", MIXER_AUDIO_EARPIECE_VOL_CTRL);
		goto error_out;
    }
    mixer_ctls->speaker_vol_ctrl = mixer_get_ctl_by_name(mixer,
		    MIXER_AUDIO_SPEAKER_VOL_CTRL);
    if (!mixer_ctls->speaker_vol_ctrl) {
	    ALOGE("Unable to find '%s' mixer control", MIXER_AUDIO_SPEAKER_VOL_CTRL);
		goto error_out;
    }
    mixer_ctls->lineout_gain_ctrl = mixer_get_ctl_by_name(mixer,
		    MIXER_AUDIO_LINE_OUT_GAIN_CTRL);
    if (!mixer_ctls->lineout_gain_ctrl) {
	    ALOGE("Unable to find '%s' mixer control", MIXER_AUDIO_LINE_OUT_GAIN_CTRL);
		goto error_out;
    }

    mixer_ctls->lineout_gain_ctrl = mixer_get_ctl_by_name(mixer,
		    MIXER_AUDIO_LINE_OUT_GAIN_CTRL);
    if (!mixer_ctls->lineout_gain_ctrl) {
	    ALOGE("Unable to find '%s' mixer control", MIXER_AUDIO_LINE_OUT_GAIN_CTRL);
		goto error_out;
    }
    
    mixer_ctls->audio_phone_out = mixer_get_ctl_by_name(mixer,
		    MIXER_AUDIO_PHONE_OUT);
    if (!mixer_ctls->audio_phone_out) {
	    ALOGE("Unable to find '%s' mixer control",MIXER_AUDIO_PHONE_OUT);
	    goto error_out;
    }
    mixer_ctls->audio_phone_in = mixer_get_ctl_by_name(mixer,
		    MIXER_AUDIO_PHONE_IN);
    if (!mixer_ctls->audio_phone_in) {
	    ALOGE("Unable to find '%s' mixer control",MIXER_AUDIO_PHONE_IN);
	    goto error_out;
    }
    mixer_ctls->audio_phone_in_left = mixer_get_ctl_by_name(mixer,
		    MIXER_AUDIO_PHONE_IN_LEFT);
    if (!mixer_ctls->audio_phone_in_left) {
	    ALOGE("Unable to find '%s' mixer control",MIXER_AUDIO_PHONE_IN_LEFT);
	    goto error_out;
    }

    mixer_ctls->audio_earpiece_out = mixer_get_ctl_by_name(mixer,
		    MIXER_AUDIO_EARPIECE_OUT);
    if (!mixer_ctls->audio_earpiece_out) {
	    ALOGE("Unable to find '%s' mixer control",MIXER_AUDIO_EARPIECE_OUT);
	    goto error_out;
    }
    mixer_ctls->audio_headphone_out = mixer_get_ctl_by_name(mixer,
		    MIXER_AUDIO_HEADPHONE_OUT);
    if (!mixer_ctls->audio_headphone_out) {
	    ALOGE("Unable to find '%s' mixer control",MIXER_AUDIO_HEADPHONE_OUT);
	    goto error_out;
    }
    mixer_ctls->audio_speaker_out = mixer_get_ctl_by_name(mixer,
		    MIXER_AUDIO_SPEAKER_OUT);
    if (!mixer_ctls->audio_speaker_out) {
	    ALOGE("Unable to find '%s' mixer control",MIXER_AUDIO_SPEAKER_OUT);
	    goto error_out;
    }
    mixer_ctls->audio_speaker_out_left = mixer_get_ctl_by_name(mixer,
		    MIXER_AUDIO_SPEAKER_OUT_LEFT);
    if (!mixer_ctls->audio_speaker_out_left) {
	    ALOGE("Unable to find '%s' mixer control",MIXER_AUDIO_SPEAKER_OUT_LEFT);
	    goto error_out;
    }
//    mixer_ctls->audio_headphone_out_left = mixer_get_ctl_by_name(mixer,
//		    MIXER_AUDIO_HEADPHONE_OUT_LEFT);
//    if (!mixer_ctls->audio_headphone_out_left) {
//	    ALOGE("Unable to find '%s' mixer control",MIXER_AUDIO_HEADPHONE_OUT_LEFT);
//	    goto error_out;
//    }

//    mixer_ctls->audio_adc_phone_in = mixer_get_ctl_by_name(mixer,
//		    MIXER_AUDIO_ADC_PHONE_IN);
//    if (!mixer_ctls->audio_adc_phone_in) {
//	    ALOGE("Unable to find '%s' mixer control",MIXER_AUDIO_ADC_PHONE_IN);
//	    goto error_out;
//    }
//    mixer_ctls->audio_dac_phone_out = mixer_get_ctl_by_name(mixer,
//		    MIXER_AUDIO_DAC_PHONE_OUT);
//    if (!mixer_ctls->audio_dac_phone_out) {
//	    ALOGE("Unable to find '%s' mixer control",MIXER_AUDIO_DAC_PHONE_OUT);
//	    goto error_out;
//    }
    mixer_ctls->audio_phone_main_mic = mixer_get_ctl_by_name(mixer,
		    MIXER_AUDIO_PHONE_MAIN_MIC);
    if (!mixer_ctls->audio_phone_main_mic) {
	    ALOGE("Unable to find '%s' mixer control",MIXER_AUDIO_PHONE_MAIN_MIC);
	    goto error_out;
    }
    mixer_ctls->audio_phone_headset_mic = mixer_get_ctl_by_name(mixer,
		    MIXER_AUDIO_PHONE_HEADSET_MIC);
    if (!mixer_ctls->audio_phone_headset_mic) {
	    ALOGE("Unable to find '%s' mixer control",MIXER_AUDIO_PHONE_HEADSET_MIC);
	    goto error_out;
    }
    mixer_ctls->audio_phone_voice_record = mixer_get_ctl_by_name(mixer,
		    MIXER_AUDIO_PHONE_VOICE_RECORDER);
    if (!mixer_ctls->audio_phone_voice_record) {
	    ALOGE("Unable to find '%s' mixer control",MIXER_AUDIO_PHONE_VOICE_RECORDER);
	    goto error_out;
    }
    mixer_ctls->audio_phone_end_call = mixer_get_ctl_by_name(mixer,
		    MIXER_AUDIO_PHONE_ENDCALL);
    if (!mixer_ctls->audio_phone_end_call) {
	    ALOGE("Unable to find '%s' mixer control",MIXER_AUDIO_PHONE_ENDCALL);
	    goto error_out;
    }
    mixer_ctls->audio_linein_record = mixer_get_ctl_by_name(mixer,
		    MIXER_AUDIO_LINEIN_RECORD);
    if (!mixer_ctls->audio_linein_record) {
	    ALOGE("Unable to find '%s' mixer control",MIXER_AUDIO_LINEIN_RECORD);
	    goto error_out;
    }
    mixer_ctls->audio_linein_in = mixer_get_ctl_by_name(mixer,
		    MIXER_AUDIO_LINEIN_IN);
    if (!mixer_ctls->audio_linein_in) {
	    ALOGE("Unable to find '%s' mixer control",MIXER_AUDIO_LINEIN_IN);
	    goto error_out;
    }
   
    mixer_ctls->audio_noise_adcin_reduced = mixer_get_ctl_by_name(mixer,
		    MIXER_AUDIO_NOISE_ADCIN_REDUCED);
    if (!mixer_ctls->audio_noise_adcin_reduced) {
	    ALOGE("Unable to find '%s' mixer control",MIXER_AUDIO_NOISE_ADCIN_REDUCED);
	    goto error_out;
    }
//    mixer_ctls->audio_noise_dacphoneout_reduced = mixer_get_ctl_by_name(mixer,
//		    MIXER_AUDIO_NOISE_DACPHONEOUT_REDUCED);
//    if (!mixer_ctls->audio_noise_dacphoneout_reduced) {
//	    ALOGE("Unable to find '%s' mixer control",MIXER_AUDIO_NOISE_DACPHONEOUT_REDUCED);
//	    goto error_out;
//    }
    mixer_ctls->audio_spk_headset_switch = mixer_get_ctl_by_name(mixer,
		    MIXER_AUDIO_NORMAL_SPEAKER_HEADSET);
    if (!mixer_ctls->audio_spk_headset_switch) {
	    ALOGE("Unable to find '%s' mixer control",MIXER_AUDIO_NORMAL_SPEAKER_HEADSET);
	    goto error_out;
    }

    mixer_ctls->audio_analog_bt_mic = mixer_get_ctl_by_name(mixer,
		    MIXER_AUDIO_ANALOG_BT_MIC);
    if (!mixer_ctls->audio_analog_bt_mic) {
	    ALOGE("Unable to find '%s' mixer control",MIXER_AUDIO_ANALOG_BT_MIC);
	    goto error_out;
    }

    mixer_ctls->audio_analog_bt_phonein = mixer_get_ctl_by_name(mixer,
		    MIXER_AUDIO_ANALOG_BT_PHONEIN);
    if (!mixer_ctls->audio_analog_bt_phonein) {
	    ALOGE("Unable to find '%s' mixer control",MIXER_AUDIO_ANALOG_BT_PHONEIN);
	    goto error_out;
    }

    mixer_ctls->audio_bt_out = mixer_get_ctl_by_name(mixer,
		    MIXER_AUDIO_BT_OUT);
    if (!mixer_ctls->audio_bt_out) {
	    ALOGE("Unable to find '%s' mixer control",MIXER_AUDIO_BT_OUT);
	    goto error_out;
    }

    mixer_ctls->audio_bt_clk_fmt = mixer_get_ctl_by_name(mixer,
		    MIXER_AUDIO_BT_CLK_FMT);
    if (!mixer_ctls->audio_bt_clk_fmt) {
	    ALOGE("Unable to find '%s' mixer control",MIXER_AUDIO_BT_CLK_FMT);
	    goto error_out;
    }

    mixer_ctls->audio_digital_main_mic = mixer_get_ctl_by_name(mixer,
		    MIXER_AUDIO_DIGITAL_MAIN_MIC);
    if (!mixer_ctls->audio_digital_main_mic) {
	    ALOGE("Unable to find '%s' mixer control", MIXER_AUDIO_DIGITAL_MAIN_MIC);
	    goto error_out;
    }

    mixer_ctls->audio_digital_headset_mic = mixer_get_ctl_by_name(mixer,
		    MIXER_AUDIO_DIGITAL_HEADSET_MIC);
    if (!mixer_ctls->audio_digital_headset_mic) {
	    ALOGE("Unable to find '%s' mixer control", MIXER_AUDIO_DIGITAL_HEADSET_MIC);
	    goto error_out;
    }

    mixer_ctls->audio_digital_phone_out = mixer_get_ctl_by_name(mixer,
		    MIXER_AUDIO_DIGITAL_PHONE_OUT);
    if (!mixer_ctls->audio_digital_phone_out) {
	    ALOGE("Unable to find '%s' mixer control", MIXER_AUDIO_DIGITAL_PHONE_OUT);
	    goto error_out;
    }

    mixer_ctls->audio_digital_dac_out = mixer_get_ctl_by_name(mixer,
		    MIXER_AUDIO_DIGITAL_DAC_OUT);
    if (!mixer_ctls->audio_digital_dac_out) {
	    ALOGE("Unable to find '%s' mixer control", MIXER_AUDIO_DIGITAL_DAC_OUT);
	    goto error_out;
    }

    mixer_ctls->audio_digital_phonein = mixer_get_ctl_by_name(mixer,
		    MIXER_AUDIO_DIGITAL_PHONEIN);
    if (!mixer_ctls->audio_digital_phonein) {
	    ALOGE("Unable to find '%s' mixer control", MIXER_AUDIO_DIGITAL_PHONEIN);
	    goto error_out;
    }

    mixer_ctls->audio_digital_clk_fmt_status = mixer_get_ctl_by_name(mixer,
		    MIXER_AUDIO_DIGITAL_BB_CLK_FMT);
    if (!mixer_ctls->audio_digital_clk_fmt_status) {
	    ALOGE("Unable to find '%s' mixer control", MIXER_AUDIO_DIGITAL_BB_CLK_FMT);
	    goto error_out;
    }

	mixer_ctls->audio_bb_bt_clk_fmt = mixer_get_ctl_by_name(mixer,
		    MIXER_AUDIO_DIGITAL_BB_BT_CLK_FMT);
    if (!mixer_ctls->audio_bb_bt_clk_fmt) {
	    ALOGE("Unable to find '%s' mixer control", MIXER_AUDIO_DIGITAL_BB_BT_CLK_FMT);
	    goto error_out;
    }
    mixer_ctls->audio_digital_bt_mic = mixer_get_ctl_by_name(mixer,
		    MIXER_AUDIO_DIGITAL_BT_MIC);
    if (!mixer_ctls->audio_digital_bt_mic) {
	    ALOGE("Unable to find '%s' mixer control", MIXER_AUDIO_DIGITAL_BT_MIC);
	    goto error_out;
    }

    mixer_ctls->audio_digital_bt_phonein = mixer_get_ctl_by_name(mixer,
		    MIXER_AUDIO_DIGITAL_BT_PHONEIN);
    if (!mixer_ctls->audio_digital_bt_phonein) {
	    ALOGE("Unable to find '%s' mixer control", MIXER_AUDIO_DIGITAL_BT_PHONEIN);
	    goto error_out;
    }

    mixer_ctls->audio_i2s_pcm_sel = mixer_get_ctl_by_name(mixer,
		    MIXER_AUDIO_I2S_PCM_SEL);
    if (!mixer_ctls->audio_i2s_pcm_sel) {
	    ALOGE("Unable to find '%s' mixer control",MIXER_AUDIO_I2S_PCM_SEL);
	    goto error_out;
    }
#if 1
    mixer_ctls->audio_digital_bb_capture_mic = mixer_get_ctl_by_name(mixer,
		    MIXER_AUDIO_DIGITAL_BB_CAP_MIC);
    if (!mixer_ctls->audio_digital_bb_capture_mic) {
	    ALOGE("Unable to find '%s' mixer control",MIXER_AUDIO_DIGITAL_BB_CAP_MIC);
	    goto error_out;
    }

    mixer_ctls->audio_digital_bb_capture_bt= mixer_get_ctl_by_name(mixer,
		    MIXER_AUDIO_DIGITAL_BB_CAP_BT);
    if (!mixer_ctls->audio_digital_bb_capture_bt) {
	    ALOGE("Unable to find '%s' mixer control",MIXER_AUDIO_DIGITAL_BB_CAP_BT);
	    goto error_out;
    }

    mixer_ctls->Audio_analog_bb_capture_mainmic= mixer_get_ctl_by_name(mixer,
		    MIXER_AUDIO_ANALOG_BB_CAP_MAINMIC);
    if (!mixer_ctls->Audio_analog_bb_capture_mainmic) {
	    ALOGE("Unable to find '%s' mixer control",MIXER_AUDIO_ANALOG_BB_CAP_MAINMIC);
	    goto error_out;
    }
    mixer_ctls->audio_analog_bb_capture_headsetmic = mixer_get_ctl_by_name(mixer,
		    MIXER_AUDIO_ANALOG_BB_CAP_HEADSETMIC);
    if (!mixer_ctls->audio_analog_bb_capture_headsetmic) {
	    ALOGE("Unable to find '%s' mixer control",MIXER_AUDIO_ANALOG_BB_CAP_HEADSETMIC);
	    goto error_out;
    }
   mixer_ctls->audio_analog_bb_capture_bt = mixer_get_ctl_by_name(mixer,
		    MIXER_AUDIO_ANALOG_BB_CAP_BT);
    if (!mixer_ctls->audio_analog_bb_capture_bt) {
	    ALOGE("Unable to find '%s' mixer control",MIXER_AUDIO_ANALOG_BB_CAP_BT);
	    goto error_out;
    }
   #endif 

    return 0;

error_out:  
	ALOGE("HUANGXIN--------------------MIXER_CLOSE ERROR!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
    mixer_close(mixer);
    return -1;
}



