**CS525 Advanced Database Organization (Fall 2018)**  
**Homework Assignment 1**  
Team members:  
*  Alexandru Iulian Orhean (aorhean@hawk.iit.edu)
*  Karthik Mahesh (kmahesh@hawk.iit.edu)
*  Harshitha Rangaswamy (hrangaswamy@hawk.iit.edu)

# Description of the page file contents on disk

The page file is stored on disk as a file composed of *n + 1* pages of size PAGE_SIZE, with *n > 0*.
The first page is a metadata page that contains in the first 4 bytes (which is a typical int in a
64bit architecture system, and we did not encode that information in complete universal way) the
number of pages stored in the file as actual data.

# Description of data structures used

In order to store the paths of the files that will contains the pages on which we are working upon,
we used a simple linked list, since no assumptions about file creation and destruction were made in
the assignment. Depending on the requirements of the system different data structures could be used.
For example if files that contain pages get opened and closed very often and if they are high in
number a search data structure could be used, such as a hashmap or a search tree. The linked list
implementation is contained in the *linked_list.h* header file and *linked_list.c* source code file.
The linked list keeps track of both the beginning of the list (the first element in the list) and
the end of the list (the last element), using the last element to perform push back operations when
new elements are added to the list. The list allows duplicates, but returns only one element when
lookups are performed (more specifically the first found element). So before insertions the
application using the linked list must check for existing elements before insertions, if duplicates
are wanted to be avoided. The remove function removes all duplicates. Insertion happens in O(1),
while lookup and removal have a time complexity of O(n), n being the length of the list. The list
makes use of the malloc and free functions to increase and decrease in size dynamically. Each
element of the list contains a file path of a page file.

# Description of the implementation of the storage manager

The storage manager is implemented using the linked list as a means to store the page file paths.
Every time a page file is opened and a new page handle is created, the first page of the file is
read and from it it is extracted the metadata information, more precisely the number of pages in the
file that represents actual data. So the actual pages start from the second page. When a new page
file is created, the page handle is initialized with current position to 0 and total number of pages
to 1 (the first page is initialized to '\0'). The C stream file pointer is stored in a mgmtinfo_t
custom data structure, that is casted eventually to a void* when the handle is initialized. When a
page file is closed, the metadata page is updated and written to disk, and the stream file pointer
is closed. When the page file is destroyed, it is removed from the linked list, and it cannot be
accessed anymore (a new page file will overwrite the contents of the old destroyed file).

The rest of the methods make use of the total number of pages and the current positions to perform
various read, write and status operations. The readPreviousBlock, readNextBlock, readFirstBlock and
readLastBlock make use of the readBlock function, while the readCurrentBlock is implemented
independently. Such is the case with the writeBlock, which makes use of the ensureCapacity method
when it tries to write data to a page that exceeds the current total number of pages. The
ensureCapacity method uses the appendEmptyBlock in turn to increase the size. Only writeCurrentBlock
is implemented independently. The functions read and write block different blocks from the file from
different positions by the use of the fseek functions, which allows us to move the current cursor on
the file to a different position (in our case in steps of PAGE_SIZE).

# How to build and run

The submission contains a Makefile with the following targets:
make (all) -> build the given test applications and the storage manager in one binary file
make run -> runs the given test application that uses the implemented storage manager
make clean -> removes all binary files (terminated with .elf)
make test-linked-list -> runs a simple linked list test
make test-complex -> runs a more complex suite of tests, that verify all of the methods that were 
not covered by the given test
make test-complex1 -> gives option to create, read and write the pages however the user wants.
		
# Description of the implemented tests

The linked list testing comprises of a simple scenario in which 10 elements are added and then 5
elements are removed, checking at every step the validity of the operations

The complex test scenario covers the testing of the methods that are not covered by the given test
file. It creates three page files and does write and read operations on them in different orders and
with different data, using different methods to increase the number of pages and in turn to read
pages. At the end of the operations it closes each file, only to reopen them at the end to scenario,
in order to check the integrity of the data and the metadata.

Another simple test case which tests the overall storage buffer functionalities. It accepts input from the 
the user to perform various operations.

