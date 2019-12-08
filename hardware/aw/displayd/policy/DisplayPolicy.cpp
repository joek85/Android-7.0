
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/wait.h>
#include <cutils/properties.h>
#include <cutils/log.h>

#include "DisplayPolicy.h"
#include "ResponseCode.h"
#include <hardware/hwcomposer.h>
#include <sysutils/SocketListener.h>
#include <sysutils/NetlinkListener.h>
#include "DisplayManager.h"
#include "DispOutputState.h"

#define MAIN_DISPLAY 0
#define AUX_DISPLAY  1
#define HDMI_HPD_STATE_FILENAME "/sys/class/switch/hdmi/state"
#define CVBS_HPD_STATE_FILENAME "/sys/class/switch/cvbs/state"

DisplayPolicy * DisplayPolicy::instance = NULL;
void DisplayPolicy::hotplugCallbakcEntry(const char *name, bool connect) {
	if (instance)
		instance->notifyDispDevicePlugChange(name, connect);
}

DisplayPolicy::DisplayPolicy(DisplayManager *dm)
:mDisplayManager(*dm) {
	mPolicy = 2; // fixme: for debug
	getDispDevices();
	initOutputState();
	instance = this;
}

void DisplayPolicy::dispDevicePlugChangeLocked(DispDevice &dispDevice, bool isPlugIn)
{
	int type = dispDevice.type;
	int priority = dispDevice.priority;

	if (!isPlugIn) {
		if ((0 == priority) && (1 < mDispDevices.size())) {
			/*
			 * fixme: should getDispDeviceByPriority, not by index
			 */
			type = mDispDevices[1].type;
		}
	}
	mDispState->devicePlugChanged(type, priority, isPlugIn);
}

void DisplayPolicy::notifyDispDevicePlugChange(int type, bool isPlugIn)
{
	if (0 == type)
		return;

	DispDevice dispDevice = getDispDeviceByType(type);

	if (type != dispDevice.type)
		return;

	if (!dispDevice.hotplugSupport)
		return;

	{
		Mutex::Autolock _l(mMutex);
		if ((0 != dispDevice.revertPlugStateType) && isPlugIn) {
			DispDevice dispDevice1 = getDispDeviceByType(dispDevice.revertPlugStateType);
			dispDevicePlugChangeLocked(dispDevice1, false);
		}
		dispDevicePlugChangeLocked(dispDevice, isPlugIn);
		if (0 != dispDevice.revertPlugStateType && !isPlugIn) {
			DispDevice dispDevice1 = getDispDeviceByType(dispDevice.revertPlugStateType);
			dispDevicePlugChangeLocked(dispDevice1, true);
		}
	}
	return;
}

void DisplayPolicy::notifyDispDevicePlugChange(const char *name, bool isPlugIn)
{
	struct _deviceIdentify {
		const char *identify;
		int type;
	};
	const _deviceIdentify __maps[] =  {
		{"hdmi", 4},
		{"cvbs", 2},
		{NULL,  -1},
	};
	int i = 0;
	int find = 0;
	while (__maps[i].identify != NULL) {
		if (!strcmp(__maps[i].identify, name)) {
			find = 1;
			break;
		}
		i++;
	}
	if (find)
		notifyDispDevicePlugChange(__maps[i].type, isPlugIn);
	return;
}

int DisplayPolicy::getDispOutputType(int display)
{
	int interface = 0;
	mDisplayManager.getCurrentInterface(display, &interface);
	return interface;
}

int DisplayPolicy::setDisplayOutput(int display, int type, int mode)
{
	return mDisplayManager.setCurrentMode_FORCE(display, type, mode);
}

bool DisplayPolicy::getHotplugSupport(int displayType)
{
	int hotplugSupport = 0;
	char property[PROPERTY_VALUE_MAX] = {0};
	char *propName = NULL;

	switch(displayType) {
	case DISP_OUTPUT_TYPE_HDMI:
		propName = (char *)"persist.sys.hdmi_hpd";
		break;
	case DISP_OUTPUT_TYPE_TV:
		propName = (char *)"persist.sys.cvbs_hpd";
		break;
	case DISP_OUTPUT_TYPE_VGA:
		propName = (char *)"persist.sys.vga_hpd";
		break;
	case DISP_OUTPUT_TYPE_LCD:
	default:
		return false;
	}

	if (0 >= property_get(propName, property, NULL)) {
		ALOGD("has no propName=%s", propName);
		return false;
	}

	hotplugSupport = atoi(property);
	return (1 == hotplugSupport);
}

int DisplayPolicy::getRevertHotplugType(int displayType)
{
	char property[PROPERTY_VALUE_MAX] = {0};
	char *propName = NULL;

	switch(displayType) {
	case DISP_OUTPUT_TYPE_HDMI:
		propName = (char *)"persist.sys.hdmi_rvthpd";
		break;
	case DISP_OUTPUT_TYPE_TV:
		propName = (char *)"persist.sys.cvbs_rvthpd";
		break;
	case DISP_OUTPUT_TYPE_VGA:
		propName = (char *)"persist.sys.vga_rvthpd";
		break;
	case DISP_OUTPUT_TYPE_LCD:
	default:
		return 0;
	}

	if (0 >= property_get(propName, property, NULL)) {
		ALOGD("has no propName=%s", propName);
		return 0;
	}

	return atoi(property);
}

void DisplayPolicy::getDispDevices()
{
	int i = 0;
	char property[PROPERTY_VALUE_MAX] = {0};
	char propName[32] = {0};
	DispDevice dispDevice;

	for(i = 0; i < 8; i++) {
		sprintf(propName, "persist.sys.disp_dev%d", i);
		if (0 >= property_get(propName, property, NULL)) {
			ALOGD("has no propName=%s", propName);
			break;
		}

		dispDevice.priority = i;
		sscanf(property, "%x", &dispDevice.type);
		dispDevice.type = (dispDevice.type >> 8) & 0xFF;
		dispDevice.hotplugSupport = getHotplugSupport(dispDevice.type);
		dispDevice.revertPlugStateType = getRevertHotplugType(dispDevice.type);
		ALOGD("device: type=%d, hotplugsuport=%d, revertplug=%d",
			dispDevice.type, dispDevice.hotplugSupport, dispDevice.revertPlugStateType);
		mDispDevices.push_back(dispDevice);
	}
	return;
}

DisplayPolicy::DispDevice &DisplayPolicy::getDispDeviceByType(int dispType) {
	vector<DispDevice>::size_type index = 0;
	for (index = 0; index < mDispDevices.size(); index++) {
		if (dispType == mDispDevices[index].type) {
			return mDispDevices[index];
		}
	}
	ALOGE("getDispDeviceByType[%d] failed!", dispType);
	return mDispDevices[0];
}

int DisplayPolicy::getStringsFromFile(char const *fileName, char *values, unsigned int size)
{
	FILE *fp;
	unsigned int i = 0;

	if (size <= 0)
		return -1;

	if (NULL == (fp = fopen(fileName, "r")))
		return -1;

	while (!feof(fp) && (i < size - 1)) {
		values[i] = fgetc(fp);
		i++;
	}
	values[i] = '\0';
	fclose(fp);
	return i;
}

static bool isPlugInState(const char *data)
{
    if (strstr(data, "1"))
        return true;
    return false;
}

bool DisplayPolicy::getPlugState(int dispType)
{
	char valueString[32] = {0};
	char *fileName = NULL;

	switch(dispType) {
	case DISP_OUTPUT_TYPE_HDMI:
		fileName = (char *)HDMI_HPD_STATE_FILENAME;
		break;
	case DISP_OUTPUT_TYPE_TV:
		fileName = (char *)CVBS_HPD_STATE_FILENAME;
		break;
	case DISP_OUTPUT_TYPE_VGA:
	default:
		ALOGD("no plug for dispType[%d]", dispType);
		return false;
	}

	memset(valueString, 0, 32);
	if ((getStringsFromFile(fileName, valueString, 32) > 0)
		&& isPlugInState(valueString))
		return true;
	return false;
}

int DisplayPolicy::initOutputState()
{
	DispDevice dispDevice;
	int dispType;
	int currentDispNum = 0;
	int mainPriority = 0;
	mMain2Dev0In = new MainDispToDev0PlugIn(this);
	mMain2Dev0InExt = new MainDispToDev0PlugInExt(this);
	mMain2Dev0Out = new MainDispToDev0PlugOut(this);
	mMain2Dev1In = new MainDispToDev1PlugIn(this);
	mMain2Dev1Out = new MainDispToDev1PlugOut(this);
	mDualDisplay = new DualDisplay(this);

	dispType = getDispOutputType(MAIN_DISPLAY);
	dispDevice = getDispDeviceByType(dispType);
	if(dispType == dispDevice.type) {
		currentDispNum++;
		mainPriority = dispDevice.priority;
		ALOGD("MAIN_DISPLAY: priority=%d, type=%d",
			dispDevice.priority, dispDevice.type);
	} else {
		ALOGE("MAIN_DISPLAY: initOutputState of type[%d]failed, fixme!!!", dispType);
	}

	dispType = getDispOutputType(AUX_DISPLAY);
	dispDevice = getDispDeviceByType(dispType);
	if(dispType == dispDevice.type) {
		currentDispNum++;
		ALOGD("AUX_DISPLAY: priority=%d, type=%d",
			dispDevice.priority, dispDevice.type);
	} else {
		ALOGD("AUX_DISPLAY: not exist the type[%d]", dispType);
	}
#if 0
	if (mainPriority == 0) {
		mDispState = mMain2Dev0In;
		ALOGD("main display assign to display 0");
	}
	else if (mainPriority == 1) {
		mDispState = mMain2Dev1In;
		ALOGD("main display assign to display 1");
	}

#else
	ALOGD("currentDispNum=%d, mainPriority=%d", currentDispNum, mainPriority);
	if(1 == currentDispNum) {
		if(1 == mainPriority) {
			mDispState = mMain2Dev1In; //always plugin
		} else {
			mDispState = mMain2Dev0In; //always plugin
			ALOGE("tiger a hdmi mode check!\n");
			setDisplayOutput(0, DISP_OUTPUT_TYPE_HDMI, 0xff);
		}
	} else if (2 == currentDispNum){
		mDispState = mDualDisplay; //always pulgin
		ALOGE("tiger a hdmi mode check!\n");
		setDisplayOutput(0, DISP_OUTPUT_TYPE_HDMI, 0xff);
	} else {
		mDispState = mMain2Dev0In; //always plugin
		ALOGE("%s:%d: badly error happen? FIXME!!", __func__, __LINE__);
	}
#endif

	for(unsigned int i = 0; i < mDispDevices.size(); i++) {
		bool isPlugIn  = getPlugState(mDispDevices[i].type);

		ALOGD("display.%d type %d, plug %s", i, mDispDevices[i].type, isPlugIn ? "in" : "out");
		notifyDispDevicePlugChange(mDispDevices[i].type, isPlugIn);
	}
	return 0;
}

DispOutputState *DisplayPolicy::getMainDispToDev0PlugIn()
{
	return mMain2Dev0In;
}
DispOutputState *DisplayPolicy::getMainDispToDev0PlugInExt()
{
	return mMain2Dev0InExt;
}
DispOutputState *DisplayPolicy::getMainDispToDev0PlugOut()
{
	return mMain2Dev0Out;
}
DispOutputState *DisplayPolicy::getMainDispToDev1PlugIn()
{
	return mMain2Dev1In;
}
DispOutputState *DisplayPolicy::getMainDispToDev1PlugOut()
{
	return mMain2Dev1Out;
}
DispOutputState *DisplayPolicy::getDualDisplayOutput()
{
	return mDualDisplay;
}
void DisplayPolicy::setOutputState(DispOutputState *state)
{
	mDispState = state;
}

