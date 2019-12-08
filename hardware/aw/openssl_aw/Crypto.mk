
#######################################
# target shared library
include $(CLEAR_VARS)
LOCAL_SHARED_LIBRARIES := $(log_shared_libraries)

# If we're building an unbundled build, don't try to use clang since it's not
# in the NDK yet. This can be removed when a clang version that is fast enough
# in the NDK.
ifeq (,$(TARGET_BUILD_APPS))
LOCAL_CLANG := true
ifeq ($(HOST_OS), darwin)
LOCAL_ASFLAGS += -no-integrated-as
LOCAL_CFLAGS += -no-integrated-as
endif
else
LOCAL_SDK_VERSION := 9
endif
LOCAL_LDFLAGS += -ldl

LOCAL_MODULE_TAGS := optional
LOCAL_MODULE := libcrypto_aw
LOCAL_ADDITIONAL_DEPENDENCIES := $(LOCAL_PATH)/android-config.mk $(LOCAL_PATH)/Crypto.mk
include $(LOCAL_PATH)/Crypto-config-target.mk
include $(LOCAL_PATH)/android-config.mk
include $(BUILD_SHARED_LIBRARY)

#######################################
# target static library
include $(CLEAR_VARS)
include $(LOCAL_PATH)/android-config.mk

LOCAL_SHARED_LIBRARIES := $(log_shared_libraries)

# If we're building an unbundled build, don't try to use clang since it's not
# in the NDK yet. This can be removed when a clang version that is fast enough
# in the NDK.
ifeq (,$(TARGET_BUILD_APPS))
LOCAL_CLANG := true
else
LOCAL_SDK_VERSION := 9
endif

LOCAL_MODULE_TAGS := optional
LOCAL_MODULE:= libcrypto_aw_static
LOCAL_ADDITIONAL_DEPENDENCIES := $(LOCAL_PATH)/android-config.mk $(LOCAL_PATH)/Crypto.mk
include $(LOCAL_PATH)/Crypto-config-target.mk
include $(LOCAL_PATH)/android-config.mk
include $(BUILD_STATIC_LIBRARY)