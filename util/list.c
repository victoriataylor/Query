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
#include <stdlib.h>
#include <stdio.h>
#include "list.h"
#include "common.h"


/* Adds DocumentNode to the end of a List of DocumentNodes.
 */
int AppendList(DocumentNode *document, List *list) 
{
	if (list->head == NULL) { //first node to be added to list
		list->head = document;
		list->tail = document;
		document->prev = NULL;
		document->next = NULL;
	}
	else {
		list->tail->next = document;
		document->prev = list->tail;
		list->tail = document;
		document->next = NULL;
	}
	return 1;
}


/* Removes the specified node from the given list and cleans up the empty
 * patch that it leaves. Makes sure the head and tail of the list remain intact.
 */
int RemoveNode(DocumentNode *document, List *list){
	if (document == list->head && document == list->tail){
		list->head = NULL;
		list->tail = NULL;
	}
	else if (document == list->head){
		list->head = document->next;
		list->head->prev = NULL;
	}
	else if (document == list->tail){
		list->tail = document->prev;
		list->tail->next = NULL;
	}
	else {
		document->prev->next = document->next;
		document->next->prev = document->prev;
	}
	return 1;
}


/* Inserts a DocumentNode before a given node- place- in the list
 */
int InsertNode(DocumentNode *doc, DocumentNode *place, List *list){
	if (place == list->head){
		list->head = doc;
		doc->prev = NULL;
	}
	else {
		place->prev->next = doc;
		doc->prev = place->prev;
	}
	place->prev = doc;
	doc->next = place;
	return 1;
}


//Returns 1 if List is empty and 0 if it is not 
int IsEmpty(List *list){
	if (!list->head){
		return 1;
	}
	else {
		return 0;
	}
}


/* Traverses List and frees all nodes before finally freeing the list itself
 */
int FreeList(List *list){
	DocumentNode *doc = list->head;
	DocumentNode *temp;
	while (doc != NULL){
		temp = doc; 
		doc = doc->next;
		free(temp);
	}
	free(list);
	return 1;
}


/* Sorts list in order of frequency, so that list->head contains the largest frequency
 * and list->tail contains the smallest frequency.
 */
int SortList(List *list){
	DocumentNode *sortPos = list->head;
	DocumentNode *temp;
	DocumentNode *largest;
	while (sortPos != list->tail){
		temp = sortPos->next;
		largest = sortPos;
		while (temp != NULL){
			if (temp->freq > largest->freq){
				largest = temp;
			}
			temp = temp->next;
		}
		//Remove the largest node and put it at the current sort-position
		if (largest == sortPos){
				sortPos = sortPos->next; //move the sort position up
		}
		else {
			RemoveNode(largest, list);
			InsertNode(largest, sortPos, list);
		}
	}
	return 1;
}