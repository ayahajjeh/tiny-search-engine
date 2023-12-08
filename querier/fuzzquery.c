/*
 * fuzzquery - generate a series of random queries for testing querier
 *
 * usage:
 *   fuzzquery indexFile numQueries randomSeed
 * 
 * compile: mygcc fuzzquery.c libcs50/libcs50.a -o fuzzquery
 *
 * David Kotz - May 2016, 2017, 2019, 2021
 * updated by Xia Zhou, August 2016
 * Tim Pierson, Fall 2022
 */

#include <stdio.h>
#include <stdlib.h> // rand, srand
#include <string.h> // strchr
#include <ctype.h>  // isalpha
#include <stdbool.h>
#include "../libcs50/mem.h"
#include "../libcs50/file.h"

/**************** file-local global variables ****************/
static char* program;
static const char* UnixDictionary = "/usr/share/dict/words";
// If that file is not available, install it with:
//     sudo apt-get install wamerican

/**************** file-local global types ****************/
typedef struct {
  int nWords;                 // number of words in the list
  char** words;               // array of words
} wordlist_t;

/**************** local functions ****************/
static void parseArgs(const int argc, char* argv[],
                      char** indexFilename, int* numQueries, int* randomSeed);
static wordlist_t* wordlist_load(const char* indexFilename);
static void wordlist_delete(wordlist_t* words);
static void generateQuery(const wordlist_t* wordlist,
                          const wordlist_t* dictionary);
static bool onlyLetters(char* word);

/**************** main ****************/
int
main(const int argc, char* argv[])
{
  char* indexFilename;
  int numQueries;
  int randomSeed;

  // parse the arguments
  // function exits if any errors in parsing
  parseArgs(argc, argv, &indexFilename, &numQueries, &randomSeed);

  // seed random number generator
  srand(randomSeed);

  // load an array full of words from the given index file
  wordlist_t* wordlist = wordlist_load(indexFilename);
  if (wordlist == NULL) {
    fprintf(stderr, "%s cannot load words from dictionary '%s'\n",
            program, indexFilename);
    exit(9);
  }

  // load an array full of words from the Unix dictionary
  wordlist_t* dictionary = wordlist_load(UnixDictionary);
  if (dictionary == NULL) {
    fprintf(stderr, "%s cannot load words from dictionary '%s'\n",
            program, UnixDictionary);
    exit(9);
  }

  fprintf(stderr, "%s: generating %d queries from %d words\n",
          program, numQueries, wordlist->nWords);

  // generate random queries, using that wordlist
  for (int q = 0; q < numQueries; q++)
    generateQuery(wordlist, dictionary);

  wordlist_delete(wordlist);
  wordlist_delete(dictionary);

  // mem_report(stderr, "end of main");
}

/**************** parseArgs ****************/
/* Parse the command-line arguments, filling in the parameters;
 * if any error, print to stderr and exit.
 */
static void
parseArgs(const int argc, char* argv[],
          char** indexFilename, int* numQueries, int* randomSeed)
{
  char extra;

  /**** usage ****/
  program = argv[0];
  if (argc != 4) {
    fprintf(stderr, "usage: %s indexFile numQueries randomSeed\n", program);
    exit(1);
  }

  /**** indexFile ****/
  *indexFilename = argv[1];

  /**** numQueries ****/
  if (sscanf(argv[2], "%d%c", numQueries, &extra) != 1 || *numQueries < 0) {
    fprintf(stderr, "usage: %s: invalid numQueries '%s'\n", program, argv[2]);
    exit (2);
  }

  /**** randomSeed ****/
  if (sscanf(argv[3], "%d%c", randomSeed, &extra) != 1 || *randomSeed < 0) {
    fprintf(stderr, "usage: %s: invalid randomSeed '%s'\n", program, argv[3]);
    exit (3);
  }
}

/**************** wordlist_load ****************/
/* load the set of words in the given index file,
 * saving them in a freshly-allocated array;
 * the caller must later call words_delete() on the result.
 */
static wordlist_t*
wordlist_load(const char* indexFilename)
{
  if (indexFilename == NULL) {
    return NULL;
  }

  FILE* fp = fopen(indexFilename, "r");
  if (fp == NULL) {
    fprintf(stderr, "%s: cannot open index file '%s'\n",
            program, indexFilename);
    return NULL;
  }

  // How many words will we have to read?
  int maxWords = file_numLines(fp);

  if (maxWords == 0) {
    fprintf(stderr, "%s: index file '%s' has no words\n",
            program, indexFilename);
    fclose(fp);
    return NULL;
  }

  // Make array to hold the words
  char** words = mem_calloc_assert(maxWords, sizeof(char*), "words[]");

  // wrap the result in a wordlist
  wordlist_t* wordlist = mem_malloc_assert(sizeof(wordlist_t), "wordlist");
  wordlist->words = words;
  wordlist->nWords = 0;

  // read in all the words
  int nWords = 0;
  char* word = NULL;
  while ((word = file_readLine(fp)) != NULL) {
    // truncate the word at the first space
    char* space = strchr(word, ' ');
    if (space != NULL)
      *space = '\0';

    // decide whether to save the word in the word list
    if (onlyLetters(word)) {
      // save it - a simple word of only letters
      words[nWords++] = word;
    } else {
      // discard it - contains non-letters like hyphen or apostrophe
      free(word);
    }
  }
  fclose(fp);

  // record the actual number of words in the list
  wordlist->nWords = nWords;

  return wordlist;
}

/**************** wordlist_delete ****************/
/* free the memory allocated by wordlist_load
 */
static void
wordlist_delete(wordlist_t* wordlist)
{
  mem_assert(wordlist, "wordlist_delete words");

  char** words = wordlist->words;
  int nWords = wordlist->nWords;
  for (int w = 0; w < nWords; w++) {
    free(words[w]);           // was allocated by file_readLine()
  }

  mem_free(wordlist->words);
  mem_free(wordlist);
}

/**************** generateQuery ****************/
/* generate one random query and print to stdout.
 * pull random words from the wordlist and from the dictionary.
 */
static void
generateQuery(const wordlist_t* wordlist, const wordlist_t* dictionary)
{
  // some parameters that affect query generation
  const int maxWords = 6;            // generate 1..maxWords
  const float orProbability = 0.3;   // P(OR between two words)
  const float andProbability = 0.2;  // P(AND between two words)
  const float dictProbability = 0.2; // P(draw from dict instead of wordlist)

  int qwords = rand() % maxWords + 1; // number of words in query
  for (int qw = 0; qw < qwords; qw++) {
    // draw a word either dictionary or wordlist
    if ((rand() % 100) < (dictProbability * 100)) {
      printf("%s ", dictionary->words[rand() % dictionary->nWords]);
    } else {
      printf("%s ", wordlist->words[rand() % wordlist->nWords]);
    }

    // last word?
    if (qw < qwords-1) {
      // which operator to print?
      int op = rand() % 100;
      if (op < (andProbability * 100)) {
        printf("AND ");
      }
      else if (op < (andProbability * 100 + orProbability * 100)) {
        printf("OR ");
      }
    }
  }
  printf("\n");
}

/**************** onlyLetters ****************/
/* return true if the word contains only letters; otherwise false.
 */
static bool
onlyLetters(char* word)
{
  for (char* p = word; *p != '\0'; p++) {
    if (!isalpha(*p)) {
      return false;
    }
  }
  return true;
}