## do not modify this file
## PLAYREADY_CFLAGS *MUST* be included in CFLAGS

PLAYREADY_CFLAGS +=						\
	-DDRM_BUILD_PROFILE=1100            \
	-DARM=1                             \
	-DBOARD_USE_PLAYREADY=${BOARD_USE_PLAYREADY} \
