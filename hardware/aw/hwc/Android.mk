# Copyright (C) 2010 The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

LOCAL_PATH := $(call my-dir)

my_has_target := 0

ifneq ($(filter $(SW_CHIP_PLATFORM),A80),)
    include $(call all-named-subdir-makefiles,jaws)
    my_has_target += 1
endif

ifneq ($(filter $(SW_CHIP_PLATFORM),H8),)
    include $(call all-named-subdir-makefiles,eagle)
    my_has_target += 1
endif

ifneq ($(filter $(SW_CHIP_PLATFORM),H5 H64),)
    include $(call all-named-subdir-makefiles,rabbit)
    my_has_target += 1
endif

ifneq ($(filter $(SW_CHIP_PLATFORM),H6),)
    include $(call all-named-subdir-makefiles,petrel)
    my_has_target += 1
endif

ifneq ($(filter $(SW_CHIP_PLATFORM),H3),)
    include $(call all-named-subdir-makefiles,dolphin)
    my_has_target += 1
endif

ifeq ($(my_has_target), 0)
$(error $(SW_CHIP_PLATFORM))
endif

include $(CLEAR_VARS)

