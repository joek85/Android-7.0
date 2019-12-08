
#define LOG_TAG "audio_raw"
// #define LOG_NDEBUG 0

#include <stdlib.h>
#include<fcntl.h>
#include<sys/stat.h>

#include <cutils/log.h>
#include <cutils/properties.h> // for property_get

#include "audio_raw.h"

#define UNUSED(x) ((void)(x))

int read_node_value(const char *path, char * str_val)
{
  int ret = -1;
  int fd = open(path, O_RDONLY);
  if (fd >= 0)
  {
    read(fd, str_val, sizeof(str_val));
    close(fd);
	ret = 0;
  }
  return ret;
}

void detectRaw(const void * audio_buf, RAW_INFO_t * raw_info)
{
	UNUSED(audio_buf);
	raw_info->raw_mode = RAW_PCM;
	raw_info->sample_rate = 44100;

    char str_val[4];
	memset(str_val, 0, sizeof(str_val));
	if (read_node_value(NODE_AUDIO_RAW, str_val) == 0)
	{
		raw_info->raw_mode = (RAW_MODE_t)atoi(str_val);
		raw_info->sample_rate = 44100;
		ALOGV("read_node_value %s ok, read: %d", NODE_AUDIO_RAW, raw_info->raw_mode);
	}
	else
	{
		//ALOGV("read_node_value %s failed: %s", NODE_AUDIO_RAW, strerror(errno));
	}
}

