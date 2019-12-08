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
#include <dirent.h>
#include <errno.h>
#include <string>
#include <linux/if.h>
#include <cutils/log.h>
#include <cutils/sockets.h>

#include <hardware/hwcomposer.h>
#include "displaydCommand.h"
#include "displaydRequest.h"
#include "sunxi_display2.h"
#include "debug.h"

static inline int getOutputFormat(hwcomposerInterface& interface,
		int display, int *type, int *mode)
{
	return interface.mHWCPerform(display, HWC_GET_OUTPUT_FORMAT, type, mode);
}

static inline int setOutputFormat(hwcomposerInterface& interface,
		int display, int type, int mode)
{
	return interface.mHWCPerform(display, HWC_SET_OUTPUT_FORMAT, type, mode);
}

static inline int getOutputPixelFormat(hwcomposerInterface& interface,
		int display)
{
	return interface.mHWCPerform(display, HWC_GET_OUTPUT_PIXEL_FORMAT);
}

static inline int setOutputPixelFormat(hwcomposerInterface& interface,
		int display, int format)
{
	return interface.mHWCPerform(display, HWC_SET_OUTPUT_PIXEL_FORMAT, format);
}

static inline int getOutputCurDataspaceMode(
		hwcomposerInterface& interface, int display)
{
	return interface.mHWCPerform(display, HWC_GET_OUTPUT_CUR_DATASPACE_MODE);
}

static inline int getOutputDataspaceMode(
		hwcomposerInterface& interface, int display)
{
	return interface.mHWCPerform(display, HWC_GET_OUTPUT_DATASPACE_MODE);
}

static inline int setOutputDataspaceMode(hwcomposerInterface& interface,
		int display, int mode)
{
	return interface.mHWCPerform(display, HWC_SET_OUTPUT_DATASPACE_MODE, mode);
}

static inline int setDeviceConfig(hwcomposerInterface& interface,
								  int display, struct disp_device_config *config)
{
	log_debug("setDeviceConfig: %d,%d,%d,%d,%d,%d,%d,%d,%d,%d",
		config->type, config->mode, config->format, config->bits,
		config->eotf, config->cs, config->dvi_hdmi, config->range,
		config->scan, config->aspect_ratio);

	return interface.mHWCPerform(display, HWC_SET_DEVICE_CONFIG, config);
}

static inline int getOverscan(hwcomposerInterface& interface,
		int display, int params[])
{
	return interface.mHWCPerform(display, HWC_GET_OVERSCAN, params);
}

static inline int setOverscan(hwcomposerInterface& interface,
		int display, int params[])
{
	return interface.mHWCPerform(display, HWC_SET_OVERSCAN, params);
}

static inline int get3DLayerMode(hwcomposerInterface& interface,
			int display, int *mode)
{
	return interface.mHWCPerform(display, HWC_GET_3D_LAYER_MODE, mode);
}

static inline int set3DLayerMode(hwcomposerInterface& interface,
			int display, int mode, int videoCropHeight)
{
	return interface.mHWCPerform(display, HWC_SET_3D_LAYER_MODE,
		mode, videoCropHeight);
}

static inline int setScreenRadio(hwcomposerInterface& interface,
			int display, int radio)
{
	return interface.mHWCPerform(display, HWC_SET_SCREEN_RADIO, radio);
}

static inline int setEnhanceMode(hwcomposerInterface& interface,
			int display, int mode)
{
	return interface.mHWCPerform(display, HWC_SET_ENHANCE_MODE, mode);
}

static inline int getEnhanceMode(hwcomposerInterface& interface,
			int display, int *mode)
{
	return interface.mHWCPerform(display, HWC_GET_ENHANCE_MODE, mode);
}

static inline int setEnhanceComponent(hwcomposerInterface& interface,
			int display, int component_type, int mode)
{
	return interface.mHWCPerform(display, HWC_SET_ENHANCE_COMPONENT, component_type, mode);
}

static inline int getEnhanceComponent(hwcomposerInterface& interface,
			int display, int component_type, int *mode)
{
	return interface.mHWCPerform(display, HWC_GET_ENHANCE_COMPONENT, component_type, mode);
}

static inline int propertyChange(hwcomposerInterface& interface,
			int display, const char *property, const char *value)
{
	return interface.mHWCPerform(display, HWC_PROPERTY_CHANGE, property, value);
}

requestCommand::requestCommand(const char *cmd)
	:FrameworkCommand(cmd) {
}

int
requestCommand::toCommandCode(char *name) {
	auto iter = mCommandMaps.find(name);
	if (iter == mCommandMaps.end())
		return -1;
	return iter->second;
}

int
displaydRequest::createSocketfd(void) {
#ifdef _SOCK_MANUAL
	struct sockaddr_in address;
	int sockfd;

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) {
		log_error("create socket error: %s(errno: %d)", strerror(errno), errno);
		return -1;
	}

	memset(&address, 0, sizeof(address));
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = htonl(INADDR_ANY);
	address.sin_port = htons(29418);

	if (bind(sockfd, (struct sockaddr*)&address, sizeof(address)) == -1) {
		log_error("bind socket error: %s(errno: %d)", strerror(errno), errno);
		close(sockfd);
		return -1;
	}
	return sockfd;
#else
	int sockfd = android_get_control_socket("hwcsocket");
	if (sockfd < 0) {
		log_error("create socket error: %s(errno: %d)", strerror(errno), errno);
		return -1;
	}
	return sockfd;
#endif
}

displaydRequest::displaydRequest(hwcomposerInterface *interface)
		:FrameworkListener("hwcsocket", true) {
	registerCmd(new interfaceCmd(interface));
	registerCmd(new overscanCmd(interface));
	registerCmd(new utilsCmd(interface));
	registerCmd(new enhanceCmd(interface));
	mInterface = interface;
}

static char *responeEncode(char *buffer, int seqnum, int result, char *payload) {
	sprintf(buffer, "%d hwcRespone %d %s", seqnum, result, payload);
	return buffer;
}

displaydRequest
::interfaceCmd::interfaceCmd(hwcomposerInterface *interface)
		:requestCommand("interface"), mInterface(*interface) {
	mCommandMaps.insert(commandMaps_t::value_type("GetFormat", HWC_GET_OUTPUT_FORMAT));
	mCommandMaps.insert(commandMaps_t::value_type("SetFormat", HWC_SET_OUTPUT_FORMAT));
	mCommandMaps.insert(commandMaps_t::value_type("GetPixelFormat", HWC_GET_OUTPUT_PIXEL_FORMAT));
	mCommandMaps.insert(commandMaps_t::value_type("SetPixelFormat", HWC_SET_OUTPUT_PIXEL_FORMAT));
	mCommandMaps.insert(commandMaps_t::value_type("GetCurDataspace", HWC_GET_OUTPUT_CUR_DATASPACE_MODE));
	mCommandMaps.insert(commandMaps_t::value_type("GetDataspace", HWC_GET_OUTPUT_DATASPACE_MODE));
	mCommandMaps.insert(commandMaps_t::value_type("SetDataspace", HWC_SET_OUTPUT_DATASPACE_MODE));
	mCommandMaps.insert(commandMaps_t::value_type("SetDeviceConfig", HWC_SET_DEVICE_CONFIG));
}

int displaydRequest
::interfaceCmd::runCommand(SocketClient *client, int argc, char **argv) {
	dumpArguments("displayd --> HWC", argc, argv);
	if (!mInterface.mHWCPerform) {
		log_error("hwcomposer perform interface not register, drop command");
		return 0;
	}

	int opcode = toCommandCode(argv[1]);
	int display = strtol(argv[2], 0, 0);
	int type, mode;
	int result = 0;
	char payload[64];
	char response[512];

	switch (opcode) {
	case HWC_GET_OUTPUT_FORMAT:
		result = getOutputFormat(mInterface, display, &type, &mode);
		if (result != 0) {
			type = mode = -1;
			log_error("getOutputFormat error");
		}
		sprintf(payload, "%d %d", type, mode);
		break;
	case HWC_SET_OUTPUT_FORMAT:
		type = strtol(argv[3], 0, 0);
		mode = strtol(argv[4], 0, 0);
		result = setOutputFormat(mInterface, display, type, mode);
		sprintf(payload, "%d", result);
		break;
	case HWC_GET_OUTPUT_PIXEL_FORMAT:
		mode = getOutputPixelFormat(mInterface, display);
		sprintf(payload, "%d", mode);
		break;
	case HWC_SET_OUTPUT_PIXEL_FORMAT:
		mode = strtol(argv[3], 0, 0);
		result = setOutputPixelFormat(mInterface, display, mode);
		sprintf(payload, "%d", result);
		break;
	case HWC_GET_OUTPUT_CUR_DATASPACE_MODE:
		mode = getOutputCurDataspaceMode(mInterface, display);
		sprintf(payload, "%d", mode);
		break;
	case HWC_GET_OUTPUT_DATASPACE_MODE:
		mode = getOutputDataspaceMode(mInterface, display);
		sprintf(payload, "%d", mode);
		break;
	case HWC_SET_OUTPUT_DATASPACE_MODE:
		mode = strtol(argv[3], 0, 0);
		result = setOutputDataspaceMode(mInterface, display, mode);
		sprintf(payload, "%d", result);
		break;
	case HWC_SET_DEVICE_CONFIG: {
		struct disp_device_config config;
		result = -1;
		if (sscanf(argv[3], "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d",
				&config.type, &config.mode, &config.format, &config.bits,
				&config.eotf, &config.cs, &config.dvi_hdmi, &config.range,
				&config.scan, &config.aspect_ratio) == 10)
			result = setDeviceConfig(mInterface, display, &config);
		else
			log_error("HWC_SET_DEVICE_CONFIG, parse config error\n");
		sprintf(payload, "%d", result);
		break;}
	default:
		log_error("unknow interface command: %s", argv[1]);
		result = -1;
		sprintf(payload, "unsupport command");
		break;
	}
	client->sendMsg(responeEncode(response, client->getCmdNum(), result, payload));
	return 0;
}

displaydRequest
::overscanCmd::overscanCmd(hwcomposerInterface *interface)
		:requestCommand("overscan"), mInterface(*interface) {
	mCommandMaps.insert(commandMaps_t::value_type("GetOverscan", HWC_GET_OVERSCAN));
	mCommandMaps.insert(commandMaps_t::value_type("SetOverscan", HWC_SET_OVERSCAN));
}

int displaydRequest
::overscanCmd::runCommand(SocketClient *client, int argc, char **argv) {
	dumpArguments("displayd --> HWC", argc, argv);
	if (!mInterface.mHWCPerform) {
		log_error("hwcomposer perform interface not register, drop command");
		return 0;
	}

	int opcode = toCommandCode(argv[1]);
	int display = strtol(argv[2], 0, 0);
	int result, params[4];
	char payload[64];
	char response[512];

	switch (opcode) {
	case HWC_GET_OVERSCAN:
		result = getOverscan(mInterface, display, params);
		if (result != 0) {
			log_error("getOverscan error");
		}
		sprintf(payload, "%d %d %d %d",
					params[0], params[1], params[2], params[3]);
		break;
	case HWC_SET_OVERSCAN:
		params[0] = strtol(argv[3], 0, 0);
		params[1] = strtol(argv[4], 0, 0);
		params[2] = strtol(argv[5], 0, 0);
		params[3] = strtol(argv[6], 0, 0);
		result = setOverscan(mInterface, display, params);
		sprintf(payload, "%d", result);
		break;
	default:
		log_error("unknow overscan command: %s", argv[1]);
		result = -1;
		sprintf(payload, "unsupport command");
		break;
	}
	client->sendMsg(responeEncode(response, client->getCmdNum(), result, payload));
	return 0;
}

displaydRequest
::utilsCmd::utilsCmd(hwcomposerInterface *interface)
		:requestCommand("utils"), mInterface(*interface) {
	mCommandMaps.insert(commandMaps_t::value_type("Get3Dmode", HWC_GET_3D_LAYER_MODE));
	mCommandMaps.insert(commandMaps_t::value_type("Set3Dmode", HWC_SET_3D_LAYER_MODE));
	mCommandMaps.insert(commandMaps_t::value_type("SetScreenRadio", HWC_SET_SCREEN_RADIO));
	mCommandMaps.insert(commandMaps_t::value_type("PropertyChange", HWC_PROPERTY_CHANGE));
}

int displaydRequest
::utilsCmd::runCommand(SocketClient *client, int argc, char **argv) {
	dumpArguments("displayd --> HWC", argc, argv);
	if (!mInterface.mHWCPerform) {
		log_error("hwcomposer perform interface not register, drop command");
		return 0;
	}

	int opcode = toCommandCode(argv[1]);
	int display = strtol(argv[2], 0, 0);
	int result, param;
	char payload[64];
	char response[512];

	switch (opcode) {
	case HWC_GET_3D_LAYER_MODE:
		result = get3DLayerMode(mInterface, display, &param);
		if (result != 0) {
			log_error("get3DLayerMode error");
		}
		sprintf(payload, "%d", param);
		break;
	case HWC_SET_3D_LAYER_MODE:
		param = strtol(argv[3], 0, 0);
		result = set3DLayerMode(mInterface, display, param, strtol(argv[4], 0, 0));
		sprintf(payload, "%d", result);
		break;
	case HWC_SET_SCREEN_RADIO:
		param = strtol(argv[3], 0, 0);
		result = setScreenRadio(mInterface, display, param);
		sprintf(payload, "%d", result);
	case HWC_PROPERTY_CHANGE:
		log_debug("property change: %s = %s", argv[3], argv[4]);
		result = propertyChange(mInterface, display, argv[3], argv[4]);
		sprintf(payload, "%d", result);
	default:
		log_error("unknow utils command: %s", argv[1]);
		result = -1;
		sprintf(payload, "unsupport command");
		break;
	}
	client->sendMsg(responeEncode(response, client->getCmdNum(), result, payload));
	return 0;
}

displaydRequest
::enhanceCmd::enhanceCmd(hwcomposerInterface *interface)
		:requestCommand("enhance"), mInterface(*interface) {
	mCommandMaps.insert(commandMaps_t::value_type("GetMode", HWC_GET_ENHANCE_MODE));
	mCommandMaps.insert(commandMaps_t::value_type("SetMode", HWC_SET_ENHANCE_MODE));
	mCommandMaps.insert(commandMaps_t::value_type("GetComponent", HWC_GET_ENHANCE_COMPONENT));
	mCommandMaps.insert(commandMaps_t::value_type("SetComponent", HWC_SET_ENHANCE_COMPONENT));
}

int displaydRequest
::enhanceCmd::runCommand(SocketClient *client, int argc, char **argv) {
	dumpArguments("displayd --> HWC", argc, argv);
	if (!mInterface.mHWCPerform) {
		log_error("hwcomposer perform interface not register, drop command");
		return 0;
	}

	int opcode = toCommandCode(argv[1]);
	int display = strtol(argv[2], 0, 0);
	int result, param, component_type;
	char payload[64];
	char response[512];

	switch (opcode) {
	case HWC_GET_ENHANCE_MODE:
		result = getEnhanceMode(mInterface, display, &param);
		if (result != 0) {
			log_error("getEnhanceMode error");
		}
		sprintf(payload, "%d", param);
		break;
	case HWC_SET_ENHANCE_MODE:
		param = strtol(argv[3], 0, 0);
		result = setEnhanceMode(mInterface, display, param);
		sprintf(payload, "%d", result);
		break;
	case HWC_GET_ENHANCE_COMPONENT:
		component_type = strtol(argv[3], 0, 0);
		result = getEnhanceComponent(mInterface, display, component_type, &param);
		if (result != 0) {
			log_error("getEnhanceComponent error");
		}
		sprintf(payload, "%d", param);
		break;
	case HWC_SET_ENHANCE_COMPONENT:
		component_type = strtol(argv[3], 0, 0);
		param = strtol(argv[4], 0, 0);
		result = setEnhanceComponent(mInterface, display, component_type, param);
		sprintf(payload, "%d", result);
		break;
	default:
		log_error("unknow utils command: %s", argv[1]);
		result = -1;
		sprintf(payload, "unsupport command");
		break;
	}
	client->sendMsg(responeEncode(response, client->getCmdNum(), result, payload));
	return 0;
}


