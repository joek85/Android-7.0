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

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#include "config.h"
#include "sunxi_display2.h"
#include "policySelector.h"

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
     	NOTICE("init_disp=0x%x, type=%d, id=%d\n", datas, type, id);
        if(((type & 0xFF) << 8) == (0xFF00 & (datas >> (id << 4))))
            return 0xFF & (datas >> (id << 4));
    }
    return 0;
}

#ifdef SUN50IW6P1
static int disp_get_device_config(int disp, struct disp_device_config *out)
{
	char tag[64];
	char buffer[1024];
	char *p;
	int fd, n, find = 0;

	fd = open(RSL_FILE_NAME, O_RDONLY);
	if (fd < 0) {
		ERROR("open '%s', failed\n", RSL_FILE_NAME);
		return -1;
	}

	sprintf(tag, "disp_config%d=", disp);
	memset(buffer, 0, 1024);
	n = read(fd, buffer, 1023);
	if ((n > 0) && (p = strstr(buffer, tag)) != 0) {
		p += strlen(tag);
		int tmp[6] = {0};
		int cnt = sscanf(p, "%d,%d - %d,%d,%d,%d",
					&tmp[0], &tmp[1], &tmp[2], &tmp[3], &tmp[4], &tmp[5]);
		if (cnt == 6) {
			find = 1;
			out->type   = static_cast<disp_output_type>(tmp[0]);
			out->mode   = static_cast<disp_tv_mode>(tmp[1]);
			out->format = static_cast<disp_csc_type>(tmp[2]);
			out->bits   = static_cast<disp_data_bits>(tmp[3]);
			out->cs     = static_cast<disp_color_space>(tmp[4]);
			out->eotf   = static_cast<disp_eotf>(tmp[5]);
		}
	}
	close(fd);
	ERROR("disp_config%d: %d,%d - %d,%d,%d,%d\n",
			disp, out->type, out->mode, out->format, out->bits, out->eotf, out->cs);
	return find ? 0 : -1;
}
#endif

static int get_strings_from_file(char const * fileName, char *values, unsigned int num)
{
	FILE *fp;
	int i = 0;

	if(NULL ==(fp = fopen(fileName, "r")))
		return -1;

	while(!feof(fp) && (i < (int)num - 1)) {
		values[i] = fgetc(fp);
		i++;
	}
	values[i] = '\0';
	fclose(fp);
	return i;
}

static int is_hdmi_plugin(void) {
	char valueString[32] = {0};

	memset(valueString, 0, 32);
	if((get_strings_from_file(HDMI_HPD_STATE_FILENAME, valueString, 32) > 0)
			&& strncmp(valueString, "0", 1))
		return 1;
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
			primaryChannel = channel;
		} else {
			device.type   = -1;
			device.mode   = -1;
			device.active = 0;
			externalChannel = channel;
		}

		NOTICE("display.%d: %s type(%02d) mode(%02d)\n",
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
#ifdef SUN50IW6P1
	struct disp_device_config bootconfig;
	int error = disp_get_device_config(channel, &bootconfig);
	if (error || bootconfig.type != type) {
		ERROR("Cant't get boot disp_config%d, use default config\n", channel);
		bootconfig.format = DISP_CSC_TYPE_YUV444;
		bootconfig.bits   = DISP_DATA_8BITS;
		bootconfig.cs     = DISP_BT709;
		bootconfig.eotf   = DISP_EOTF_GAMMA22;
	}
	/*
	 * TODO: check color space for 4K output
	 */
	if (mode == DISP_TV_MOD_3840_2160P_60HZ
			|| mode == DISP_TV_MOD_3840_2160P_50HZ) {
		bootconfig.format = DISP_CSC_TYPE_YUV420;
		bootconfig.bits   = DISP_DATA_8BITS;
		bootconfig.cs     = DISP_BT709;
		bootconfig.eotf   = DISP_EOTF_GAMMA22;
	} else {
		bootconfig.format = DISP_CSC_TYPE_YUV444;
		bootconfig.bits   = DISP_DATA_8BITS;
		bootconfig.cs     = DISP_BT709;
		bootconfig.eotf   = DISP_EOTF_GAMMA22;
	}
	bootconfig.type = static_cast<disp_output_type>(type);
	bootconfig.mode = static_cast<disp_tv_mode>(mode);
	bootconfig.dvi_hdmi = static_cast<enum disp_dvi_hdmi>(0);
	bootconfig.range = static_cast<enum disp_color_range>(2);
	bootconfig.scan = static_cast<enum disp_scan_info>(0);
	bootconfig.aspect_ratio = 8;

	unsigned long args[4] = {0};
	args[0] = channel;
	args[1] = (unsigned long)&bootconfig;
	error = mDispManager.getControler()->disp_ioctl(DISP_DEVICE_SET_CONFIG, (unsigned long)args);
	ERROR("Config display-%d: "
		"type-%d mode-%d fmt-0x%04x bits-%d cs-0x%04x eotf-0x%04x, return %d\n",
		channel, bootconfig.type, bootconfig.mode, bootconfig.format,
		bootconfig.bits, bootconfig.cs, bootconfig.eotf, error);
	return error;
#else
	unsigned long args[4] = {0};
	args[0] = channel;
	args[1] = type;
	args[2] = mode;
	return mDispManager.getControler()->disp_ioctl(DISP_DEVICE_SWITCH, (unsigned long)args);
#endif
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
		ERROR("disp_reset_scn_win: not support the mode[%d]\n", mode);
	}
}

int policyBuilder::copyBootLogoLayer(void) {
	int source = -1;
	int target = -1;

	if (primaryChannel < 0 || externalChannel < 0) {
		NOTICE("primary display (%d) or external display (%d) is't exist",
			primaryChannel, externalChannel);
		return 0;
	}

	source = primaryChannel;
	target = externalChannel;
	NOTICE("Copy Layer: from channel %d to channel %d\n", source, target);

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
				NOTICE("Copy Layer: find layer channel %d layer %d\n", channel, layer);
				goto __copy;
			}
		}
	ERROR("Copy Layer: can't find bootlogo layer.\n");
	return 0;

__copy:
	/* correct screen win for the target device */
	correct_screen_window(&layer_config.info.screen_win, mDevice[target].mode);
	args[0] = target;
	args[1] = (unsigned long)&layer_config;
	args[2] = 1;
	mDispManager.getControler()->disp_ioctl(DISP_LAYER_SET_CONFIG, (unsigned long)args);
	return 0;
}

int policyBuilder::dispCheckHdmiOutput() {
	const int HDMI_MODES[] = {
		DISP_TV_MOD_3840_2160P_60HZ,
		DISP_TV_MOD_3840_2160P_30HZ,
		DISP_TV_MOD_3840_2160P_25HZ,
		DISP_TV_MOD_1080P_60HZ,
		DISP_TV_MOD_1080P_50HZ,
		DISP_TV_MOD_1080I_60HZ,
		DISP_TV_MOD_1080I_50HZ,
		DISP_TV_MOD_720P_60HZ,
		DISP_TV_MOD_720P_50HZ,
		DISP_TV_MOD_576P,
		DISP_TV_MOD_480P,
	};
	deviceInfo& device = mDevice[HDMI_CHANNEL];
	unsigned long args[4] = {0};

	if (!is_hdmi_plugin()) {
		NOTICE("HDMI not plugin now, skip output mode detect\n");
		return 0;
	}

	args[0] = HDMI_CHANNEL;
	args[1] = device.mode;
	if (mDispManager.getControler()->disp_ioctl(DISP_HDMI_SUPPORT_MODE, (unsigned long)args)) {
		NOTICE("HDMI current output mode %d, supported\n", device.mode);
		return 0;
	}

	NOTICE("HDMI current output mode %d, not supported\n", device.mode);
	int mode = device.default_mode;
	for (size_t i = 0; i < sizeof(HDMI_MODES) / sizeof(HDMI_MODES[0]); i++) {
		args[0] = HDMI_CHANNEL;
		args[1] = HDMI_MODES[i];
		if (mDispManager.getControler()->disp_ioctl(DISP_HDMI_SUPPORT_MODE, (unsigned long)args)) {
			mode = HDMI_MODES[i];
			NOTICE("HDMI output mode set to %d\n", mode);
			break;
		}
	}
	/*
	dispDeviceSwitch(HDMI_CHANNEL, DISP_OUTPUT_TYPE_HDMI, mode);
	*/
	return 0;
}

void defaultBuilder::initializer() {
	NOTICE("selected display policy: default\n");

	dispDeviceDiscovery();
	return;
}

void singleBuilder::initializer() {
	NOTICE("selected display policy: single\n");
	/*
	 * TODO: handle hotplug befor hwc create_device
	 */

	dispDeviceDiscovery();
	return;
}

void dualBuilder::initializer() {
	NOTICE("selected display policy: dual\n");

	dispDeviceDiscovery();
	NOTICE("do open device\n");
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
			device.type = device.expect_type;
			device.mode = mode;
			device.active = 1;
		}
	}
	copyBootLogoLayer();
#endif

	/* check hdmi output mode here */
	dispCheckHdmiOutput();
	return;

_error:
	ERROR("unmatch display: channel!!\n");
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
	mDispManager.getControler()->exit();
	return 0;
}
