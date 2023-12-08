/*
 *
 * crawler.c crawls a website and retrieves webpages starting with a specified URL. 
 * It parses the initial webpage, extracts any embedded URLs and retrieves those pages, 
 * and crawls the pages found at those URLs, 
 * but limiting itself to some threshold number of hops from the seed URL (the ‘depth’), 
 * and avoiding visiting any given URL more than once. It saves the pages, and the URL and depth for each, 
 * in files. When the crawler process is complete, the indexing of the collected documents can begin.
 *
 * see crawler.h for more information
 *
 * Aya Hajjeh, Oct 12, 2022
 * Lab4
 * CS50, Fall22
 *
 */

#include "../crawler/crawler.h"
#include "../libcs50/mem.h"


static void pageScan(webpage_t* page, bag_t* pagesToCrawl, hashtable_t* pagesSeen){

  int pos = 0;
  char* url;
// while there is another URL in the page
  while ((url = webpage_getNextURL(page, &pos)) != NULL) {

//  if that URL is Internal,
    if(isInternalURL(url)){

//  	insert the webpage into the hashtable
      if(hashtable_insert(pagesSeen, url, "")){

// 	  	if that succeeded,
// 			  create a webpage_t for it
          webpage_t* webpage = webpage_new(url, webpage_getDepth(page) + 1, NULL);

// 		  	insert the webpage into the bag
         bag_insert(pagesToCrawl, webpage);
      }
    }
  }
// 	free the URL
    free(url);
}


static void crawl(char* seedURL, char* pageDirectory, const int maxDepth){
    
  // for the purpose of this assignment, 200 is an convenient number as our hashtable does not dynamically grow 
  const int numSlots = 200;

  // initialize the hashtable and add the seedURL
  hashtable_t* pagesSeen = hashtable_new(numSlots);
  hashtable_insert(pagesSeen, seedURL, "");

  // initialize the bag and add a webpage representing the seedURL at depth 0
  bag_t* pagesToCrawl = bag_new();
  webpage_t* webpage = webpage_new(seedURL, 0, NULL);
  bag_insert(pagesToCrawl, webpage);

  int docID = 0;
  // while bag is not empty
  // 	pull a webpage from the bag
  while ((webpage = bag_extract(pagesToCrawl)) != NULL){

  // 	fetch the HTML for that webpage
  // 	if fetch was successful,
      if(webpage_fetch(webpage)){
  // 		save the webpage to pageDirectory
        pagedir_save(webpage, pageDirectory, docID);
  // 		if the webpage is not at maxDepth,
        if(webpage_getDepth(webpage) < maxDepth)
  // 			pageScan that HTML
          pageScan(webpage, pagesToCrawl, pagesSeen);
  // 	delete that webpage
      webpage_delete(webpage);
  // delete the hashtable
      hashtable_delete(pagesSeen, webpage_delete);
  // delete the bag
      bag_delete(pagesToCrawl, mem_free);
      }

    docID++;
  }
}


static void parseArgs(const int argc, char* argv[],
                      char** seedURL, char** pageDirectory, int* maxDepth){

  if (argc != 4) exitNonZero("invalid numebr of parameters");

  // for seedURL, normalize the URL and validate it is an internal URL using normalizeURL and isInternalURL from webpage.c
  char* normalizedURL = normalizeURL(argv[1]);
  if (isInternalURL(normalizedURL))
    *seedURL = normalizedURL;
  else
    exitNonZero("URL is not internal");


  // for pageDirectory, call pagedir_init(), see pagedir below
  pagedir_init(argv[2]);
  *pageDirectory = argv[2];

  // for maxDepth, ensure it is an integer in specified range [0 … 10]
  if ((int)*argv[3] >= 0 && (int)*argv[3] <= 10)
    maxDepth = (int *) argv[3];

  else
    exitNonZero("invalid maxDepth. Depth out of range [0-10]");

  // if any trouble is found, we print an error to stderr and exit non-zero.
}

/**
 * Crawler driver
 */
int main(int argc, char* argv[]){

  char* seedURL;
  char* pageDirectory;
  int maxDepth;

  //The main function simply calls parseArgs and crawl, then exits zero.
  parseArgs(argc, argv, &seedURL, &pageDirectory, &maxDepth);
  crawl(seedURL, pageDirectory, maxDepth);
  
  return 0;

}