LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)
 
LOCAL_SRC_FILES:=\
	Si4703_Breakout.cpp
 
LOCAL_CFLAGS:=-O2 -g
#LOCAL_CFLAGS+=-DLINUX
 
LOCAL_MODULE_TAGS := eng
 
#LOCAL_MODULE_PATH := $(TARGET_OUT_OPTIONAL_EXECUTABLES)
 
LOCAL_MODULE:= libSi4703

LOCAL_CFLAGS += -std=c99
# gold in binutils 2.22 will warn about the usage of mktemp
LOCAL_LDFLAGS += -Wl,--no-fatal-warnings
 
include $(BUILD_SHARED_LIBRARY)


include $(CLEAR_VARS)
 
LOCAL_SRC_FILES:=\
	example/Radio.cpp
 
LOCAL_CFLAGS:=-O2 -g
#LOCAL_CFLAGS+=-DLINUX
 
LOCAL_MODULE_TAGS := eng
 
LOCAL_MODULE_PATH := $(TARGET_OUT_OPTIONAL_EXECUTABLES)
 
LOCAL_MODULE:=radio
LOCAL_SHARED_LIBRARIES += libSi4703
#LOCAL_CFLAGS += -std=c99
# gold in binutils 2.22 will warn about the usage of mktemp
LOCAL_LDFLAGS += -Wl,--no-fatal-warnings
 
include $(BUILD_EXECUTABLE)
