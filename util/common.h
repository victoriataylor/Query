/* ========================================================================== */
/* File: common.h
 *
 * Project name: CS50 Tiny Search Engine
 * Component name: Query
 *
 * This file contains the common defines and data structures.
 *
 */
/* ========================================================================== */
#ifndef COMMON_H
#define COMMON_H

// ---------------- Prerequisites e.g., Requires "math.h"

// ---------------- Constants

// ---------------- Structures/Types

typedef struct DocumentNode {
  struct DocumentNode *next;         // pointer to the next member of the list.
  struct DocumentNode *prev;
  int doc_id;                        // document identifier
  int freq;                          // number of occurrences of the word
} DocumentNode;

// ---------------- Public Variables

// ---------------- Prototypes/Macros

#endif // COMMON_H

