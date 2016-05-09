/* ========================================================================== */
/* File: list.h
 *
 * Project name: CS50 Tiny Search Engine
 * Component name: Query
 *
 * This file contains the definitions for a doubly-linked list of WebPages.
 *
 */
/* ========================================================================== */
#ifndef LIST_H
#define LIST_H

// ---------------- Prerequisites e.g., Requires "math.h"
#include "common.h"                          // common functionality

// ---------------- Constants

// ---------------- Structures/Types

typedef struct List {
   	struct DocumentNode *head;                          // "beginning" of the list
   	struct DocumentNode *tail;
} List;

// ---------------- Public Variables

// ---------------- Prototypes/Macros
int AppendList(DocumentNode *document, List *list);
int RemoveNode(DocumentNode *document, List *list);
int InsertNode(DocumentNode *doc, DocumentNode *place, List *list);
int FreeList(List *list);
int SortList(List *list);
int IsEmpty(List *list);

#endif // LIST_H