#include <stdlib.h>
#include <string.h>

#include "rs_clock.h"
#include "dt.h"

int RS_clock_init(RS_clock_t *self, const char *pageFile, int numPages)
{
    strcpy(self->pageFile, pageFile);
    self->numPages = numPages;

    self->shutdown = RS_clock_shutdown;
    self->flush = RS_clock_flush;
    self->mark = RS_clock_mark;
    self->unpin = RS_clock_unpin;
    self->force = RS_clock_force;
    self->pin = RS_clock_pin;
    self->get_frame_contents = RS_clock_get_frame_contents;
    self->get_dirty_flags = RS_clock_get_dirty_flags;
    self->get_fix_counts = RS_clock_get_fix_counts;
    self->get_num_read = RS_clock_get_num_read;
    self->get_num_write = RS_clock_get_num_write;

    // TODO implement and initialize the CLOCK store data structure
    
    return 0;
}

int RS_clock_shutdown(RS_clock_t *self)
{
    // TODO implement the CLOCK method

    return 0;
}

int RS_clock_flush(RS_clock_t *self)
{
    // TODO implement the CLOCK method

    return 0;
}

int RS_clock_mark(RS_clock_t *self)
{
    // TODO implement the CLOCK method

    return 0;
}

int RS_clock_unpin(RS_clock_t *self)
{
    // TODO implement the CLOCK method

    return 0;
}

int RS_clock_force(RS_clock_t *self)
{
    // TODO implement the CLOCK method

    return 0;
}

int RS_clock_pin(RS_clock_t *self)
{
    // TODO implement the CLOCK method

    return 0;
}

int* RS_clock_get_frame_contents(RS_clock_t *self)
{
    // TODO implement the CLOCK method

    return NULL;
}

bool* RS_clock_get_dirty_flags(RS_clock_t *self)
{
    // TODO implement the CLOCK method

    return NULL;
}

int* RS_clock_get_fix_counts(RS_clock_t *self)
{
    // TODO implement the CLOCK method

    return NULL;
}

int RS_clock_get_num_read(RS_clock_t *self)
{
    // TODO implement the CLOCK method

    return 0;
}

int RS_clock_get_num_write(RS_clock_t *self)
{
    // TODO implement the CLOCK method

    return 0;
}
