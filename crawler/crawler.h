/*
*
* crawler.h is the header file for crawler.c
*
* Aya Hajjeh, Oct 12, 2022
* CS50, Fall22
* Lab4
*
 */

#include "../libcs50/webpage.h"
#include "../libcs50/bag.h"
#include "../libcs50/hashtable.h"
#include "../common/pagedir.h"


//purpose: Given arguments from the command line, extract them into the function parameters
//parameters: argument count from command line, argument values from command line,
//            pointer to the seedURL string,
//            pointer to the page directory string,
//            and a pointer to a max depth integer
//return: return only if successful, otherwise it exits non-zero
static void parseArgs(const int argc, char* argv[],
                      char** seedURL, char** pageDirectory, int* maxDepth);

//purpose: Do the real work of crawling from seedURL to maxDepth and saving pages in pageDirectory. 
//parameters: string that is the seed URL, 
//            string that is the page directory where to save the crawled webpages, 
//            and an int of the max depth to which the function crawls
//return: return only if successful, otherwise it exits non-zero
static void crawl(char* seedURL, char* pageDirectory, const int maxDepth);

//purpose: This function implements the pagescanner mentioned in the design. Given a webpage, 
//         scan the given page to extract any links (URLs), 
//         ignoring non-internal URLs; 
//         for any URL not already seen before (i.e., not in the hashtable), 
//         add the URL to both the hashtable pages_seen and to the bag pages_to_crawl.
//parameters: pointer to a webpage, pointer to a bag of pages to crawl, pointer to a hashtable of seen pages
//return: return only if successful, otherwise it exits non-zero
static void pageScan(webpage_t* page, bag_t* pagesToCrawl, hashtable_t* pagesSeen);