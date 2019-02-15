#include <stdio.h>
#include <stdlib.h>
#include "storage_mgr.h"
#include "dberror.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <unistd.h>

#define FILENAME_LENGTH 256

SM_FileHandle sm_FileHandle;

void main(){
SM_PageHandle memPage;
memPage = (char*)calloc(PAGE_SIZE, sizeof(char));
int choice, currentPagePos, pageNo, numberOfPages;
char fileName[FILENAME_LENGTH],ch;


while(1)
{
	printf("\n1. Create page file\n"); 
	printf("2. Open page file\n");
	printf("3. Close page file\n");
	printf("4. Destroy page file\n");
	printf("5. Read block\n");
	printf("6. Get current block position\n");
	printf("7. Read first block\n");
	printf("8. Read previous block\n");
	printf("9. Read current block\n");
	printf("10. Read next block\n");
	printf("11. Read last block\n");
	printf("12. Write to a block\n");
	printf("13. Write to the current block\n");
	printf("14. Append empty block to the file\n");
	printf("15. Ensure capacity of the file\n");
	printf("16. Exit\n");
	printf("Enter your choice : ");
	scanf("%d", &choice);
	
	switch(choice){
		case 1: 
			printf("Enter file name to be created : ");
			scanf("%s", fileName);
			if(createPageFile (fileName) == RC_OK){
				printf("\nFile created successfully!\n");
			}else{
				printf("\nFile can't be created/file already created with the same name.Please do check\n");
			}
			break;

		case 2:
			printf("Enter file name to be opened : ");
			scanf("%s", fileName);
			if(openPageFile(fileName, &
			sm_FileHandle) == RC_OK){
				printf("\nFile opened successfully\n");
			}else{
				printf("\nUnable to open file.Please check if the file is open\n");
			}
			break;

		case 3:
			if(closePageFile(&sm_FileHandle) == RC_FILE_NOT_FOUND){
				printf("\nNo file is opened to close...Please do check\n");
			}else{
				printf("\nFile is closed successfully\n");
			}
			break;

		case 4: 
			printf("Enter file name to delete : ");
			scanf("%s", fileName);
			if(destroyPageFile(fileName) == RC_WRITE_FAILED){
				printf("\nFile can not be deleted'\n");
			}else{
				printf("\nFile is deleted\n");
			}
			break;

		case 5:
			printf("Enter the page number to read:");
			scanf("%d", &pageNo);
			if(readBlock(pageNo, &sm_FileHandle, memPage) == RC_OK){
				printf("\nContents of the page\n%s\n", memPage);
				printf("\n");
			}else{
				printf("\nPlease check if file is open or entered the correct page number\n");
			}
			break;

		case 6: 
			currentPagePos = getBlockPos (&sm_FileHandle);
			if (currentPagePos == -1){
				printf("\nPlease open the file\n");				
			}else{
				printf("\nCurrent page position is : %d\n", currentPagePos);
			}
			break;

		case 7:
			if(readFirstBlock (&sm_FileHandle, memPage) == RC_FILE_NOT_FOUND){
				printf("\nPlease ensure file is open\n");
			}else{
				printf("\nContents of the page : \n%s\n\n", memPage);
			}
			break;

		case 8:
			if(readPreviousBlock (&sm_FileHandle, memPage) == RC_OK){
				printf("\nnContents of the page :\n%s\n", memPage);				
			}else{
				printf("\nPlease ensure file is open\n\n");
			}
			break;

		case 9:
			if(readCurrentBlock (&sm_FileHandle, memPage) == RC_FILE_NOT_FOUND){
				printf("\nPlease open the file\n");
			}else{
				printf("\nContents of the page : \n%s\n\n", memPage);
			}
			break;

		case 10:
			if (readNextBlock (&sm_FileHandle, memPage) == RC_OK){
				printf("\nContents of the page :  \n%s\n", memPage);				
			}else{
				printf("\nPlease ensure file is open\n\n");
			}
			break;

		case 11:
			if (readLastBlock (&sm_FileHandle, memPage) == RC_FILE_NOT_FOUND){
				printf("\nPlease ensure file is open\n");
			}else{
				printf("\nContents of the page :   \n%s\n\n", memPage);
			}
			break;

		case 12:
			printf("Enter the page number :");
			scanf("%d", &pageNo);
			printf("Enter a character to be inserted:");
			scanf(" %c", &ch);
			memset(memPage, ch, PAGE_SIZE);
			if (writeBlock (pageNo, &sm_FileHandle, memPage) == RC_OK){
				printf("\nThe page has been filled with the given character\n");
			}else{
				printf("\nPlease ensure file is open\n");
			}
			break;

		case 13:
			printf("Enter a character to be inserted in page :");
			scanf(" %c", &ch);
			memset(memPage, ch, PAGE_SIZE);
			if(writeCurrentBlock (&sm_FileHandle, memPage) == RC_OK){
				printf ("\nCurrent block has been filled with the given character\n");
			}else{
				printf("\nPlease ensure file is open\n");
			}
			break;

		case 14:
			if(appendEmptyBlock (&sm_FileHandle) == RC_FILE_NOT_FOUND){
				printf("\nPlease ensure file is open\n");
			}else{
				printf("\nEmpty block is added to the file\n");
			}
			break;

		case 15:
			printf("Enter the number of pages:");
			scanf("%d", &numberOfPages);
			if(ensureCapacity (numberOfPages, &sm_FileHandle) == RC_FILE_NOT_FOUND){
				printf("\nPlease ensure file is open\n");
			}else{
				printf ("\nThe total number of pages : %d", numberOfPages);
			}
			break;

		case 16:
			exit(0);

		default:
			printf("\nPlease enter valid choice.");
	}
}

}
