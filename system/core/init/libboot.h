/*
 * Copyright (C) 2014 The Android Open Source Project
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

#ifndef __LIBBOOT_H__
#define __LIBBOOT_H__

__BEGIN_DECLS

struct user_display_param {
	/*
	 * resolution store like that:
	 *  (type << 8 | mode)
	 */
	char resolution[32];
	char margin[32];

	/*
	 * vender id of the last hdmi output device
	 */
	char vendorid[64];

	/*
	 * Add more fields for HDR support
	 */
	int format;       /* RGB / YUV444 / YUV422 / YUV420     */
	int depth;        /* Color depth: 8 / 10 / 12 / 16      */
	int eotf;         /* Electro-Optical Transfer Functions */
	int color_space;  /* BT.601 / BT.709 / BT.2020          */
};

int libboot_read_display_param(struct user_display_param *out);
int libboot_sync_display_param(struct user_display_param *in);
int libboot_sync_display_param_keyvalue(const char *key, char *value);

__END_DECLS

#endif
