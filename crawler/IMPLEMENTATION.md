# CS50 TSE Crawler
## Implementation Specification

In this document we reference the [Requirements Specification](REQUIREMENTS.md) and [Design Specification](DESIGN.md) and focus on the implementation-specific decisions.
Implementation may include many topics; not all are relevant to the TSE or the Crawler.
Here we focus on the core subset:

-  Data structures
-  Control flow: pseudo code for overall flow, and for each of the functions
-  Detailed function prototypes and their parameters
-  Error handling and recovery
-  Testing plan

## Data structures 

We use two data structures: a 'bag' of pages that need to be crawled, and a 'hashtable' of URLs that we have seen during our crawl.
Both start empty.
The size of the hashtable (slots) is impossible to determine in advance, so we use 200.  (NOTE: our hashtable implementation does not grow in size as hashtables did in CS 10 to keep a low load factor, but a starting size of 200 works well for this assignment)

## Control flow

The Crawler is implemented in one file `crawler.c`, with four functions.

### 1. main

The `main` function simply calls `parseArgs` and `crawl`, then exits zero.

### 2. parseArgs

Given arguments from the command line, extract them into the function parameters; return only if successful.

* for `seedURL`, normalize the URL and validate it is an internal URL using `normalizeURL` and `isInternalURL` from `webpage.c`
* for `pageDirectory`, call `pagedir_init()`, see [pagedir below](#pagedir)
* for `maxDepth`, ensure it is an integer in specified range [0 ... 10]
* if any trouble is found, print an error to stderr and exit non-zero.

### 3. crawl

Do the real work of crawling from `seedURL` to `maxDepth` and saving pages in `pageDirectory`.
Pseudocode:

    initialize the hashtable and add the seedURL
    initialize the bag and add a webpage representing the seedURL at depth 0
    while bag is not empty
        pull a webpage from the bag
        fetch the HTML for that webpage
        if fetch was successful,
            save the webpage to pageDirectory
            if the webpage is not at maxDepth,
                pageScan that HTML
        delete that webpage
    delete the hashtable
    delete the bag

### 4. pageScan

This function implements the *pagescanner* mentioned in the design.
Given a `webpage`, scan the given page to extract any links (URLs), ignoring non-internal URLs; for any URL not already seen before (i.e., not in the hashtable), add the URL to both the hashtable `pages_seen` and to the bag `pages_to_crawl`.
Pseudocode:

    while there is another URL in the page
        if that URL is Internal,
            insert the webpage into the hashtable
            if that succeeded,
                create a webpage_t for it
                insert the webpage into the bag
        free the URL

## Other modules

<a id="pagedir"></a>
### pagedir

We create a re-usable module `pagedir.c` to handles the *pagesaver*  mentioned in the design (writing a page to the pageDirectory), and marking it as a Crawler-produced pageDirectory (as required in the spec).
We chose to write this as a separate module, in `../common`, to encapsulate all the knowledge about how to initialize and validate a pageDirectory, and how to write and read page files, in one place... anticipating future use by the Indexer and Querier.

Pseudocode for `pagedir_init`:

    construct the pathname for the .crawler file in that directory
    open the file for writing; on error, return false.
    close the file and return true.


Pseudocode for `pagedir_save`:

    construct the pathname for the page file in pageDirectory
    open that file for writing
    print the URL
    print the depth
    print the contents of the webpage
    close the file

### libcs50

We leverage the modules of libcs50, most notably `bag`, `hashtable`, and `webpage`.
See that directory for module interfaces.
The new `webpage` module allows us to represent pages as `webpage_t` objects, to fetch a page from the Internet, and to scan a (fetched) page for URLs; in that regard, it serves as the *pagefetcher* described in the design.
Indeed, `webpage_fetch` enforces the 1-second delay for each fetch, so our crawler need not implement that part of the spec.

## Function prototypes

### crawler

Detailed descriptions of each function's interface is provided as a paragraph comment prior to each function's implementation in `crawler.c` and is not repeated here.

```c
int main(const int argc, char* argv[]);
static void parseArgs(const int argc, char* argv[],
                      char** seedURL, char** pageDirectory, int* maxDepth);
static void crawl(char* seedURL, char* pageDirectory, const int maxDepth);
static void pageScan(webpage_t* page, bag_t* pagesToCrawl, hashtable_t* pagesSeen);
```

### pagedir

Detailed descriptions of each function's interface is provided as a paragraph comment prior to each function's declaration in `pagedir.h` and is not repeated here.

```c
bool pagedir_init(const char* pageDirectory);
void pagedir_save(const webpage_t* page, const char* pageDirectory, const int docID);
```

## Error handling and recovery

All the command-line parameters are rigorously checked before any data structures are allocated or work begins; problems result in a message printed to stderr and a non-zero exit status.

Out-of-memory errors are handled by variants of the `mem_assert` functions, which result in a message printed to stderr and a non-zero exit status.
We anticipate out-of-memory errors to be rare and thus allow the program to crash (cleanly) in this way.

All code uses defensive-programming tactics to catch and exit (using variants of the `mem_assert` functions), e.g., if a function receives bad parameters.

That said, certain errors are caught and handled internally: for example, `pagedir_init` returns false if there is any trouble creating the `.crawler` file, allowing the Crawler to decide what to do; the `webpage` module returns false when URLs are not retrievable, and the Crawler does not treat that as a fatal error.

## Testing plan

Here is an implementation-specific testing plan.

### Unit testing

There are only two units (crawler and pagedir).
The crawler represents the whole system and is covered below.
The pagedir unit is tiny; it could be tested using a small C 'driver' to invoke its functions with various arguments, but it is likely sufficient to observe its behavior during the system test.

### Regression testing

The crawler can take a long time to run on some sites when `maxDepth` is more than 2.
For routine regression tests, we crawl the `letters` site at moderate depths; save the pageDirectory from one working run to compare (with `diff -r`) against future runs.

> For Lab 4, you are not required to script regression tests, though you may find the technique useful for your own testing/debugging process.

### Integration/system testing

We write a script `testing.sh` that invokes the crawler several times, with a variety of command-line arguments.
First, a sequence of invocations with erroneous arguments, testing each of the possible mistakes that can be made.
Second, a run with valgrind over a moderate-sized test case (such as `toscrape` at depth 1).
Third, runs over all three CS50 websites (`letters` at depths 0,1,2,10, `toscrape` at depths 0,1,2,3, `wikipedia` at depths 0,1,2).
Run that script with `bash -v testing.sh` so the output of crawler is intermixed with the commands used to invoke the crawler.
Verify correct behavior by studying the output, and by sampling the files created in the respective pageDirectories.

> For Lab 4, as noted in the assignment, you may submit a smaller test run.
> Furthermore, we recommend turning off detailed logging output for these tests, as they make `testing.out` rather large!


<a id="crawler-output"></a>
## Crawler Output


Below is the output of our crawler when the program crawls one of our simple test websites to a maximum depth of 10 (though it reaches all pages within this website in only five hops from the seed).
The crawler prints status information as it goes along.


```
$ crawler/crawler http://cs50tse.cs.dartmouth.edu/tse/letters/index.html data/letters-3 10
 0   Fetched: http://cs50tse.cs.dartmouth.edu/tse/letters/index.html
 0  Scanning: http://cs50tse.cs.dartmouth.edu/tse/letters/index.html
 0     Found: http://cs50tse.cs.dartmouth.edu/tse/letters/A.html
 0     Added: http://cs50tse.cs.dartmouth.edu/tse/letters/A.html
 1    Fetched: http://cs50tse.cs.dartmouth.edu/tse/letters/A.html
 1   Scanning: http://cs50tse.cs.dartmouth.edu/tse/letters/A.html
 1      Found: https://en.wikipedia.org/wiki/Algorithm
 1   IgnExtrn: https://en.wikipedia.org/wiki/Algorithm
 1      Found: http://cs50tse.cs.dartmouth.edu/tse/letters/B.html
 1      Added: http://cs50tse.cs.dartmouth.edu/tse/letters/B.html
 1      Found: http://cs50tse.cs.dartmouth.edu/tse/letters/index.html
 1    IgnDupl: http://cs50tse.cs.dartmouth.edu/tse/letters/index.html
 2     Fetched: http://cs50tse.cs.dartmouth.edu/tse/letters/B.html
 2    Scanning: http://cs50tse.cs.dartmouth.edu/tse/letters/B.html
 2       Found: https://en.wikipedia.org/wiki/Breadth-first_search
 2    IgnExtrn: https://en.wikipedia.org/wiki/Breadth-first_search
 2       Found: http://cs50tse.cs.dartmouth.edu/tse/letters/C.html
 2       Added: http://cs50tse.cs.dartmouth.edu/tse/letters/C.html
 2       Found: http://cs50tse.cs.dartmouth.edu/tse/letters/D.html
 2       Added: http://cs50tse.cs.dartmouth.edu/tse/letters/D.html
 2       Found: http://cs50tse.cs.dartmouth.edu/tse/letters/E.html
 2       Added: http://cs50tse.cs.dartmouth.edu/tse/letters/E.html
 2       Found: http://cs50tse.cs.dartmouth.edu/tse/letters/index.html
 2     IgnDupl: http://cs50tse.cs.dartmouth.edu/tse/letters/index.html
 3      Fetched: http://cs50tse.cs.dartmouth.edu/tse/letters/E.html
 3     Scanning: http://cs50tse.cs.dartmouth.edu/tse/letters/E.html
 3        Found: https://en.wikipedia.org/wiki/ENIAC
 3     IgnExtrn: https://en.wikipedia.org/wiki/ENIAC
 3        Found: http://cs50tse.cs.dartmouth.edu/tse/letters/F.html
 3        Added: http://cs50tse.cs.dartmouth.edu/tse/letters/F.html
 3        Found: http://cs50tse.cs.dartmouth.edu/tse/letters/G.html
 3        Added: http://cs50tse.cs.dartmouth.edu/tse/letters/G.html
 3        Found: http://cs50tse.cs.dartmouth.edu/tse/letters/A.html
 3      IgnDupl: http://cs50tse.cs.dartmouth.edu/tse/letters/A.html
 3        Found: http://cs50tse.cs.dartmouth.edu/tse/letters/index.html
 3      IgnDupl: http://cs50tse.cs.dartmouth.edu/tse/letters/index.html
 4       Fetched: http://cs50tse.cs.dartmouth.edu/tse/letters/G.html
 4      Scanning: http://cs50tse.cs.dartmouth.edu/tse/letters/G.html
 4         Found: https://en.wikipedia.org/wiki/Graph_traversal
 4      IgnExtrn: https://en.wikipedia.org/wiki/Graph_traversal
 4         Found: http://cs50tse.cs.dartmouth.edu/tse/letters/H.html
 4         Added: http://cs50tse.cs.dartmouth.edu/tse/letters/H.html
 4         Found: http://cs50tse.cs.dartmouth.edu/tse/letters/index.html
 4       IgnDupl: http://cs50tse.cs.dartmouth.edu/tse/letters/index.html
 5        Fetched: http://cs50tse.cs.dartmouth.edu/tse/letters/H.html
 5       Scanning: http://cs50tse.cs.dartmouth.edu/tse/letters/H.html
 5          Found: https://en.wikipedia.org/wiki/Huffman_coding
 5       IgnExtrn: https://en.wikipedia.org/wiki/Huffman_coding
 5          Found: http://cs50tse.cs.dartmouth.edu/tse/letters/B.html
 5        IgnDupl: http://cs50tse.cs.dartmouth.edu/tse/letters/B.html
 5          Found: http://cs50tse.cs.dartmouth.edu/tse/letters/index.html
 5        IgnDupl: http://cs50tse.cs.dartmouth.edu/tse/letters/index.html
 4       Fetched: http://cs50tse.cs.dartmouth.edu/tse/letters/F.html
 4      Scanning: http://cs50tse.cs.dartmouth.edu/tse/letters/F.html
 4         Found: https://en.wikipedia.org/wiki/Fast_Fourier_transform
 4      IgnExtrn: https://en.wikipedia.org/wiki/Fast_Fourier_transform
 4         Found: http://cs50tse.cs.dartmouth.edu/tse/letters/H.html
 4       IgnDupl: http://cs50tse.cs.dartmouth.edu/tse/letters/H.html
 4         Found: http://cs50tse.cs.dartmouth.edu/tse/letters/index.html
 4       IgnDupl: http://cs50tse.cs.dartmouth.edu/tse/letters/index.html
 3      Fetched: http://cs50tse.cs.dartmouth.edu/tse/letters/D.html
 3     Scanning: http://cs50tse.cs.dartmouth.edu/tse/letters/D.html
 3        Found: https://en.wikipedia.org/wiki/Depth-first_search
 3     IgnExtrn: https://en.wikipedia.org/wiki/Depth-first_search
 3        Found: http://cs50tse.cs.dartmouth.edu/tse/letters/index.html
 3      IgnDupl: http://cs50tse.cs.dartmouth.edu/tse/letters/index.html
 3      Fetched: http://cs50tse.cs.dartmouth.edu/tse/letters/C.html
 3     Scanning: http://cs50tse.cs.dartmouth.edu/tse/letters/C.html
 3        Found: https://en.wikipedia.org/wiki/Computational_biology
 3     IgnExtrn: https://en.wikipedia.org/wiki/Computational_biology
 3        Found: http://cs50tse.cs.dartmouth.edu/tse/letters/D.html
 3      IgnDupl: http://cs50tse.cs.dartmouth.edu/tse/letters/D.html
 3        Found: http://cs50tse.cs.dartmouth.edu/tse/letters/index.html
 3      IgnDupl: http://cs50tse.cs.dartmouth.edu/tse/letters/index.html
$ 
```

For each URL crawled the program creates a file and places in the file the URL and filename followed by all the contents of the webpage.
Below is a peek at the files created during the above crawl.
Notice how each page starts with the URL, then a number (the depth of that page during the crawl), then the contents of the page (here I printed only the first line of the content).

```
$ cd data/letters-3/
$ ls
1  10  2  3  4  5  6  7  8  9
$ head -3 1
http://cs50tse.cs.dartmouth.edu/tse/letters/index.html
0
<html>
$ head -3 2
http://cs50tse.cs.dartmouth.edu/tse/letters/A.html
1
<html>
$ head -3 10
http://cs50tse.cs.dartmouth.edu/tse/letters/C.html
3
<html>
$ 
```

## Logging
In the [demo output of our crawler](#crawler-output), notice how it printed the depth of the current crawl at left, then indented slightly based on the current depth, then printed a single word meant to indicate what is being done, then printed the URL.
By ensuring a consistent format, and choosing a simple/unique word for each type of line, we can post-process the output with `grep`, `awk`, and so forth, enabling us to run various checks on the output of the crawler.
(Much better than a mish-mash of arbitrary output formats!)
The code `IgnDupl` meant the crawler was ignoring a duplicate URL (one it had seen before), and `IgnExtrn` meant the crawler was ignoring an 'external' URL (because we limit CS50 crawlers to our webserver to avoid causing trouble on any real webservers).

All of this 'logging' output was produced by a simple function:

```c
// log one word (1-9 chars) about a given url                                   
static void logr(const char *word, const int depth, const char *url)
{
  printf("%2d %*s%9s: %s\n", depth, depth, "", word, url);
}
```

The program thus has just one `printf` call; if we want to tweak the format, we just need to edit one line and not every log-type `printf` in the code.

Actually, the code is like this:

```c
// log one word (1-9 chars) about a given url                                   
static void logr(const char *word, const int depth, const char *url)
{
#ifdef APPTEST
  printf("%2d %*s%9s: %s\n", depth, depth, "", word, url);
#else
  ;
#endif
}
```

Notice the `#ifdef` block that can be triggered by a compile-time switch.

Anyway, at strategic points in the code, there is a call like this one:

```c
logr("Fetched", page->depth, page->url);
```

Such code is compact, readable, maintainable, and can be entirely flipped on and off with one `#ifdef`.

