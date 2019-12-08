LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_SRC_FILES:= \
	sunxi_tee_api.c

LOCAL_SHARED_LIBRARIES:=\
		libcutils \
		libutils

LOCAL_MODULE_TAGS := optional

LOCAL_MODULE:= libtee_client

include $(BUILD_SHARED_LIBRARY)
################################
include $(CLEAR_VARS)

LOCAL_SRC_FILES:= \
	test_api.c

LOCAL_SHARED_LIBRARIES:=\
	libtee_client \
	libcutils \
	libutils

LOCAL_MODULE_TAGS := optional

LOCAL_MODULE:= test_api

#include $(BUILD_EXECUTABLE)
