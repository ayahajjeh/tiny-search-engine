/*
 *
 * word.c implements normalizeWord
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
#include "word.h"

char* normalizeWord (char* word){
    if (word == NULL)
        return NULL;
    
    for (int i=0; i<strlen(word); i++)
        word[i] = tolower(word[i]);
    return word;
}