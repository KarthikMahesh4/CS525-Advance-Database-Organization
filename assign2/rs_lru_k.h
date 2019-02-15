#ifndef RS_LRU_K_H
#define RS_LRU_K_H

#include "dt.h"

typedef struct RS_lru_k {
    // class members
    // TODO add more members as required
    char pageFile[4096];
    int numPages;

    // class methods (always called with a self object)
    int (*init) (struct RS_lru_k *self, const char *pageFile, int numPages);
    int (*shutdown) (struct RS_lru_k *self);
    int (*flush) (struct RS_lru_k *self);
    int (*mark) (struct RS_lru_k *self);
    int (*unpin) (struct RS_lru_k *self);
    int (*force) (struct RS_lru_k *self);
    int (*pin) (struct RS_lru_k *self);
    int* (*get_frame_contents) (struct RS_lru_k *self);
    bool* (*get_dirty_flags) (struct RS_lru_k *self);
    int* (*get_fix_counts) (struct RS_lru_k *self);
    int (*get_num_read) (struct RS_lru_k *self);
    int (*get_num_write) (struct RS_lru_k *self);
} RS_lru_k_t;

// class method prototypes
int RS_lru_k_init(RS_lru_k_t *self, const char *pageFile, int numPages);
int RS_lru_k_shutdown(RS_lru_k_t *self);
int RS_lru_k_flush(RS_lru_k_t *self);
int RS_lru_k_mark(RS_lru_k_t *self);
int RS_lru_k_unpin(RS_lru_k_t *self);
int RS_lru_k_force(RS_lru_k_t *self);
int RS_lru_k_pin(RS_lru_k_t *self);
int* RS_lru_k_get_frame_contents(RS_lru_k_t *self);
bool* RS_lru_k_get_dirty_flags(RS_lru_k_t *self);
int* RS_lru_k_get_fix_counts(RS_lru_k_t *self);
int RS_lru_k_get_num_read(RS_lru_k_t *self);
int RS_lru_k_get_num_write(RS_lru_k_t *self);

#endif
