/*
 * sunxi DE2.0 Write-Back utils library
 * -- Write-Back interface for hwcomposer
 *
 * Copyright (C) 2015-2018 AllwinnerTech, Inc.
 *
 * Contacts:
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>

#include "log.h"
#include "hwc_capture.h"

#define ALIGN_SIZE  (4)
#define ALIGN(x,a)  (((x) + (a) - 1L) & ~((a) - 1L))

static int
hwc_capture_enable(struct hwc_capture_context *captr, capture_config_t *config) {

    int i;
    struct buffer_slot *slots;
    struct disp_capture_buffer *buffers;

    pthread_mutex_lock(&captr->mutex);
    if (captr->state == HWC_CAPTURE_ACTIVE)
        return 0;

    captr->dispfd = open("/dev/disp", O_RDWR);
    if (captr->dispfd < 0) {
        logerr("Can't open display device, %s\n", strerror(errno));
        pthread_mutex_unlock(&captr->mutex);
        return -EBUSY;
    }

    captr->format         = config->format;
    captr->capture_width  = config->width;
    captr->capture_height = config->height;
    slots = &captr->slots[0];
    buffers = &captr->buffers[0];

    /*
     * check cycle_before_release first,
     * > 0 : the allocated buffer is not release yet
     * = 0 :  no allocated buffer, allocate it
     */
    int alloc_width  = ALIGN(config->width, ALIGN_SIZE);
    int alloc_height = config->height;
    if (captr->cycle_before_release == 0) {
#ifndef IOMMU_ENABLE
        slots[0].ion_handle = captr->allocator->alloc(captr->allocator, -1,
                                    alloc_width, alloc_height * (MAX_BUFFER_COUNT));
        if (!slots[0].ion_handle) {
            logerr("Allocate physcial memory failed\n");
            goto _alloc_error;
        }
#else
		int error = 0;
		for (int i = 0; i < MAX_BUFFER_COUNT; i++) {
            slots[i].ion_handle = captr->allocator->alloc(captr->allocator, -1, alloc_width, alloc_height);
            if (!slots[0].ion_handle) {
                logerr("Allocate physcial memory failed\n");
                error = -1;
                break;
            }
        }
        if (error)
            goto _alloc_error;
#endif
    } else {
        logerr("cycle_before_release = %d.", captr->cycle_before_release);
        if (captr->cycle_before_release < 0)
            goto _alloc_error;
    }

#ifndef IOMMU_ENABLE
    for (i = 0; i < MAX_BUFFER_COUNT; i++) {
        buffers[i].handle  = i;
        buffers[i].format  = (disp_pixel_format)captr->format;
        buffers[i].crop.x  = buffers[i].crop.y = 0;
        buffers[i].width   = buffers[i].crop.width  = captr->capture_width;
        buffers[i].height  = buffers[i].crop.height = captr->capture_height;
        buffers[i].addr[0] = slots[0].ion_handle->phy_address + (alloc_width * alloc_height * 4) * i;
        if (captr->format == DISP_FORMAT_YUV420_P || captr->format == DISP_FORMAT_YUV420_SP_UVUV) {
            buffers[i].addr[1] = buffers[i].addr[0] + (alloc_width * alloc_height);
            buffers[i].addr[2] = buffers[i].addr[1] + (alloc_width * alloc_height);
        }

        slots[i].buf = &buffers[i];
    }
#else
    for (i = 0; i < MAX_BUFFER_COUNT; i++) {
        buffers[i].handle = i;
        buffers[i].format = (disp_pixel_format)captr->format;
        buffers[i].crop.x = buffers[i].crop.y = 0;
        buffers[i].width  = buffers[i].crop.width  = captr->capture_width;
        buffers[i].height = buffers[i].crop.height = captr->capture_height;
        buffers[i].fd     = slots[0].ion_handle->fd;
        buffers[i].size[0].width  = captr->capture_width;
        buffers[i].size[0].height = captr->capture_height;
        buffers[i].size[1].width  = captr->capture_width  / 2;
        buffers[i].size[1].height = captr->capture_height / 2;
        slots[i].buf = &buffers[i];
    }
#endif

    /*
     * queue buffers into DE's writeback module
     */
    unsigned long args[4];
    args[0] = captr->screen_id;
    args[1] = DISP_CAPTURE_E_BUFFER_LIST_INIT;
    args[2] = (unsigned long)buffers;
    args[3] = MAX_BUFFER_COUNT;

    if (ioctl(captr->dispfd, DISP_CAPTURE_EXTEND, args) != 0) {
        logerr("initial de capture failed\n");
        goto _alloc_error;
    }

    /*
     * start write-back operation
     */
    args[0] = captr->screen_id;
    args[1] = DISP_CAPTURE_E_CTRL;
    args[2] = 1;
    if (ioctl(captr->dispfd, DISP_CAPTURE_EXTEND, args) != 0) {
        logerr("start de capture failed\n");
        goto _start_wb_error;
    }

    captr->state = HWC_CAPTURE_ACTIVE;
    captr->cycle_before_release = 3;
    pthread_mutex_unlock(&captr->mutex);
    return 0;

_start_wb_error:
    /*
     * Clear buffer list from driver
     */
    args[0] = captr->screen_id;
    args[1] = DISP_CAPTURE_E_BUFFER_LIST_CLEAR;
    ioctl(captr->dispfd, DISP_CAPTURE_EXTEND, args);

_alloc_error:
    slots = &captr->slots[0];
    for (i = 0; i < MAX_BUFFER_COUNT; i++) {
        if (slots[i].ion_handle) {
            captr->allocator->free(captr->allocator, slots[i].ion_handle);
        }
    }
    pthread_mutex_unlock(&captr->mutex);
    return -ENOMEM;
}

static int
hwc_capture_resize(struct hwc_capture_context *captr) {
    (void)captr;
    return 0;
}

static int
hwc_capture_disable(struct hwc_capture_context *captr) {
    int i;
    unsigned long args[4];
    struct buffer_slot *slots;

    pthread_mutex_lock(&captr->mutex);
    if (captr->state == HWC_CAPTURE_ACTIVE) {
        /* stop write-back operation */
        args[0] = captr->screen_id;
        args[1] = DISP_CAPTURE_E_CTRL;
        args[2] = 0;
        if (ioctl(captr->dispfd, DISP_CAPTURE_EXTEND, args) != 0) {
            logerr("stop de capture failed\n");
            pthread_mutex_unlock(&captr->mutex);
            return -1;
        }

        /* clear capture buffer list and free */
        args[0] = captr->screen_id;
        args[1] = DISP_CAPTURE_E_BUFFER_LIST_CLEAR;
        ioctl(captr->dispfd, DISP_CAPTURE_EXTEND, args);

        captr->current.handle = -1;
        captr->state = HWC_CAPTURE_IDLE;
        close(captr->dispfd);
    } else if (captr->state == HWC_CAPTURE_IDLE) {
        logdebug("delay before release buffer");
        captr->cycle_before_release--;
        if (captr->cycle_before_release == 0) {
            slots = &captr->slots[0];
            for (i = 0; i < MAX_BUFFER_COUNT; i++) {
                if (slots[i].ion_handle) {
                    captr->allocator->free(captr->allocator, slots[i].ion_handle);
                    slots[i].ion_handle = 0;
                }
            }
            captr->state = HWC_CAPTURE_CLOSE;
            logdebug("release buffer");
        }
    }

    pthread_mutex_unlock(&captr->mutex);
    return 0;
}

static int
hwc_capture_buffer_refs_count(struct hwc_capture_context *captr) {
    return captr->cycle_before_release;
}

static int
hwc_capture_buffer_refs_dec(struct hwc_capture_context *captr) {
    if (captr->cycle_before_release)
        captr->cycle_before_release--;

    if (captr->cycle_before_release == 0) {
        struct buffer_slot *slots = &captr->slots[0];
        for (int i = 0; i < MAX_BUFFER_COUNT; i++) {
            if (slots[i].ion_handle) {
                captr->allocator->free(captr->allocator, slots[i].ion_handle);
                slots[i].ion_handle = 0;
            }
        }
        captr->state = HWC_CAPTURE_CLOSE;
        logdebug("release buffer");
    }
    return captr->cycle_before_release;
}

static int
hwc_capture_state(struct hwc_capture_context *captr) {
    return (captr->state == HWC_CAPTURE_ACTIVE);
}

static int
hwc_capture_acquire_buffer(struct hwc_capture_context *captr, struct disp_capture_handle *buf) {
    unsigned long args[4];
    args[0] = captr->screen_id;
    args[1] = DISP_CAPTURE_E_ACQUIRE_BUFFER;
    args[2] = (unsigned long)buf;
    args[3] = 0;
    if (ioctl(captr->dispfd, DISP_CAPTURE_EXTEND, args) != 0) {
        logerr("acquire buffer error, %s\n", strerror(errno));
        return -ENOMEM;
    }

    if (buf->handle < 0) {
        logerr("invalid buffer handle!\n");
        return -EINVAL;
    }
    return 0;
}

static int
hwc_capture_release_buffer(struct hwc_capture_context *captr, struct disp_capture_handle *buf) {
    unsigned long args[4];

    if (buf->handle < 0 || buf->handle >= MAX_BUFFER_COUNT)
        return -EINVAL;

    args[0] = captr->screen_id;
    args[1] = DISP_CAPTURE_E_RELEASE_BUFFER;
    args[2] = (unsigned long)buf;
    if (ioctl(captr->dispfd, DISP_CAPTURE_EXTEND, args) != 0) {
        logerr("release buffer error, %s\n", strerror(errno));
        return -1;
    }
    return 0;
}

static int hwc_capture_stop(void) {
    unsigned long args[4];
    int fd = open("/dev/disp", O_RDWR);
    if (fd < 0) {
        logerr("Can't open display device, %s\n", strerror(errno));
        return -EBUSY;
    }
	/* stop write-back operation */
	args[0] = 0;
	args[1] = DISP_CAPTURE_E_CTRL;
	args[2] = 0;
	if (ioctl(fd, DISP_CAPTURE_EXTEND, args) != 0) {
		logerr("stop de capture failed\n");
		close(fd);
		return -1;
	}

	/* clear capture buffer list and free */
	args[0] = 0;
	args[1] = DISP_CAPTURE_E_BUFFER_LIST_CLEAR;
	ioctl(fd, DISP_CAPTURE_EXTEND, args);
	close(fd);
	return 0;
}

struct hwc_capture_context *
hwc_capture_init(int screen_id) {
    struct hwc_capture_context *captr;

    hwc_capture_stop();
    captr = (struct hwc_capture_context *)malloc(sizeof(*captr));
    if (!captr) {
        logerr("Can't alloc memory for capture modeule, %s\n",
            strerror(errno));
        return NULL;
    }

    memset(captr, 0, sizeof(*captr));
    captr->screen_id = screen_id;
    captr->allocator = buffer_allocator_initial();
    if (!captr->allocator) {
        logerr("Can't initial buffer allocator for capture modeule\n");
        free(captr);
        return NULL;
    }
    captr->enable = hwc_capture_enable;
    captr->resize = hwc_capture_resize;
    captr->disable = hwc_capture_disable;
    captr->is_enable = hwc_capture_state;
    captr->acquire_buffer = hwc_capture_acquire_buffer;
    captr->release_buffer = hwc_capture_release_buffer;

    captr->buffer_refs_count = hwc_capture_buffer_refs_count;
    captr->buffer_refs_decrease = hwc_capture_buffer_refs_dec;

    captr->cycle_before_release = 0;
    captr->current.handle = -1;

    pthread_mutex_init(&captr->mutex, NULL);
    return captr;
}

