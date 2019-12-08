#ifndef _PARSE_EDID_SUNXI_H_
#define _PARSE_EDID_SUNXI_H_

#include "edid_parse/parse_edid_i.h"

//########################### --> adapte level for AW display  ###########################//

int initParseEdid(int hwDisp);
/*
*  resetParseEdid4Disp:
*  check vendor info first, reset all variables if the vendor is changed.
*  it should be called when hdmi_hpd is 1.
*/
int resetParseEdidForDisp(int hwDisp);
int isHdmiModeSupport(int hwDisp, int mode);
int getBestHdmiMode(int hwDisp);
int isHdmi3DSupport(int hwDisp);
int getVendorInfo(int hwDisp, vendor_info_t *vendor);
int getCecInfo(int hwDisp, cec_info_t *cec);

//########################### adapte level for AW display <-- ###########################//

int getHdmiCapability(sunxi_hdmi_capability_t *cap);

#endif //#ifndef _PARSE_EDID_SUNXI_H_
