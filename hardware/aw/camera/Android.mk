
LOCAL_PATH := $(call my-dir)
$(warning $(TARGET_BOARD_PLATFORM))

include $(CLEAR_VARS)
LOCAL_MODULE := libfacedetection
LOCAL_MODULE_SUFFIX := .so
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SRC_FILES_32 := lib32/facedetection/libfacedetection.so
LOCAL_SRC_FILES_64 := lib32/facedetection/libfacedetection.so
LOCAL_MULTILIB:= both
LOCAL_MODULE_TAGS := optional
include $(BUILD_PREBUILT)
include $(CLEAR_VARS)
LOCAL_MODULE := libSmileEyeBlink
LOCAL_MODULE_SUFFIX := .so
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SRC_FILES_32 := lib32/facedetection/libSmileEyeBlink.so
LOCAL_SRC_FILES_64 := lib32/facedetection/libSmileEyeBlink.so
LOCAL_MULTILIB:= both
LOCAL_MODULE_TAGS := optional
include $(BUILD_PREBUILT)
include $(CLEAR_VARS)
LOCAL_MODULE := libapperceivepeople
LOCAL_MODULE_SUFFIX := .so
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SRC_FILES_32 := lib32/facedetection/libapperceivepeople.so
LOCAL_SRC_FILES_64 := lib32/facedetection/libapperceivepeople.so
LOCAL_MULTILIB:= both
LOCAL_MODULE_TAGS := optional
include $(BUILD_PREBUILT)
include $(CLEAR_VARS)

#LOCAL_MODULE_PATH := $(TARGET_OUT_SHARED_LIBRARIES)/hw
LOCAL_MODULE_RELATIVE_PATH := hw


LOCAL_SHARED_LIBRARIES:= \
    libbinder \
    libutils \
    libcutils \
    libcamera_client \
    libui

# cedarx libraries
LOCAL_SHARED_LIBRARIES += \
	libvencoder \
	libvdecoder \
	libfacedetection \
	libSmileEyeBlink \
	libapperceivepeople

LOCAL_C_INCLUDES += \
	frameworks/base/core/jni/android/graphics 		\
	frameworks/native/include/media/openmax			\
	hardware/libhardware/include/hardware			\
	frameworks/native/include						\
	frameworks/av/media/libcedarc/include \
	frameworks/av/media/libcedarc/include \
	frameworks/native/include/media/hardware \
	system/core/include/camera \
	device/softwinner/common/hardware/camera/libfacedetection \
	hardware/aw/include \
        system/media/camera/include

LOCAL_SRC_FILES := \
	memory/memoryAdapter.c \
	memory/ionMemory/ionAlloc.c \
	HALCameraFactory.cpp \
	PreviewWindow.cpp \
	CallbackNotifier.cpp \
	CCameraConfig.cpp \
	BufferListManager.cpp \
	OSAL_Mutex.c \
	OSAL_Queue.c \
	scaler.c \
	Libve_Decoder2.c \
	CameraHardware2.cpp \
	V4L2CameraDevice2.cpp

#use the csi vin driver
#LOCAL_CFLAGS += -DUSE_CSI_VIN_DRIVER

#for memoryAdapter to use the linux 3.10
LOCAL_CFLAGS += -DCONF_KERNEL_VERSION_3_10

#open IOMMU to palloc memory
#LOCAL_CFLAGS += -DCONF_USE_IOMMU

ifneq ($(filter nuclear%,$(TARGET_DEVICE)),)
LOCAL_CFLAGS += -D__SUN5I__
endif

ifneq ($(filter crane%,$(TARGET_DEVICE)),)
LOCAL_CFLAGS += -D__SUN4I__
endif

ifneq ($(filter fiber%,$(TARGET_DEVICE)),)
LOCAL_CFLAGS += -D__SUN6I__
endif

ifneq ($(filter wing%,$(TARGET_DEVICE)),)
LOCAL_CFLAGS += -D__SUN7I__
endif

ifneq ($(filter jaws%,$(TARGET_DEVICE)),)
LOCAL_CFLAGS += -D__SUN9I__
endif

LOCAL_MODULE := camera.$(TARGET_BOARD_PLATFORM)
$(warning $(LOCAL_MODULE))

LOCAL_MODULE_TAGS := optional
include $(BUILD_SHARED_LIBRARY)
