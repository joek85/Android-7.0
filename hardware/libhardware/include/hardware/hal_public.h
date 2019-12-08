#ifndef  HAL_PUBLIC_H
#define  HAL_PUBLIC_H

#define SW_JAWS 0x00
#define SW_EAGLE 0x02
#define SW_DOLPHIN 0x03
#define SW_RABBIT 0x04
#define SW_CHEETAH 0x05
#define SW_PETREL  0x06

#if (SW_CHIP_PLATFORM == SW_DOLPHIN \
     || SW_CHIP_PLATFORM == SW_RABBIT \
     || SW_CHIP_PLATFORM == SW_CHEETAH)
#include "hal_public/hal_mali_utgard.h"
#elif (SW_CHIP_PLATFORM == SW_PETREL)
#include "hal_public/hal_mali_midgard.h"
#elif (SW_CHIP_PLATFORM == SW_EAGLE)
#include "hal_public/hal_img_sgx544.h"
#elif (SW_CHIP_PLATFORM == SW_JAWS)
#include "hal_public/hal_img_rgx6230.h"
#else
#error "please select a platform\n"
#endif

#endif
