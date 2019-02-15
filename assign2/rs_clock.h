#ifndef RS_CLOCK_H
#define RS_CLOCK_H

#include "dt.h"

typedef struct RS_clock {
    // class members
    // TODO add more members as required
    char pageFile[4096];
    int numPages;

    // class methods (always called with a self object)
    int (*init) (struct RS_clock *self, const char *pageFile, int numPages);
    int (*shutdown) (struct RS_clock *self);
    int (*flush) (struct RS_clock *self);
    int (*mark) (struct RS_clock *self);
    int (*unpin) (struct RS_clock *self);
    int (*force) (struct RS_clock *self);
    int (*pin) (struct RS_clock *self);
    int* (*get_frame_contents) (struct RS_clock *self);
    bool* (*get_dirty_flags) (struct RS_clock *self);
    int* (*get_fix_counts) (struct RS_clock *self);
    int (*get_num_read) (struct RS_clock *self);
    int (*get_num_write) (struct RS_clock *self);
} RS_clock_t;

// class method prototypes
int RS_clock_init(RS_clock_t *self, const char *pageFile, int numPages);
int RS_clock_shutdown(RS_clock_t *self);
int RS_clock_flush(RS_clock_t *self);
int RS_clock_mark(RS_clock_t *self);
int RS_clock_unpin(RS_clock_t *self);
int RS_clock_force(RS_clock_t *self);
int RS_clock_pin(RS_clock_t *self);
int* RS_clock_get_frame_contents(RS_clock_t *self);
bool* RS_clock_get_dirty_flags(RS_clock_t *self);
int* RS_clock_get_fix_counts(RS_clock_t *self);
int RS_clock_get_num_read(RS_clock_t *self);
int RS_clock_get_num_write(RS_clock_t *self);

#endif
