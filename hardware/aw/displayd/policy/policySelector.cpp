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
#include <fcntl.h>

#include "config.h"
#include "sunxi_display2.h"
#include "policySelector.h"
#include <cutils/log.h>

static int disp_get_rsl(int type, int id)
{
    char cmdline[1024];
    char *ptr;
    int fd;
    unsigned int datas;

    fd = open(RSL_FILE_NAME, O_RDONLY);
    if(fd >= 0) {
        int n = read(fd, cmdline, 1023);
        if (n < 0) n = 0;
        /* get rid of trailing newline, it happens */
        if (n > 0 && cmdline[n-1] == '\n') n--;
        cmdline[n] = 0;
        close(fd);
    } else {
        cmdline[0] = 0;
    }

    ptr = strstr(cmdline, "init_disp=");
    if(ptr != NULL) {
        ptr += strlen("init_disp=");
        datas = (unsigned int)strtoul(ptr, NULL, 16);
        ALOGE("init_disp=0x%x, type=%d, id=%d\n", datas, type, id);
        if(((type & 0xFF) << 8) == (0xFF00 & (datas >> (id << 4))))
            return 0xFF & (datas >> (id << 4));
    }
    return 0;
}

bool policyBuilder::isValidChannel(int channel) {
	return (channel >= 0 && channel < HW_NUM_DISPLAY);
}

int policyBuilder::dispDeviceDiscovery() {
	for (int channel = 0; channel < HW_NUM_DISPLAY; channel++) {
		int type, mode;
		deviceInfo& device = mDevice[channel];

		switch (channel) {
		case HDMI_CHANNEL:
			device.expect_type  = DISP_OUTPUT_TYPE_HDMI;
			device.default_mode = DEFAULT_HDMI_MODE;
			break;
		case CVBS_CHANNEL:
			device.expect_type  = DISP_OUTPUT_TYPE_TV;
			device.default_mode = DEFAULT_CVBS_MODE;
			break;
		default:
			device.expect_type  = -1;
			device.default_mode = -1;;
			break;
		}

		if (!dispDeviceGetOutput(channel, &type, &mode) && type != 0) {
			device.type   = type;
			device.mode   = mode;
			device.active = 1;
		} else {
			device.type   = -1;
			device.mode   = -1;
			device.active = 0;
		}

		ALOGD("display.%d: %s type(%02d) mode(%02d)",
			channel,
			device.active ? "opened" : "closed",
			device.type,
			device.mode);
	}
	return 0;
}

int policyBuilder::dispDeviceGetOutput(int channel, int *type, int *mode) {
	disp_output output;
	unsigned long args[4] = {0};

	args[0] = (unsigned long)channel;
	args[1] = (unsigned long)&output;
	if (mDispManager.getControler()->disp_ioctl(DISP_GET_OUTPUT, (unsigned long)args) == 0) {
		*type = output.type;
		*mode = output.mode;
		return 0;
	}
	return -1;
}

int policyBuilder::dispDeviceSwitch(int channel, int type, int mode) {
	unsigned long args[4] = {0};
	args[0] = channel;
	args[1] = type;
	args[2] = mode;
	return mDispManager.getControler()->disp_ioctl(DISP_DEVICE_SWITCH, (unsigned long)args);
}

static void correct_screen_window(disp_rect *win, int mode) {
	win->x = 0; win->y = 0;
	switch(mode) {
	case DISP_TV_MOD_PAL:
		win->width = 720; win->height = 576;
		break;
	case DISP_TV_MOD_NTSC:
		win->width = 720; win->height = 480;
		break;
	case DISP_TV_MOD_720P_50HZ:
	case DISP_TV_MOD_720P_60HZ:
		win->width = 1280; win->height = 720;
		break;
	case DISP_TV_MOD_1080P_50HZ:
	case DISP_TV_MOD_1080P_60HZ:
		win->width = 1920; win->height = 1080;
		break;
	default:
		win->width = 0; win->height = 0;
		ALOGE("disp_reset_scn_win: not support the mode[%d]\n", mode);
	}
}

int policyBuilder::copyBootLogoLayer(void) {
	int source = -1;
	int target = -1;

	for (int channel = 0; channel < HW_NUM_DISPLAY; channel++) {
		deviceInfo& device = mDevice[channel];
		if (device.active) source = channel;
		else target = channel;
	}
	ALOGD("Copy Layer: from channel %d to channel %d", source, target);

	unsigned long args[4] = {0};
	disp_layer_config layer_config;
	int layer, channel;
	int channel_cnt = (source == 0) ? 4 : 2;
	for (channel = 0; channel < channel_cnt; channel++)
		for (layer = 0; layer < 4; layer++) {
			layer_config.layer_id = layer;
			layer_config.channel  = channel;
			layer_config.enable   = 0;

			args[0] = source;
			args[1] = (unsigned long)&layer_config;
			args[2] = 1;
			mDispManager.getControler()->disp_ioctl(DISP_LAYER_GET_CONFIG, (unsigned long)args);
			if (layer_config.enable) {
				ALOGD("Copy Layer: find layer channel %d layer %d", channel, layer);
				break;
			}
		}
	/* correct screen win for the target device */
	correct_screen_window(&layer_config.info.screen_win, mDevice[target].mode);
	args[0] = target;
	args[1] = (unsigned long)&layer_config;
	args[2] = 1;
	mDispManager.getControler()->disp_ioctl(DISP_LAYER_SET_CONFIG, (unsigned long)args);
	return 0;
}

void defaultBuilder::initializer() {
	ALOGD("selected display policy: default");

	dispDeviceDiscovery();
	return;
}

void singleBuilder::initializer() {
	ALOGD("selected display policy: single");
	/*
	 * TODO: handle hotplug befor hwc create_device
	 */

	dispDeviceDiscovery();
	return;
}

void dualBuilder::initializer() {
	ALOGD("selected display policy: dual");

	dispDeviceDiscovery();
	ALOGD("do open device");
#if 1
	/*
	 * Check the opened channel output type,
	 * If it's type is not match to the configuration, just return.
	 */
	for (int channel = 0; channel < HW_NUM_DISPLAY; channel++) {
		deviceInfo& device = mDevice[channel];
		if (!device.active)
			continue;

		if (device.type != device.expect_type)
			goto _error;
	}

	/*
	 * Init the unopen channel,
	 * And setup the bootlogo layer on this channel.
	 */
	for (int channel = 0; channel < HW_NUM_DISPLAY; channel++) {
		deviceInfo& device = mDevice[channel];
		if (device.active)
			continue;

		if (device.expect_type != -1) {
			int mode = disp_get_rsl(device.expect_type, channel);
			if (mode <= 0)
				mode = device.default_mode;
			dispDeviceSwitch(channel, device.expect_type, mode);
			device.mode = mode;
		}
	}
	copyBootLogoLayer();
#endif
	return;

_error:
	ALOGE("unmatch display: channel!!");
}

policySelector::policySelector(DisplayManager *manager)
	: mDispManager(*manager), mPolicy(0)
{
	builder.insert(std::map<int, policyBuilder *>::value_type(DISP_POLICY_DEFAULT, new defaultBuilder(manager)));
	builder.insert(std::map<int, policyBuilder *>::value_type(DISP_POLICY_SINGLE,  new singleBuilder(manager)));
	builder.insert(std::map<int, policyBuilder *>::value_type(DISP_POLICY_DUAL,    new dualBuilder(manager)));
}

policySelector::~policySelector() {}

int
policySelector::getPolicy() {
	return DISP_POLICY_DUAL;
}

int
policySelector::setup(int policy) {
	auto itme = builder.find(policy);
	if (itme == builder.end())
		return -1;

	policyBuilder *pb = itme->second;
	pb->initializer();
	return 0;
}
