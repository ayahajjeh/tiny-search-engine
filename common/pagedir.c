/*
*
* pagedir.c a re-usable module  to handle interactions with page directories required by the crawler and the
* indexer.
* We chose to write this as a separate module, in ../common, to encapsulate all the knowledge about how 
* to initialize and validate a pageDirectory, and how to write and read page files, in one place.
*
* see pagedir.h for more information
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

#include "../common/pagedir.h"

void exitNonZero (char* message){
    fprintf(stderr, "%s\n", message);
    exit(1);
}

bool pagedir_init(const char* pageDirectory){

    if (pageDirectory==NULL) return false;

    char* fileName = "/.crawler";
    char* pathName = mem_malloc(sizeof(fileName) + sizeof(pageDirectory));
    memcpy(pathName, pageDirectory, sizeof(pageDirectory));
    strcat(pathName, fileName);

    FILE* fp = fopen(pathName,"w");
    if (fp == NULL) return false;

    fclose(fp);
    free(pathName);
    return true;

}

void pagedir_save(webpage_t* page, const char* pageDirectory, const int docID){

    if (page == NULL || pageDirectory == NULL || docID < 0) exitNonZero("invalid parameters in pagedir_save");

    char fileName = (char) docID;
    char* pathName = mem_malloc(sizeof(fileName) + sizeof(pageDirectory));
    memcpy(pathName, pageDirectory, sizeof(pageDirectory));
    strcat(pathName, fileName);

    FILE* fp = fopen(pathName, "w");
    if (fp == NULL) exitNonZero("could not open the page directory");

    if (webpage_getURL(page) == NULL) exitNonZero("could get URL from webpage");
    fprintf(fp, "%s\n", webpage_getURL(page));

    if (webpage_getDepth(page) < 0) exitNonZero("webpage depth is invalid");
    fprintf(fp, "%d\n", webpage_getDepth(page));

    if (!webpage_fetch(page)) exitNonZero("could not fetch webpage");
    if (webpage_getHTML(page) == NULL) exitNonZero("could not get HTML from webpage");
    fprintf(fp, "%s\n", webpage_getHTML(page));

    fclose(fp);

}

bool pagedir_validate(char* dir){
    if (dir == NULL) exitNonZero("directory name is invalid");
    char* fileName = "/.crawler";
    char* pathName = mem_malloc(sizeof(fileName) + sizeof(dir));
    memcpy(pathName, dir, sizeof(dir));
    strcat(pathName, fileName);
    printf("%s\n", pathName);
    
    if (access(pathName, F_OK) == 0) {
        free(pathName);
        return true;
    } 
    else {
        free(pathName);
        return false;
    }
}

webpage_t* pagedir_load(char* pageDirectory, int docID){
    webpage_t* page;
    if (pageDirectory == NULL) exitNonZero("directory name is invalid");
    char* fileName = "/.crawler";
    char* pathName = mem_malloc(sizeof(fileName) + sizeof(pageDirectory));
    memcpy(pathName, pageDirectory, sizeof(pageDirectory));
    strcat(pathName, fileName);
    if (access(pathName, F_OK) == 0) {
        char* URL;
        char* depth;
        FILE* fp = fopen(pathName, "w");
        if (fp == NULL) exitNonZero("could not open file in pae directory");
        URL = file_readLine(fp);
        depth = file_readLine(fp);
        page = webpage_new(URL, (int) *depth, NULL);
        free(URL);
        free(depth);
        webpage_fetch(page);
        fclose(fp);
    } 
    else {
        free(pathName);
        return NULL;
    }

    free(pathName);
    return page;
}
