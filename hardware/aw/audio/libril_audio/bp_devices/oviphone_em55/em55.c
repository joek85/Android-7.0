
#define LOG_TAG "audio_ril_em55"
#define LOG_NDEBUG 0

#include <stdlib.h>
#include <string.h>
#include <utils/Log.h>

#include <cutils/properties.h>

#include "hal_bp.h"

#define AT_PATH "/dev/mux2"

#define UNUSED(x) ((void)(x))

static char tty_dev[32] = AT_PATH;

static int em55_get_tty_dev(char *name)
{
   UNUSED(name);
   ALOGD("em55_get_tty_dev\n");
   return 0;
}

static int em55_set_call_volume(ril_audio_path_type_t path, int vol)
{
   UNUSED(path);
   UNUSED(vol);
   ALOGD("em55_set_call_volume\n");
   return 0;
}

static int em55_set_call_path(ril_audio_path_type_t path)
{
   UNUSED(path);
   ALOGD("em55_set_call_path\n");
   return 0;
}

static int em55_set_call_at(char *at)
{
    exec_at(tty_dev,at);

   ALOGD("em55_set_call_at\n");
   return 0;
}

struct bp_ops em55_ops = {
    .get_tty_dev = em55_get_tty_dev,
    .set_call_volume= em55_set_call_volume,
    .set_call_path = em55_set_call_path,
    .set_call_at = em55_set_call_at,
};



