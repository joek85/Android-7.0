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

#ifndef _DISPLAYD_DEBUG
#define _DISPLAYD_DEBUG

#define log_error(fmt, args...) ALOGE(fmt, ##args)
#define log_debug(fmt, args...) ALOGD(fmt, ##args)

#ifdef VARIABLE
#define _variable(fmt, args...) ALOGD(fmt, ##args)
#else
#define _variable(fmt, args...)
#endif

#define dumpArguments(__prefix, __argc, __argv)  \
	do {                                         \
	std::string debug;                           \
	for (int _idx = 0; _idx < __argc; _idx++) {  \
		debug += " ";                            \
		debug += __argv[_idx];}                  \
	_variable("%s : [%s]", __prefix, debug.c_str()); \
	} while (0)

#endif
