# Makefile for 'indexer' directory
# I directly copied the Makefile for cs50 lib and edited it as appropriate  
# 
# Aya Hajjeh
# CS 50, Fall 2022

# object files, and the target library
LLIBS = ../common/common.a ../libcs50/libcs50-given.a
TEST_PROG = testing.sh
PROG1 = indexer
OBJ1 = indexer.o
PROG2 = indextest
OBJ2 = indextest.o



# add -DNOSLEEP to disable the automatic sleep after web-page fetches
#               (students, please do not use -DNOSLEEP!)
# add -DMEMTEST for memory tracking report in indexer
# add -DDEBUG to enable debug mode
# (and run `make clean; make` whenever you change this)
FLAGS = -DDEBUG=1 # -DMEMTEST # -DNOSLEEP

CFLAGS = -Wall -pedantic -std=c11 -ggdb $(FLAGS)
CC = gcc


all: $(PROG1) $(PROG2)

# build indexer
$(PROG1): $(OBJ1)
	$(CC) $(CFLAGS) $^ $(LLIBS) -o $@
# build indextest
$(PROG2): $(OBJ2)
	$(CC) $(CFLAGS) $^ $(LLIBS) -o $@

indexer.o: ../common/index.h ../common/word.h ../common/pagedir.h ../libcs50/set.h ../libcs50/counters.h ../libcs50/hashtable.h ../libcs50/webpage.h ../libcs50/file.h
indextest.o: ../libcs50/hashtable.h ../libcs50/mem.h ../libcs50/webpage.h ../common/index.h ../common/pagedir.h

.PHONY: test clean

test: $(TEST_PROG) 
	bash $(TEST_PROG) &> testing.out

# clean up after compilation
clean:
	rm -f core
	rm -f $(LIB) *~ *.o
	rm -f letters*
	rm -f wiki*
	rm -f indexer-out*
	rm -f withoutwrite
	rm -f indexer
	rm -f indextest