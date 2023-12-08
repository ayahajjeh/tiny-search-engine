/*
 * set.c - a set maintains an unordered collection of (key,item) pairs; any given key 
 * can only occur in the set once. It starts out empty and grows as the caller inserts 
 * new (key,item) pairs. The caller can retrieve items by asking for their key, but 
 * cannot remove or update pairs. Items are distinguished by their key.
 *
 * Aya Hajjeh, Lab 3, CS50, Fall 2022
 */

#include "set.h"
#include "memory.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**************** file-local global variables ****************/
/* none */

/**************** local types ****************/
typedef struct setnode {
    char* key;
    void* item;
    struct setnode* next;
} setnode_t;

/**************** global types ****************/
typedef struct set {
    struct setnode* head;
} set_t;

/**************** global functions ****************/
/* that is, visible outside this file */
/* see set.h for comments about exported functions */

/**************** local functions ****************/
/* not visible outside this file */
static setnode_t* setnode_new(const char* key, void* item);
static setnode_t* setnode_find(setnode_t* set, const char* key);
static bool contains_key(set_t* set, const char* key);

/**************** set_new() ****************/
set_t* set_new(void)
{
    set_t* set = count_malloc(sizeof(set_t));

    if (set == NULL) {
        return NULL;
    } else {
        set->head = NULL;
        return set;
    }
}

/**************** set_insert() ****************/
bool set_insert(set_t* set, const char* key, void* item)
{
    bool rst = false;
    if (set != NULL && item != NULL && key != NULL) {
        if (!contains_key(set, key)) {
            setnode_t* new = setnode_new(key, item);
            if (new != NULL) {
                // add new node to the head
                new->next = set->head;
                set->head = new;
                rst = true;
            }
        }
    }

#ifdef MEMTEST
    count_report(stdout, "After set_insert");
#endif

    return rst;
}

/**************** setnode_find() ****************/
/* find the node match with key */
static setnode_t* setnode_find(setnode_t* set, const char* key)
{
    if (set == NULL) {
        return NULL;
    } else if (strcmp(set->key, key) == 0) {
        return set;
    } else {
        return setnode_find(set->next, key);
    }
}

/**************** contains_key() ****************/
/* check if key exists in set */
static bool contains_key(set_t* set, const char* key)
{
    setnode_t* node = setnode_find(set->head, key);
    if (node == NULL)
        return false;
    else
        return true;
}

/**************** setnode_new() ****************/
/* create new setnode */
static setnode_t* setnode_new(const char* key, void* items)
{
    setnode_t* node = count_malloc(sizeof(setnode_t));

    if (node == NULL) {
        return NULL;
    } else {
        char* key_cpy = count_malloc(strlen(key) + 1);
        if (key_cpy == NULL) { // could not allocate key
            count_free(node);
            return NULL;
        }
        memcpy(key_cpy, key, strlen(key) + 1);

        node->key = key_cpy;
        node->item = items;
        node->next = NULL;
        return node;
    }
}

/**************** set_find() ****************/
void* set_find(set_t* set, const char* key)
{
    setnode_t* node = setnode_find(set->head, key);
    if (node == NULL)
        return NULL;
    else
        return node->item;
}

/**************** set_iterate() ****************/
void set_iterate(set_t* set, void* arg,
    void (*itemfunc)(void* arg, const char* key, void* item))
{
    if (set != NULL && itemfunc != NULL) {
        // call itemfunc with arg on each item
        for (setnode_t* node = set->head; node != NULL; node = node->next) {
            (*itemfunc)(arg, node->key, node->item);
        }
    }
}

/**************** set_print() ****************/
void set_print(set_t* set, FILE* fp,
    void (*itemprint)(FILE* fp, const char* key, void* item))
{
    if (fp != NULL) {
        if (set != NULL) {
            fputc('{', fp);
            for (setnode_t* node = set->head; node != NULL; node = node->next) {
                if (itemprint != NULL) {
                    (*itemprint)(fp, node->key, node->item);
                    fputc(',', fp);
                }
            }
            fputc('}', fp);
        } else {
            fputs("(null)", fp);
        }
    }
}

/**************** set_delete() ****************/
void set_delete(set_t* set, void (*itemdelete)(void* item))
{

    if (set != NULL) {
        for (setnode_t* node = set->head; node != NULL;) {
            count_free(node->key);
            if (itemdelete != NULL) {
                (*itemdelete)(node->item);
            }
            setnode_t* next = node->next;
            count_free(node);
            node = next;
        }

        count_free(set);
    }

#ifdef MEMTEST
    count_report(stdout, "End of bag_delete");
#endif
}
