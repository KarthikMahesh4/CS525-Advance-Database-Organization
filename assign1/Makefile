CC=gcc
CFLAGS=-Wall -O3

all: test_assign1_1.elf

run: test_assign1_1.elf
	./test_assign1_1.elf

test_assign1_1.elf: test_assign1_1.c storage_mgr.c dberror.c linked_list.c
	$(CC) $(CFLAGS) -o test_assign1_1.elf test_assign1_1.c storage_mgr.c dberror.c linked_list.c

test-complex: test_assign1_2.elf
	./test_assign1_2.elf

test-complex1: test_assign1_3.elf
	./test_assign1_3.elf

test_assign1_2.elf: test_assign1_2.c storage_mgr.c dberror.c linked_list.c
	$(CC) $(CFLAGS) -o test_assign1_2.elf test_assign1_2.c storage_mgr.c dberror.c linked_list.c
	
test_assign1_3.elf: test_assign1_3.c storage_mgr.c dberror.c linked_list.c
	$(CC) $(CFLAGS) -o test_assign1_3.elf test_assign1_3.c storage_mgr.c dberror.c linked_list.c

test-linked-list: test_linked_list.elf
	./test_linked_list.elf

test_linked_list.elf: test_linked_list.c linked_list.c
	$(CC) $(FLAGS) -o test_linked_list.elf test_linked_list.c linked_list.c

clean:
	rm *.elf
