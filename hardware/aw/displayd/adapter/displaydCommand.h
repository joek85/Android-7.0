
#ifndef _DISPLAYDCOMMAND_H
#define _DISPLAYDCOMMAND_H

class DisplaydCommandOpcode {
public:
	static const int Set3DMode             = 800;
	static const int SetDisplayEnhanceMode = 801;
	static const int SetOutputMode         = 802;
	static const int SetOverscan           = 803;
	static const int SetScreenRadio        = 804;

	static const int GetOutputType         = 900;
	static const int GetOutputMode         = 901;
	static const int CheckHdmiMode         = 902;
	static const int Check3DMode           = 903;
	static const int GetVerticalOverscan   = 904;
	static const int GetHorizontalOverscan = 905;
	static const int GetDisplayFPS         = 906;
};
#endif
