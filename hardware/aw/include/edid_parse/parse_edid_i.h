#ifndef _PARSE_EDID_I_H_
#define _PARSE_EDID_I_H_

#include <cutils/log.h>

#define HDMI1440_480I       6
#define HDMI1440_576I       21
#define HDMI480P            2
#define HDMI576P            17
#define HDMI720P_50         19
#define HDMI720P_60         4
#define HDMI1080I_50        20
#define HDMI1080I_60        5
#define HDMI1080P_50        31
#define HDMI1080P_60        16
#define HDMI1080P_24        32
#define HDMI1080P_25        33
#define HDMI1080P_30        34
#define HDMI1080P_24_3D_FP  (HDMI1080P_24 + 0x80)
#define HDMI720P_50_3D_FP   (HDMI720P_50  + 0x80)
#define HDMI720P_60_3D_FP   (HDMI720P_60  + 0x80)
#define HDMI3840_2160P_30   95
#define HDMI3840_2160P_25   94
#define HDMI3840_2160P_24   93
#define HDMI4096_2160P_24   98
#define HDMI3840_2160P_50   96
#define HDMI3840_2160P_60   97
#define HDMI4096_2160P_60   107

#define HDMI3840_2160P_30_BIT_MASK   1
#define HDMI3840_2160P_25_BIT_MASK   2
#define HDMI3840_2160P_24_BIT_MASK   3
#define HDMI3840_2160P_24_SMPTE_BIT_MASK   4

typedef enum
{
    SUNXI_TV_MOD_480I                = 0,
    SUNXI_TV_MOD_576I                = 1,
    SUNXI_TV_MOD_480P                = 2,
    SUNXI_TV_MOD_576P                = 3,
    SUNXI_TV_MOD_720P_50HZ           = 4,
    SUNXI_TV_MOD_720P_60HZ           = 5,
    SUNXI_TV_MOD_1080I_50HZ          = 6,
    SUNXI_TV_MOD_1080I_60HZ          = 7,
    SUNXI_TV_MOD_1080P_24HZ          = 8,
    SUNXI_TV_MOD_1080P_50HZ          = 9,
    SUNXI_TV_MOD_1080P_60HZ          = 0xa,
    SUNXI_TV_MOD_1080P_24HZ_3D_FP    = 0x17,
    SUNXI_TV_MOD_720P_50HZ_3D_FP     = 0x18,
    SUNXI_TV_MOD_720P_60HZ_3D_FP     = 0x19,
    SUNXI_TV_MOD_1080P_25HZ          = 0x1a,
    SUNXI_TV_MOD_1080P_30HZ          = 0x1b,
    SUNXI_TV_MOD_PAL                 = 0xb,
    SUNXI_TV_MOD_PAL_SVIDEO          = 0xc,
    SUNXI_TV_MOD_NTSC                = 0xe,
    SUNXI_TV_MOD_NTSC_SVIDEO         = 0xf,
    SUNXI_TV_MOD_PAL_M               = 0x11,
    SUNXI_TV_MOD_PAL_M_SVIDEO        = 0x12,
    SUNXI_TV_MOD_PAL_NC              = 0x14,
    SUNXI_TV_MOD_PAL_NC_SVIDEO       = 0x15,
    SUNXI_TV_MOD_3840_2160P_30HZ     = 0x1c,
    SUNXI_TV_MOD_3840_2160P_25HZ     = 0x1d,
    SUNXI_TV_MOD_3840_2160P_24HZ     = 0x1e,
    SUNXI_TV_MODE_NUM                = 0x1f,
}sunxi_tv_mode; // definition in sunxi_disp_driver, as same as disp_tv_mode

typedef struct vendor_info {
    char manufacturer[4];
    unsigned int model;
    unsigned int SN;
    unsigned int week;
    unsigned int year;
    char datas[10];
}vendor_info_t;

typedef struct hdmi_mode_info {
    unsigned long long common_mode; //include native mode
    unsigned long long standard_mode;
    unsigned int native_mode;
    unsigned int hd_2160P_mode;
    unsigned int stereo_present;
}hdmi_mode_info_t;

typedef struct cec_info {
    unsigned int phy_addr;
}cec_info_t;

enum {
    EDID_GET_FINISH = 0,
    EDID_GET_VERDOR_INFO, //see vendor_info_t
    EDID_GET_HDMI_MODE, //see hdmi_mode_info_t
    EDID_GET_CEC_INFO, //see cec_info_t

    EDID_GET_CMD_NUM,
};

typedef struct edid_info {
    int cmd;
    union {
        int length;
        int ret;
    };
    void *data;
}edid_info_t;


/* Electro-Optical Transfer Function */
typedef enum {
    SDR_LUMINANCE_RANGE = (1 << 0),
    HDR_LUMINANCE_RANGE = (1 << 1),
    SMPTE_ST_2048       = (1 << 2),
    FUTURE_EOTF         = (1 << 3),
} eotf_t;

/* colorimetry */
typedef enum {
    STANDARD_XVYCC601     = 1 << 0,
    STANDARD_XVYCC709     = 1 << 1,
    STANDARD_SYCC601      = 1 << 2,
    STANDARD_ADOBE_YCC601 = 1 << 3,
    STANDARD_ADOBE_RGB    = 1 << 4,
    STANDARD_BT2020_CYCC  = 1 << 5,
    STANDARD_BT2020_YCC   = 1 << 6,
    STANDARD_BT2020_RGB   = 1 << 7,
} colorimetry_t;

/* video formats */
enum video_fmt_type {
    FMT_RGB888 = 0,
    FMT_YUV444 = 1,
    FMT_YUV422 = 2,
    FMT_YUV420 = 3,
    FMT_MAX
};

typedef enum {
    VIDEO_FORMAT_RGB      = 1 << FMT_RGB888,
    VIDEO_FORMAT_YCBCR444 = 1 << FMT_YUV444,
    VIDEO_FORMAT_YCBCR422 = 1 << FMT_YUV422,
    VIDEO_FORMAT_YCBCR420 = 1 << FMT_YUV420,
} video_format_t;

/* pixel encoding */
enum color_depth_t {
    COLOR_DEPTH_8BIT  = (1 << 0),
    COLOR_DEPTH_10BIT = (1 << 1),
    COLOR_DEPTH_12BIT = (1 << 2),
    COLOR_DEPTH_16BIT = (1 << 3),
};

typedef enum {
    PIXEL_ENCODING_YUV420_8BIT  = (COLOR_DEPTH_8BIT  << 0),
    PIXEL_ENCODING_YUV420_10BIT = (COLOR_DEPTH_10BIT << 0),
    PIXEL_ENCODING_YUV420_12BIT = (COLOR_DEPTH_12BIT << 0),
    PIXEL_ENCODING_YUV420_16BIT = (COLOR_DEPTH_16BIT << 0),
} pixel_encoding_t;

typedef struct hdr_capability {
    int eotf;
    int metadata_descriptor;
    int max_luminance;
    int max_frame_average_luminance;
    int min_luminance;
} hdr_capability_t;

typedef struct hdmi_forum_vsdb {
    int ieee_oui;
    int version;
    int max_tmds_rate;
    int reserved;
    int deep_color_420_encoding;
} hdmi_forum_vsdb_t;

struct video_format {
    int supported;
    int bits;
};

struct vic_info {
    unsigned char vic;
    unsigned char sunxi_disp_mode;

    /*
     * for ycbcr420 vic
     * to detect RGB/Y444/Y422 sampling mode support
     */
    unsigned char support_regular_sampling_mode;
};

typedef struct sunxi_hdmi_capability {
    hdr_capability_t capability;
    int colorimetry;
    int video_format;
    int pixel_encoding;
    int ycbcr420_resolution[8];
    int max_tmds_rate;

    struct video_format supported_format[4];
    struct vic_info ycbcr420_vic[8];
    int total_ycbcr420_vic;
    int all_svd_support_ycbcr420;

    int rgb_only;
    int dvihdmi;  /* 1 - dvi, 2 - hdmi */
} sunxi_hdmi_capability_t;

#endif
