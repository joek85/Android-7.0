#ifndef _SUN50IW6P1_H_
#define _SUN50IW6P1_H_

#include "platform.h"
#include "sunxi_display2.h"

const static struct display_mode_info _display_modes[] = {

//	{"480i-60Hz" , DISP_TV_MOD_480I            },
//	{"576i-50Hz" , DISP_TV_MOD_576I            },
	{"480p-60Hz" , DISP_TV_MOD_480P            },
	{"576p-50Hz" , DISP_TV_MOD_576P            },
	{"720p-50Hz" , DISP_TV_MOD_720P_50HZ       },
	{"720p-60Hz" , DISP_TV_MOD_720P_60HZ       },

	{"1080p-24Hz", DISP_TV_MOD_1080P_24HZ      },
	{"1080p-50Hz", DISP_TV_MOD_1080P_50HZ      },
	{"1080p-60Hz", DISP_TV_MOD_1080P_60HZ      },
	{"1080i-50Hz", DISP_TV_MOD_1080I_50HZ      },
	{"1080i-60Hz", DISP_TV_MOD_1080I_60HZ      },

	{"4K-24",      DISP_TV_MOD_3840_2160P_24HZ },
	{"4K-25",      DISP_TV_MOD_3840_2160P_25HZ },
	{"4K-30",      DISP_TV_MOD_3840_2160P_30HZ },
	{"4K-60",      DISP_TV_MOD_3840_2160P_60HZ },

	/* The last two modes are just for cvbs,
	 * They must store in the end of this array */
	{"NTSC"      , DISP_TV_MOD_NTSC            },
	{"PAL"       , DISP_TV_MOD_PAL             },
};

#endif
