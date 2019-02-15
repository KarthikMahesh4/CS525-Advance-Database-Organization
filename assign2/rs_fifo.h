#ifndef RS_FIFO_H
#define RS_FIFO_H

#include "buffer_mgr.h"
#include "dt.h"

typedef struct fifo_frame {
    int page_num;
    char data[PAGE_SIZE];
    bool dirty;
    int fixcount;
    unsigned long ticket;
} fifo_frame_t;

typedef struct RS_fifo {
    char filename[4096];
    int max_num_pages;
    int num_pages;
    fifo_frame_t *frames;
    unsigned long next_ticket;
    int read_io;
    int write_io;
} RS_fifo_t;

int RS_fifo_init(RS_fifo_t *rs, const char *pageFile, int numPages);
int RS_fifo_shutdown(RS_fifo_t *rs);
int RS_fifo_flush(RS_fifo_t *rs);
int RS_fifo_mark(RS_fifo_t *rs, BM_PageHandle *const page);
int RS_fifo_unpin(RS_fifo_t *rs, BM_PageHandle *const page);
int RS_fifo_force(RS_fifo_t *rs, BM_PageHandle *const page);
int RS_fifo_pin(RS_fifo_t *rs, BM_PageHandle *const page,
        const PageNumber pageNum);
int* RS_fifo_get_frame_contents(RS_fifo_t *rs);
bool* RS_fifo_get_dirty_flags(RS_fifo_t *rs);
int* RS_fifo_get_fix_counts(RS_fifo_t *rs);
int RS_fifo_get_num_read(RS_fifo_t *rs);
int RS_fifo_get_num_write(RS_fifo_t *rs);

#endif
