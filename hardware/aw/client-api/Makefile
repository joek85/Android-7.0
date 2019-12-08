CC=arm-linux-gnueabihf-gcc
AR=arm-linux-gnueabihf-ar

CFLAGS:=-Werror -O2 -D__OS_LINUX

TARGET_DIR=.

LOCAL_SRC_FILES:= \
	sunxi_tee_api.c

BUILD_STATIC_LIB:=1

ifeq ($(BUILD_STATIC_LIB),1)
LIB= $(TARGET_DIR)/libtee_client.a
else
LIB= $(TARGET_DIR)/libtee_client.so
CFLAGS+=-fPIC
endif


OBJS=$(addsuffix .o, $(basename $(LOCAL_SRC_FILES)))
all:$(LIB)

obj:$(OBJS)

%.o:%.c
	$(CC) $(CFLAGS) -o $@ -c $<

%.o:%.S
	$(CC) $(CFLAGS) -o $@ -c $<

ifeq ($(BUILD_STATIC_LIB),1)
$(LIB):$(OBJS)
	$(AR) $(ARFLAGS) $@ $^
else
$(LIB):$(OBJS)
	$(CC) -shared -o $@ $^
endif

test:
	$(CC) -static test_api.c $(CFLAGS) -ltee_client -lpthread -L./ -o test_api

clean:
	rm -rf $(OBJS) $(LIB)
	rm -rf test_api.o
