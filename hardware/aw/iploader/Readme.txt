
1. Add the dlhal static library ( libip_hal.a ) to your Android.mk
		LOCAL_STATIC_LIBRARIES += libip_hal

2. Link the share library to your Android.mk
		LOCAL_SHARED_LIBRARIES += libip_loader

3. Refer the sample.c for the allwinner dl APIs

