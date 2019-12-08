#ifndef _PARSE_EDID_H_
#define _PARSE_EDID_H_


#include "edid_parse/parse_edid_i.h"

int check_edid_block0(char * edid,int const length);
int check_extension_block(char const * edid,int const length);
int get_vendor_info(char const *vendor, vendor_info_t *vendor_info);
int get_hdmi_mode(char * edid,int extension_blocks,hdmi_mode_info_t * info);
int get_cec_info(char * edid,int extension_blocks,cec_info_t * info);

int get_edid_info(char *edid_buf, int const length, edid_info_t *info);
int get_hdmi_capability_from_edid(char *edid, int length, sunxi_hdmi_capability_t *cap);

#endif
