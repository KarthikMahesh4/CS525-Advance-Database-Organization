#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include "rs_lfu.h"

#include "storage_mgr.h"
#include "dt.h"
#include "dberror.h"

int RS_lfu_init(RS_lfu_t *rs, const char *pageFile, int numPages)
{
    int i;
    
    // if the filename is larger than UNIX limit return error
    if (strlen(pageFile) > 4096) {
        return -1;
    }

    strcpy(rs->filename, pageFile);
    rs->max_num_pages = numPages;
    rs->num_pages = 0;
    rs->frames = (lfu_frame_t*) malloc(sizeof(lfu_frame_t) * numPages);
    rs->read_io = 0;
    rs->write_io = 0;
    
    /* initialize all the frames with NO_PAGE page number, zeros for data, false
     * dirty flag, zero fix count and zero age */
    for (i = 0; i < rs->max_num_pages; i++) {
        rs->frames[i].page_num = NO_PAGE;
        memset(rs->frames[i].data, 0, sizeof(rs->frames[i].data));
        rs->frames[i].dirty = false;
        rs->frames[i].fixcount = 0;
        rs->frames[i].freq = 0;
    }
    
    return 0;
}

int RS_lfu_shutdown(RS_lfu_t *rs)
{
    int i;
    SM_FileHandle file_handle;
    int rc;

    /* check for non-zero fix count pages and return error if there exist such
     * cases */
    for (i = 0; i < rs->max_num_pages; i++) {
        if (rs->frames[i].page_num != NO_PAGE && rs->frames[i].fixcount != 0) {
            return -1;
        }
    }

    // open the page file handler
    rc = openPageFile(rs->filename, &file_handle);
    if (rc != RC_OK) {
        return -1;
    }

    // write out dirty pages before shutdown
    for (i = 0; i < rs->max_num_pages; i++) {
        if (rs->frames[i].page_num != NO_PAGE) {
            if (rs->frames[i].dirty) {
                rc = writeBlock(rs->frames[i].page_num, 
                        &file_handle,
                        rs->frames[i].data);

                // if write was not successful return error
                if (rc != RC_OK) {
                    rc = closePageFile(&file_handle);
                    return -1;
                }

                // statistics
                rs->write_io++;
            }
        }
    }
    
    // close the page file handler
    rc = closePageFile(&file_handle);
    if (rc != RC_OK) {
        return -1;
    }
   
    free(rs->frames);

    return 0;
}

int RS_lfu_flush(RS_lfu_t *rs)
{
    int i;
    SM_FileHandle file_handle;
    int rc;

    // open the page file handler
    rc = openPageFile(rs->filename, &file_handle);
    if (rc != RC_OK) {
        return -1;
    }
   
    // write out to disk the pages that are dirty and have zero fix count
    for (i = 0; i < rs->max_num_pages; i++) {
        if (rs->frames[i].page_num != NO_PAGE) {
            if (rs->frames[i].fixcount == 0 && rs->frames[i].dirty) {
                rc = writeBlock(rs->frames[i].page_num, 
                        &file_handle,
                        rs->frames[i].data);

                // return error if write was not succesful
                if (rc != RC_OK) {
                    rc = closePageFile(&file_handle);
                    return -1;
                }

                // statistics
                rs->write_io++;

                // set the dirty flag to false
                rs->frames[i].dirty = false;
            }
        }
    }

    // close the page file handler
    rc = closePageFile(&file_handle);
    if (rc != RC_OK) {
        return -1;
    }
    
    return 0;
}

int RS_lfu_mark(RS_lfu_t *rs, BM_PageHandle *const page)
{
    int i;

    // if there are no pages in the buffer return error
    if (rs->num_pages == 0) {
        return -1;
    }

    // search for the page and return error if it is not found
    for (i = 0; i < rs->max_num_pages; i++) {
        if (rs->frames[i].page_num == page->pageNum) {
            rs->frames[i].dirty = true;
            return 0;
        }
    }

    return -1;
}

int RS_lfu_unpin(RS_lfu_t *rs, BM_PageHandle *const page)
{
    int i;

    // return error if there are not pages in the page buffer
    if (rs->num_pages == 0) {
        return -1;
    }

    // search for the page and return error if it is not found
    for (i = 0; i < rs->max_num_pages; i++) {
        if (rs->frames[i].page_num == page->pageNum) {
            /* if the fix count is zero return error, since a client cannot
             * unpin a page that is not pinned at all */
            if (rs->frames[i].fixcount == 0) {
                return -1;
            }

            // decrement the fix count of the page
            rs->frames[i].fixcount--;
            
            // since unpin is considered an interaction update the frequency
            rs->frames[i].freq++;
            
            // unpin does not actually remove the page from the buffer pool
            //rs->num_pages--;
            return 0;
        }
    }

    return -1;
}

int RS_lfu_force(RS_lfu_t *rs, BM_PageHandle *const page)
{
    int i;
    SM_FileHandle file_handle;
    int rc;

    // return error if there are not pages in the buffer pool
    if (rs->num_pages == 0) {
        return -1;
    }

    // open the page file handler
    rc = openPageFile(rs->filename, &file_handle);
    if (rc != RC_OK) {
        return -1;
    }
   
    // write out to disk the requested page
    for (i = 0; i < rs->max_num_pages; i++) {
        if (rs->frames[i].page_num == page->pageNum) {
            rc = writeBlock(rs->frames[i].page_num, 
                    &file_handle,
                    rs->frames[i].data);

            // return error if write was not succesful
            if (rc != RC_OK) {
                rc = closePageFile(&file_handle);
                return -1;
            }

            // statistics
            rs->write_io++;

            // set the dirty flag to false
            rs->frames[i].dirty = false;

            break;
        }
    }

    // close the page file handler
    rc = closePageFile(&file_handle);
    if (rc != RC_OK) {
        return -1;
    }

    return 0;
}

int RS_lfu_pin(RS_lfu_t *rs, BM_PageHandle *const page,
        const PageNumber pageNum)
{
    int i, k;
    unsigned long min_freq;
    SM_FileHandle file_handle;
    int rc;
    
    // open the page file handler
    rc = openPageFile(rs->filename, &file_handle);
    if (rc != RC_OK) {
        return -1;
    }

    // search for the frame and if it exists, return it and update freq
    for (i = 0; i < rs->max_num_pages; i++) {
        if (rs->frames[i].page_num == pageNum) {
            page->pageNum = rs->frames[i].page_num;
            page->data = rs->frames[i].data;
            rs->frames[i].fixcount++;
            rs->frames[i].freq++;

            return 0;
        }
    }
    
    /* if there is space in the buffer pool just add the page, otherwise find
     * the least recently used page that has a zero fix count and evict */
    if (rs->num_pages < rs->max_num_pages ) {
        // find first available slot
        for (i = 0; i < rs->max_num_pages; i++) {
            if (rs->frames[i].page_num == NO_PAGE) {
                rs->frames[i].page_num = pageNum;
                
                // read the new page from disk
                rc = readBlock(rs->frames[i].page_num, 
                        &file_handle,
                        rs->frames[i].data);

                // if page does not exist, create it
                if (rc == RC_READ_NON_EXISTING_PAGE) {
                    rc = ensureCapacity(pageNum + 1, &file_handle);
                }

                // return error if expansion was not succesful
                if (rc != RC_OK) {
                    rc = closePageFile(&file_handle);
                    return -1;
                }

                // statistics
                rs->read_io++;

                rs->frames[i].dirty = false;
                rs->frames[i].fixcount++;
                rs->frames[i].freq = 1;

                page->pageNum = rs->frames[i].page_num;
                page->data = rs->frames[i].data;
                
                rs->num_pages++;
                break;
            }
        }
    } else {
        // find the slot that has fix count set to zero and has max age
        min_freq = ULONG_MAX;
        k = -1;

        for (i = 0; i < rs->max_num_pages; i++) {
            if (rs->frames[i].page_num != NO_PAGE 
                    && rs->frames[i].fixcount == 0
                    && rs->frames[i].freq < min_freq ) {
                min_freq = rs->frames[i].freq;
                k = i;
            }
        }

        // if there are not available slots return error
        if (k == -1) {
            rc = closePageFile(&file_handle);
            return -1;
        }

        // if found slot is dirty write out to disk
        if (rs->frames[k].dirty) {
            rc = writeBlock(rs->frames[k].page_num, 
                    &file_handle,
                    rs->frames[k].data);

            // return error if read was not succesful
            if (rc != RC_OK) {
                rc = closePageFile(&file_handle);
                return -1;
            }

            // statistics
            rs->write_io++;
        }

        // read the new page from disk
        rc = readBlock(pageNum, &file_handle, rs->frames[k].data);

        // if page does not exist, create it
        if (rc == RC_READ_NON_EXISTING_PAGE) {
            rc = ensureCapacity(pageNum + 1, &file_handle);
        }

        // return error if expansion was not succesful
        if (rc != RC_OK) {
            rc = closePageFile(&file_handle);
            return -1;
        }

        // statistics
        rs->read_io++;

        rs->frames[k].page_num = pageNum;
        rs->frames[k].dirty = false;
        rs->frames[k].fixcount++;
        rs->frames[k].freq = 1;

        page->pageNum = rs->frames[k].page_num;
        page->data = rs->frames[k].data;
        
        rs->num_pages++;
    }

    // close the page file handler
    rc = closePageFile(&file_handle);
    if (rc != RC_OK) {
        return -1;
    }

    return 0;
}

int* RS_lfu_get_frame_contents(RS_lfu_t *rs)
{
    int i;
    int *res;

    res = (int*) malloc(sizeof(int) * rs->max_num_pages);

    for (i = 0; i < rs->max_num_pages; i++) {
        res[i] = rs->frames[i].page_num;
    }

    return res;
}

bool* RS_lfu_get_dirty_flags(RS_lfu_t *rs)
{
    int i;
    bool *res;

    res = (bool*) malloc(sizeof(bool) * rs->max_num_pages);

    for (i = 0; i < rs->max_num_pages; i++) {
        res[i] = rs->frames[i].dirty;
    }

    return res;
}

int* RS_lfu_get_fix_counts(RS_lfu_t *rs)
{
    int i;
    int *res;

    res = (int*) malloc(sizeof(int) * rs->max_num_pages);

    for (i = 0; i < rs->max_num_pages; i++) {
        res[i] = rs->frames[i].fixcount;
    }

    return res;
}

int RS_lfu_get_num_read(RS_lfu_t *rs)
{
    return rs->read_io;
}

int RS_lfu_get_num_write(RS_lfu_t *rs)
{
    return rs->write_io;
}
