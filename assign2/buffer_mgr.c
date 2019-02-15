#include <stdlib.h>
#include <string.h>

#include "buffer_mgr.h"
#include "dt.h"

#include "rs_fifo.h"
#include "rs_lru.h"
#include "rs_clock.h"
#include "rs_lfu.h"
#include "rs_lru_k.h"

RC initBufferPool(BM_BufferPool *const bm, const char *const pageFileName, 
		  const int numPages, ReplacementStrategy strategy, void *stratData)
{
    int rc, n;
    RS_fifo_t *rs1;
    RS_lru_t *rs2;
    RS_clock_t *rs3;
    RS_lfu_t *rs4;
    RS_lru_k_t *rs5;

    n = strlen(pageFileName) + 1;
    bm->pageFile = (char*) malloc(sizeof(char) * n);
    strcpy(bm->pageFile, pageFileName);
    bm->numPages = numPages;
    bm->strategy = strategy;

    rc = 0;
    switch(bm->strategy) {
        case RS_FIFO:
            // instantiate the replacement strategy object
            rs1 = (RS_fifo_t*) malloc(sizeof(RS_fifo_t));
            if (rs1 == NULL) {
                // could not allocate memory
                // maybe print error?
                // TODO change rc to proper error type
                rc = -1;
                break;
            }
            
            // initialize the replacement strategy object
            rc = RS_fifo_init(rs1, pageFileName, numPages);

            // attach the replacement strategy object to buffer pool
            bm->mgmtData = (void*) rs1;
            break;
        case RS_LRU:
            // instantiate the replacement strategy object
            rs2 = (RS_lru_t*) malloc(sizeof(RS_lru_t));
            if (rs2 == NULL) {
                // could not allocate memory
                // maybe print error?
                // TODO change rc to proper error type
                rc = -1;
                break;
            }
            
            // initialize the replacement strategy object
            rc = RS_lru_init(rs2, pageFileName, numPages);

            // attach the replacement strategy object to buffer pool
            bm->mgmtData = (void*) rs2;
            break;
        case RS_CLOCK:
            // instantiate the replacement strategy object
            rs3 = (RS_clock_t*) malloc(sizeof(RS_clock_t));
            if (rs3 == NULL) {
                // could not allocate memory
                // maybe print error?
                // TODO change rc to proper error type
                rc = -1;
                break;
            }
            
            // initialize the replacement strategy object
            rc = RS_clock_init(rs3, pageFileName, numPages);

            // attach the replacement strategy object to buffer pool
            bm->mgmtData = (void*) rs3;
            break;
        case RS_LFU:
            // instantiate the replacement strategy object
            rs4 = (RS_lfu_t*) malloc(sizeof(RS_lfu_t));
            if (rs4 == NULL) {
                // could not allocate memory
                // maybe print error?
                // TODO change rc to proper error type
                rc = -1;
                break;
            }
            
            // initialize the replacement strategy object
            rc = RS_lfu_init(rs4, pageFileName, numPages);

            // attach the replacement strategy object to buffer pool
            bm->mgmtData = (void*) rs4;
            break;
        case RS_LRU_K:
            // instantiate the replacement strategy object
            rs5 = (RS_lru_k_t*) malloc(sizeof(RS_lru_k_t));
            if (rs5 == NULL) {
                // could not allocate memory
                // maybe print error?
                // TODO change rc to proper error type
                rc = -1;
                break;
            }
            
            // initialize the replacement strategy object
            rc = RS_lru_k_init(rs5, pageFileName, numPages);

            // attach the replacement strategy object to buffer pool
            bm->mgmtData = (void*) rs5;
            break;
        default:
            // maybe print error?
            // TODO change rc to correct error type
            rc = -1;
            break;
    }

    if (rc == 0) {
        return RC_OK;
    } else {
        return RC_RM_UNKOWN_DATATYPE;
    }
}

RC shutdownBufferPool(BM_BufferPool *const bm)
{
    int rc;
    RS_fifo_t *rs1;
    RS_lru_t *rs2;
    RS_clock_t *rs3;
    RS_lfu_t *rs4;
    RS_lru_k_t *rs5;

    rc = 0;
    switch(bm->strategy) {
        case RS_FIFO:
            // get the replacement strategy object from the buffer pool
            rs1 = (RS_fifo_t*) bm->mgmtData;

            // shutdown the replacement strategy object
            rc = RS_fifo_shutdown(rs1);

            // TODO change '0' to proper exit code
            if (rc == 0) {
                free(rs1);
                bm->mgmtData = NULL;
            }

            break;
        case RS_LRU:
            // get the replacement strategy object from the buffer pool
            rs2 = (RS_lru_t*) bm->mgmtData;

            // shutdown the replacement strategy object
            rc = RS_lru_shutdown(rs2);
            
            // TODO change '0' to proper exit code
            if (rc == 0) {
                free(rs2);
                bm->mgmtData = NULL;
            }
            
            break;
        case RS_CLOCK:
            // get the replacement strategy object from the buffer pool
            rs3 = (RS_clock_t*) bm->mgmtData;

            // shutdown the replacement strategy object
            rc = RS_clock_shutdown(rs3);
            
            // TODO change '0' to proper exit code
            if (rc == 0) {
                free(rs3);
                bm->mgmtData = NULL;
            }
            
            break;
        case RS_LFU:
            // get the replacement strategy object from the buffer pool
            rs4 = (RS_lfu_t*) bm->mgmtData;

            // shutdown the replacement strategy object
            rc = RS_lfu_shutdown(rs4);
            
            // TODO change '0' to proper exit code
            if (rc == 0) {
                free(rs4);
                bm->mgmtData = NULL;
            }
            
            break;
        case RS_LRU_K:
            // get the replacement strategy object from the buffer pool
            rs5 = (RS_lru_k_t*) bm->mgmtData;

            // shutdown the replacement strategy object
            rc = RS_lru_k_shutdown(rs5);
            
            // TODO change '0' to proper exit code
            if (rc == 0) {
                free(rs5);
                bm->mgmtData = NULL;
            }
            
            break;
        default:
            // maybe print error?
            // TODO change rc to correct error type
            rc = -1;
            break;
    }

    if (rc == 0) {
        return RC_OK;
    } else {
        return RC_RM_UNKOWN_DATATYPE;
    }
}

RC forceFlushPool(BM_BufferPool *const bm)
{
    int rc;
    RS_fifo_t *rs1;
    RS_lru_t *rs2;
    RS_clock_t *rs3;
    RS_lfu_t *rs4;
    RS_lru_k_t *rs5;

    rc = 0;
    switch(bm->strategy) {
        case RS_FIFO:
            // get the replacement strategy object from the buffer pool
            rs1 = (RS_fifo_t*) bm->mgmtData;

            // force flush the replacement strategy object
            rc = RS_fifo_flush(rs1);
            break;
        case RS_LRU:
            // get the replacement strategy object from the buffer pool
            rs2 = (RS_lru_t*) bm->mgmtData;

            // force flush the replacement strategy object
            rc = RS_lru_flush(rs2);
            break;
        case RS_CLOCK:
            // get the replacement strategy object from the buffer pool
            rs3 = (RS_clock_t*) bm->mgmtData;

            // force flush the replacement strategy object
            rc = RS_clock_flush(rs3);
            break;
        case RS_LFU:
            // get the replacement strategy object from the buffer pool
            rs4 = (RS_lfu_t*) bm->mgmtData;

            // force flush the replacement strategy object
            rc = RS_lfu_flush(rs4);
            break;
        case RS_LRU_K:
            // get the replacement strategy object from the buffer pool
            rs5 = (RS_lru_k_t*) bm->mgmtData;

            // force flush the replacement strategy object
            rc = RS_lru_k_flush(rs5);
            break;
        default:
            // maybe print error?
            // TODO change rc to correct error type
            rc = -1;
            break;
    }

    if (rc == 0) {
        return RC_OK;
    } else {
        return RC_RM_UNKOWN_DATATYPE;
    }
}

RC markDirty (BM_BufferPool *const bm, BM_PageHandle *const page)
{
    int rc;
    RS_fifo_t *rs1;
    RS_lru_t *rs2;
    RS_clock_t *rs3;
    RS_lfu_t *rs4;
    RS_lru_k_t *rs5;

    rc = 0;
    switch(bm->strategy) {
        case RS_FIFO:
            // get the replacement strategy object from the buffer pool
            rs1 = (RS_fifo_t*) bm->mgmtData;

            // mark dirty
            rc = RS_fifo_mark(rs1, page);
            break;
        case RS_LRU:
            // get the replacement strategy object from the buffer pool
            rs2 = (RS_lru_t*) bm->mgmtData;

            // mark dirty
            rc = RS_lru_mark(rs2, page);
            break;
        case RS_CLOCK:
            // get the replacement strategy object from the buffer pool
            rs3 = (RS_clock_t*) bm->mgmtData;

            // mark dirty
            rc = RS_clock_mark(rs3);
            break;
        case RS_LFU:
            // get the replacement strategy object from the buffer pool
            rs4 = (RS_lfu_t*) bm->mgmtData;

            // mark dirty
            rc = RS_lfu_mark(rs4, page);
            break;
        case RS_LRU_K:
            // get the replacement strategy object from the buffer pool
            rs5 = (RS_lru_k_t*) bm->mgmtData;

            // mark dirty
            rc = RS_lru_k_mark(rs5);
            break;
        default:
            // maybe print error?
            // TODO change rc to correct error type
            rc = -1;
            break;
    }

    if (rc == 0) {
        return RC_OK;
    } else {
        return RC_RM_UNKOWN_DATATYPE;
    }
}

RC unpinPage (BM_BufferPool *const bm, BM_PageHandle *const page)
{
    int rc;
    RS_fifo_t *rs1;
    RS_lru_t *rs2;
    RS_clock_t *rs3;
    RS_lfu_t *rs4;
    RS_lru_k_t *rs5;

    rc = 0;
    switch(bm->strategy) {
        case RS_FIFO:
            // get the replacement strategy object from the buffer pool
            rs1 = (RS_fifo_t*) bm->mgmtData;

            // unpin
            rc = RS_fifo_unpin(rs1, page);
            break;
        case RS_LRU:
            // get the replacement strategy object from the buffer pool
            rs2 = (RS_lru_t*) bm->mgmtData;

            // unpin
            rc = RS_lru_unpin(rs2, page);
            break;
        case RS_CLOCK:
            // get the replacement strategy object from the buffer pool
            rs3 = (RS_clock_t*) bm->mgmtData;

            // unpin
            rc = RS_clock_unpin(rs3);
            break;
        case RS_LFU:
            // get the replacement strategy object from the buffer pool
            rs4 = (RS_lfu_t*) bm->mgmtData;

            // unpin
            rc = RS_lfu_unpin(rs4, page);
            break;
        case RS_LRU_K:
            // get the replacement strategy object from the buffer pool
            rs5 = (RS_lru_k_t*) bm->mgmtData;

            // unpin
            rc = RS_lru_k_unpin(rs5);
            break;
        default:
            // maybe print error?
            // TODO change rc to correct error type
            rc = -1;
            break;
    }

    if (rc == 0) {
        return RC_OK;
    } else {
        return RC_RM_UNKOWN_DATATYPE;
    }
}

RC forcePage (BM_BufferPool *const bm, BM_PageHandle *const page)
{
    int rc;
    RS_fifo_t *rs1;
    RS_lru_t *rs2;
    RS_clock_t *rs3;
    RS_lfu_t *rs4;
    RS_lru_k_t *rs5;

    rc = 0;
    switch(bm->strategy) {
        case RS_FIFO:
            // get the replacement strategy object from the buffer pool
            rs1 = (RS_fifo_t*) bm->mgmtData;

            // force
            rc = RS_fifo_force(rs1, page);
            break;
        case RS_LRU:
            // get the replacement strategy object from the buffer pool
            rs2 = (RS_lru_t*) bm->mgmtData;

            // force
            rc = RS_lru_force(rs2, page);
            break;
        case RS_CLOCK:
            // get the replacement strategy object from the buffer pool
            rs3 = (RS_clock_t*) bm->mgmtData;

            // force
            rc = RS_clock_force(rs3);
            break;
        case RS_LFU:
            // get the replacement strategy object from the buffer pool
            rs4 = (RS_lfu_t*) bm->mgmtData;

            // force
            rc = RS_lfu_force(rs4, page);
            break;
        case RS_LRU_K:
            // get the replacement strategy object from the buffer pool
            rs5 = (RS_lru_k_t*) bm->mgmtData;

            // force
            rc = RS_lru_k_force(rs5);
            break;
        default:
            // maybe print error?
            // TODO change rc to correct error type
            rc = -1;
            break;
    }

    if (rc == 0) {
        return RC_OK;
    } else {
        return RC_RM_UNKOWN_DATATYPE;
    }
}

RC pinPage (BM_BufferPool *const bm, BM_PageHandle *const page, 
	    const PageNumber pageNum)
{
    int rc;
    RS_fifo_t *rs1;
    RS_lru_t *rs2;
    RS_clock_t *rs3;
    RS_lfu_t *rs4;
    RS_lru_k_t *rs5;

    rc = 0;
    switch(bm->strategy) {
        case RS_FIFO:
            // get the replacement strategy object from the buffer pool
            rs1 = (RS_fifo_t*) bm->mgmtData;

            // pin
            rc = RS_fifo_pin(rs1, page, pageNum);
            break;
        case RS_LRU:
            // get the replacement strategy object from the buffer pool
            rs2 = (RS_lru_t*) bm->mgmtData;

            // pin
            rc = RS_lru_pin(rs2, page, pageNum);
            break;
        case RS_CLOCK:
            // get the replacement strategy object from the buffer pool
            rs3 = (RS_clock_t*) bm->mgmtData;

            // pin
            rc = RS_clock_pin(rs3);
            break;
        case RS_LFU:
            // get the replacement strategy object from the buffer pool
            rs4 = (RS_lfu_t*) bm->mgmtData;

            // pin
            rc = RS_lfu_pin(rs4, page, pageNum);
            break;
        case RS_LRU_K:
            // get the replacement strategy object from the buffer pool
            rs5 = (RS_lru_k_t*) bm->mgmtData;

            // pin
            rc = RS_lru_k_pin(rs5);
            break;
        default:
            // maybe print error?
            // TODO change rc to correct error type
            rc = -1;
            break;
    }

    if (rc == 0) {
        return RC_OK;
    } else {
        return RC_RM_UNKOWN_DATATYPE;
    }
}

PageNumber *getFrameContents (BM_BufferPool *const bm)
{
    PageNumber *rc;
    RS_fifo_t *rs1;
    RS_lru_t *rs2;
    RS_clock_t *rs3;
    RS_lfu_t *rs4;
    RS_lru_k_t *rs5;

    rc = NULL;
    switch(bm->strategy) {
        case RS_FIFO:
            // get the replacement strategy object from the buffer pool
            rs1 = (RS_fifo_t*) bm->mgmtData;

            // get_frame_contents
            rc = (PageNumber*) RS_fifo_get_frame_contents(rs1);
            break;
        case RS_LRU:
            // get the replacement strategy object from the buffer pool
            rs2 = (RS_lru_t*) bm->mgmtData;

            // get_frame_contents
            rc = (PageNumber*) RS_lru_get_frame_contents(rs2);
            break;
        case RS_CLOCK:
            // get the replacement strategy object from the buffer pool
            rs3 = (RS_clock_t*) bm->mgmtData;

            // get_frame_contents
            rc = (PageNumber*) RS_clock_get_frame_contents(rs3);
            break;
        case RS_LFU:
            // get the replacement strategy object from the buffer pool
            rs4 = (RS_lfu_t*) bm->mgmtData;

            // get_frame_contents
            rc = (PageNumber*) RS_lfu_get_frame_contents(rs4);
            break;
        case RS_LRU_K:
            // get the replacement strategy object from the buffer pool
            rs5 = (RS_lru_k_t*) bm->mgmtData;

            // get_frame_contents
            rc = (PageNumber*) RS_lru_k_get_frame_contents(rs5);
            break;
        default:
            // maybe print error?
            // TODO change rc to correct error type
            rc = NULL;
            break;
    }

    return rc;
}

bool *getDirtyFlags (BM_BufferPool *const bm)
{
    bool *rc;
    RS_fifo_t *rs1;
    RS_lru_t *rs2;
    RS_clock_t *rs3;
    RS_lfu_t *rs4;
    RS_lru_k_t *rs5;

    rc = false;
    switch(bm->strategy) {
        case RS_FIFO:
            // get the replacement strategy object from the buffer pool
            rs1 = (RS_fifo_t*) bm->mgmtData;

            // get_dirty_flags
            rc = RS_fifo_get_dirty_flags(rs1);
            break;
        case RS_LRU:
            // get the replacement strategy object from the buffer pool
            rs2 = (RS_lru_t*) bm->mgmtData;

            // get_dirty_flags
            rc = RS_lru_get_dirty_flags(rs2);
            break;
        case RS_CLOCK:
            // get the replacement strategy object from the buffer pool
            rs3 = (RS_clock_t*) bm->mgmtData;

            // get_dirty_flags
            rc = RS_clock_get_dirty_flags(rs3);
            break;
        case RS_LFU:
            // get the replacement strategy object from the buffer pool
            rs4 = (RS_lfu_t*) bm->mgmtData;

            // get_dirty_flags
            rc = RS_lfu_get_dirty_flags(rs4);
            break;
        case RS_LRU_K:
            // get the replacement strategy object from the buffer pool
            rs5 = (RS_lru_k_t*) bm->mgmtData;

            // get_dirty_flags
            rc = RS_lru_k_get_dirty_flags(rs5);
            break;
        default:
            // maybe print error?
            // TODO change rc to correct error type
            rc = NULL;
            break;
    }

    return rc;
}

int *getFixCounts (BM_BufferPool *const bm)
{
    int *rc;
    RS_fifo_t *rs1;
    RS_lru_t *rs2;
    RS_clock_t *rs3;
    RS_lfu_t *rs4;
    RS_lru_k_t *rs5;

    rc = 0;
    switch(bm->strategy) {
        case RS_FIFO:
            // get the replacement strategy object from the buffer pool
            rs1 = (RS_fifo_t*) bm->mgmtData;

            // get_fix_counts
            rc = RS_fifo_get_fix_counts(rs1);
            break;
        case RS_LRU:
            // get the replacement strategy object from the buffer pool
            rs2 = (RS_lru_t*) bm->mgmtData;

            // get_fix_counts
            rc = RS_lru_get_fix_counts(rs2);
            break;
        case RS_CLOCK:
            // get the replacement strategy object from the buffer pool
            rs3 = (RS_clock_t*) bm->mgmtData;

            // get_fix_counts
            rc = RS_clock_get_fix_counts(rs3);
            break;
        case RS_LFU:
            // get the replacement strategy object from the buffer pool
            rs4 = (RS_lfu_t*) bm->mgmtData;

            // get_fix_counts
            rc = RS_lfu_get_fix_counts(rs4);
            break;
        case RS_LRU_K:
            // get the replacement strategy object from the buffer pool
            rs5 = (RS_lru_k_t*) bm->mgmtData;

            // get_fix_counts
            rc = RS_lru_k_get_fix_counts(rs5);
            break;
        default:
            // maybe print error?
            // TODO change rc to correct error type
            rc = NULL;
            break;
    }

    return rc;
}

int getNumReadIO (BM_BufferPool *const bm)
{
    int rc;
    RS_fifo_t *rs1;
    RS_lru_t *rs2;
    RS_clock_t *rs3;
    RS_lfu_t *rs4;
    RS_lru_k_t *rs5;

    rc = 0;
    switch(bm->strategy) {
        case RS_FIFO:
            // get the replacement strategy object from the buffer pool
            rs1 = (RS_fifo_t*) bm->mgmtData;

            // get_num_read
            rc = RS_fifo_get_num_read(rs1);
            break;
        case RS_LRU:
            // get the replacement strategy object from the buffer pool
            rs2 = (RS_lru_t*) bm->mgmtData;

            // get_num_read
            rc = RS_lru_get_num_read(rs2);
            break;
        case RS_CLOCK:
            // get the replacement strategy object from the buffer pool
            rs3 = (RS_clock_t*) bm->mgmtData;

            // get_num_read
            rc = RS_clock_get_num_read(rs3);
            break;
        case RS_LFU:
            // get the replacement strategy object from the buffer pool
            rs4 = (RS_lfu_t*) bm->mgmtData;

            // get_num_read
            rc = RS_lfu_get_num_read(rs4);
            break;
        case RS_LRU_K:
            // get the replacement strategy object from the buffer pool
            rs5 = (RS_lru_k_t*) bm->mgmtData;

            // get_num_read
            rc = RS_lru_k_get_num_read(rs5);
            break;
        default:
            // maybe print error?
            // TODO change rc to correct error type
            rc = -1;
            break;
    }

    return rc;
}

int getNumWriteIO (BM_BufferPool *const bm)
{
    int rc;
    RS_fifo_t *rs1;
    RS_lru_t *rs2;
    RS_clock_t *rs3;
    RS_lfu_t *rs4;
    RS_lru_k_t *rs5;

    rc = 0;
    switch(bm->strategy) {
        case RS_FIFO:
            // get the replacement strategy object from the buffer pool
            rs1 = (RS_fifo_t*) bm->mgmtData;

            // get_num_write
            rc = RS_fifo_get_num_write(rs1);
            break;
        case RS_LRU:
            // get the replacement strategy object from the buffer pool
            rs2 = (RS_lru_t*) bm->mgmtData;

            // get_num_write
            rc = RS_lru_get_num_write(rs2);
            break;
        case RS_CLOCK:
            // get the replacement strategy object from the buffer pool
            rs3 = (RS_clock_t*) bm->mgmtData;

            // get_num_write
            rc = RS_clock_get_num_write(rs3);
            break;
        case RS_LFU:
            // get the replacement strategy object from the buffer pool
            rs4 = (RS_lfu_t*) bm->mgmtData;

            // get_num_write
            rc = RS_lfu_get_num_write(rs4);
            break;
        case RS_LRU_K:
            // get the replacement strategy object from the buffer pool
            rs5 = (RS_lru_k_t*) bm->mgmtData;

            // get_num_write
            rc = RS_lru_k_get_num_write(rs5);
            break;
        default:
            // maybe print error?
            // TODO change rc to correct error type
            rc = -1;
            break;
    }

    return rc;
}
