#include "storage_mgr.h"
#include "buffer_mgr_stat.h"
#include "buffer_mgr.h"
#include "dberror.h"
#include "test_helper.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// var to store the current test's name
char *testName;

// check whether two the content of a buffer pool is the same as an expected content 
// (given in the format produced by sprintPoolContent)
#define ASSERT_EQUALS_POOL(expected,bm,message)			        \
  do {									\
    char *real;								\
    char *_exp = (char *) (expected);                                   \
    real = sprintPoolContent(bm);					\
    if (strcmp((_exp),real) != 0)					\
      {									\
	printf("[%s-%s-L%i-%s] FAILED: expected <%s> but was <%s>: %s\n",TEST_INFO, _exp, real, message); \
	free(real);							\
	exit(1);							\
      }									\
    printf("[%s-%s-L%i-%s] OK: expected <%s> and was <%s>: %s\n",TEST_INFO, _exp, real, message); \
    free(real);								\
  } while(0)

// test and helper methods
static void testCreateWriteReadStorageManager(int n);
static void testCreateWriteReadBufferManagerLFU(int n, int m);

// main method
int 
main (void) 
{
  initStorageManager();
  testName = "";

    testCreateWriteReadStorageManager(20);
    testCreateWriteReadBufferManagerLFU(20, 8);

  return 0;
}

/* create n pages with contents from 'a' to 'j' and read them back to check 
 * if the content is right */
static void testCreateWriteReadStorageManager(int n)
{
    int i;
    char page1[PAGE_SIZE], page2[PAGE_SIZE];
    testName = "Create, Write and Read Storage Manager";
    SM_FileHandle file_handle;
    SM_PageHandle mempage1, mempage2;

    mempage1 = (SM_PageHandle) page1;
    mempage2 = (SM_PageHandle) page2;

    TEST_CHECK(createPageFile("testbuffer_lfu.bin"));
    TEST_CHECK(openPageFile("testbuffer_lfu.bin", &file_handle));
    
    for (i = 0; i < n; i += 2) {
        memset(page1, 'a' + (char) i, sizeof(page1));
        TEST_CHECK(writeBlock(i, &file_handle, mempage1));
    }

    for (i = n - 1; i > 0; i -= 2) {
        memset(page1, 'a' + (char) i, sizeof(page1));
        TEST_CHECK(writeBlock(i, &file_handle, mempage1));
    }

    for (i = 0; i < n; i += 1) {
        memset(page1, 'a' + (char) i, sizeof(page1));
        memset(page2, 0, sizeof(page2));
        TEST_CHECK(readBlock(i, &file_handle, mempage2));
        page1[PAGE_SIZE - 1] = '\0';
        page2[PAGE_SIZE - 1] = '\0';
        ASSERT_EQUALS_STRING(page1, page2, "checking storage manager pages");
    }

    TEST_CHECK(closePageFile(&file_handle));

    TEST_DONE();
}

static void testCreateWriteReadBufferManagerLFU(int n, int m)
{
    int i;
    char **pages1;
    char **pages2;
    testName = "Create, Write and Read LFU Buffer Manager";
    SM_FileHandle file_handle;
    SM_PageHandle mempage;
    BM_PageHandle *mempages;
    BM_BufferPool bm;
    int *frame_contents;
    bool *dirty_flags;
    int *fix_counts;

    pages1 = (char **) malloc(sizeof(char*) * n);
    pages2 = (char **) malloc(sizeof(char*) * n);
    mempages = (BM_PageHandle*) malloc(sizeof(BM_PageHandle) * n);
    for (i = 0; i < n; i++) {
        pages1[i] = (char *) malloc(sizeof(char) * PAGE_SIZE);
        pages2[i] = (char *) malloc(sizeof(char) * PAGE_SIZE);
        memset(pages1[i], 'a' + (char) i, PAGE_SIZE);
        mempages[i].data = NULL;
    }

    TEST_CHECK(openPageFile("testbuffer_lfu.bin", &file_handle));
    TEST_CHECK(initBufferPool(&bm, "testbuffer_lfu.bin", m, RS_LFU, NULL));
    
    for (i = 0; i < n; i++) {
        mempage = (SM_PageHandle) pages1[i];
        TEST_CHECK(writeBlock(i, &file_handle, mempage));
    }

    // Scenario 1 - pin page and read
    for (i = 0; i < m; i++) {
        TEST_CHECK(pinPage(&bm, &mempages[i], (PageNumber) i));
    }
    
    frame_contents = getFrameContents(&bm);
    dirty_flags = getDirtyFlags(&bm);
    fix_counts = getFixCounts(&bm);

    for (i = 0; i < m; i++) {
        ASSERT_TRUE(i == frame_contents[i], "SC1 check remaining frames");
        ASSERT_TRUE(false == dirty_flags[i], "SC1 check remaining flags");
        ASSERT_TRUE(1 == fix_counts[i], "SC1 check remaining counts");
    }
    
    for (i = 0; i < m; i++) {
        memset(pages1[i], 'a' + (char) i, PAGE_SIZE);
        pages1[i][PAGE_SIZE - 1] = '\0';
        mempages[i].data[PAGE_SIZE - 1] = '\0';
        ASSERT_EQUALS_STRING(pages1[i], mempages[i].data, 
                "SC1 checking buffer pool pages");
        mempages[i].data[PAGE_SIZE - 1] = 'a' + (char) i;
    }
    
    for (i = 0; i < m; i++) {
        mempage = (SM_PageHandle) pages2[i];
        memset(pages1[i], 'a' + (char) i, PAGE_SIZE);
        memset(pages2[i], 0, PAGE_SIZE);
        TEST_CHECK(readBlock(i, &file_handle, mempage));
        pages1[i][PAGE_SIZE - 1] = '\0';
        pages2[i][PAGE_SIZE - 1] = '\0';
        ASSERT_EQUALS_STRING(pages1[i], pages2[i], 
                "SC1 checking storage manager pages");
    }
    
    // Scenario 2 - write and unpin but do not flush
    for (i = 0; i < m; i++) {
        memset(mempages[i].data, 'A' + (char) i, PAGE_SIZE);
        TEST_CHECK(markDirty(&bm, &mempages[i]));
        TEST_CHECK(unpinPage(&bm, &mempages[i]));
    }

    frame_contents = getFrameContents(&bm);
    dirty_flags = getDirtyFlags(&bm);
    fix_counts = getFixCounts(&bm);

    for (i = 0; i < m; i++) {
        ASSERT_TRUE(i == frame_contents[i], "SC2 check remaining frames");
        ASSERT_TRUE(true == dirty_flags[i], "SC2 check remaining flags");
        ASSERT_TRUE(0 == fix_counts[i], "SC2 check remaining counts");
    }
    
    for (i = 0; i < m; i++) {
        memset(pages1[i], 'A' + (char) i, PAGE_SIZE);
        pages1[i][PAGE_SIZE - 1] = '\0';
        mempages[i].data[PAGE_SIZE - 1] = '\0';
        ASSERT_EQUALS_STRING(pages1[i], mempages[i].data, 
                "SC2 checking buffer pool pages");
        mempages[i].data[PAGE_SIZE - 1] = 'A' + (char) i;
    }
    
    for (i = 0; i < m; i++) {
        mempage = (SM_PageHandle) pages2[i];
        memset(pages1[i], 'a' + (char) i, PAGE_SIZE);
        memset(pages2[i], 0, PAGE_SIZE);
        TEST_CHECK(readBlock(i, &file_handle, mempage));
        pages1[i][PAGE_SIZE - 1] = '\0';
        pages2[i][PAGE_SIZE - 1] = '\0';
        ASSERT_EQUALS_STRING(pages1[i], pages2[i], 
                "SC2 checking storage manager pages");
    }
    
    // Scenario 3 - force flush
    TEST_CHECK(forceFlushPool(&bm));
    
    frame_contents = getFrameContents(&bm);
    dirty_flags = getDirtyFlags(&bm);
    fix_counts = getFixCounts(&bm);

    for (i = 0; i < m; i++) {
        ASSERT_TRUE(i == frame_contents[i], "SC3 check remaining frames");
        ASSERT_TRUE(false == dirty_flags[i], "SC3 check remaining flags");
        ASSERT_TRUE(0 == fix_counts[i], "SC3 check remaining counts");
    }
    
    for (i = 0; i < m; i++) {
        memset(pages1[i], 'A' + (char) i, PAGE_SIZE);
        pages1[i][PAGE_SIZE - 1] = '\0';
        mempages[i].data[PAGE_SIZE - 1] = '\0';
        ASSERT_EQUALS_STRING(pages1[i], mempages[i].data, 
                "SC3 checking buffer pool pages");
        mempages[i].data[PAGE_SIZE - 1] = 'A' + (char) i;
    }
    
    for (i = 0; i < m; i++) {
        mempage = (SM_PageHandle) pages2[i];
        memset(pages1[i], 'A' + (char) i, PAGE_SIZE);
        memset(pages2[i], 0, PAGE_SIZE);
        TEST_CHECK(readBlock(i, &file_handle, mempage));
        pages1[i][PAGE_SIZE - 1] = '\0';
        pages2[i][PAGE_SIZE - 1] = '\0';
        ASSERT_EQUALS_STRING(pages1[i], pages2[i], 
                "SC3 checking storage manager pages");
    }

    // Scenario 4 - test LFU replacement
    for (i = 0; i < m / 2; i++) {
        TEST_CHECK(pinPage(&bm, &mempages[i], (PageNumber) i));
        memset(mempages[i].data, 'z' - (char) i, PAGE_SIZE);
        TEST_CHECK(markDirty(&bm, &mempages[i]));
        TEST_CHECK(forcePage(&bm, &mempages[i]));
        TEST_CHECK(unpinPage(&bm, &mempages[i]));
    }
    
    for (i = m; i < m + (m / 2); i++) {
        TEST_CHECK(pinPage(&bm, &mempages[i], (PageNumber) i));
        memset(mempages[i].data, 'Z' - (char) i, PAGE_SIZE);
        TEST_CHECK(markDirty(&bm, &mempages[i]));
        TEST_CHECK(forcePage(&bm, &mempages[i]));
    }

    frame_contents = getFrameContents(&bm);
    dirty_flags = getDirtyFlags(&bm);
    fix_counts = getFixCounts(&bm);

    for (i = 0; i < m / 2; i++) {
        ASSERT_TRUE(i == frame_contents[i], "SC4 check remaining frames");
        ASSERT_TRUE(false == dirty_flags[i], "SC4 check remaining flags");
        ASSERT_TRUE(0 == fix_counts[i], "SC4 check remaining counts");
    }

    for (i = m; i < m + m / 2; i++) {
        ASSERT_TRUE(i == frame_contents[(m / 2 + (i - m))], 
                "SC4 check remaining frames");
        ASSERT_TRUE(false == dirty_flags[(m / 2 + (i - m))],
                "SC4 check remaining flags");
        ASSERT_TRUE(1 == fix_counts[(m / 2 + (i - m))], 
                "SC4 check remaining counts");
    }
    
    for (i = 0; i < m / 2; i++) {
        memset(pages1[i], 'z' - (char) i, PAGE_SIZE);
        pages1[i][PAGE_SIZE - 1] = '\0';
        mempages[i].data[PAGE_SIZE - 1] = '\0';
        ASSERT_EQUALS_STRING(pages1[i], mempages[i].data, 
                "SC4 checking buffer pool pages");
        mempages[i].data[PAGE_SIZE - 1] = 'z' - (char) i;
    }
    
    for (i = 0; i < m / 2; i++) {
        mempage = (SM_PageHandle) pages2[i];
        memset(pages1[i], 'z' - (char) i, PAGE_SIZE);
        memset(pages2[i], 0, PAGE_SIZE);
        TEST_CHECK(readBlock(i, &file_handle, mempage));
        pages1[i][PAGE_SIZE - 1] = '\0';
        pages2[i][PAGE_SIZE - 1] = '\0';
        ASSERT_EQUALS_STRING(pages1[i], pages2[i], 
                "SC4 checking storage manager pages");
    }
    
    for (i = m; i < m + m / 2; i++) {
        memset(pages1[i], 'Z' - (char) i, PAGE_SIZE);
        pages1[i][PAGE_SIZE - 1] = '\0';
        mempages[i].data[PAGE_SIZE - 1] = '\0';
        ASSERT_EQUALS_STRING(pages1[i], mempages[i].data, 
                "SC4 checking buffer pool pages");
        mempages[i].data[PAGE_SIZE - 1] = 'Z' - (char) i;
    }
    
    for (i = m; i < m + m / 2; i++) {
        mempage = (SM_PageHandle) pages2[i];
        memset(pages1[i], 'Z' - (char) i, PAGE_SIZE);
        memset(pages2[i], 0, PAGE_SIZE);
        TEST_CHECK(readBlock(i, &file_handle, mempage));
        pages1[i][PAGE_SIZE - 1] = '\0';
        pages2[i][PAGE_SIZE - 1] = '\0';
        ASSERT_EQUALS_STRING(pages1[i], pages2[i], 
                "SC4 checking storage manager pages");
    }
  

    TEST_CHECK(closePageFile(&file_handle));
    TEST_CHECK(!shutdownBufferPool(&bm));
    
    for (i = m; i < m + (m / 2); i++) {
        TEST_CHECK(unpinPage(&bm, &mempages[i]));
    }
    
    ASSERT_EQUALS_INT(16, getNumWriteIO(&bm), "check number of write I/Os");
    ASSERT_EQUALS_INT(12, getNumReadIO(&bm), "check number of read I/Os");
    
    TEST_CHECK(shutdownBufferPool(&bm));

    for (i = 0; i < n; i++) {
        free(pages1[i]);
        free(pages2[i]);
    }
    free(mempages);
    free(pages2);
    free(pages1);

    TEST_DONE();
}

