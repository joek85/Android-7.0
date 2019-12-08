
#include <stdlib.h>
#include <errno.h>
#include <cutils/log.h>

#include "libdispclient.h"

enum disp_tv_mode
{
	DISP_TV_MOD_480I                = 0,
	DISP_TV_MOD_576I                = 1,
	DISP_TV_MOD_480P                = 2,
	DISP_TV_MOD_576P                = 3,
	DISP_TV_MOD_720P_50HZ           = 4,
	DISP_TV_MOD_720P_60HZ           = 5,
	DISP_TV_MOD_1080I_50HZ          = 6,
	DISP_TV_MOD_1080I_60HZ          = 7,
	DISP_TV_MOD_1080P_24HZ          = 8,
	DISP_TV_MOD_1080P_50HZ          = 9,
	DISP_TV_MOD_1080P_60HZ          = 0xa,
	DISP_TV_MOD_1080P_24HZ_3D_FP    = 0x17,
	DISP_TV_MOD_720P_50HZ_3D_FP     = 0x18,
	DISP_TV_MOD_720P_60HZ_3D_FP     = 0x19,
	DISP_TV_MOD_1080P_25HZ          = 0x1a,
	DISP_TV_MOD_1080P_30HZ          = 0x1b,
	DISP_TV_MOD_PAL                 = 0xb,
	DISP_TV_MOD_PAL_SVIDEO          = 0xc,
	DISP_TV_MOD_NTSC                = 0xe,
	DISP_TV_MOD_NTSC_SVIDEO         = 0xf,
	DISP_TV_MOD_PAL_M               = 0x11,
	DISP_TV_MOD_PAL_M_SVIDEO        = 0x12,
	DISP_TV_MOD_PAL_NC              = 0x14,
	DISP_TV_MOD_PAL_NC_SVIDEO       = 0x15,
	DISP_TV_MOD_3840_2160P_30HZ     = 0x1c,
	DISP_TV_MOD_3840_2160P_25HZ     = 0x1d,
	DISP_TV_MOD_3840_2160P_24HZ     = 0x1e,
	DISP_TV_MOD_4096_2160P_24HZ     = 0x1f,
	DISP_TV_MOD_4096_2160P_25HZ     = 0x20,
	DISP_TV_MOD_4096_2160P_30HZ     = 0x21,
	DISP_TV_MOD_3840_2160P_60HZ     = 0x22,
	DISP_TV_MOD_4096_2160P_60HZ     = 0x23,
	DISP_TV_MOD_3840_2160P_50HZ     = 0x24,
	DISP_TV_MOD_4096_2160P_50HZ     = 0x25,
};

typedef struct {
	int mode;
	const char *name;
} output_mode_t;

#define __DEF_MODE(_mode) \
{                         \
	.mode = _mode,        \
	.name = #_mode,       \
}

const output_mode_t output_mode_arrary[] = {
	__DEF_MODE(DISP_TV_MOD_480I           ),
	__DEF_MODE(DISP_TV_MOD_576I           ),
	__DEF_MODE(DISP_TV_MOD_480P           ),
	__DEF_MODE(DISP_TV_MOD_576P           ),
	__DEF_MODE(DISP_TV_MOD_720P_50HZ      ),
	__DEF_MODE(DISP_TV_MOD_720P_60HZ      ),
	__DEF_MODE(DISP_TV_MOD_1080I_50HZ     ),
	__DEF_MODE(DISP_TV_MOD_1080I_60HZ     ),
	__DEF_MODE(DISP_TV_MOD_1080P_24HZ     ),
	__DEF_MODE(DISP_TV_MOD_1080P_50HZ     ),
	__DEF_MODE(DISP_TV_MOD_1080P_60HZ     ),
	__DEF_MODE(DISP_TV_MOD_3840_2160P_24HZ),
	__DEF_MODE(DISP_TV_MOD_3840_2160P_30HZ),
	__DEF_MODE(DISP_TV_MOD_3840_2160P_50HZ),
	__DEF_MODE(DISP_TV_MOD_3840_2160P_60HZ),
};

const char *getModeName(int mode) {
	int modeCnt = sizeof(output_mode_arrary) / sizeof(output_mode_arrary[0]);
	for (int i = 0; i < modeCnt; i++) {
		if (mode == output_mode_arrary[i].mode)
			return output_mode_arrary[i].name;
	}
	return "unknown";
}

#define MLEN(_x) (sizeof(_x) / sizeof(_x[0]))

int main(int argc, char **argv)
{
	if (argc < 2)
		return 0;

	displaydClient *client = new displaydClient();
	printf("current mode: %s\n", getModeName(client->getOutputMode(1)));

	if (!strcmp(argv[1], "-p")) {
		printf("Supported modes:\n");
		for (size_t i = 0; i < MLEN(output_mode_arrary); i++) {
			if (client->isSupportMode(1, output_mode_arrary[i].mode) == 1)
				printf("  %2d  %s\n", output_mode_arrary[i].mode, output_mode_arrary[i].name);
		}
	}
	else if (!strcmp(argv[1], "-s")) {
		int mode = strtoul(argv[2], 0, 0);
		int find = 0;
		for (size_t i = 0; i < MLEN(output_mode_arrary); i++) {
			if (output_mode_arrary[i].mode == mode) {
				find = 1;
				break;
			}
		}

		if (find) {
			int result = client->setOutputMode(1, mode);
			printf("Set output mode '%s' %s\n", getModeName(mode), result == 0 ? "OK" : "ERROR");
		} else {
			printf("Error: unknow mode %d\n", mode);
		}
	}

	delete client;
	return 0;
}
