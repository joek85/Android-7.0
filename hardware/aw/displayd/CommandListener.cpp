/*
 * Copyright (C) 2008 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <dirent.h>
#include <errno.h>
#include <string>

#include <linux/if.h>
#include <cutils/log.h>
#include <sysutils/SocketClient.h>

#include "CommandListener.h"
#include "ResponseCode.h"
#include "commands.h"
#include "debug.h"

/*
 * Convert idx from framework to the local define value.
 *
 * Type define in android.view.display:
 *  - TYPE_UNKNOWN  = 0
 *  - TYPE_BUILT_IN = 1 (primary display)
 *
 * idx used in displayd/hwc:
 *  - primary display = 0
 *  - aux display     = 1
 */
static int toDisplayIdx(int framework_display_type) {
	int idx = 0;
	switch (framework_display_type) {
	case 0:
	case 1:
		idx = 0;
		break;
	default:
		idx = 1;
		break;
    }
	return idx;
}

DisplaydCommand::DisplaydCommand(const char *cmd)
	:FrameworkCommand(cmd) {
}

int
DisplaydCommand::toCommandCode(char *name) {
	auto iter = mCommandMaps.find(name);
	if (iter == mCommandMaps.end())
		return -1;
	return iter->second;
}

#define CHECK_ARGC(__client, __argc, __limit)               \
if (__argc < __limit) {                                     \
	__client->sendMsg(ResponseCode::CommandSyntaxError,     \
								"Missing argument", false); \
	return 0;                                               \
}

#define UNKNOW_COMMAND(__client)                            \
__client->sendMsg(ResponseCode::CommandSyntaxError,         \
				 				"Unkown command", false)

CommandListener::CommandListener(DisplayManager *dm)
	:FrameworkListener("displayd", true) {
	registerCmd(new InterfaceCmd(dm));
	registerCmd(new OverscanCmd(dm));
	registerCmd(new UtilsCmd(dm));
	registerCmd(new EnhanceCmd(dm));
	mDisplayManager = dm;
}

CommandListener::InterfaceCmd::InterfaceCmd(DisplayManager *dm)
	:DisplaydCommand("interface"), mManager(*dm) {

	mCommandMaps.insert(commandMaps_t::value_type("GetFormat", CMD_IFACE_GET_FORMAT));
	mCommandMaps.insert(commandMaps_t::value_type("SetFormat", CMD_IFACE_SET_FORMAT));
	mCommandMaps.insert(commandMaps_t::value_type("GetType",   CMD_IFACE_GET_TYPE));
	mCommandMaps.insert(commandMaps_t::value_type("GetMode",   CMD_IFACE_GET_MODE));
	mCommandMaps.insert(commandMaps_t::value_type("SetMode",   CMD_IFACE_SET_MODE));
	mCommandMaps.insert(commandMaps_t::value_type("CheckMode", CMD_IFACE_CHECK_MODE));
	mCommandMaps.insert(commandMaps_t::value_type("ListMode",  CMD_IFACE_LIST_MODE));
	mCommandMaps.insert(commandMaps_t::value_type("GetPixelFormat",  CMD_IFACE_GET_PIXEL_FORMAT));
	mCommandMaps.insert(commandMaps_t::value_type("SetPixelFormat",  CMD_IFACE_SET_PIXEL_FORMAT));
	mCommandMaps.insert(commandMaps_t::value_type("GetCurDataspace", CMD_IFACE_GET_CUR_DATASPACE_MODE));
	mCommandMaps.insert(commandMaps_t::value_type("GetDataspace",    CMD_IFACE_GET_DATASPACE_MODE));
	mCommandMaps.insert(commandMaps_t::value_type("SetDataspace",    CMD_IFACE_SET_DATASPACE_MODE));
	mCommandMaps.insert(commandMaps_t::value_type("GetDeviceConfig", CMD_IFACE_GET_DEVICE_CONFIG));
	mCommandMaps.insert(commandMaps_t::value_type("SetDeviceConfig", CMD_IFACE_SET_DEVICE_CONFIG));
}

int
CommandListener
::InterfaceCmd::runCommand(SocketClient *client, int argc, char **argv) {
	CHECK_ARGC(client, argc, 3);

	char response[1024];
	int command = toCommandCode(argv[1]);
	int display = toDisplayIdx(atoi(argv[2]));
	int format, mode, type;
	struct disp_device_config config;
	std::vector<int> supportModes;
	int error = 0;

	dumpArguments("Framework --> displayd", argc, argv);
	switch (command) {
	case CMD_IFACE_GET_FORMAT:
		format = mManager.getOutputFormat(display);
		sprintf(response, "%d", format);
		break;
	case CMD_IFACE_SET_FORMAT:
		CHECK_ARGC(client, argc, 4);
		format = atoi(argv[3]);
		error = mManager.setOutputFormat(display, format);
		sprintf(response, "%s", !error ? "Set format completed" : "unknow error");
		break;
	case CMD_IFACE_GET_TYPE:
		type = mManager.getOutputType(display);
		sprintf(response, "%d", type);
		break;
	case CMD_IFACE_GET_MODE:
		mode = mManager.getOutputMode(display);
		sprintf(response, "%d", mode);
		break;
	case CMD_IFACE_SET_MODE:
		CHECK_ARGC(client, argc, 4);
		mode = atoi(argv[3]);
		error = mManager.setOutputMode(display, mode);
		sprintf(response, "%s", !error ? "Set mode completed" : "unknow error");
		break;
	case CMD_IFACE_CHECK_MODE:
		CHECK_ARGC(client, argc, 4);
		mode = atoi(argv[3]);
		sprintf(response, "%s", mManager.isSupportMode(display, mode) == 1 ? "true" : "false");
		break;
	case CMD_IFACE_LIST_MODE:
		mManager.listSupportModes(display, &supportModes);
		for (auto& item : supportModes) {
			sprintf(response, "%d", item);
			client->sendMsg(ResponseCode::ModeListResult, response, false);
		}
		sprintf(response, "%s", "Interface list completed");
		break;
	case CMD_IFACE_GET_PIXEL_FORMAT:
		mode = mManager.getOutputPixelFormat(display);
		sprintf(response, "%d", mode);
		break;
	case CMD_IFACE_SET_PIXEL_FORMAT:
		CHECK_ARGC(client, argc, 4);
		mode = atoi(argv[3]);
		error = mManager.setOutputPixelFormat(display, mode);
		sprintf(response, "%s", !error ? "Set pixelFormat completed" : "unknow error");
		break;
	case CMD_IFACE_GET_CUR_DATASPACE_MODE:
		mode = mManager.getOutputCurDataspaceMode(display);
		sprintf(response, "%d", mode);
		break;
	case CMD_IFACE_GET_DATASPACE_MODE:
		mode = mManager.getOutputDataspaceMode(display);
		sprintf(response, "%d", mode);
		break;
	case CMD_IFACE_SET_DATASPACE_MODE:
		CHECK_ARGC(client, argc, 4);
		mode = atoi(argv[3]);
		error = mManager.setOutputDataspaceMode(display, mode);
		sprintf(response, "%s", !error ? "Set DataspaceMode completed" : "unknow error");
		break;
	case CMD_IFACE_GET_DEVICE_CONFIG:
		error = mManager.getDeviceConfig(display, &config);
		sprintf(response, "%d %d,%d,%d,%d,%d,%d,%d,%d,%d,%d",
				error, config.type, config.mode, config.format, config.bits,
				config.eotf, config.cs, config.dvi_hdmi, config.range, config.scan,
				config.aspect_ratio);
		break;
	case CMD_IFACE_SET_DEVICE_CONFIG:
		if (sscanf(argv[3], "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d",
				&config.type, &config.mode, &config.format, &config.bits,
				&config.eotf, &config.cs, &config.dvi_hdmi, &config.range,
				&config.scan, &config.aspect_ratio) == 10) {
			error = mManager.setDeviceConfig(display, &config);
		} else {
			log_error("CommandListener: parse device config error");
			error = -1;
		}
		sprintf(response, "%s", !error ? "Set Device Config completed" : "unknow error");
		break;
	default:
		sprintf(response, "unknow commands");
		error = -1;
		break;
	}
	client->sendMsg(!error ? ResponseCode::CommandOkay : ResponseCode::OperationFailed, response, false);
	return 0;
}

CommandListener::OverscanCmd::OverscanCmd(DisplayManager *dm)
	:DisplaydCommand("overscan"), mManager(*dm) {

	mCommandMaps.insert(commandMaps_t::value_type("GetMargin", CMD_OVERSCAN_GET_MARGIN));
	mCommandMaps.insert(commandMaps_t::value_type("SetMargin", CMD_OVERSCAN_SET_MARGIN));
	mCommandMaps.insert(commandMaps_t::value_type("GetOffset", CMD_OVERSCAN_GET_OFFSET));
	mCommandMaps.insert(commandMaps_t::value_type("SetOffset", CMD_OVERSCAN_SET_OFFSET));
}

int
CommandListener
::OverscanCmd::runCommand(SocketClient *client, int argc, char **argv) {
	CHECK_ARGC(client, argc, 3);

	int command = toCommandCode(argv[1]);
	int display = toDisplayIdx(atoi(argv[2]));
	int xvalue, yvalue;
	char response[1024];
	int error = 0;

	dumpArguments("Framework --> displayd", argc, argv);
	switch (command) {
	case CMD_OVERSCAN_GET_MARGIN:
		xvalue = yvalue = 0;
		mManager.getDisplayMargin(display, &xvalue, &yvalue);
		sprintf(response, "%d %d", xvalue, yvalue);
		break;
	case CMD_OVERSCAN_SET_MARGIN:
		CHECK_ARGC(client, argc, 5);
		xvalue = atoi(argv[3]);
		yvalue = atoi(argv[4]);
		error = mManager.setDisplayMargin(display, xvalue, yvalue);
		sprintf(response, "%s", !error ? "Set margin completed" : "unknow error");
		break;
	case CMD_OVERSCAN_GET_OFFSET:
		xvalue = yvalue = 0;
		mManager.getDisplayOffset(display, &xvalue, &yvalue);
		sprintf(response, "%d %d", xvalue, yvalue);
		break;
	case CMD_OVERSCAN_SET_OFFSET:
		CHECK_ARGC(client, argc, 5);
		xvalue = atoi(argv[3]);
		yvalue = atoi(argv[4]);
		error = mManager.setDisplayOffset(display, xvalue, yvalue);
		sprintf(response, "%s", !error ? "Set offset completed" : "unknow error");
		break;
	default:
		sprintf(response, "unknow commands");
		error = -1;
		break;
	}
	client->sendMsg(!error ? ResponseCode::CommandOkay : ResponseCode::OperationFailed, response, false);
	return 0;
}

CommandListener::UtilsCmd::UtilsCmd(DisplayManager *dm)
	:DisplaydCommand("utils"), mManager(*dm) {

	mCommandMaps.insert(commandMaps_t::value_type("Set3DLayerMode", CMD_UTILS_SET_3D_LAYER_MODE));
	mCommandMaps.insert(commandMaps_t::value_type("Set3DLayerOffset", CMD_UTILS_SET_3D_LAYER_OFFSET));
	mCommandMaps.insert(commandMaps_t::value_type("GetSupport3DMode", CMD_UTILS_GET_SUPPORT_3D_MODE));
}

int
CommandListener
::UtilsCmd::runCommand(SocketClient *client, int argc, char **argv) {
	CHECK_ARGC(client, argc, 3);

	int command = toCommandCode(argv[1]);
	int display = toDisplayIdx(atoi(argv[2]));
	int offset;
	int mode;
	int support;
	char response[1024];
	int error = 0;

	dumpArguments("Framework --> displayd", argc, argv);
	switch (command) {
	case CMD_UTILS_SET_3D_LAYER_MODE:
		CHECK_ARGC(client, argc, 5);
		mode = atoi(argv[3]);
		error = mManager.set3DLayerMode(display, mode, atoi(argv[4]));
		sprintf(response, "%s", !error ? "Set 3D layer mode completed" : "unknow error");
		break;
	case CMD_UTILS_SET_3D_LAYER_OFFSET:
		CHECK_ARGC(client, argc, 4);
		offset = atoi(argv[3]);
		error = 0;
		sprintf(response, "%s", !error ? "Set 3D layer offset completed" : "unknow error");
		break;
	case CMD_UTILS_GET_SUPPORT_3D_MODE:
		support = mManager.isSupport3DMode(display, -1);
		sprintf(response, "%d", support);
		break;
	default:
		sprintf(response, "unknow commands");
		error = -1;
		break;
	}
	client->sendMsg(!error ? ResponseCode::CommandOkay : ResponseCode::OperationFailed, response, false);
	return 0;
}

CommandListener::EnhanceCmd::EnhanceCmd(DisplayManager *dm)
	:DisplaydCommand("enhance"), mManager(*dm) {

	mCommandMaps.insert(commandMaps_t::value_type("GetEdge"      , CMD_ENHANCE_GET_EDGE));
	mCommandMaps.insert(commandMaps_t::value_type("SetEdge"      , CMD_ENHANCE_SET_EDGE));
	mCommandMaps.insert(commandMaps_t::value_type("GetDetail"    , CMD_ENHANCE_GET_DETAIL));
	mCommandMaps.insert(commandMaps_t::value_type("SetDetail"    , CMD_ENHANCE_SET_DETAIL));
	mCommandMaps.insert(commandMaps_t::value_type("GetBright"    , CMD_ENHANCE_GET_BRIGHT));
	mCommandMaps.insert(commandMaps_t::value_type("SetBright"    , CMD_ENHANCE_SET_BRIGHT));
	mCommandMaps.insert(commandMaps_t::value_type("GetDenoise"   , CMD_ENHANCE_GET_DENOISE));
	mCommandMaps.insert(commandMaps_t::value_type("SetDenoise"   , CMD_ENHANCE_SET_DENOISE));
	mCommandMaps.insert(commandMaps_t::value_type("GetContrast"  , CMD_ENHANCE_GET_CONTRAST));
	mCommandMaps.insert(commandMaps_t::value_type("SetContrast"  , CMD_ENHANCE_SET_CONTRAST));
	mCommandMaps.insert(commandMaps_t::value_type("GetSaturation", CMD_ENHANCE_GET_SATURATION));
	mCommandMaps.insert(commandMaps_t::value_type("SetSaturation", CMD_ENHANCE_SET_SATURATION));
	mCommandMaps.insert(commandMaps_t::value_type("GetMode"      , CMD_ENHANCE_GET_MODE));
	mCommandMaps.insert(commandMaps_t::value_type("SetMode"      , CMD_ENHANCE_SET_MODE));
}

int
CommandListener
::EnhanceCmd::runCommand(SocketClient *client, int argc, char **argv) {
	CHECK_ARGC(client, argc, 3);

	int command = toCommandCode(argv[1]);
	int display = toDisplayIdx(atoi(argv[2]));
	char response[1024];
	int error = 0;
	int result, target;

	dumpArguments("Framework --> displayd", argc, argv);
	switch (command) {
	case CMD_ENHANCE_GET_EDGE:
		result = mManager.getDisplayEdge(display);
		sprintf(response, "%d", result);
		break;
	case CMD_ENHANCE_SET_EDGE:
		CHECK_ARGC(client, argc, 4);
		target = atoi(argv[3]);
		error = mManager.setDisplayEdge(display, target);
		sprintf(response, "%s", !error ? "Set enhance edge completed" : "unknow error");
		break;
	case CMD_ENHANCE_GET_DETAIL:
		result = mManager.getDisplayDetail(display);
		sprintf(response, "%d", result);
		break;
	case CMD_ENHANCE_SET_DETAIL:
		CHECK_ARGC(client, argc, 4);
		target = atoi(argv[3]);
		error = mManager.setDisplayDetail(display, target);
		sprintf(response, "%s", !error ? "Set enhance detail completed" : "unknow error");
		break;
	case CMD_ENHANCE_GET_BRIGHT:
		result = mManager.getDisplayBright(display);
		sprintf(response, "%d", result);
		break;
	case CMD_ENHANCE_SET_BRIGHT:
		CHECK_ARGC(client, argc, 4);
		target = atoi(argv[3]);
		error = mManager.setDisplayBright(display, target);
		sprintf(response, "%s", !error ? "Set enhance bright completed" : "unknow error");
		break;
	case CMD_ENHANCE_GET_DENOISE:
		result = mManager.getDisplayDenoise(display);
		sprintf(response, "%d", result);
		break;
	case CMD_ENHANCE_SET_DENOISE:
		CHECK_ARGC(client, argc, 4);
		target = atoi(argv[3]);
		error = mManager.setDisplayDenoise(display, target);
		sprintf(response, "%s", !error ? "Set enhance denoise completed" : "unknow error");
		break;
	case CMD_ENHANCE_GET_CONTRAST:
		result = mManager.getDisplayContrast(display);
		sprintf(response, "%d", result);
		break;
	case CMD_ENHANCE_SET_CONTRAST:
		CHECK_ARGC(client, argc, 4);
		target = atoi(argv[3]);
		error = mManager.setDisplayContrast(display, target);
		sprintf(response, "%s", !error ? "Set enhance contrast completed" : "unknow error");
		break;
	case CMD_ENHANCE_GET_SATURATION:
		result = mManager.getDisplaySaturation(display);
		sprintf(response, "%d", result);
		break;
	case CMD_ENHANCE_SET_SATURATION:
		CHECK_ARGC(client, argc, 4);
		target = atoi(argv[3]);
		error = mManager.setDisplaySaturation(display, target);
		sprintf(response, "%s", !error ? "Set enhance saturation completed" : "unknow error");
		break;
	case CMD_ENHANCE_GET_MODE:
		result = mManager.getDisplayEnhanceMode(display);
		sprintf(response, "%d", result);
		break;
	case CMD_ENHANCE_SET_MODE:
		CHECK_ARGC(client, argc, 4);
		target = atoi(argv[3]);
		error = mManager.setDisplayEnhanceMode(display, target);
		sprintf(response, "%s", !error ? "Set enhance mode completed" : "unknow error");
		break;
	default:
		sprintf(response, "unknow commands");
		error = -1;
		break;
	}
	client->sendMsg(!error ? ResponseCode::CommandOkay : ResponseCode::OperationFailed, response, false);
	return 0;
}


#if 0
int
CommandListener
::InterfaceCmd::runCommand(SocketClient *client, int argc, char **argv) {
	CHECK_ARGC(client, argc, 3);

	int command = toCommandCode(argv[1]);
	int display = atoi(argv[2]);

	switch (command) {
	case CMD_LIST_INTERFACE: {
		std::vector<int> supportInterface;
		mManager.listInterface(display, &supportInterface);
		for (auto& interface : supportInterface) {
			client->sendMsg(ResponseCode::InterfaceListResult, interfaceToString(interface), false);
		}
		client->sendMsg(ResponseCode::CommandOkay, "Interface list completed", false);
		break; }
	case CMD_GET_CURRENT_INTERFACE: {
		int interface;
		mManager.getCurrentInterface(display, &interface);
		client->sendMsg(ResponseCode::CommandOkay, interfaceToString(interface), false);
		break; }
	case CMD_SET_CURRENT_INTERFACE: {
		int interface = stringToInterface(argv[3]);
		int enable = strcmp(argv[4], "true") == 0 ? 1 : 0;
		int result = mManager.setCurrentInterface(display, interface, enable);
		int response = result ? ResponseCode::OperationFailed : ResponseCode::CommandOkay;
		const char *message = result ? "Interface set failed" : "Interface set completed";
		client->sendMsg(response, message, false);
		break; }
	case CMD_SWITCH_INTERFACE:
		ALOGD("request: display %d switch to next interface", display);
		client->sendMsg(ResponseCode::CommandOkay, "Switch interface completed", false);
		break;
	default:
		UNKNOW_COMMAND(client);
		break;
	}
	return 0;
}

CommandListener::ModeCmd::ModeCmd(DisplayManager *dm)
	:DisplaydCommand("mode"), mManager(*dm) {

	mCommandMaps.insert(commandMaps_t::value_type("list",       CMD_LIST_MODE));
	mCommandMaps.insert(commandMaps_t::value_type("get",        CMD_GET_CURRENT_MODE));
	mCommandMaps.insert(commandMaps_t::value_type("set",        CMD_SET_CURRENT_MODE));
	mCommandMaps.insert(commandMaps_t::value_type("get3dmodes", CMD_GET_3D_MODES));
	mCommandMaps.insert(commandMaps_t::value_type("get3dmode",  CMD_GET_3D_MODE));
	mCommandMaps.insert(commandMaps_t::value_type("set3dmode",  CMD_SET_3D_MODE));
}

int CommandListener::ModeCmd::runCommand(SocketClient *client, int argc, char **argv) {
	CHECK_ARGC(client, argc, 4);

	int command = toCommandCode(argv[1]);
	int display = atoi(argv[2]);
	int interface = stringToInterface(argv[3]);

	switch (command) {
	case CMD_LIST_MODE: {
		std::vector<int> supportModes;
		mManager.listMode(display, interface, &supportModes);
		for (auto& mode : supportModes) {
			client->sendMsg(ResponseCode::ModeListResult, modeToString(mode), false);
		}
		client->sendMsg(ResponseCode::CommandOkay, "Mode list completed", false);
		break; }
	case CMD_GET_CURRENT_MODE: {
		int mode;
		if (mManager.getCurrentMode(display, interface, &mode))
			client->sendMsg(ResponseCode::CommandParameterError, "Missing iface", false);

		client->sendMsg(ResponseCode::CommandOkay, modeToString(mode), false);
		break; }
	case CMD_SET_CURRENT_MODE: {
		int mode = stringToMode(argv[4]);
		mManager.setCurrentMode(display, interface, mode);
		client->sendMsg(ResponseCode::CommandOkay, "Mode set completed", false);
		break; }
	case CMD_GET_3D_MODES:
	case CMD_GET_3D_MODE:
		ALOGD("request: display %d get 3D mode", display);
		client->sendMsg(ResponseCode::CommandOkay, "completed", false);
		break;
	case CMD_SET_3D_MODE:
		ALOGD("request: display %d set 3D mode", display);
		client->sendMsg(ResponseCode::CommandOkay, "completed", false);
		break;
	default:
		UNKNOW_COMMAND(client);
		break;
	}
	return 0;
}
#endif

