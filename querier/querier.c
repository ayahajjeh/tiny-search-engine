/*
 *
 * querier.c reads the index file produced by the TSE Indexer, and page files produced by the TSE Crawler,
 *  and answers search queries submitted via stdin
 *
 *
 * Aya Hajjeh, Oct 23, 2022
 * Lab6
 * CS50, Fall22
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>

#include "../libcs50/set.h"
#include "../libcs50/counters.h"
#include "../common/index.h"
#include "freadline.h"

/**************** file-local global variables ****************/
char* loc = "/thayerfs/courses/22fall/cosc050/workspace/tse/tse-output";

/**************** local types ****************/
typedef struct twocounters {
	counters_t* orSequence;
	counters_t* otherSequence;
} twocounters_t;

typedef struct node {
	int docID;
	int count;
} node_t;

/**************** local functions prototypes ****************/
void usageSyntax();
bool validateParameters(char* pageDirectory, char* indexFilename);
static void prompt();
bool tokenize(char* line, int* wordsCount, char* words[]);
bool printQuery(char* words[], int* wordsCount);
counters_t* processQuery(char* words[], int* wordsCount, index_t* index);
counters_t* merge(counters_t* andSequence, counters_t* orSequence);
void unionFunction(counters_t* orSequence, counters_t* otherSequence);
void unionFunctionHelper(counters_t* arg, int key, int count);
void intersectFunction(counters_t* orSequence, counters_t* otherSequence);
int min(int x, int y);
void intersectFunctionHelper(twocounters_t* arg, int key, int count);
void getMaxCounterHelper(node_t* maxNode, const int key, const int count);
int getMaxCounter(counters_t* counters);
void printURL(char* pageDirectory, int docID);
void getMatchesNumberHelper(int* matchesCount, const int key, const int count);
int getMatchesNumber(counters_t* queryResult);
void printQueryResult(counters_t* queryResult, char* pageDirectory);

/**************** main() ****************/
int main(int argc, char* argv[]){
   if (argc != 3) exitNonZero("invalid number of parameters");
   char* pageDirectory = argv[1];
   char* indexFilename = argv[2];
   int queryMaxLength = 1000; // we assume a maximum length of 1000 for the query string
   char* line;
   int* wordsCount = malloc(sizeof(int));
   counters_t* queryResult = counters_new();

   if (!validateParameters(pageDirectory, indexFilename)){
      printf("invalid parameters. Please see the correct usage below:\n");
      usageSyntax();
      free(wordsCount);
      counters_delete(queryResult);
      exit(1);
   }
   
   FILE* fp = fopen(indexFilename, "r");
   if (fp == NULL) {
      printf("could not open %s to read\n", indexFilename);
      free(wordsCount);
      counters_delete(queryResult);
      exit(1);
   }
   index_t* index = index_new(file_numLines(fp));
   fclose(fp);
   index = index_load(indexFilename);
   
   //read query, tokenize query, and then process it
   while (true) {
      line = malloc(queryMaxLength);
      *wordsCount = 0;
      prompt();
      if (freadline(stdin, line, queryMaxLength)) {
         char* words[strlen(line)/2 + 1];
         if (!tokenize(line, wordsCount, words)){
            counters_delete(queryResult);
            free(line);
            continue;
         }
         printf("Query: ");
         if (printQuery(words, wordsCount)){
            queryResult = processQuery(words, wordsCount, index);
            printQueryResult(queryResult, pageDirectory);
         }
         counters_delete(queryResult);
         free(line);
      }
      else
         break;
   }
   index_delete(index);
   free(wordsCount);
   return 0;
}

/**************** usageSyntax() ****************/
void usageSyntax(){
   printf("./querier pageDirectory indexFilename\n");
   printf("    where pageDirectory is the pathname of a directory produced by the Crawler, and\n");
   printf("    where indexFilename is the pathname of a file produced by the Indexer.\n");   
}

/**************** validateParameters() ****************/
bool validateParameters(char* pageDirectory, char* indexFilename){
   if (pageDirectory == NULL || indexFilename == NULL) return false;
   char* pathName = malloc(strlen("/.crawler") + strlen(pageDirectory) + 1);
   sprintf(pathName, "%s%s", pageDirectory, "/.crawler");
   FILE* fp = fopen(pathName, "r");
   if (fp == NULL) {
      free(pathName);
      return false;
   }
   free(pathName);
   fclose(fp);
   fp = fopen(indexFilename, "r");
   if (fp == NULL) return false;
   fclose(fp);
   return true;
}

/**************** prompt() ****************/
/* 
 * The fileno() function is provided by stdio,
 * but for some reason not declared by stdio.h, so declare here.
 */
int fileno(FILE *stream);

static void prompt(void){
  // print a prompt iff stdin is a tty (terminal)
  if (isatty(fileno(stdin))) {
    printf("Query? ");
  }
}

/**************** tokenize() ****************/
bool tokenize(char* line, int* wordsCount, char* words[]){
   int length = strlen(line);
   for (int i=0; i<length; i++){
      if (isalpha(line[i])){
         line[i] = tolower(line[i]);
         if (i > 0){
            if (isspace(line[i-1]) || line[i-1] == '\0'){
               words[(*wordsCount)] = &(line[i]);
               (*wordsCount)++;
            }
         }
         else if (i == 0){
            words[(*wordsCount)] = &(line[i]);
            (*wordsCount)++;
         }
         if (i+1 < length){
            if (isspace(line[i+1])){
               line[i+1] = '\0';
               i++;
            }
         }
      }
      else if (isspace(line[i]))
         continue;
      else if ((!isalpha(line[i])) && (!isspace(line[i]))) {
         printf("Error: bad character '%c' in query.\n", line[i]);
         return false;
      }
   }
   return true;
}

/**************** printQuery() ****************/
bool printQuery(char* words[], int* wordsCount){
   if (words == NULL){
      printf("invalid query. Please re-enter a valid query to perform.\n");
      return false;
   }
   for (int i=0; i<(*wordsCount); i++){
      printf("%s ", words[i]);
   }
   printf("\n");
      if (!(strcmp(words[0], "and"))){
         printf("Error: 'and' cannot be first\n");
         return false;
      }
   if (!(strcmp(words[0], "or"))){
      printf("Error: 'or' cannot be first\n");
      return false;
   }
   if (!(strcmp(words[(*wordsCount)-1], "and"))){
         printf("Error: 'and' cannot be last\n");
         return false;
      }
   if (!(strcmp(words[(*wordsCount)-1], "or"))){
      printf("Error: 'or' cannot be last\n");
      return false;
   }
   for (int i=1; i<(*wordsCount); i++){
      if (!(strcmp(words[i], "and"))){
         if (!(strcmp(words[i-1], "and"))){
            printf("Error: two 'and' operators cannot be adjacent\n");
            return false;
         }
         if (!(strcmp(words[i-1], "or"))){
            printf("Error: 'or' and 'and' operators cannot be adjacent\n");
            return false;
         }
      }
      if (!(strcmp(words[i], "or"))){
         if (!(strcmp(words[i-1], "or"))){
            printf("Error: two 'or' operators cannot be adjacent\n");
            return false;
         }
         if (!(strcmp(words[i-1], "and"))){
            printf("Error: 'and' and 'or' operators cannot be adjacent\n");
            return false;
         }
      }
   }
   return true;
}

/**************** processQuery() ****************/
// Query expressions solution
counters_t* processQuery(char* words[], int* wordsCount, index_t* index){
   // Create two counters initially set to NULL:
   counters_t* andSequence = NULL;  // holds result of current AND operations
   counters_t* orSequence = NULL;  // accumulates the final result combining OR sequences
   counters_t* match = NULL;
   bool shortCircuit = false;  // stop early if AND sequence will fail
   char* word;
   //Loop over all words in query
   for (int i=0; i<(*wordsCount); i++){
      word = words[i];
      if (!strcmp(word, "or")){
         // orSequence holds final result, merge in andSequence seen so far
         // think of stepping back after previously stepping away to calculate AND
         orSequence = merge(andSequence, orSequence);
         shortCircuit = false;  //reset shortCircuit
         if (andSequence != NULL)
            andSequence = NULL;
         continue;
      }
      else if (shortCircuit)
         // at least one word returned no results, so AND sequence will fail
         // skip the words until see OR and shortCircuit is reset
         continue;
      else if (!strcmp(word, "and")){
         continue; //to next word // same as if AND wasn't there
      }
      else {
         //found non operator word
         //think of stepping away to calculate AND sequence
         //get pages for this word with match = index_find(word)
         match = index_find(index, word);
         if (match == NULL){  // no pages contain this query word
            shortCircuit = true; // AND will fail for next word, so skip it and others until see OR
            if (andSequence != NULL){
               // because this word returned no pages, drop everything AND'ed so far
               counters_delete(andSequence);
               andSequence = NULL;
            }
         }
         else {  // found counters for this word in index
            if (andSequence == NULL){ // this is the first word of AND sequence
               andSequence = counters_new();
               unionFunction(andSequence, match);  //essentially copies match to andSequence
            }
            else { // not first word in AND sequence
               intersectFunction(andSequence, match); //intersect this word's pages
            }
         }
      }
   }
   orSequence = merge(andSequence, orSequence);
   return orSequence;
}

/**************** merge() ****************/
counters_t* merge(counters_t* andSequence, counters_t* orSequence){
   // push temporary result andSequence to final result orSequence
   // also reset andSequence
   if (andSequence != NULL){
      if (orSequence == NULL)
         orSequence = counters_new();
      unionFunction(orSequence, andSequence); //union andSequence into orSequence
      counters_delete(andSequence);
      andSequence = NULL;
   }
   return orSequence;
}

/**************** unionFunction() ****************/
void unionFunction(counters_t* orSequence, counters_t* otherSequence){
    // union orSequence and otherSequence by adding counts for matching keys
    // store results in orSequence
    counters_iterate(otherSequence, orSequence, unionFunctionHelper);
}

/**************** unionfunctionHelper() ****************/
void unionFunctionHelper(counters_t* orSequence, int key, int count){
   // update orSequence counters 
   int wordsCount = counters_get(orSequence, key); // returns 0 if key not found
   counters_set(orSequence, key, count + wordsCount);
}

/**************** intersectFunction() ****************/
void intersectFunction(counters_t* orSequence, counters_t* otherSequence){
   // intersect orSequence and otherSequence by taking of counts for matching keys
   // store results in orSequence
   twocounters_t args = {orSequence, otherSequence};
   counters_iterate(orSequence, &args, intersectFunctionHelper);
}

/**************** min() ****************/
int min (int x, int y){
   if (x < y)
      return x;
   return y;
}

/**************** intersectFunctionHelper() ****************/
void intersectFunctionHelper(twocounters_t* args, int key, int count){
    counters_set((*args).orSequence, key, min(count, counters_get((*args).otherSequence, key)));
}

/**************** getMaxCounterHelper() ****************/
void getMaxCounterHelper(node_t* maxNode, const int key, const int count){
   if (count > (maxNode->count)){
      maxNode->count = count;
      maxNode->docID = key;
   }
}

/**************** getMaxCounter() ****************/
int getMaxCounter(counters_t* counters){
   node_t* maxNode = malloc(sizeof(node_t));
   maxNode->docID = -1;
   maxNode->count = 0;
   counters_iterate(counters, maxNode, getMaxCounterHelper);
   int docID = maxNode->docID;
   free(maxNode);
   return docID;
}

/**************** printURL() ****************/
void printURL(char* pageDirectory, int docID){
   if (pageDirectory == NULL) {
      printf("(null)\n");
      return;
   }
   char* pathName = malloc(sizeof(int) + strlen(pageDirectory) + strlen("/") + 1);
   sprintf(pathName, "%s/%d", pageDirectory, docID);
   FILE* fp = fopen(pathName, "r");
   if (fp == NULL) {
      free(pathName);
      printf("(null)\n");
   }
   int maxLength = 1000;
   char* URL = malloc(sizeof(char) * maxLength);
   fscanf(fp, "%s\n", URL);
   printf("%s\n", URL);
   free(pathName);
   free(URL);
   fclose(fp);
}

/**************** getMatchesNumberHelper() ****************/
void getMatchesNumberHelper(int* matchesCount, const int key, const int count){
   (*matchesCount)++;
}

/**************** getMatchesNumber() ****************/
int getMatchesNumber(counters_t* queryResult){
   int* matchesCount;
   *matchesCount = 0;
   counters_iterate(queryResult, matchesCount, getMatchesNumberHelper);
   return *matchesCount;
}

/**************** printQueryResult() ****************/
void printQueryResult(counters_t* queryResult, char* pageDirectory){
   if (queryResult == NULL){ //if a counter is empty it means it is null?*************
      printf("No documents matched!\n");
      printf("-----------------------------------------------\n");
      return;
   }
   //if result is not empty
   int docID;
   printf("Matches %d documents (ranked):\n", getMatchesNumber(queryResult));
   while ((docID = getMaxCounter(queryResult)) > 0){
      printf("score %d doc %d: ", counters_get(queryResult, docID), docID);
      printURL(pageDirectory, docID);
      counters_set(queryResult, docID, 0);
   }
   printf("-----------------------------------------------\n");
}
