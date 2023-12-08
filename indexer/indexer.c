/*
 *
 * indexer.c builds an indexer using the index module
 *
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

#include "../common/index.h"
#include "../common/word.h"
#include "../common/pagedir.h"
#include "../libcs50/set.h"
#include "../libcs50/counters.h"
#include "../libcs50/hashtable.h"
#include "../libcs50/webpage.h"
#include "../libcs50/file.h"

index_t* indexBuild(char* pageDirectory);
void indexPage(index_t* index, webpage_t* page, int docID);

/**
 * indexer driver
 */
int main(int argc, char* argv[]){

    if (argc != 3) exitNonZero("invalid number of arguments");
    char* pageDirectory = argv[1];
    if (!pagedir_validate(pageDirectory))
        exitNonZero("invalid page directory");
    char* indexFilename = argv[2];
    index_t* index = indexBuild(pageDirectory);
    index_save(index, indexFilename);

    index_delete(index);
  
  return 0;
}


index_t* indexBuild(char* pageDirectory){

    index_t* index = index_new(1000);
    int docID = 1;

    webpage_t* page = pagedir_load(pageDirectory, docID);
    while (page != NULL){
        indexPage(index, page, docID);
        webpage_delete(page);
        docID++;
        page = pagedir_load(pageDirectory, docID);
    }
    return index;
}


void indexPage(index_t* index, webpage_t* page, int docID){
    
    int pos = 0;
    char* word = webpage_getNextWord(page, &pos);
    
    while (word != NULL) {
        if (strlen(word) > 2)
            normalizeWord(word);
        counters_t* counters = index_find(index, word);
        if (counters == NULL) {
            counters_t* counters = counters_new();
            index_insert(index, word, counters);
        }
        counters_add(index_find(index, (char*) (&docID)), docID);
        word = webpage_getNextWord(page, &pos);
    }
    free(word);
}
