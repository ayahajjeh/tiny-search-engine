# Makefile for 'querier' directory
# I directly copied the Makefile for cs50 lib and edited it as appropriate  
# 
# Aya Hajjeh
# CS 50, Fall 2022

# object files, and the target library
LLIBS = ../common/common.a ../libcs50/libcs50-given.a
PROGS = freadline querier fuzzquery

FLAGS = -DDEBUG=1 # -DMEMTEST # -DNOSLEEP

CFLAGS = -Wall -pedantic -std=c11 -ggdb $(FLAGS)
CC = gcc

.PHONY: test clean all

all: $(PROGS)

# build freadline
freadline: freadline.o
	ar cr freadline freadline.o

# build querier
querier: querier.o freadline.o $(LLIBS)
	$(CC) $(CFLAGS) $(FLAGS) $^ -o $@
# build fuzzquery
fuzzquery: fuzzquery.o $(LLIBS)
	$(CC) $(CFLAGS) $^ -o $@

freadline.o: freadline.h

test:
	bash testing.sh &> testing.out

# clean up after our compilation
clean:
	rm -f core
	rm -f $(LIB) *~ *.o
	rm -f freadline
	rm -f querier
	rm -f fuzzquery