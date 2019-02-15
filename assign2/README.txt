**CS525 Advanced Database Organization (Fall 2018)**  
**Homework Assignment 2**  
Team members:  
*  Alexandru Iulian Orhean (aorhean@hawk.iit.edu)
*  Karthik Mahesh (kmahesh@hawk.iit.edu)
*  Harshitha Rangaswamy (hrangaswamy@hawk.iit.edu)

# Description of the storage manager

We are using the Homework Assignment 1 implementation of the storage manager with no modifications.

# Description of data structures and implementation

The replacement strategies and thus the buffer manager make use of a statically allocated vector of
page frames, that contain all the necessary information for determining a course of action when a
replacement is needed or when the flags and fix counts are changed. For all the replacement
strategies implemented, all lookups, insertions and deletions take O(n) since the vector is not
sorted at any point in time, allowing for a more simpler correct implementation. Advanced data
structures could be used to sort the elements and organize them in different layers. For example,
for the LRU strategy, we could have a hashmap or a self-balancing tree that allows quick lookups
over the page frames using the page number as a value and then we could have another linked list
that keeps track of the actual page frames. The elements from the hashmap or tree need to point to
the elements in the linked list and vice versa. We tried to implement an AVL tree, but we
reconsidered the benefits of using this data structure. This approach raises the complexity of the
system and could have certain benefits if the buffer is large, allowing for O(log(n)) insertion,
removal and lookup times, while allowing to find the least recently used frame in O(1). But the
actual operations incur additional penalties, thus for small buffer sizes and small datasets, this 
approach suffices. In the future (the next Homework Assignment) we will reconsider these data
structures.

The FIFO strategy implementation can be found in the *rs_fifo.c* and *rs_fifo.h* files. It used a
ticketing system, the *RS_fifo_t* data structure containing the value of the next available ticket,
while each frame storing the received ticket. The tickets determine which frame is going to be
evicted next, that is, the frame with the smallest ticket, of course if the fix count is zero. The 
tickets are not updated if multiple pinning happen on a frame that already exists in the buffer,
thus only when a new page is added to the buffer a new ticket is being generated.

The LRU strategy implementation can be found in the *rs_lru.c* and *rs_lru.h* files. It uses an
aging mechanism, in which each frame has an age associated with it, representing how far away from a
temporal point of view, is the frame from the most recent "interaction" with the rest of the frames.
The most recent frame will have an age of 1, while the least recently used one can have a maximum
age of ULONG_MAX. Of course when a frame needs to be evicted, the age information is used and the
frame with the larges age (and fix count set to zero) is evicted. After each pinning and unpinning
operations all the frames increase their age with one (including the new frame, which starts with
age 0 and thus increments it to 1).

The LFU strategy implementation can be found in the *rs_lfu.c* and *rs_lfu.h* files. It uses a
variable for each frame that stores the number of interactions with that frame (a.k.a. the
frequency). Using a similar approach to the FIFO strategy, the frame that gets to be evicted is
determined by the frequency, the frame with the smallest frequency and with fix count set to zero
being the one selected for eviction. After each interaction (pinning and unpinning) the frequency of
the frame is increased by one, and it is initialized to 1 when a new frame gets pinned.

All of the implemented strategies flush pages to disk when the dirty flag is set under the
conditions determined by the specification. The strategies that have not been mentioned are not
implemented, even though the source files are uploaded to the repository. Also no attempts to
parallelize the buffer manager have been made, but one naive approach would be to add locks to each
function allowing multiple threads to use them concurrently, though this simple approach might not
yield great performance.

# How to build and run

The submission contains a Makefile with the following targets:
make (all) -> build the given test applications and the buffer manager in one binary file
make run -> runs the given test application that uses the implemented buffer manager
make clean -> removes all binary files (terminated with .elf)
make test-fifo -> runs a simple FIFO strategy test
make test-lru -> runs a simple LRU strategy test
		
# Description of the implemented tests

All custom test implemented (not given by the assignment) follow 4 scenarios, testing various
functions throughout them, trying to cover as much as possible the implemented code. Scenario 1 only
pins the pages and tests if they are viewed correct both by the buffer manager and the storage
manager. Scenario 2 modifies the page frames, but does not unpin them or force them to disk, thus
the buffer manager and storage manager should have different views of the data. Scenario 3 flushes
all the modifications to disk restoring consistency between the buffer manager and the storage
manager. The last scenario tests if the strategy was implemented correctly, by pinning, writing,
unpinning and reading specific pages in order to validate the implementation.
