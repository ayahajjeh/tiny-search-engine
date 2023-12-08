/*
 * counters.c - a set of counters, each distinguished by an integer key. 
 * It’s a set - each key can only occur once in the set - but instead of 
 * storing (key,item) pairs, it tracks a counter for each key.
 *
 * Aya Hajjeh, Lab 3, CS50, Fall 2022
 */

#include "counters.h"
#include "memory.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**************** file-local global variables ****************/
/* none */

/**************** local types ****************/
typedef struct countersnode {
    int key;
    int counter;
    struct countersnode* next;
} countersnode_t;

/**************** global types ****************/
typedef struct counters {
    struct countersnode* head;
} counters_t;

/**************** global functions ****************/
/* that is, visible outside this file */
/* see counters.h for comments about exported functions */

/**************** local functions ****************/
/* not visible outside this file */
static countersnode_t* countersnode_find(countersnode_t* ctrs, const int key);
static countersnode_t* countersnode_new(const int key);

/**************** countersnode_find() ****************/
/* find node has given key */
static countersnode_t* countersnode_find(countersnode_t* ctrs, const int key)
{
    if (ctrs == NULL)
        return NULL;
    else if (ctrs->key == key)
        return ctrs;
    else
        return countersnode_find(ctrs->next, key);
}

/**************** countersnode_new() ****************/
/* create new node */
static countersnode_t* countersnode_new(const int key)
{
    countersnode_t* new = count_malloc(sizeof(countersnode_t));
    if (new == NULL) {
        return NULL;
    } else { // new node
        new->key = key;
        new->counter = 0;
        new->next = NULL;
        return new;
    }
}

/**************** counters_new() ****************/
counters_t* counters_new(void)
{
    counters_t* counters = count_malloc(sizeof(counters_t));
    if (counters == NULL) {
        return NULL;
    } else {
        counters->head = NULL;
        return counters;
    }
}

/**************** counters_set() ****************/
bool counters_set(counters_t* ctrs, const int key, const int count)
{
    if (ctrs != NULL && key >= 0 && count >= 0) {
        countersnode_t* node = countersnode_find(ctrs->head, key);

        if (node == NULL) { // new key
            node = countersnode_new(key);

            if (node == NULL) // error on generating new
                return false;

            node->next = ctrs->head;
            ctrs->head = node;

#ifdef MEMTEST
            count_report(stdout, "After create new node in counters");
#endif
        }

        node->counter = count;
        return true;
    } else {
        return false;
    }
}

/**************** counters_add() ****************/
int counters_add(counters_t* ctrs, const int key)
{
    if (ctrs != NULL && key >= 0) {
        countersnode_t* node = countersnode_find(ctrs->head, key);
        int count = (node != NULL) ? node->counter : 0;
        bool status = counters_set(ctrs, key, ++count);
        return (status == true) ? count : 0;
    } else {
        return 0;
    }
}

/**************** counters_get() ****************/
int counters_get(counters_t* ctrs, const int key)
{
    if (ctrs != NULL && key >= 0) {
        countersnode_t* node = countersnode_find(ctrs->head, key);
        if (node == NULL)
            return 0;
        else
            return node->counter;
    } else {
        return 0;
    }
}

/**************** counters_print() ****************/
void counters_print(counters_t* ctrs, FILE* fp)
{
    if (fp != NULL) {
        if (ctrs != NULL) {
            fputc('{', fp);
            countersnode_t* node;
            for (node = ctrs->head; node != NULL; node = node->next) {
                fprintf(fp, "{%d, %d}, ", node->key, node->counter);
            }
            fputc('}', fp);
        } else {
            fputs("(null)", fp);
        }
    }
}

/**************** counters_iterate() ****************/
void counters_iterate(counters_t* ctrs, void* arg,
    void (*itemfunc)(void* arg, const int key, const int count))
{
    if (ctrs != NULL && itemfunc != NULL) {
        countersnode_t* node = ctrs->head;
        for (; node != NULL; node = node->next) {
            (*itemfunc)(arg, node->key, node->counter);
        }
    }
}

/**************** counters_delete() ****************/
void counters_delete(counters_t* ctrs)
{
    if (ctrs != NULL) {
        for (countersnode_t* node = ctrs->head; node != NULL;) {
            countersnode_t* next = node->next;
            count_free(node);
            node = next;
        }
        count_free(ctrs);
    }

#ifdef MEMTEST
    count_report(stdout, "End of counters delete");
#endif
}

