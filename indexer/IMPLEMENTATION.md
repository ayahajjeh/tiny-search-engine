# TSE Indexer Implementation Spec
# Aya Hajjeh, Lab5 
# CS50 Fall22

# CS50 TSE Indexer
# Implementation Specification

In this document we reference the [Requirements Specification](REQUIREMENTS.md) and [Design Specification](DESIGN.md) and focus on the implementation-specific decisions.

We will focus in this documents on the core subset:

-  Data structures
-  Control flow: pseudo code for overall flow, and for each of the functions
-  Detailed function prototypes and their parameters
-  Error handling and recovery
-  Testing plan

## Data structures 

We use two data structures: a customized hashtable that we call an index. Keys for the hashtable are words,
while items are counter sets. Keys for the counter sets are document IDs, while the items for the counters
set is the count of how many times the key word appeared in the document ID.
The index starts empty with a number of time slots of a 1000. The size of the hashtable (slots) is impossible to determine in advance, so we use 1000.  
(NOTE: our hashtable implementation does not grow in size as hashtables did in CS 10 to keep a low load factor, but a starting size of 1000 works well for this assignment)

## Control flow

The indexer is implemented in one directory `indexer.c`, with the help of two other libraries libcs50-given.a
and common.a

### Functions in indexer.c

### 1. main

main basically parses arguments and initializes other modules;

    pseudo code:
    1. parse the command line, validate parameters (pageDirectory and indexFilename)
    2. Initialize data structure index
    3. build the index in memory by processing each file (webpage) in pageDirectory 
    4. save index to file with indexFilename
    5. clean up data structures


### 2. indexPage

indexPage scans a webpage document to add its words to the index.

    pseudo code:
    1. Find words on a single page
    2. create a counter for each word
    3. make entry into index (hashtable)

### 3. indexBuild

indexBuild builds an in-memory index from webpage files it finds in the pageDirectory;

    pseudo code:
    1. Loop over all webpages stored by crawler in pageDirectory,
    2. index those pages by looking for each word on each page,
    3. make entry in index (hashtable) for each word.

Description for Index module
Use index module to implement the TSE indexer
Create index (hashtable) with one entry in table for each word in webpages crawled. 
Each entry is a Set of Counters. Set has a key for each word, and a value of a counter.
The counter has a key of docID and count of how many times the word appeared in the document.

## Other modules and files

### pagedir

We create a re-usable module `pagedir.c` to handle interactions with page directories that the indexer requires.
We chose to write this as a separate module, in `../common`, to encapsulate all the knowledge about how to initialize and validate a pageDirectory, and how to write and read page files, in one place.

`pagedir_validate`:
pagedir_validate verifies whether the passed directory is indeed a Crawler-produced directory

`pagedir_load`:
pagedir_load loads a page from a file into a webpage


### indextest.c
    
indextest.c loads an index output file back to an index. It only has a main function.
    
    pseudo code for main function:
    1. Process and validate command-line parameters
    2. Initialize data structure index
    3. index_load(file1, index)
    4. index_save(file2, index)
    5. clean up data structures

### word.c
word.c implements the normalizeWord function which changes the entered word to all lower-case

### index.c
index.c is an index module.
 * This module is a wrapper around a hashtable that focuses on storing counters as items in the hashtable’s
 * set as describe in IMPLEMENTATION.md. In addition to implementing a hashtable of counters, this index 
 * module provides functions to save an index to disk and to load it into memory from disk. 
    
    pseudo code for index.c:
    1. use typedef to create index_t from hashtable_t which is essentially a wrapper over hashtable_t
    2. create functions index_X that call corresponding hashtable_X functions
    3. write supplemental functions to help print the index counters and keys
    4. create index_save(index_t *index, const char *indexFilename ) to save an index to disk
    5. create index_save and index_load

    pseudo code for index_save:
    1. safety checks on index and indexFilename
    2. open fp on indexFilename for writing (with safety check that it opened)
    3. iterate over hashtable with hashtable_iterate(index, fp, functionToPrintKey)
    4. output should be a file named indexFilename with one line for each word in the hashtable, 
       where the word begins the line and is followed by the docID and count of all documents that 
       contain the word.

    index_load essentially loads an index from disk
    pseudo code for index_load:
    1. safety checks on indexFilename
    2. open fp with indexFilename for reading (with safety check that it opened)
    3. check for empty file (can use file_numlines(fp) to get word count because each word is on one line)
    4. print error message and return NULL if file contains no words
    5. create new index with index_new (how many slots? wordcount/2+1?)
    6. read each word in file with file_readWord(fp)
        7. create new counter for word with counters_new()
        8. expect one or more docID/count pairs on line following word
        9. while line has docID/count pair
            10. increment counters with counters_set(counters, docID, count)
            11. counters now contains one element for each docID
            12. add counters to hashtable with hashtable_insert(index, word, counters)
            13. free word
    14. close fp
    Output should be an index loaded into memory.

### libcs50

We leverage the modules of libcs50, most notably `bag`, `hashtable`, and `webpage`.
See that directory for module interfaces.
The new `webpage` module allows us to represent pages as `webpage_t` objects, to fetch a page from the Internet, and to scan a (fetched) page for URLs; in that regard, it serves as the *pagefetcher* described in the design.
Indeed, `webpage_fetch` enforces the 1-second delay for each fetch.

## Function prototypes

### indexer

```c
int main(int argc, char* argv[]);
index_t* indexBuild(char* pageDirectory);
void indexPage(index_t* index, webpage_t* page, int docID);
```

### pagedir

```c
bool pagedir_init(const char* pageDirectory);
void pagedir_save(webpage_t* page, const char* pageDirectory, const int docID);
void exitNonZero (char* message);
bool pagedir_validate(char* dir);
webpage_t* pagedir_load(char* pageDirectory, int docID);
```

### word

```c
char* normalizeWord (char* word);
```

### index

```c
index_t *index_new(int number_slots);
bool index_insert(index_t* index, const char* key, counters_t* item);
counters_t* index_find(index_t* index, const char* key);
void index_print (index_t* index, FILE* fp, 
                     void (*counterPrint)(FILE* fp, const char* key, counters_t* item));
void index_iterate(index_t* index, void* arg,
                       void (*counterfunc)(void* arg, const char* key, counters_t* item));
void item_delete(counters_t* item);
void index_delete(index_t* index);
void functionToPrintCounter(FILE* arg, const int key, const int count);
void functionToPrintKey(void* arg, const char* key, counters_t* counter);
bool index_save(index_t *index, const char *indexFilename);
index_t* index_load(const char *indexFilename);
```

### indextest

```c
int main(int argc, char* argv[]);
```

## Error handling and recovery

problems result in a message printed to stderr and a non-zero exit status.

Out-of-memory errors are handled by variants of the `mem_assert` functions, which result in a message printed to stderr and a non-zero exit status.

All code uses defensive-programming tactics to catch and exit (using variants of the `mem_assert` functions), e.g., if a function receives bad parameters.

That said, certain errors are caught and handled internally: for example, some functions do not exit non-zero 
when errors happen. Rather, it returns false or NULL and allow the main function to handle the error after
freeing all the necessary memory allocationa.

## Testing plan

We write a script `testing.sh` that invokes the indexer several times, with a variety of command-line arguments.

## Indexer Output
The indexer writes the inverted index to a file, and both the index tester and the querier read the inverted index from a file; the file shall be in the following format:

    one line per word, one word per line
    each line provides the word and one or more (docID, count) pairs, in the format
    word docID count [docID count]...
    where word is a string of lower-case letters,
    where docID is a positive non-zero integer,
    where count is a positive non-zero integer,
    where the word and integers are separated by spaces;
    note the [ ] brackets and elipses ... are not part of the file format – we use them here to indicate the docID count occurs one or more times.

Within the file, the lines may be in any order.
Within a line, the docIDs may be in any order.

