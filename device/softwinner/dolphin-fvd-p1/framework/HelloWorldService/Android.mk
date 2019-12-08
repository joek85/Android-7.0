LOCAL_PATH := $(call my-dir)
BASE_SHARED_LIBARIES := libutils liblog libbinder libcutils
JACK_ROOT := $(call my-dir)/../../../../../external/jack2
#JACK_STL_LDFLAGS := -L$(call my-dir)/../../../../../prebuilts/ndk/current/sources/cxx-stl/gnu-libstdc++/libs/$(TARGET_CPU_ABI) -lgnustl_static
JACK_STL_INCLUDES := $(JACK_ROOT)/android/cxx-stl/gnu-libstdc++/libs/$(TARGET_CPU_ABI)/include \
                     prebuilts/ndk/current/sources/cxx-stl/gnu-libstdc++/libs/$(TARGET_CPU_ABI)/include \
                     prebuilts/ndk/current/sources/cxx-stl/gnu-libstdc++/include

common_cflags := -O0 -g -Wall -fexceptions -std=c99
common_cflags += -Wno-unused -Wno-sign-compare -Wno-deprecated-declarations -Wno-cpp
common_cppflags := -frtti -Wno-sign-promo -fcheck-new
common_shm_cflags := -O0 -g -Wall -fexceptions -DHAVE_CONFIG_H -Wno-unused
common_ldflags := -ldl
common_c_includes := \
    $(JACK_ROOT) \
    $(JACK_ROOT)/common \
    $(JACK_ROOT)/common/jack \
    $(JACK_ROOT)/android \
    $(JACK_ROOT)/linux \
    $(JACK_ROOT)/linux/alsa \
    $(JACK_ROOT)/posix \
    $(JACK_STL_INCLUDES)

#----------------- libhelloworld_service begin
include $(CLEAR_VARS)

LOCAL_MODULE := libhelloworldservice
LOCAL_MODULE_PATH := $(TARGET_OUT)/lib
LOCAL_PRELINK_MODULE := false
LOCAL_MODULE_TAGS := eng optional
LOCAL_CFLAGS := $(common_cflags) -DSERVER_SIDE
LOCAL_LDFLAGS := $(common_ldflags)
LOCAL_C_INCLUDES := $(common_c_includes) LogHelper.h 
LOCAL_SHARED_LIBRARIES = $(BASE_SHARED_LIBARIES) libjack libjackserver libc libdl libcutils libutils libbinder

LOCAL_SRC_FILES := BnHelloWorldService.cpp BpHelloWorldService.cpp IHelloWorldService.cpp HelloWorldService.cpp Mixer_Main.cpp compressor.c \
Equalizer.cpp Biquad.cpp Filter.cpp StereoEnhancer.cpp Limiter.cpp #/Effects/ParametricEQ.cpp Mixer_Channel.cpp

include $(BUILD_SHARED_LIBRARY)
#----------------- helloworld_client begin
#include $(CLEAR_VARS)

#LOCAL_MODULE := helloworldclient
#LOCAL_MODULE_PATH := $(TARGET_OUT)/bin
#LOCAL_C_INCLUDES := $(common_c_includes)
#LOCAL_LDFLAGS := $(common_ldflags) $(JACK_STL_LDFLAGS)
#LOCAL_CFLAGS := $(common_cflags) -DSERVER_SIDE
#LOCAL_SHARED_LIBRARIES = $(BASE_SHARED_LIBARIES) libhelloworldservice
#LOCAL_MODULE_TAGS := eng optional
#LOCAL_SRC_FILES := HelloWorldClient.cpp

#include $(BUILD_EXECUTABLE)
#----------------- helloworld_client end
include $(CLEAR_VARS)

LOCAL_MODULE := helloworldserver
LOCAL_MODULE_PATH := $(TARGET_OUT)/bin
LOCAL_CFLAGS := $(common_cflags) -DSERVER_SIDE
LOCAL_LDFLAGS := $(common_ldflags)
LOCAL_C_INCLUDES := $(common_c_includes)
LOCAL_SHARED_LIBRARIES = $(BASE_SHARED_LIBARIES) libhelloworldservice
LOCAL_MODULE_TAGS := eng optional
LOCAL_SRC_FILES := HelloWorldServer.cpp

include $(BUILD_EXECUTABLE)
#----------------- helloworld_server end

