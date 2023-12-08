/*
 *
 * index.h is the header file for the index module index.c
 *
 * Aya Hajjeh, Oct 18, 2022
 * Lab5
 * CS50, Fall22
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "../libcs50/hashtable.h"
#include "../libcs50/set.h"
#include "../libcs50/counters.h"
#include "../libcs50/file.h"
#include "pagedir.h"

// 1. use typedef to create index_t from hashtable_t 
// (index_t is essentially a wrapper over hashtable_t)
typedef hashtable_t index_t;

// 2. create functions index_X that call corresponding hashtable_X functions
index_t *index_new(int number_slots);

bool index_insert(index_t* index, const char* key, counters_t* item);

counters_t* index_find(index_t* index, const char* key);

void index_print (index_t* index, FILE* fp, 
                     void (*counterPrint)(FILE* fp, const char* key, counters_t* item));

void index_iterate(index_t* index, void* arg,
                       void (*counterfunc)(void* arg, const char* key, counters_t* item));

void item_delete(counters_t* item);

void index_delete(index_t* index);

// a supplemental function for functionToPrintKey to help print the index counters
void functionToPrintCounter(FILE* arg, const int key, const int count);

// a supplemental function for index_save to help print the index table
void functionToPrintKey(void* arg, const char* key, counters_t* counter);


// 3. create index_save(index_t *index, const char *indexFilename ) to save an index to disk
bool index_save(index_t *index, const char *indexFilename);

// create index_load(const char *indexFilename) to load an index from disk
// (you may find functions in libcs50\file.h helpful)
index_t* index_load(const char *indexFilename);