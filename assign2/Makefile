CC=gcc
CFLAGS=-Wall -O3

SRC=buffer_mgr.c buffer_mgr_stat.c rs_fifo.c rs_lru.c rs_clock.c rs_lfu.c rs_lru_k.c
DEP-SRC=storage_mgr.c linked_list.c
ERR-SRC=dberror.c

all: test_assign2_1.elf

run: test_assign2_1.elf
	./test_assign2_1.elf

test_assign2_1.elf: test_assign2_1.c $(SRC) $(DEP-SRC) $(ERR-SRC)
	$(CC) $(CFLAGS) -o test_assign2_1.elf test_assign2_1.c $(SRC) $(DEP-SRC) $(ERR-SRC)

test_lru: test_assign2_lru.c $(SRC) $(DEP-SRC) $(ERR-SRC)
	$(CC) $(CFLAGS) -o test_assign2_lru.elf test_assign2_lru.c $(SRC) $(DEP-SRC) $(ERR-SRC)

test_fifo: test_assign2_fifo.c $(SRC) $(DEP-SRC) $(ERR-SRC)
	$(CC) $(CFLAGS) -o test_assign2_fifo.elf test_assign2_fifo.c $(SRC) $(DEP-SRC) $(ERR-SRC)

test_lfu: test_assign2_lfu.c $(SRC) $(DEP-SRC) $(ERR-SRC)
	$(CC) $(CFLAGS) -o test_assign2_lfu.elf test_assign2_lfu.c $(SRC) $(DEP-SRC) $(ERR-SRC)

clean:
	rm *.elf *.bin
