
#ifndef __HWC_WRITEBACK_H__
#define __HWC_WRITEBACK_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdint.h>
#include <linux/ion.h>
#include "sunxi_display2.h"

#define IOMMU_ENABLE        1

#define MAX_BUFFER_COUNT    4

#define HWC_CAPTURE_ACTIVE  1
#define HWC_CAPTURE_IDLE    2
#define HWC_CAPTURE_CLOSE   3

struct buffer_handle;
struct buffer_slot {
    struct disp_capture_buffer *buf;
    int state;
    uint64_t timestamp;

    struct buffer_handle *ion_handle;
};

struct buffer_handle {
    int size;
#ifdef IOMMU_ENABLE
    int fd;
#else
	int phy_address;
#endif
};

struct buffer_allocator {
    struct buffer_handle * (*alloc)(struct buffer_allocator *allocator, int format, int width, int height);
    int (*free)(struct buffer_allocator *allocator, struct buffer_handle *handle);
};

typedef struct capture_config {
    int format;
    int width;
    int height;
    int buffer_count;
} capture_config_t;

struct hwc_capture_context {
    int dispfd;
    int screen_id;
    int state;
    int capture_width;
    int capture_height;
    int format;

    /* mutex to protect buffer manager */
    pthread_mutex_t mutex;

    /*
     * commit cycle before release buffer
     *  > 0 it means that the allocated buffer is not release yet
     *  = 0 it means that there is not allocated buffer
     *  < 0 **should not happend**
     */
    int cycle_before_release;
    struct buffer_allocator *allocator;
    struct buffer_slot slots[MAX_BUFFER_COUNT];
    struct disp_capture_buffer buffers[MAX_BUFFER_COUNT];
    struct disp_capture_handle current;

    int (*enable)(struct hwc_capture_context *captr, capture_config_t *config);
    int (*disable)(struct hwc_capture_context *captr);
    int (*is_enable)(struct hwc_capture_context *captr);
    int (*resize)(struct hwc_capture_context *captr);
    int (*acquire_buffer)(struct hwc_capture_context *captr, struct disp_capture_handle *buf);
    int (*release_buffer)(struct hwc_capture_context *captr, struct disp_capture_handle *buf);

    int (*buffer_refs_count)(struct hwc_capture_context *captr);
    int (*buffer_refs_decrease)(struct hwc_capture_context *captr);
};

struct buffer_allocator *buffer_allocator_initial(void);
int buffer_allocator_destroy(struct buffer_allocator *allocator);
struct hwc_capture_context * hwc_capture_init(int screen_id);

#ifdef __cplusplus
}
#endif

#endif
