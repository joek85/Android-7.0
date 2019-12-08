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

#ifndef _HWCOMPOSERINTERFACE_H
#define _HWCOMPOSERINTERFACE_H

enum {
	HWC_GET_OUTPUT_FORMAT = 0x100,
	HWC_SET_OUTPUT_FORMAT = 0x101,
	HWC_SET_OVERSCAN      = 0x102,
	HWC_GET_OVERSCAN      = 0x103,
	HWC_GET_3D_LAYER_MODE = 0x104,
	HWC_SET_3D_LAYER_MODE = 0x105,
	HWC_SET_SCREEN_RADIO  = 0x106,
	HWC_GET_OUTPUT_PIXEL_FORMAT       = 0x107,
	HWC_SET_OUTPUT_PIXEL_FORMAT       = 0x108,
	HWC_GET_OUTPUT_CUR_DATASPACE_MODE = 0x109,
	HWC_GET_OUTPUT_DATASPACE_MODE     = 0x10A,
	HWC_SET_OUTPUT_DATASPACE_MODE     = 0x10B,
	HWC_SET_DEVICE_CONFIG             = 0x10C,

	HWC_GET_ENHANCE_MODE  = 0x200,
	HWC_SET_ENHANCE_MODE  = 0x201,
	HWC_GET_ENHANCE_COMPONENT = 0x202,
	HWC_SET_ENHANCE_COMPONENT = 0x203,

	HWC_PROPERTY_CHANGE   = 0x400,
};

/* Enhance component type define */
typedef enum {
	ENHANCE_EDGE,
	ENHANCE_DETAIL,
	ENHANCE_BRIGHT,
	ENHANCE_DENOISE,
	ENHANCE_CONTRAST,
	ENHANCE_SATURATION,
} enhance_component_type_t;

typedef int (*HWC_PERFORM)(int display, int cmd, ...);

struct hwcomposerInterface {
	HWC_PERFORM mHWCPerform;
};

int displaydRequest_init(void *perform_pfn);

#endif
