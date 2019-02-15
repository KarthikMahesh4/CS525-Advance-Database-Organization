#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "storage_mgr.h"
#include "dberror.h"
#include "test_helper.h"

#define N 10

// test name
char *testName;

/* test output files */
#define TESTPF1 "test_pagefile_1.bin"
#define TESTPF2 "test_pagefile_2.bin"
#define TESTPF3 "test_pagefile_3.bin"

/* prototypes for test functions */
static void testCreateOpenWriteReadClose(void);
static void testCreateOpenAppendWriteReadNextClose(void);
static void testCreateOpenCapacityWriteReadPrevClose(void);
static void testDestroyAll(void);

/* main function running all tests */
int
main (void)
{
  testName = "";
  
  initStorageManager();

  testCreateOpenWriteReadClose();
  testCreateOpenAppendWriteReadNextClose();
  testCreateOpenCapacityWriteReadPrevClose();
  testDestroyAll();

  return 0;
}

int compare_pages(char * page1, char *page2)
{
    int i;

    for (i = 0; i < PAGE_SIZE; i++) {
        if (page1[i] < page2[i])
            return -1;
        if (page1[i] > page2[i])
            return 1;
    }

    return 0;
}

void testCreateOpenWriteReadClose(void)
{
    int i;
    SM_FileHandle fh;
    char page1[PAGE_SIZE], page2[PAGE_SIZE];
    SM_PageHandle mempage1, mempage2;

    mempage1 = (SM_PageHandle) page1;
    mempage2 = (SM_PageHandle) page2;

    testName = "test write and read block methods";

    TEST_CHECK(createPageFile(TESTPF1));
    TEST_CHECK(openPageFile(TESTPF1, &fh));
    ASSERT_TRUE(strcmp(fh.fileName, TESTPF1) == 0, "filename correct");

    for (i = 0; i < N; i++) {
        memset(page1, 'A' + (char) i, sizeof(page1));
        TEST_CHECK(writeBlock(i, &fh, mempage1));
    }

    for (i = 0; i < N; i++) {
        memset(page1, 'A' + (char) i, sizeof(page1));
        memset(page2, '\0', sizeof(page2));
        TEST_CHECK(readBlock(i, &fh, mempage2));
        ASSERT_TRUE(compare_pages(mempage1, mempage2) == 0, "correct page");
    }
    
    TEST_CHECK(closePageFile(&fh));
    
    TEST_DONE();
}
static void testCreateOpenAppendWriteReadNextClose(void)
{
    int i;
    SM_FileHandle fh;
    char page1[PAGE_SIZE], page2[PAGE_SIZE];
    SM_PageHandle mempage1, mempage2;

    mempage1 = (SM_PageHandle) page1;
    mempage2 = (SM_PageHandle) page2;

    testName = "test write and read block methods with append";

    TEST_CHECK(createPageFile(TESTPF2));
    TEST_CHECK(openPageFile(TESTPF2, &fh));
    ASSERT_TRUE(strcmp(fh.fileName, TESTPF2) == 0, "filename correct");

    for (i = 1; i < N; i++) {
        TEST_CHECK(appendEmptyBlock(&fh));
    }
    
    for (i = 0; i < N; i++) {
        memset(page1, '\0', sizeof(page1));
        memset(page2, '\n', sizeof(page2));
        TEST_CHECK(readBlock(i, &fh, mempage2));
        ASSERT_TRUE(compare_pages(mempage1, mempage2) == 0, "correct page");
    }
    
    for (i = 0; i < N; i++) {
        memset(page1, 'E' + (char) i, sizeof(page1));
        TEST_CHECK(writeBlock(i, &fh, mempage1));
    }

    for (i = 0; i < N; i++) {
        memset(page1, 'E' + (char) i, sizeof(page1));
        memset(page2, '\0', sizeof(page2));
        if (i == 0) {
            TEST_CHECK(readFirstBlock(&fh, mempage2));
        } else {
            TEST_CHECK(readNextBlock(&fh, mempage2));
        }

        ASSERT_TRUE(compare_pages(mempage1, mempage2) == 0, "correct page");
    }
    
    TEST_CHECK(closePageFile(&fh));
    
    TEST_DONE();
}

static void testCreateOpenCapacityWriteReadPrevClose(void)
{
    int i;
    SM_FileHandle fh;
    char page1[PAGE_SIZE], page2[PAGE_SIZE];
    SM_PageHandle mempage1, mempage2;

    mempage1 = (SM_PageHandle) page1;
    mempage2 = (SM_PageHandle) page2;

    testName = "test write and read in reverse order";

    TEST_CHECK(createPageFile(TESTPF3));
    TEST_CHECK(openPageFile(TESTPF3, &fh));
    ASSERT_TRUE(strcmp(fh.fileName, TESTPF3) == 0, "filename correct");

    TEST_CHECK(ensureCapacity(N, &fh));
    
    for (i = 0; i < N; i++) {
        memset(page1, '\0', sizeof(page1));
        memset(page2, 'N', sizeof(page2));
        TEST_CHECK(readBlock(i, &fh, mempage2));
        ASSERT_TRUE(compare_pages(mempage1, mempage2) == 0, "correct page");
    }
    
    for (i = N - 1; i >= 0; i--) {
        memset(page1, 'K' + (char) i, sizeof(page1));
        TEST_CHECK(writeBlock(i, &fh, mempage1));
    }

    for (i = N - 1; i >= 0; i--) {
        memset(page1, 'K' + (char) i, sizeof(page1));
        memset(page2, '\0', sizeof(page2));
        if (i == N - 1) {
            TEST_CHECK(readLastBlock(&fh, mempage2));
        } else {
            TEST_CHECK(readPreviousBlock(&fh, mempage2));
        }

        ASSERT_TRUE(compare_pages(mempage1, mempage2) == 0, "correct page");
    }
    
    TEST_CHECK(closePageFile (&fh));
    
    TEST_DONE();
}

static void testDestroyAll(void)
{
    SM_FileHandle fh;
    char page1[PAGE_SIZE], page2[PAGE_SIZE];
    SM_PageHandle mempage1, mempage2;

    mempage1 = (SM_PageHandle) page1;
    mempage2 = (SM_PageHandle) page2;

    testName = "test file re-open and destruction";

    // TETSPF1
    TEST_CHECK(openPageFile(TESTPF1, &fh));
    ASSERT_TRUE(strcmp(fh.fileName, TESTPF1) == 0, "filename correct");

    memset(page1, 'Z', sizeof(page1));
    TEST_CHECK(writeBlock(1, &fh, mempage1));

    memset(page1, 'A', sizeof(page1));
    memset(page2, '\0', sizeof(page2));
    TEST_CHECK(readBlock(0, &fh, mempage2));
    ASSERT_TRUE(compare_pages(mempage1, mempage2) == 0, "correct page");
    
    memset(page1, 'Z', sizeof(page1));
    memset(page2, '\0', sizeof(page2));
    TEST_CHECK(readBlock(1, &fh, mempage2));
    ASSERT_TRUE(compare_pages(mempage1, mempage2) == 0, "correct page");
    
    ASSERT_TRUE(getBlockPos(&fh) == 1, "correct block position");
    
    TEST_CHECK(closePageFile (&fh));
    TEST_CHECK(destroyPageFile (TESTPF1));
    
    ASSERT_TRUE((openPageFile(TESTPF1, &fh) != RC_OK),
            "opening non-existing file should return an error.");
    
    // TESTPF3
    TEST_CHECK(openPageFile(TESTPF3, &fh));
    ASSERT_TRUE(strcmp(fh.fileName, TESTPF3) == 0, "filename correct");

    memset(page1, 'Z', sizeof(page1));
    TEST_CHECK(writeBlock(1, &fh, mempage1));

    memset(page1, 'K', sizeof(page1));
    memset(page2, '\0', sizeof(page2));
    TEST_CHECK(readBlock(0, &fh, mempage2));
    ASSERT_TRUE(compare_pages(mempage1, mempage2) == 0, "correct page");
    
    memset(page1, 'Z', sizeof(page1));
    memset(page2, '\0', sizeof(page2));
    TEST_CHECK(readBlock(1, &fh, mempage2));
    ASSERT_TRUE(compare_pages(mempage1, mempage2) == 0, "correct page");
    
    ASSERT_TRUE(getBlockPos(&fh) == 1, "correct block position");
    
    TEST_CHECK(closePageFile (&fh));
    TEST_CHECK(destroyPageFile (TESTPF3));
    
    ASSERT_TRUE((openPageFile(TESTPF3, &fh) != RC_OK),
            "opening non-existing file should return an error.");
    
    // TESTPF2
    TEST_CHECK(openPageFile(TESTPF2, &fh));
    ASSERT_TRUE(strcmp(fh.fileName, TESTPF2) == 0, "filename correct");

    memset(page1, 'Z', sizeof(page1));
    TEST_CHECK(writeBlock(1, &fh, mempage1));

    memset(page1, 'E', sizeof(page1));
    memset(page2, '\0', sizeof(page2));
    TEST_CHECK(readBlock(0, &fh, mempage2));
    ASSERT_TRUE(compare_pages(mempage1, mempage2) == 0, "correct page");
    
    memset(page1, 'Z', sizeof(page1));
    memset(page2, '\0', sizeof(page2));
    TEST_CHECK(readBlock(1, &fh, mempage2));
    ASSERT_TRUE(compare_pages(mempage1, mempage2) == 0, "correct page");
    
    ASSERT_TRUE(getBlockPos(&fh) == 1, "correct block position");
    
    TEST_CHECK(closePageFile (&fh));
    TEST_CHECK(destroyPageFile (TESTPF2));
    
    ASSERT_TRUE((openPageFile(TESTPF2, &fh) != RC_OK),
            "opening non-existing file should return an error.");
    
    TEST_DONE();
}
