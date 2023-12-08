# TSE Querier Design Spec
## Aya Hajjeh, Lab 6, CS50 Fall 2022, Oct 31
### NOTE: I used the crawler DESIGN.md as a template and edited it as appropriate to fit the querier

### General Description: 
The TSE Querier is a standalone program that reads the index file produced by the TSE Indexer, and page files produced by the TSE Querier, and answers search queries submitted via stdin.

### User interface

The querier's only interface with the user is on the command-line; it must always have three arguments where we provide pageDirectory and indexFilename.

syntax:  ./querier pageDirectory indexFilename
    * where pageDirectory is the pathname of a directory produced by the Crawler, and
    * where indexFilename is the pathname of a file produced by the Indexer.

### Inputs and outputs

Input: command-line parameters where we provide page f=directory and index filename (see above)

Output: the putput follows the following syntax
Score [number] docID [number]: URL [url]
  * the documents are sorted in decreasing order by the score number
  * the url is the url link found in the docID

Below is an example of running the querier on the toscrape in depth 1 and index 1:

$ ./querier /thayerfs/courses/22fall/cosc050/cs50tse/tse-output/toscrape-depth-1/ /thayerfs/courses/22fall/cosc050/cs50tse/tse-output/toscrape-index-1 
Query? Europe and Travel
Query: europe and travel 
Matches 1 documents (ranked):
score   1 doc  72: http://cs50tse.cs.dartmouth.edu/tse/toscrape/catalogue/category/books/travel_2/index.html
----------------------------------------------

### Functional decomposition into modules

We anticipate the following modules or functions:

querier.c

1. main
2. void usageSyntax();
3. bool validateParameters(char* pageDirectory, char* indexFilename);
4. static void prompt();
5. bool tokenize(char* line, int* wordsCount, char* words[]);
6. bool printQuery(char* words[], int* wordsCount);
7. counters_t* processQuery(char* words[], int* wordsCount, index_t* index);
8. counters_t* merge(counters_t* andSequence, counters_t* orSequence);
9. void unionFunction(counters_t* orSequence, counters_t* otherSequence);
10. void unionFunctionHelper(counters_t* arg, int key, int count);
11. void intersectFunction(counters_t* orSequence, counters_t* otherSequence);
12. int min (int x, int y);
13. void intersectFunctionHelper(twocounters_t* arg, int key, int count);
14. void getMaxCounterHelper(node_t* maxNode, const int key, const int count);
15. int getMaxCounter (counters_t* counters);
16. void printURL (char* pageDirectory, int docID);
17. void getMatchesNumberHelper(int* matchesCount, const int key, const int count);
18. int getMatchesNumber(counters_t* queryResult);
19. void printQueryResult(counters_t* queryResult, char* pageDirectory);



### Pseudo code for the general logic/algorithmic flow

1. main function checks the right number of parameters
2. validateParameters() checks that pageDirectory is a directory produced by crawler and indexFilename can be opened to read
3. the program waits for the user to enter a search query
4. tokenize() chops the query into words after it trims white space and checks that no bad characters are entered.
*bad characters* are any characters that are neither alphabetical nor white space
5. printQuery() prints the search query after normalizing the words and trimming white space
*normalizing* means changing all alphabets in the query to lower case
6. processQuery() processes the query word by word with taking into account logical operators `and` and `or` and the precedence of `or` over `and`
7. processQuery() uses merge(), intersectFunction(), and unionFunction() as helper functions to process logical operators
8. printQueryResult() prints the resulting matches of the processed query. It uses getMatchesNumber() t figure out the number of the documents matched. It also uses getMaxCounter() to sort the documents by decreasing order of their scores. It also uses printURL() to get the URL for each document.

### Major data structures

I used the index data structure to load indexFilename into it. Indes data structure was implemented as part of the previous lab as a hashtable and counters set. We use the index data structure to find the matches of each word in each document.
I also used the counters_t data structure a lot througout this lab. We use it to represent andSequences, orSequences, matches for each word, and the final queryResult.
`andSequences` are words that are linked by the `and` operator.
`orSequences` are `andSequences` that are linked by the `or` operator.
I also used two other local data structures `node_t` and `twocounters_t`. Please see `IMPLEMENTATION.md` for further information about local data structures.


### Testing plan

1. Integration testing:

    1. Test the program with various forms of incorrect command-line arguments to ensure that its command-line parsing, and validation of those parameters, works correctly.

    2. Test the querier with a nonexistant pageDirectory.

    3. Test the querier with a non-crawler generated pageDirectory.

    4. Test the querier with a non-readable indexFilename.

    5. Test the querier on an empty query.

    6. Test the querier on a query with only whitespaces.

    7. Test the querier with non-letter characters.

    8. Test the querier with a starting operator.
    
    9. Test the querier with an ending operator.
    
    10. Test the querier with two consecutive operators.

    11. Test the querier with words that have no matches in the index file.

    12. Pass to the querier files in different directories: (letters, toscrape, and wikipedia) at different depths and different search queries.

    13. Fuzz testing.

2. Fuzzy testing: fuzz testing is a form of black-box testing in which you fire thousands of random inputs at the program to see how it reacts. The chances of triggering an unconsidered test case is far greater if you try a lot of cases!

We use fuzzquery.c to generate a series of random queries on stdout, which it then gets piped into the querier through stdin.

3. Memory testing: we use valgrind to check memory leaks