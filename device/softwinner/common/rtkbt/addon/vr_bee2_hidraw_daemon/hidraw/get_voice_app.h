
//#define  MSBC_SUPPORT
#define  VOICE_KEY_DOWN 0x3e
#define  VOICE_BUFFER_REPORT_ID    0x5a
#define  ADD_FOR_VOICE_CONTROL
#define  VOICE_FILE_WITH_MSBC           "/tmp/voice_msbc"
#define  VOICE_FILE_WITH_NO_MSBC    "/tmp/voice_no_msbc"

typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int uint32;
typedef signed char int8;
typedef signed short int16;
typedef signed int int32;

void RS_voice_app_create_origin_output();

int RS_deal_voice_stream_data(const uint8* input, size_t voice_buf_size, unsigned char* output, size_t output_buf_size, int* frame_len);

int RS_stop_voice_stream_data (void);

void RS_voice_app_create_output();