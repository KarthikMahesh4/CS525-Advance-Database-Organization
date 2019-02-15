#ifndef RS_LFU_H
#define RS_LFU_H

#include "buffer_mgr.h"
#include "dt.h"

typedef struct lfu_frame {
    int page_num;
    char data[PAGE_SIZE];
    bool dirty;
    int fixcount;
    unsigned long freq;
} lfu_frame_t;

typedef struct RS_lfu {
    char filename[4096];
    int max_num_pages;
    int num_pages;
    lfu_frame_t *frames;
    int read_io;
    int write_io;
} RS_lfu_t;

int RS_lfu_init(RS_lfu_t *rs, const char *pageFile, int numPages);
int RS_lfu_shutdown(RS_lfu_t *rs);
int RS_lfu_flush(RS_lfu_t *rs);
int RS_lfu_mark(RS_lfu_t *rs, BM_PageHandle *const page);
int RS_lfu_unpin(RS_lfu_t *rs, BM_PageHandle *const page);
int RS_lfu_force(RS_lfu_t *rs, BM_PageHandle *const page);
int RS_lfu_pin(RS_lfu_t *rs, BM_PageHandle *const page,
        const PageNumber pageNum);
int* RS_lfu_get_frame_contents(RS_lfu_t *rs);
bool* RS_lfu_get_dirty_flags(RS_lfu_t *rs);
int* RS_lfu_get_fix_counts(RS_lfu_t *rs);
int RS_lfu_get_num_read(RS_lfu_t *rs);
int RS_lfu_get_num_write(RS_lfu_t *rs);

#endif
