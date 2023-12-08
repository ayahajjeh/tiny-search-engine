/*
 *
 * indextest.c loads an index output file back to an index
 *
 *
 * Aya Hajjeh, Oct 18, 2022
 * Lab5
 * CS50, Fall22
 *
 */

#include "../libcs50/hashtable.h"
#include "../libcs50/mem.h"
#include "../libcs50/webpage.h"
#include "../common/index.h"
#include "../common/pagedir.h"
#include <stdio.h>

int main(int argc, char* argv[]){
    
    if (argc != 3) exitNonZero("invalid number of arguments");
    char* file1 = argv[1];
    char* file2 = argv[2];
    FILE* fp = fopen(file1, "r");
    if (fp == NULL) exitNonZero("could not open old file name");

    index_t* index;

    index = index_load(file1);
    if (index == NULL)
        exitNonZero("could not load index");

    
    if (!index_save(index, file2)) {
        index_delete(index);
        exitNonZero("could not open the new file name");
    }

    index_delete(index);
    return 0;
}