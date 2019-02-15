#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "storage_mgr.h"
#include "dberror.h"

#include "linked_list.h"

/* The data structure that hold additional page handle information; */
typedef struct mgmtinfo_t {
    FILE *file;
} mgmtinfo_t;

/* Global linked list that stores the list of page files; */
linked_list_t list;

void initStorageManager (void)
{
    linked_list_init(&list);
}

RC createPageFile (char *fileName)
{
    FILE *file;
    char firstpage[PAGE_SIZE], metadata[PAGE_SIZE];
    int numpages;

    // If the file is already created do not create a duplicate
    if (linked_list_lookup(&list, fileName))
        return RC_FILE_NOT_FOUND;

    // Test if the file can be opened
    file = fopen(fileName, "wb");
	if(file == NULL)
		return RC_FILE_NOT_FOUND;

    // Initialize the metadata page and the first empty page
	memset(firstpage, '\0', PAGE_SIZE);
    memset(metadata, '\0', PAGE_SIZE);
	
	numpages = 1;
    sprintf(metadata, "%d ", numpages);

    // Write the metadata page and the first empty page to file
    fwrite(metadata, sizeof(char), PAGE_SIZE, file);
	fwrite(firstpage, sizeof(char), PAGE_SIZE, file);

    // Add the page file to the linked list
    linked_list_push_back(&list, fileName);
    
	fclose(file);

	return RC_OK;
}

RC openPageFile (char *fileName, SM_FileHandle *fHandle)
{
    FILE *file;
    int rc, numpages;
    char metadata[PAGE_SIZE];
    mgmtinfo_t *mgmtinfo;

    // Test if the file was created before
    rc = linked_list_lookup(&list, fileName);
    if (rc == 0)
        return RC_FILE_NOT_FOUND;

    // Test if the file can be opened
    file = fopen(fileName, "rb+");
    if (file == NULL)
        return RC_FILE_NOT_FOUND;

    // Read metadata page
    rc = fread(metadata, sizeof(char), PAGE_SIZE, file);
    if (rc != PAGE_SIZE)
        return RC_OK;
    sscanf(metadata, "%d ", &numpages);
    
    // Configure page handle
    fHandle->fileName = (char*) malloc(sizeof(char) * (strlen(fileName) + 1));
    strcpy(fHandle->fileName, fileName);
    fHandle->totalNumPages = numpages;
    fHandle->curPagePos = 0;
    mgmtinfo = (mgmtinfo_t*) malloc(sizeof(mgmtinfo_t));
    mgmtinfo->file = file;
    fHandle->mgmtInfo = (void*) mgmtinfo;
	
	return RC_OK;
}

RC closePageFile (SM_FileHandle *fHandle)
{
    FILE *file;
    char metadata[PAGE_SIZE];
    mgmtinfo_t *mgmtinfo;

    // Test if file handle is initialized
    if (fHandle->mgmtInfo == NULL)
        return RC_FILE_HANDLE_NOT_INIT;

    mgmtinfo = fHandle->mgmtInfo;
    file = mgmtinfo->file;

    // Update file metadata page before file close
    fseek(file, 0, SEEK_SET);
    memset(metadata, '\0', PAGE_SIZE);
    sprintf(metadata, "%d ", fHandle->totalNumPages);
    fwrite(metadata, sizeof(char), PAGE_SIZE, file);

    // Close file/stream descriptor and free allocated data structures
    fclose(file);
    free(mgmtinfo);
    free(fHandle->fileName);
	
    fHandle->fileName = NULL;
	fHandle->totalNumPages = 0;
	fHandle->curPagePos = 0;
	fHandle->mgmtInfo = NULL;

	return RC_OK;
}

RC destroyPageFile (char *fileName)
{
    int rc;

    rc = linked_list_lookup(&list, fileName);
    if (rc == 0)
        return RC_FILE_NOT_FOUND;

    linked_list_remove(&list, fileName);

	return RC_OK;
}

RC readBlock (int pageNum, SM_FileHandle *fHandle, SM_PageHandle memPage)
{
    int rc;
    FILE *file;
    char page[PAGE_SIZE];
    mgmtinfo_t *mgmtinfo;

    // Test if file handle is initialized
    if (fHandle->mgmtInfo == NULL)
        return RC_FILE_HANDLE_NOT_INIT;

    if (pageNum >= fHandle->totalNumPages)
        return RC_READ_NON_EXISTING_PAGE;

    mgmtinfo = (mgmtinfo_t*) fHandle->mgmtInfo;
    file = mgmtinfo->file;

    memset(page, '\0', sizeof(page));
    rc = fseek(file, (pageNum + 1) * PAGE_SIZE, SEEK_SET);
    if (rc != 0)
        return RC_READ_NON_EXISTING_PAGE;
    rc = fread(page, sizeof(char), PAGE_SIZE, file);
    if (rc != PAGE_SIZE)
        return RC_READ_NON_EXISTING_PAGE;
    memcpy(memPage, page, PAGE_SIZE);

    fHandle->curPagePos = pageNum;

    return RC_OK;
}

int getBlockPos (SM_FileHandle *fHandle)
{
    return fHandle->curPagePos;
}

RC readFirstBlock (SM_FileHandle *fHandle, SM_PageHandle memPage)
{
    return readBlock(0, fHandle, memPage);
}

RC readPreviousBlock (SM_FileHandle *fHandle, SM_PageHandle memPage)
{
    if (fHandle->curPagePos == 0)
        return RC_READ_NON_EXISTING_PAGE;
    
    return readBlock(fHandle->curPagePos - 1, fHandle, memPage);
}

RC readCurrentBlock (SM_FileHandle *fHandle, SM_PageHandle memPage)
{
    int rc;
    FILE *file;
    char page[PAGE_SIZE];
    mgmtinfo_t *mgmtinfo;
    
    // Test if file handle is initialized
    if (fHandle->mgmtInfo == NULL)
        return RC_FILE_HANDLE_NOT_INIT;

    mgmtinfo = (mgmtinfo_t*) fHandle->mgmtInfo;
    file = mgmtinfo->file;

    rc = fseek(file, (fHandle->curPagePos + 1) * PAGE_SIZE, SEEK_SET);
    if (rc != 0)
        return RC_READ_NON_EXISTING_PAGE;
    rc = fread(page, sizeof(char), PAGE_SIZE, file);
    if (rc != PAGE_SIZE)
        return RC_READ_NON_EXISTING_PAGE;
    memcpy(memPage, page, PAGE_SIZE);

    return RC_OK;
}

RC readNextBlock (SM_FileHandle *fHandle, SM_PageHandle memPage)
{
    if (fHandle->curPagePos == (fHandle->totalNumPages - 1))
        return RC_READ_NON_EXISTING_PAGE;

    return readBlock(fHandle->curPagePos + 1, fHandle, memPage);
}

RC readLastBlock (SM_FileHandle *fHandle, SM_PageHandle memPage)
{
    return readBlock(fHandle->totalNumPages - 1, fHandle, memPage);
}

RC writeBlock (int pageNum, SM_FileHandle *fHandle, SM_PageHandle memPage)
{
    int rc;
    FILE *file;
    char page[PAGE_SIZE];
    mgmtinfo_t *mgmtinfo;

    // Test if file handle is initialized
    if (fHandle->mgmtInfo == NULL)
        return RC_FILE_HANDLE_NOT_INIT;

    mgmtinfo = (mgmtinfo_t*) fHandle->mgmtInfo;
    file = mgmtinfo->file;

    if (pageNum >= fHandle->totalNumPages) {
        rc = ensureCapacity(pageNum + 1, fHandle);
        if (rc != RC_OK) {
            return rc;
        }
    }

    memcpy(page, memPage, PAGE_SIZE);
    rc = fseek(file, (pageNum + 1) * PAGE_SIZE, SEEK_SET);
    if (rc != 0)
        return RC_WRITE_FAILED;
    rc = fwrite(page, sizeof(char), PAGE_SIZE, file);
    if (rc != PAGE_SIZE)
        return RC_WRITE_FAILED;

    fHandle->curPagePos = pageNum;

    return RC_OK;
}

RC writeCurrentBlock (SM_FileHandle *fHandle, SM_PageHandle memPage)
{
    int rc;
    FILE *file;
    char page[PAGE_SIZE];
    mgmtinfo_t *mgmtinfo;
    
    // Test if file handle is initialized
    if (fHandle->mgmtInfo == NULL)
        return RC_FILE_HANDLE_NOT_INIT;

    mgmtinfo = (mgmtinfo_t*) fHandle->mgmtInfo;
    file = mgmtinfo->file;

    memcpy(page, memPage, PAGE_SIZE);
    rc = fseek(file, (fHandle->curPagePos + 1) * PAGE_SIZE, SEEK_SET);
    if (rc != 0)
        return RC_WRITE_FAILED;
    rc = fwrite(page, sizeof(char), PAGE_SIZE, file);
    if (rc != PAGE_SIZE)
        return RC_WRITE_FAILED;

    return RC_OK;
}

RC appendEmptyBlock (SM_FileHandle *fHandle)
{
    int rc;
    FILE *file;
    char page[PAGE_SIZE];
    mgmtinfo_t *mgmtinfo;

    // Test if file handle is initialized
    if (fHandle->mgmtInfo == NULL)
        return RC_FILE_HANDLE_NOT_INIT;

    mgmtinfo = (mgmtinfo_t*) fHandle->mgmtInfo;
    file = mgmtinfo->file;

    memset(page, '\0', sizeof(page));
    rc = fseek(file, (fHandle->totalNumPages + 1) * PAGE_SIZE, SEEK_SET);
    if (rc != 0)
        return RC_WRITE_FAILED;
    rc = fwrite(page, sizeof(char), PAGE_SIZE, file);
    if (rc != PAGE_SIZE)
        return RC_WRITE_FAILED;
    fHandle->totalNumPages++;

    return RC_OK;
}

RC ensureCapacity (int numberOfPages, SM_FileHandle *fHandle)
{
    int rc, i, n;

    if (numberOfPages > fHandle->totalNumPages) {
        n = numberOfPages - fHandle->totalNumPages;
        for (i = 0; i < n; i++) {
            rc = appendEmptyBlock(fHandle);
            if (rc != RC_OK)
                return RC_WRITE_FAILED;
        }
    }

    return RC_OK;
}
