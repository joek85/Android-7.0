LOCAL_PATH:= $(call my-dir)
Si4703_ROOT := $(call my-dir)/../rpi-si4703-master
common_cflags := -O0 -g -Wall -fexceptions
common_cflags += -Wno-unused -Wno-sign-compare -Wno-deprecated-declarations -Wno-cpp
common_cppflags := -frtti -Wno-sign-promo -fcheck-new
common_shm_cflags := -O0 -g -Wall -fexceptions -DHAVE_CONFIG_H -Wno-unused
common_ldflags := -ldl
BASE_SHARED_LIBARIES := libutils liblog libbinder libcutils
#JACK_STL_LDFLAGS := -L$(call my-dir)/../../../../../../prebuilts/ndk/current/sources/cxx-stl/gnu-libstdc++/libs/$(TARGET_CPU_ABI) -lgnustl_static
common_c_includes := \
 	$(Si4703_ROOT)

include $(CLEAR_VARS)

LOCAL_MODULE := libAuxRadioService
LOCAL_MODULE_PATH := $(TARGET_OUT)/lib
LOCAL_PRELINK_MODULE := false
LOCAL_MODULE_TAGS := eng optional
LOCAL_CFLAGS := $(common_cflags) -DSERVER_SIDE
LOCAL_LDFLAGS := $(common_ldflags) #$(JACK_STL_LDFLAGS)
LOCAL_C_INCLUDES := $(common_c_includes) LogHelper.h 
LOCAL_SHARED_LIBRARIES = libc libdl libcutils libutils libbinder libSi4703

LOCAL_SRC_FILES := BnAuxRadioService.cpp BpAuxRadioService.cpp IAuxRadioService.cpp AuxRadioService.cpp

include $(BUILD_SHARED_LIBRARY)


include $(CLEAR_VARS)

LOCAL_MODULE := AuxRadioServer
LOCAL_MODULE_PATH := $(TARGET_OUT)/bin
LOCAL_CFLAGS := $(common_cflags) -DSERVER_SIDE
LOCAL_LDFLAGS := $(common_ldflags) #$(JACK_STL_LDFLAGS)
LOCAL_C_INCLUDES := $(common_c_includes)
LOCAL_SHARED_LIBRARIES = $(BASE_SHARED_LIBARIES) libAuxRadioService
LOCAL_MODULE_TAGS := eng optional
LOCAL_SRC_FILES := AuxRadioServer.cpp

include $(BUILD_EXECUTABLE)
