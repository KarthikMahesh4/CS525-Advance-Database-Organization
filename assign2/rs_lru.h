#ifndef RS_LRU_H
#define RS_LRU_H

#include "buffer_mgr.h"
#include "dt.h"

typedef struct lru_frame {
    int page_num;
    char data[PAGE_SIZE];
    bool dirty;
    int fixcount;
    unsigned long age;
} lru_frame_t;

typedef struct RS_lru {
    char filename[4096];
    int max_num_pages;
    int num_pages;
    lru_frame_t *frames;
    int read_io;
    int write_io;
} RS_lru_t;

void increase_age_all(RS_lru_t *rs);

int RS_lru_init(RS_lru_t *rs, const char *pageFile, int numPages);
int RS_lru_shutdown(RS_lru_t *rs);
int RS_lru_flush(RS_lru_t *rs);
int RS_lru_mark(RS_lru_t *rs, BM_PageHandle *const page);
int RS_lru_unpin(RS_lru_t *rs, BM_PageHandle *const page);
int RS_lru_force(RS_lru_t *rs, BM_PageHandle *const page);
int RS_lru_pin(RS_lru_t *rs, BM_PageHandle *const page,
        const PageNumber pageNum);
int* RS_lru_get_frame_contents(RS_lru_t *rs);
bool* RS_lru_get_dirty_flags(RS_lru_t *rs);
int* RS_lru_get_fix_counts(RS_lru_t *rs);
int RS_lru_get_num_read(RS_lru_t *rs);
int RS_lru_get_num_write(RS_lru_t *rs);

#endif
