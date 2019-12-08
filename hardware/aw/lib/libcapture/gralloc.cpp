/*
 * sunxi DE2.0 Write-Back utils library
 * -- Physically contiguous memory allocator interface
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

#include <hardware/hardware.h>
#include <hardware/gralloc.h>
#include <hardware/hwcomposer.h>

#include <cutils/native_handle.h>
#include <cutils/native_handle.h>
#include <cutils/log.h>

#include <hardware/hal_public.h>
#include <linux/ion.h>
#include <ion/ion.h>
#include <ion/sunxi_ion.h>

#include "log.h"
#include "hwc_capture.h"

#define MAX_ALLOC_COUNT (16)

struct buffer_node {
    struct buffer_handle handle;

    struct ANativeWindowBuffer *buffer;
    ion_user_handle_t ion_handle;
};

struct gralloc_proxy {
    struct buffer_allocator allocator;

    int ion_fd;
    hw_module_t const *module;
    alloc_device_t *gralloc;

    struct buffer_node *reference[MAX_ALLOC_COUNT];
};

static struct buffer_handle *__buffer_alloc(
            struct buffer_allocator *allocator, int format, int width, int height);
static int __buffer_free(
            struct buffer_allocator *allocator, struct buffer_handle *handle);

struct buffer_allocator *buffer_allocator_initial(void)
{
    int err;
    struct gralloc_proxy *proxy;

    proxy = new gralloc_proxy();
    if (!proxy) {
        logerr("malloc %d bytes failed", sizeof(*proxy));
        return NULL;
    }
    memset(proxy, 0, sizeof(*proxy));

    proxy->ion_fd = ion_open();
    if (proxy->ion_fd < 0) {
        logerr("ion open error");
        goto __error_ion;
    }

    if (hw_get_module(GRALLOC_HARDWARE_MODULE_ID, &proxy->module) != 0) {
        logerr("can't open gralloc module\n");
        goto __error_hw_module;
    }

    err = gralloc_open(proxy->module, &proxy->gralloc);
    if (err) {
        logerr("can't open gralloc HAL (%s)\n", strerror(-err));
        goto __error_gralloc;
    }

    proxy->allocator.alloc = __buffer_alloc;
    proxy->allocator.free = __buffer_free;
    return &proxy->allocator;

__error_gralloc:
__error_hw_module:
    ion_close(proxy->ion_fd);
__error_ion:
    delete proxy;
    return NULL;
}

int buffer_allocator_destroy(struct buffer_allocator *allocator)
{
    struct gralloc_proxy *proxy = (struct gralloc_proxy *)allocator;
    gralloc_close(proxy->gralloc);
    ion_close(proxy->ion_fd);
    delete(proxy);
    return 0;
}

static unsigned int __get_phyadr(int fd, ion_user_handle_t handle) {
	int ret = 0;
	struct ion_custom_data custom_data;
	sunxi_phys_data phys_data;
	custom_data.cmd = ION_IOC_SUNXI_PHYS_ADDR;
	phys_data.handle = handle;
	custom_data.arg = (unsigned long)&phys_data;
	if (ioctl(fd, ION_IOC_CUSTOM, &custom_data) < 0)
		return 0;
	return phys_data.phys_addr;
}

static struct buffer_handle *__buffer_alloc(struct buffer_allocator *allocator,
                        int format, int width, int height)
{
    int err;
    int usage;
    struct buffer_node *node;
    struct gralloc_proxy *proxy = (struct gralloc_proxy *)allocator;

    node = new buffer_node();
    memset(node, 0, sizeof(*node));
    node->buffer = new ANativeWindowBuffer();

    format = format > 0 ? format : HAL_PIXEL_FORMAT_RGBA_8888;
    usage = GRALLOC_USAGE_HW_COMPOSER
            | GRALLOC_USAGE_SW_WRITE_NEVER | GRALLOC_USAGE_SW_READ_NEVER;
    err = proxy->gralloc->alloc(proxy->gralloc, width, height,
                    format, usage, &node->buffer->handle, &node->buffer->stride);
    if (err) {
        logerr("gralloc of %d x %d, format %d, failed\n", width, height, format);
        delete node->buffer;
        delete node;
        return NULL;
    }

    struct private_handle_t *handle = (private_handle_t *) node->buffer->handle;
    ion_import(proxy->ion_fd, handle->share_fd, &node->ion_handle);
    node->handle.phy_address = __get_phyadr(proxy->ion_fd, node->ion_handle);

    /*
     * FIXME
     */
    node->handle.size = width * height * 4;

    return &node->handle;
}

static int __buffer_free(struct buffer_allocator *allocator,
             struct buffer_handle *handle)
{
    struct buffer_node *node = (struct buffer_node *)handle;
    struct gralloc_proxy *proxy = (struct gralloc_proxy *)allocator;

    ion_free(proxy->ion_fd, node->ion_handle);
    proxy->gralloc->free(proxy->gralloc, node->buffer->handle);

    delete node->buffer;
    delete node;
    return 0;
}
