/*
 *
 * pagedir.h header file for pagedir.c
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

#include "../libcs50/webpage.h"
#include <stdlib.h>
#include "../libcs50/mem.h"
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "../libcs50/file.h"

//purpose: initialize a directory where we will save our crawled webpages by creating a .crawler file in that directory
//parameters: page directory path
//return: true if initialization successfully completed, false if anything else
bool pagedir_init(const char* pageDirectory);

//purpose: save a  webpage to a file
//parameters: a webpage to save, page directory path, and the document ID 
//return: none
void pagedir_save(webpage_t* page, const char* pageDirectory, const int docID);

//purpose: exit the program non-zero after printing an error msg to stderr
//parameters: the error msg we want to print to stderr
//return: exits non-zero
void exitNonZero (char* message);

// pagedir_validate(dir) to verify whether dir is indeed a Crawler-produced directory
bool pagedir_validate(char* dir);

// pagedir_load to load a page from a file in that directory.
webpage_t* pagedir_load(char* pageDirectory, int docID);