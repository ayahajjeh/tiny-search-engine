# Makefile for 'common' directory
# I directly copied the Makefile for cs50 lib and edited it as appropriate  
# 
# Aya Hajjeh
# CS 50, Fall 2022

# object files, and the target library
OBJS = pagedir.o index.o word.o
LIB = common.a


CFLAGS = -Wall -pedantic -std=c11 -ggdb $(FLAGS)
CC = gcc
MAKE = make

# Build $(LIB) by archiving object files
$(LIB): $(OBJS)
	ar cr $(LIB) $(OBJS)

# Dependencies: object files depend on header files
pagedir.o: pagedir.h ../libcs50/webpage.h ../libcs50/mem.h ../libcs50/file.h
index.o: index.h ../libcs50/hashtable.h ../libcs50/set.h ../libcs50/counters.h pagedir.h
word.o: word.h

.PHONY: clean sourcelist

# list all the sources and docs in this directory.
# (this rule is used only by the Professor in preparing the starter kit)
sourcelist: Makefile *.md *.c *.h
	@echo $^

# clean up after our compilation
clean:
	rm -f core
	rm -f $(LIB) *~ *.o
