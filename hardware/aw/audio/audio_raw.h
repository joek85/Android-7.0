#ifndef __AUDIO_RAW__
#define __AUDIO_RAW__

#define SUPPORT_AUDIO_RAW
#define NODE_AUDIO_RAW		"/data/audio/audio_raw"

typedef enum RAW_MODE {
	RAW_UNKNOWN,
	RAW_PCM		= 1,	// pcm data
	RAW_AC3		= 2,
	RAW_MPEG1	= 3,
	RAW_MP3		= 4,
	RAW_MPEG2	= 5,
	RAW_AAC		= 6,
	RAW_DTS		= 7,
	RAW_ATRAC	= 8,
	RAW_ONE_BIT_AUDIO = 9,
	RAW_DOLBY_DIGITAL_PLUS = 10,
	RAW_DTS_HD	= 11,
	RAW_MAT		= 12,
	RAW_DST		= 13,
	RAW_WMAPRO	= 14
}RAW_MODE_t;

typedef struct RAW_INFO {
	RAW_MODE_t 		raw_mode;
	int				sample_rate;
}RAW_INFO_t;

void detectRaw(const void * audio_buf, RAW_INFO_t * raw_info);
int read_node_value(const char *path, char * str_val);

#endif // __AUDIO_RAW__