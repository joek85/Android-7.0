/*
 * sunxi DE2.0 Write-Back utils library
 * -- Physically contiguous memory allocator interface
 *
 * Copyright (C) 2015-2018 AllwinnerTech, Inc.
 *
 * Contacts:
 * Zeng.Yajian <zengyajian@allwinnertech.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <linux/ion.h>
#include <ion/ion.h>

#include "log.h"
#include "hwc_capture.h"

#define ION_ALLOC_ALIGN		(1024 * 4)
#define DEV_NAME			"/dev/ion"

struct ion_allocator {
	struct buffer_allocator interface;
	int fd;
	int ref;
};

struct buffer_node {
	struct buffer_handle handle;
	ion_user_handle_t ion_handle;

#ifdef _WRITE_BACK_DEBUG_
	unsigned char *vir_address;
	int mapfd;
#endif
};

static struct buffer_handle *
	__buffer_alloc(
	struct buffer_allocator *allocator, int format, int width, int height);
static int
	__buffer_free(struct buffer_allocator *allocator, struct buffer_handle *handle);

struct buffer_allocator * buffer_allocator_initial(void)
{
	int fd = ion_open();
	if (fd < 0) {
		logerr("ion open error!\n");
		return NULL;
	}

	struct ion_allocator *allocator =
			(struct ion_allocator *)malloc(sizeof(struct ion_allocator));
	if (!allocator) {
		close(fd);
		return NULL;
	}

	allocator->fd = fd;
	allocator->ref = 0;
	allocator->interface.alloc = __buffer_alloc;
	allocator->interface.free  = __buffer_free;
	return &(allocator->interface);
}

int buffer_allocator_destroy(struct buffer_allocator *allocator)
{
	struct ion_allocator *alloc = (struct ion_allocator *)allocator;
	if (!alloc || alloc->ref) {
		loginfo("%s: null or reference error, cant't destroy buffer allocator\n", __func__);
		return -1;
	}
	ion_close(alloc->fd);
	free(alloc);
	return 0;
}

static struct buffer_handle * __buffer_alloc(struct buffer_allocator *allocator, int format,
	int width, int height)
{
	int retval;
	int size = width * height * 4;
	struct ion_allocator *alloc = (struct ion_allocator *)allocator;
	struct buffer_node *node = (struct buffer_node *)malloc(sizeof(struct buffer_node));

	if (!alloc || (size <= 0) || !node)
		return NULL;

#ifndef IOMMU_ENABLE
	/* ION_HEAP_CARVEOUT_MASK is not support in suni50w2p1 platform */
	retval = ion_alloc(alloc->fd, size, ION_ALLOC_ALIGN,
						ION_HEAP_TYPE_DMA_MASK, 0, &node->ion_handle);
#else
	retval = ion_alloc(alloc->fd, size, ION_ALLOC_ALIGN,
						ION_HEAP_SYSTEM_MASK, 0, &node->ion_handle);
#endif
	if (retval) {
		logerr("ion alloc failed, size %d\n", size);
		free(node);
		return NULL;
	}
	alloc->ref++;
	node->handle.size = size;
#ifdef IOMMU_ENABLE
	/* int ion_share(int fd, ion_user_handle_t handle, int *share_fd) */
	ion_share(alloc->fd, node->ion_handle, &node->handle.fd);
#else
	node->handle.phy_address = ion_getphyadr(alloc->fd, node->ion_handle);
#endif

#ifdef _WRITE_BACK_DEBUG_
	unsigned char *ptr;
	int map_fd = -1;
	retval = ion_map(alloc->fd, node->ion_handle, size,
				PROT_READ | PROT_WRITE, MAP_SHARED, 0, &ptr, &map_fd);
	if (retval != 0) {
		logerr("ion map failed, '%s'\n", strerror(errno));
		goto __out;
	}
	node->vir_address = ptr;
	node->mapfd = map_fd;
__out:
#endif
	return &(node->handle);
}

static int __buffer_free(struct buffer_allocator *allocator, struct buffer_handle *handle)
{
	struct ion_allocator *alloc = (struct ion_allocator *)allocator;
	struct buffer_node *node = (struct buffer_node *)handle;
	if (!alloc || !node)
		return -1;

	alloc->ref--;
	close(handle->fd);
	ion_free(alloc->fd, node->ion_handle);
#ifdef _WRITE_BACK_DEBUG_
	munmap(node->vir_address, node->size);
	close(node->mapfd);
#endif
	free(node);
	return 0;
}

