/* Copyright (c) Imagination Technologies Ltd.
 *
 * The contents of this file are subject to the MIT license as set out below.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#ifndef HAL_PUBLIC_6230_H
#define HAL_PUBLIC_6230_H

/* Authors of third party hardware composer (HWC) modules will need to include
 * this header to access functionality in the gralloc and framebuffer HALs.
 */

#include <hardware/gralloc.h>

#define ALIGN(x,a)	(((x) + (a) - 1L) & ~((a) - 1L))
#define HW_ALIGN	32

/* This can be tuned down as appropriate for the SOC.
 *
 * IMG formats are usually a single sub-alloc.
 * Some OEM video formats are two sub-allocs (Y, UV planes).
 * Future OEM video formats might be three sub-allocs (Y, U, V planes).
 */
#define MAX_SUB_ALLOCS 3


/* This defines the maximum server sync objects used per allocation. */

/* Note: It's unfortunate that we have to change the handle size dependent
 * on a build option, but we have no choice because 'fd' fields must all
 * be utilized so they are valid to be dup'ed, and we don't need some of
 * the extra fds in a native_fence_sync build.
 */
#define PVR_ANDROID_NATIVE_WINDOW_HAS_SYNC
#if defined(PVR_ANDROID_NATIVE_WINDOW_HAS_SYNC)
#define MAX_SRV_SYNC_OBJS    2
#else
#define MAX_SRV_SYNC_OBJS    4
#endif

typedef struct
{
	native_handle_t base;

	/* These fields can be sent cross process. They are also valid
	 * to duplicate within the same process.
	 *
	 * A table is stored within psPrivateData on gralloc_module_t (this
	 * is obviously per-process) which maps stamps to a mapped
	 * PVRSRV_MEMDESC in that process. Each map entry has a lock
	 * count associated with it, satisfying the requirements of the
	 * Android API. This also prevents us from leaking maps/allocations.
	 *
	 * This table has entries inserted either by alloc()
	 * (alloc_device_t) or map() (gralloc_module_t). Entries are removed
	 * by free() (alloc_device_t) and unmap() (gralloc_module_t).
	 *
	 * As a special case for framebuffer_device_t, framebuffer_open()
	 * will add and framebuffer_close() will remove from this table.
	 */

#define IMG_NATIVE_HANDLE_NUMFDS (MAX_SRV_SYNC_OBJS + MAX_SUB_ALLOCS)
	/* The `syncfd' field is used to export PVRSRV_CLIENT_SYNC_PRIM to
	 * another process. Its producer/consumer rules should match the
	 * PVRSRV_MEMDESC handles, except that there is only one sync
	 * per N memdesc objects.
	 *
	 * This should be listed before `fd' because it is not variable
	 * width. The problem with variable width is that in the case we
	 * export framebuffer allocations, we may want to patch some of
	 * the fds to (unused) ints, so we can't leave gaps.
	 */
	int aiSyncFD[MAX_SRV_SYNC_OBJS];

	/* The `fd' field is used to "export" a meminfo to another process.
	 * Therefore, it is allocated by alloc_device_t, and consumed by
	 * gralloc_module_t. The framebuffer_device_t does not need a handle,
	 * and the special value IMG_FRAMEBUFFER_FD is used instead.
	 */
	int fd[MAX_SUB_ALLOCS];

#define IMG_NATIVE_HANDLE_NUMINTS ((sizeof(unsigned long long) / sizeof(int)) + 5)
	/* A KERNEL unique identifier for any exported kernel meminfo. Each
	 * exported kernel meminfo will have a unique stamp, but note that in
	 * userspace, several meminfos across multiple processes could have
	 * the same stamp. As the native_handle can be dup(2)'d, there could be
	 * multiple handles with the same stamp but different file descriptors.
	 */
	unsigned long long ui64Stamp;

	/* This is used for buffer usage validation when locking a buffer,
	 * and also in WSEGL (for the composition bypass feature).
	 */
	int usage;

	/* In order to do efficient cache flushes we need the buffer dimensions
	 * and format. These are available on the ANativeWindowBuffer,
	 * but the platform doesn't pass them down to the graphics HAL.
	 *
	 * These fields are also used in the composition bypass. In this
	 * capacity, these are the "real" values for the backing allocation.
	 */
	int iWidth;
	int iHeight;
	int iFormat;
	unsigned int uiBpp;
}
__attribute__((aligned(sizeof(int)),packed)) IMG_native_handle_t;

typedef struct
{
	int l, t, w, h;
}
IMG_write_lock_rect_t;

/* Keep this in sync with SGX */
typedef int (*IMG_buffer_format_compute_params_pfn)(
	unsigned int uiPlane, int *piWidth, int *piHeight, int *piStride,
	int *piVStride, unsigned long *pulPlaneOffset);

/* Keep this in sync with SGX */
typedef struct IMG_buffer_format_public_t
{
	/* Buffer formats are returned as a linked list */
	struct IMG_buffer_format_public_t *psNext;

	/* HAL_PIXEL_FORMAT_... enumerant */
	int iHalPixelFormat;

	/* IMG_PIXFMT_... enumerant */
	int iIMGPixelFormat;

	/* Friendly name for format */
	const char *const szName;

	/* Bits (not bytes) per pixel */
	unsigned int uiBpp;

	/* Supported HW usage bits. If this is GRALLOC_USAGE_HW_MASK, all usages
	 * are supported. Used for HAL_PIXEL_FORMAT_IMPLEMENTATION_DEFINED.
	 */
	int iSupportedUsage;

	/* YUV output format */
	int bIsYUVFormat;

	/* TRUE if U/Cb follows Y, FALSE if V/Cr follows Y */
	int bUVCbCrOrdering;

	/* Utility function for adjusting YUV per-plane parameters */
	IMG_buffer_format_compute_params_pfn pfnComputeParams;
}
IMG_buffer_format_public_t;

#if defined(SUPPORT_ANDROID_FRAMEBUFFER_HAL)

#include <hardware/hwcomposer.h>

/* This structure loosely mirrors the inner fields of hwc_layer_t.
 * It should be kept in sync, as the HWC interface grows.
 *
 * We don't support HWC_BACKGROUND right now, so we don't need the
 * `compositionType' field. The HWC implementation should handle
 * `hints' and `flags', so we don't include those either.
 *
 * For more information about what these fields mean, look in:
 * hardware/libhardware/include/hardware/hwcomposer.h
 *
 * SurfaceFlinger will sort the hwc_layer_t by z-order when passing
 * them to the hardware composer. The Post2() interface also requires
 * that layers are in z-order, from furthest (lowest) z-order first to
 * nearest (highest) z-order last.
 *
 * NOTE: The `custom' field is new here. This will be passed through
 * to your display engine in the ui32Custom field of the CONFIG_INFO.
 *
 * It can be any arbitrary word-sized data, however conventionally
 * it would be the overlay index (pipe index), if the display engine
 * differentiates between pipe order and z-order.
 */
typedef struct
{
	/* Handle of buffer to compose */
	buffer_handle_t handle;

	/* Transform to apply to buffer during composition */
	uint32_t transform;

	/* Blending to apply during composition */
	int32_t blending;

	/* Area of the source to consider */
	hwc_rect_t sourceCrop;

	/* Where to composite source crop on the display.
	 * NOTE: This is how to exploit scaling and translation.
	 */
	hwc_rect_t displayFrame;

	/* Custom data for the display engine */
	uint32_t custom;
}
IMG_hwc_layer_t;

typedef struct
{
	framebuffer_device_t base;

	/* The HWC was loaded. post() is no longer responsible for presents */
	int bBypassPost;

	/* Maximum number of discrete display pipes / overlay planes */
	unsigned int uiMaxPipes;

	/* Check to see if a DC configuration will apply based on a
	 * HWC layer list.
	 *
	 * Returns <0: If there was an internal graphics HAL consistency problem.
	 * Returns  0: If the configuration would apply.
	 * Returns >0: If the code should be cast to PVRSRV_ERROR (provides more
	 *             information about why the config failed to apply.
	 */
	int (*Post2Check)(framebuffer_device_t *fb, IMG_hwc_layer_t *layers,
					  int num_layers);

	/* Apply a DC configuration based on HWC layer list. */
	int (*Post2)(framebuffer_device_t *fb, IMG_hwc_layer_t *layers,
				 int num_layers);
}
IMG_framebuffer_device_public_t;

#endif /* defined(SUPPORT_ANDROID_FRAMEBUFFER_HAL) */

typedef struct IMG_gralloc_module_public_t
{
	gralloc_module_t base;

#if defined(SUPPORT_ANDROID_FRAMEBUFFER_HAL)
	/* If the framebuffer has been opened, this will point to the
	 * framebuffer device data required by the allocator, WSEGL
	 * modules and composerhal.
	 */
	IMG_framebuffer_device_public_t *psFrameBufferDevice;
#endif /* defined(SUPPORT_ANDROID_FRAMEBUFFER_HAL) */

	const IMG_buffer_format_public_t *(*GetBufferFormats)(void);

	/* Functionality before this point should be in sync with SGX.
	 * After this point will be different.
	 */

	/* Custom-blit components in lieu of overlay hardware */
	int (*Blit)(struct IMG_gralloc_module_public_t const *module,
				 buffer_handle_t src, buffer_handle_t dest,
				 int w, int h, int x, int y,
				 int transform,
				 int async);

	int (*Blit3)(struct IMG_gralloc_module_public_t const *module,
				 unsigned long long ui64SrcStamp, int iSrcWidth,
				 int iSrcHeight, int iSrcFormat, int eSrcRotation,
				 buffer_handle_t dest, int eDestRotation);
}
IMG_gralloc_module_public_t;

#endif /* HAL_PUBLIC_H */
