#########################################################################
# COMMON COMPILATION FLAGS                                              #
#########################################################################

CROSS_COMPILE   ?= $(COMPILER_DIR)/aarch64-linux-gnu-
CC              := $(CROSS_COMPILE)gcc

CFLAGS          := -Wall -Wbad-function-cast -Wcast-align \
		   -Werror-implicit-function-declaration -Wextra \
		   -Wfloat-equal -Wformat-nonliteral -Wformat-security \
		   -Wformat=2 -Winit-self -Wmissing-declarations \
		   -Wmissing-format-attribute \
		   -Wmissing-noreturn -Wmissing-prototypes -Wnested-externs \
		   -Wpointer-arith -Wshadow -Wstrict-prototypes \
		   -Wswitch-default -Wunsafe-loop-optimizations \
		   -Wwrite-strings -Werror
#CFLAGS          += -Wmissing-include-dirs
CFLAGS          += -c -fPIC

DEBUG       ?= 0
ifeq ($(DEBUG), 1)
CFLAGS          += -DDEBUG -O0 -g
endif

RM              := rm -rf
