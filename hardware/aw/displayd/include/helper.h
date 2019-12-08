#ifndef _HELPER_H
#define _HELPER_H

#include <vector>
#include "sunxi_display2.h"

int stringToMode(char *mode);
const char *modeToString(int mode);

int stringToInterface(char *interface);
const char *interfaceToString(int interface);

void getHdmiModes(std::vector<int>* modes);
void getCvbsModes(std::vector<int>* modes);
#endif
