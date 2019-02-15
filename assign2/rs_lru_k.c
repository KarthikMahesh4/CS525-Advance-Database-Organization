#include <stdlib.h>
#include <string.h>

#include "rs_lru_k.h"
#include "dt.h"

int RS_lru_k_init(RS_lru_k_t *self, const char *pageFile, int numPages)
{
    strcpy(self->pageFile, pageFile);
    self->numPages = numPages;

    self->shutdown = RS_lru_k_shutdown;
    self->flush = RS_lru_k_flush;
    self->mark = RS_lru_k_mark;
    self->unpin = RS_lru_k_unpin;
    self->force = RS_lru_k_force;
    self->pin = RS_lru_k_pin;
    self->get_frame_contents = RS_lru_k_get_frame_contents;
    self->get_dirty_flags = RS_lru_k_get_dirty_flags;
    self->get_fix_counts = RS_lru_k_get_fix_counts;
    self->get_num_read = RS_lru_k_get_num_read;
    self->get_num_write = RS_lru_k_get_num_write;

    // TODO implement and initialize the LRU_K store data structure
    
    return 0;
}

int RS_lru_k_shutdown(RS_lru_k_t *self)
{
    // TODO implement the LRU_K method

    return 0;
}

int RS_lru_k_flush(RS_lru_k_t *self)
{
    // TODO implement the LRU_K method

    return 0;
}

int RS_lru_k_mark(RS_lru_k_t *self)
{
    // TODO implement the LRU_K method

    return 0;
}

int RS_lru_k_unpin(RS_lru_k_t *self)
{
    // TODO implement the LRU_K method

    return 0;
}

int RS_lru_k_force(RS_lru_k_t *self)
{
    // TODO implement the LRU_K method

    return 0;
}

int RS_lru_k_pin(RS_lru_k_t *self)
{
    // TODO implement the LRU_K method

    return 0;
}

int* RS_lru_k_get_frame_contents(RS_lru_k_t *self)
{
    // TODO implement the LRU_K method

    return NULL;
}

bool* RS_lru_k_get_dirty_flags(RS_lru_k_t *self)
{
    // TODO implement the LRU_K method

    return NULL;
}

int *RS_lru_k_get_fix_counts(RS_lru_k_t *self)
{
    // TODO implement the LRU_K method

    return NULL;
}

int RS_lru_k_get_num_read(RS_lru_k_t *self)
{
    // TODO implement the LRU_K method

    return 0;
}

int RS_lru_k_get_num_write(RS_lru_k_t *self)
{
    // TODO implement the LRU_K method

    return 0;
}
