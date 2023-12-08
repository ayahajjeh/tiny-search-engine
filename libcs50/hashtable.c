/*
 * hashtable.c - A hashtable is a set of (key,item) pairs. 
 * It acts just like a set, but is far more efficient for large collections.
 *
 * Aya Hajjeh, Lab 3, CS50, Fall 2022
 */


#include "hashtable.h"
#include "hash.h"
#include "memory.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**************** file-local global variables ****************/
/* none */

/**************** local types ****************/
typedef struct hashnode {
  char* key;
  void* item;
} hashnode_t;

/**************** global types ****************/
typedef struct hashtable {
  int num;           // number of hash value
  hashnode_t* table; // pointer to hash array
} hashtable_t;

/**************** global functions ****************/
/* that is, visible outside this file */
/* see bag.h for comments about exported functions */

/**************** local functions ****************/
/* not visible outside this file */

/**************** hashtable_new() ****************/
hashtable_t* hashtable_new(const int num_slots)
{
  if (num_slots > 0) {
    hashtable_t* new = count_malloc(sizeof(hashtable_t));
    if (new == NULL)
      return NULL;

    new->num = num_slots;
    new->table = count_calloc(num_slots, sizeof(hashnode_t));
    if (new->table == NULL) {
      count_free(new);
      return NULL;
    }

    for (int i = 0; i < num_slots; i++) { // initialize data with full null
      new->table[i].key = NULL;
      new->table[i].item = NULL;
    }

    return new;
  } else {
    return NULL;
  }
}

/**************** hashtable_insert() ****************/
bool hashtable_insert(hashtable_t* ht, const char* key, void* item)
{
  if (ht != NULL && key != NULL && item != NULL) {
    // copy of key
    char* new_key = count_malloc(strlen(key) + 1);
    if (new_key == NULL)
      return false;
    memcpy(new_key, key, strlen(key) + 1);

    // new pair
    int idx = JenkinsHash(key, ht->num);
    if (ht->table[idx].key == NULL) {
      ht->table[idx].key = new_key;
      ht->table[idx].item = item;
      return true;
    } else {
      count_free(new_key);
      return false;
    }
  } else {
    return false;
  }
}

/**************** hashtable_find() ****************/
void* hashtable_find(hashtable_t* ht, const char* key)
{
  if (ht != NULL && key != NULL) {
    int idx = JenkinsHash(key, ht->num);
    hashnode_t* node = &ht->table[idx];

    if (node->key != NULL && strcmp(node->key, key) == 0) {
      return node->item;
    }
  }
  return NULL;
}

/**************** hashtable_print ****************/
void hashtable_print(hashtable_t* ht, FILE* fp,
    void (*itemprint)(FILE* fp, const char* key, void* item))
{
  if (fp != NULL) {
    if (ht != NULL) {
      fputc('{', fp);

      for (int i = 0; i < ht->num; i++) {
        if (ht->table[i].key != NULL && itemprint != NULL) {
          (*itemprint)(fp, ht->table[i].key, ht->table[i].item);
          fputs(", ", fp);
        }
      }

      fputc('}', fp);
    } else {
      fputs("(null)", fp);
    }
  }
}

/**************** hashtable_iterate ****************/
void hashtable_iterate(hashtable_t* ht, void* arg,
    void (*itemfunc)(void* arg, const char* key, void* item))
{
  if (ht != NULL && itemfunc != NULL)
    for (int i = 0; i < ht->num; i++)
      if (ht->table[i].key != NULL)
        (*itemfunc)(arg, ht->table[i].key, ht->table[i].item);
}

/**************** hashtable_delete ****************/
void hashtable_delete(hashtable_t* ht, void (*itemdelete)(void* item))
{
  if (ht != NULL) {
    for (int i = 0; i < ht->num; i++) {
      if (ht->table[i].key != NULL) {
        count_free(ht->table[i].key);
        if (itemdelete != NULL)
          (*itemdelete)(ht->table[i].item);
      }
    }
    count_free(ht->table);
    count_free(ht);
  }
}
