/*
 *
 * index.c is an index module.
 * This module is a wrapper around a hashtable that focuses on storing counters as items in the hashtable’s
 * set as describe in IMPLEMENTATION.md. In addition to implementing a hashtable of counters, this index 
 * module provides functions to save an index to disk and to load it into memory from disk. 
 *
 * Aya Hajjeh, Oct 18, 2022
 * Lab5
 * CS50, Fall22
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "index.h"


typedef hashtable_t index_t;


index_t *index_new(int number_slots){
    return (index_t *) hashtable_new(number_slots);
}

bool index_insert(index_t* index, const char* key, counters_t* item){
    return hashtable_insert(index, key, item);
}

counters_t* index_find(index_t* index, const char* key){
    return (counters_t*) hashtable_find(index, key);
}

void index_print (index_t* index, FILE* fp, 
                     void (*itemprint)(FILE* fp, const char* key, counters_t* item)){
    hashtable_print(index, fp, itemprint);
}

void index_iterate(index_t* index, void* arg,
                       void (*counterfunc)(void* arg, const char* key, counters_t* item)){
    hashtable_iterate(index, arg, counterfunc);
}

void item_delete(counters_t* item){
    counters_delete(item);
}

void index_delete(index_t* index){
    hashtable_delete(index, item_delete);
}

void functionToPrintCounter(FILE* arg, const int key, const int count){
    fprintf(arg, "%d %d ", key, count);
}

void functionToPrintKey(void* arg, const char* key, counters_t* counter){
    FILE* fp = arg;
    fprintf(fp, "%s ", key);

    counters_iterate(counter, fp, functionToPrintCounter);
    fprintf(fp, "\n");
}


bool index_save(index_t *index, const char *indexFilename){

    if (index == NULL) exitNonZero("invalid index");
    if (indexFilename == NULL) exitNonZero("invalid index filename");

    FILE* fp = fopen(indexFilename, "w");
    if (fp == NULL) return false;

    index_iterate(index, fp, functionToPrintKey);

    fclose(fp);
    return true;
}

index_t* index_load(const char *indexFilename){
    
    if (indexFilename == NULL) return NULL;
    
    FILE* fp = fopen(indexFilename, "r");
    if (fp == NULL) return NULL;
    
    int numlines;
    if ((numlines = file_numLines(fp)) == 0)
        return NULL;
    
    index_t* index = index_new(numlines/2 + 1);
    
    char* word;
    while ((word = file_readWord(fp)) != NULL){
        
        counters_t* counter = counters_new();
        
        if (counter == NULL) {
            index_delete(index);
            return NULL;
        }
        int pair, docID, count;
        
        while ((pair = fscanf(fp, "%d %d", &docID, &count)) == 2) {
            
            if (!counters_set(counter, docID, count)){
                index_delete(index);
                return NULL;
            }
        }
        
        index_insert(index, word, counter);
        free(word);
    }
    
    fclose(fp);
    return index;
}