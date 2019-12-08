LOCAL_PATH:= $(call my-dir)

#
# libmediaplayerservice
#

include $(CLEAR_VARS)
include $(LOCAL_PATH)/../libcedarx/config.mk

LOCAL_SRC_FILES:=               \
    ActivityManager.cpp         \
    HDCP.cpp                    \
    MediaPlayerFactory.cpp      \
    MediaPlayerService.cpp      \
    MediaRecorderClient.cpp     \
    MetadataRetrieverClient.cpp \
    RemoteDisplay.cpp           \
    StagefrightRecorder.cpp     \
    TestPlayerStub.cpp          \
    SimpleMediaFormatProbe.cpp

LOCAL_SHARED_LIBRARIES :=       \
    libbinder                   \
    libcamera_client            \
    libcrypto                   \
    libcutils                   \
    libdrmframework             \
    liblog                      \
    libdl                       \
    libgui                      \
    libmedia                    \
    libmediautils               \
    libmemunreachable           \
    libsonivox                  \
    libstagefright              \
    libstagefright_foundation   \
    libstagefright_httplive     \
    libstagefright_omx          \
    libstagefright_wfd          \
    libutils                    \
    libvorbisidec               \
    libxplayer                  \
    libaw_output                \
    libawmetadataretriever      \
    libawplayer                 \
    libisomountmanagerservice

LOCAL_STATIC_LIBRARIES :=       \
    libstagefright_nuplayer     \
    libstagefright_rtsp         \
    libstagefright_timedtext    \

LOCAL_C_INCLUDES :=                                                 \
    $(TOP)/frameworks/av/media/libstagefright/include               \
    $(TOP)/frameworks/av/media/libstagefright/rtsp                  \
    $(TOP)/frameworks/av/media/libstagefright/wifi-display          \
    $(TOP)/frameworks/av/media/libstagefright/webm                  \
    $(TOP)/frameworks/av/include/media                              \
    $(TOP)/frameworks/av/include/camera                             \
    $(TOP)/frameworks/native/include/media/openmax                  \
    $(TOP)/frameworks/native/include/media/hardware                 \
    $(TOP)/external/tremolo/Tremolo                                 \
    libcore/include                                                 \
    $(TOP)/frameworks/av/media/libcedarx/android_adapter/awplayer/   \
    $(TOP)/frameworks/av/media/libcedarx/android_adapter/output/   \
    $(TOP)/frameworks/av/media/libcedarx/android_adapter/metadataretriever/       \
    $(TOP)/frameworks/av/media/libcedarc/include  \
    $(TOP)/hardware/aw/include \

LOCAL_CFLAGS += -Werror -Wno-error=deprecated-declarations -Wall
LOCAL_CLANG := true

LOCAL_MODULE:= libmediaplayerservice

LOCAL_32_BIT_ONLY := true

include $(BUILD_SHARED_LIBRARY)

include $(call all-makefiles-under,$(LOCAL_PATH))
