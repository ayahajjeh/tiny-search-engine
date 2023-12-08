# TSE Querier Implementation Spec
## Aya Hajjeh, Lab 6, CS50 Fall 2022, Oct 31
### NOTE: I used the crawler IMPLEMENTATION.md as a template and edited it as appropriate to fit the querier

## Pseudo Code for each of the Objects/Components/Functions
### Querier

```c
/**************** usageSyntax() ****************/
/**
 * @purpose: prints out the usage syntax for the querier
 * @param: void
 * @return: void
 */
void usageSyntax();

/**************** validateParameters() ****************/
/**
 * @purpose: validates the two strings passed to the main function
 * @param pageDirectory: a string that represents a directory
 * @param indexFilename: a string that represents a pathname
 * @return: true if the pageDirectory is a directory that is produced by a crawler and the indexFilename can be opened to be read; else it returns false
 */
bool validateParameters(char* pageDirectory, char* indexFilename);

/**************** prompt() ****************/
/**
 * @purpose: prints out `Query?` to make the user interface more interactive and readbale to the user. It only prints the `Query?` phrase to stdout if the input is coming from stdin and not being piped in through a bash script file
 * @param: void
 * @return: void
 */
static void prompt();

/**************** tokenize() ****************/
/**
 * @purpose: chop a string query line into an array of words
 * @param line: the string query line that the user entered
 * @param wordsCount: a pointer to an integer to keep track of the words count in the query. The pointer is initialized to 0
 * @param words: a string array that stores the words in `line` string
 * @return: false if any entered charachter is not alphabetical or a white space. It returns true if all the entered characters are either white space or alphabetical.
 */
bool tokenize(char* line, int* wordsCount, char* words[]);

/**************** printQuery() ****************/
/**
 * @purpose: prints the query after being normalized aka after lower-casing all the alphabets in the query and trimming the white spaces.
 * @param words: a string array that stores the words in the entered query
 * @param wordsCount: an int pointer that stores the words count the were found the entered query string
 * @return: false if any logical operator is entered first or last. false if any two logical operators are consecuitive. Else, it returns true.
 */
bool printQuery(char* words[], int* wordsCount);

/**************** processQuery() ****************/
/**
 * @purpose: processes the query with taking into account `or` precedence over `and`
 * @param words: a string array the stores the words found in the query string
 * @param wordsCount: an int pointer that stores the words count found in the query string
 * @param index: index data structure that is loaded through indexFilename.
 * @return: a counters set that stores the matching docIDs for the query with the representative scores. The counters set is null if there were no matching documents for the query.
 */
counters_t* processQuery(char* words[], int* wordsCount, index_t* index);

/**************** merge() ****************/
/**
 * @purpose: merge the second counters set into the first counters set
 * @param andSequence: a counters set that stores the matching documents and their representative scores for a sequence of words linked by the operator `and`
 * @param orSequence: a counters set that stores the matching documents and their representative scores for a sequence of andSequences linked by the operator `or`
 * @return: the orSequence after neing merged with the andSequence
 */
counters_t* merge(counters_t* andSequence, counters_t* orSequence);

/**************** unionFunction() ****************/
/**
 * @purpose: unions the second counters set into the first counters set
 * @param orSequence: a counters set that stores the matching documents and their representative scores for a sequence of andSequences linked by the operator `or`
 * @param otherSequence: a counters set
 * @return: void
 */
void unionFunction(counters_t* orSequence, counters_t* otherSequence);

/**************** unionFunctionHelper() ****************/
/**
 * @purpose: a helper function for unionFunction to use it with counters_iterate
 * @param arg: a counters set that we are merging the (key, count) into
 * @param key: a docID of a counters node
 * @param count: a count of a counter node
 * @return: void
 */
void unionFunctionHelper(counters_t* arg, int key, int count);

/**************** intersectFunction() ****************/
/**
 * @purpose: intersects the second counters set with the first counters set and stores the result in the first one
 * @param orSequence: a counters set that stores the matching documents and their representative scores for a sequence of andSequences linked by the operator `or`
 * @param otherSequence: a counters set
 * @return: void
 */
void intersectFunction(counters_t* orSequence, counters_t* otherSequence);

/**************** min() ****************/
/**
 * @purpose: returns the minimum of two integers
 * @param x: an integer
 * @param y: an integer
 * @return: the minimum of x and y
 */
int min(int x, int y);

/**************** intersectFunctionHelper() ****************/
/**
 * @purpose: a helper function for intersectFunction to use it with counters_iterate
 * @param arg: a two counters set
 * @param key: a docID of a counters node
 * @param count: a count of a counter node
 * @return: void
 */
void intersectFunctionHelper(twocounters_t* arg, int key, int count);

/**************** getMaxCounterHelper() ****************/
/**
 * @purpose: a helper function for getMaxCounter to use it with counters_iterate
 * @param maxNode: a node type data structure to store the max node in as we iterate over the nodes of the counters set
 * @param key: a docID of a counters node
 * @param count: a count of a counter node
 * @return:
 */
void getMaxCounterHelper(node_t* maxNode, const int key, const int count);

/**************** getMaxCounter() ****************/
/**
 * @purpose: get the docID of the node that has the max count in a counters set
 * @param counters: a counter set
 * @return: the docID of the node that has the max count in a counters set
 */
int getMaxCounter(counters_t* counters);

/**************** printURL() ****************/
/**
 * @purpose: print the url of a file produced by the crawler
 * @param pageDirectory: a directory produced by a crawler
 * @param docID: the doc ID for the file that we need its url
 * @return: void
 */
void printURL(char* pageDirectory, int docID);

/**************** getMatchesNumberHelper() ****************/
/**
 * @purpose: a helper function for getMatchesNumber to use it with counters_iterate
 * @param matchesCount: an integer pointer to keep track of the number of nodes in a counters set
 * @param key: a docID of a counters node
 * @param count: a count of a counter node
 * @return: void
 */
void getMatchesNumberHelper(int* matchesCount, const int key, const int count);

/**************** getMatchesNumber() ****************/
/**
 * @purpose: get the number of nodes in the queryResult counters set to know how many matches to print
 * @param queryResult: a counters set that we stored the query result in
 * @return: the number of nodes in the queryResult aka the number of matches
 */
int getMatchesNumber(counters_t* queryResult);

/**************** printQueryResult() ****************/
/**
 * @purpose: print the queryResult counters set in decreasing order by score following the syntax:
 *  score [score] docID [docID]: url [url]
 *  it prints out `no documents match` if the queryResult is empty.
 * @param queryResult: a counters set that has the result of the query search
 * @param pageDirectory: a directory that produced by the crawler
 * @return: void
 */
void printQueryResult(counters_t* queryResult, char* pageDirectory);
```

## Data Structures (e.g., struct names and members),
major data structures: index and counters_t data structures. 
Please see `DESIGN.md` for ruther information about major data structures.
I also used local data structures which are:
  1. node_t
    node_t is basically one node of a counters_t data structre where it stores a docID and a count.
    We use the node_t data structure to store the the counters node that has he highest score.
    We need to get the highest score node in a counters set when printing out the query result.
  2. twocounters_t
    twocounters_t is basically two counters_t* wrapped together in one data structure.
    We need twocounters_t in the intersect function when we pass it as an argument to counters_iterate

