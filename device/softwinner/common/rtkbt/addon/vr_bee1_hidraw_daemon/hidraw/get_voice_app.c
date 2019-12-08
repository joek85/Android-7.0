#include <stdio.h>
#include <stdlib.h>
#include "string.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <cutils/log.h>
#include "get_voice_app.h"
#include "msbc.h"


#define DECODE_SIZE  58
#define VOICE_MTU                   60
#define VOICDE_DECODE_UNIT          (VOICE_MTU*4)


static uint8* _pVoiceBuf = NULL;
static uint8* _pdecodeBuf = NULL;
static uint8* _pdecodeFrameBuf = NULL;
int voice_fd = -1;
int voice_fd_origin = -1;
int voice_fd_wav = -1;
sbc_t t_sbc;
static int file_index = 0;

struct wavfile
{
    char    id[4];          // should always contain "RIFF"
    int     totallength;    // total file length minus 8
    char    wavefmt[8];     // should be "WAVEfmt "
    int     format;         // 16 for PCM format
    short   pcm;            // 1 for PCM format
    short   channels;       // channels
    int     frequency;      // sampling frequency, 16000 in this case
    int     bytes_per_second;
    short   bytes_by_capture;
    short   bits_per_sample;
    char    data[4];        // should always contain "data"
    int     bytes_in_data;
};

void write_wav_header(char* name, int samples, int channels){
    struct wavfile filler;
    FILE *pcm_fd;
	FILE *wav_fd;
	char record_name[50] = {0};
	int size;
	char ch;

	ALOGE("Start to convert pcm to wav\n");
    strncpy(filler.id, "RIFF", 4);
//    filler.totallength = (samples * channels) + sizeof(struct wavfile) - 8; //81956
    strncpy(filler.wavefmt, "WAVEfmt ", 8);
    filler.format = 16;
    filler.pcm = 1;
    filler.channels = channels;
    filler.frequency = 16000;
    filler.bits_per_sample = 16;
    filler.bytes_per_second = filler.channels * filler.frequency * filler.bits_per_sample/8;
    filler.bytes_by_capture = filler.channels*filler.bits_per_sample/8;
//    filler.bytes_in_data = samples * filler.channels * filler.bits_per_sample/8;    
    strncpy(filler.data, "data", 4);

	pcm_fd = fopen(name, "rb");
	fseek(pcm_fd, 0, SEEK_END);
	size = ftell(pcm_fd);
	fseek(pcm_fd, 0, SEEK_SET);
	
	filler.bytes_in_data = size;
	filler.totallength = size + sizeof(filler) - 8;
#if 0	// just print data
	printf("File description header: %s\n", filler.id);
	printf("Size of file: %d\n", filler.totallength);
	printf("WAV Format description header: %s\n", filler.wavefmt);
	printf("Size of WAVE section chunck: %d\n", filler.format);
	printf("WAVE type format: %d\n", filler.pcm);
	printf("Number of channels: %d\n", filler.channels);
	printf("Samples per second: %d\n", filler.frequency);
	printf("Bytes per second: %d\n", filler.bytes_per_second);
	printf("Block alignment: %d\n", filler.bytes_by_capture);
	printf("Bits per sample: %d\n", filler.bits_per_sample);
	printf("Data description header: %s\n", filler.data);
	printf("Size of data: %d\n", filler.bytes_in_data);
#endif


    sprintf(record_name,"voice_msbc_wav%d.wav",file_index);

	// store wav header
    wav_fd = fopen(record_name, "wb");
    fwrite(&filler, 1, sizeof(filler), wav_fd);
	fclose(wav_fd);

	// store voice data
	wav_fd = fopen(record_name, "ab");
    while(!feof(pcm_fd)) { 
        fread(&ch, 1, 1, pcm_fd);
        
        if(!feof(pcm_fd)) { 
            fwrite(&ch, 1, 1, wav_fd);
        }
    } 

    fclose(pcm_fd);
	fclose(wav_fd);
}

void RS_init_sbc()
{
    //msbc
    sbc_init (&t_sbc, 0L);  
}

void RS_voice_app_create_origin_output()
{
    char record_name[50] = {0};

    sprintf(record_name,"/data/vr/voice_msbc_origin%d",file_index);
 
    voice_fd_origin =  open (record_name, O_WRONLY | O_CREAT |O_TRUNC, 0644);

    
    if(voice_fd_origin < 0)
    {
        ALOGE("can't open record origin  file:%s ",record_name);
    }

	RS_init_sbc();
	
	return;
}

void RS_voice_app_create_output()
{
    char record_name[50] = {0}; 
    sprintf(record_name,"/data/vr/voice_msbc%d",file_index);

    voice_fd =  open (record_name, O_WRONLY | O_CREAT |O_TRUNC, 0644);

    if(voice_fd < 0)
    {
        ALOGE("can't open record file:%s ",record_name);
    }


	return;
}

int RS_stop_voice_stream_data (void)
{
	char record_name[50] = {0};
	
    ALOGE ("in\r\n");
	
    if (voice_fd > 0 && -1 == close (voice_fd)  )
    {
        ALOGE ("close voice file fail\r\n");
    }

    if (voice_fd_origin > 0 && -1 == close (voice_fd_origin)  )
    {
        ALOGE ("close voice file fail\r\n");
    }

    voice_fd = 0;
    voice_fd_origin = 0;
 
    sprintf(record_name,"voice_msbc%d",file_index);

	//write_wav_header(record_name, 16000, 1);

    sbc_finish (&t_sbc);

	file_index = ((++file_index)>= 10) ? 0 : file_index;
	
    return 0;
}

int RS_write_decode_buf(uint8* pu_decode, ssize_t len)
{
        if (voice_fd > 0)
        {
            write (voice_fd, pu_decode, len);
            return 0;
        }
        return -1;
}

int RS_deal_voice_stream_data(const uint8* input, size_t voice_buf_size, unsigned char* output, size_t output_buf_size, int* frame_len)
{
    int framelen;

//    RS_init_sbc();

    

    if (NULL == input || 0 == voice_buf_size)
    {
        ALOGE ("input is null \r\n");
        return -1;
    }


    if (NULL == output || 0 == output_buf_size)
    {
        ALOGE ("output is null");
        return -1;
    }

    if(voice_buf_size != VOICE_MTU) {
        ALOGE ("input size error: %d", voice_buf_size);
        return -1;
    }


    if (input[0] == 0x01 && input[2] == 0xad)
    { 
        write(voice_fd_origin, input, voice_buf_size);
        framelen = sbc_decode (&t_sbc, &input[2], DECODE_SIZE, output, output_buf_size,frame_len);
        if (framelen <= 0)
        {
            ALOGE("fail to decode");
            return -1;
        }
        RS_write_decode_buf(output, *frame_len);
        return 0;
    }else {
        ALOGE("msbc format error !!!");
        return -1;
    }
}
    

