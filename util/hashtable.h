/* ========================================================================== */
/* File: hashtable.h
 *
 * Project name: CS50 Tiny Search Engine
 * Component name: Query
 *
 * This file contains the definitions for a hashtable of void pointers, 
 * Also contains definitions for the nodes that the hashtable will contain.
 *
 */
/* ========================================================================== */
#ifndef HASHTABLE_H
#define HASHTABLE_H

// ---------------- Prerequisites e.g., Requires "math.h"
#include "common.h"
// ---------------- Constants
#define MAX_HASH_SLOT 10000                  // number of "buckets"

// ---------------- Structures/Types

typedef struct WordNode {
  struct WordNode *next;            // pointer to the next word (for collisions)
  char *word;                       // the word
  struct DocumentNode *page;               // pointer to the first element of the page list.
} WordNode;

typedef struct HashTable {
    WordNode *table[MAX_HASH_SLOT];     // actual hashtable
} HashTable;

// ---------------- Prototypes/Macros
unsigned long JenkinsHash(const char *str, unsigned long mod);
int addWord(char *w, int docId, HashTable *Index, int remakeFlag, int freq);
int cleanIndex(HashTable *Index);
HashTable *FileToIndex(char *indexfile);
#endif // HASHTABLE_H