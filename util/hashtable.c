/* ========================================================================== */
/* File: hashtable.c
 *
 * Project name: CS50 Tiny Search Engine
 * Component name: Query
 *
 * Author: Victoria M. Taylor
 * Date:
 *
 * You should include in this file your functionality for the hashtable as
 * described in the assignment and lecture.
 */
/* ========================================================================== */

// ---------------- Open Issues

// ---------------- System includes e.g., <stdio.h>
#include <string.h>                          // strlen
#include <stdio.h>
#include <stdlib.h>

// ---------------- Local includes  e.g., "file.h"
#include "hashtable.h"                       // hashtable functionality
//maybe include common.h

// ---------------- Constant definitions

// ---------------- Macro definitions

// ---------------- Structures/Types

// ---------------- Private variables

// ---------------- Private prototypes

unsigned long JenkinsHash(const char *str, unsigned long mod)
{
    size_t len = strlen(str);
    unsigned long hash, i;

    for(hash = i = 0; i < len; ++i)
    {
        hash += str[i];
        hash += (hash << 10);
        hash ^= (hash >> 6);
    }

    hash += (hash << 3);
    hash ^= (hash >> 11);
    hash += (hash << 15);

    return hash % mod;
}

/* Adds word to the Index. If a WordNode is already in the word's hash slot, check
 * to see if the words match. If not, traverse the singly linked list until a match 
 * is found or the list end is found. If a match is found, check to see if the current
 * document was recorded. If it was, up the freq of the word. If not, create a new
 * document node and set the freq to 1. 
 * If the remakeFlag = 1, Index is being rebuilt from a file, and document frequency
 * will be set from freq rather than incremented.
//  */
int addWord(char *w, int docId, HashTable *Index, int remakeFlag, int freq)
{

    int hashIndex = JenkinsHash(w, MAX_HASH_SLOT);
    struct WordNode *currWord = Index->table[hashIndex];

    // Nothing in hash index
    if (currWord == NULL){
        struct WordNode *newWord= malloc(sizeof(WordNode));
        struct DocumentNode *doc = malloc(sizeof(DocumentNode));
        if (!doc || !newWord){
            printf("malloc didn't work properly");
        }
        newWord->word = w;
        newWord->page = doc;
        newWord->next = NULL;
        doc->doc_id = docId;
        doc->next = NULL;
        if (remakeFlag == 1) doc->freq = freq;
        if (remakeFlag == 0) doc->freq = 1;
        Index->table[hashIndex] = newWord;
        return 1;
    }
    // Collision: word already in hash index
    else{
        while (currWord != NULL){
            if ((strcmp(currWord->word, w) == 0)) {
                //free the pointers to words that match words already stored
                if (remakeFlag != 1){
                    free(w);
                    w = NULL;
                }
                //If document already exists for word, increase its freq
                struct DocumentNode *currDoc = currWord->page; 
                if (!currDoc){
                    printf("Allocation of DocumentNode failed.\n");
                    exit(1);
                }
                while (currDoc != NULL){
                    //printf("docId=%d currDoc->doc_id= %d",docId, currDoc->doc_id);
                     if ((docId == currDoc->doc_id)){
                        currDoc->freq++;
                        return 1;
                    }
                    currDoc = currDoc->next;
                }

                //Word is found in a new document
                struct DocumentNode *newdoc = malloc(sizeof(DocumentNode));
                if (!newdoc){
                    fprintf(stderr, "Allocation of DocumentNode failed\n.");
                    exit(1);
                }
                newdoc->freq = 1; 
                newdoc->doc_id = docId;
                newdoc->next = currWord->page;
                currWord->page = newdoc;
                return 1;
            }
            currWord = currWord->next;
        }
        //If word not found in word linked list
        struct DocumentNode *docnode = malloc(sizeof(DocumentNode));
        if (!docnode){
            fprintf(stderr, "Allocation of DocumentNode failed.\n");
            exit(1);
        }
        docnode->doc_id = docId;
        docnode->next = NULL; //first document to find word
        if (remakeFlag == 1) docnode->freq = freq;
        if (remakeFlag == 0) docnode->freq = 1;

        struct WordNode *NewWordNode = malloc(sizeof(WordNode));
        if (!NewWordNode){
            fprintf(stderr, "Allocation of WordNode failed.");
            exit(1);
        }
        NewWordNode->word = w;
        NewWordNode->page = docnode;
        //Add word to beginning of linked list
        NewWordNode->next = Index->table[hashIndex];
        Index->table[hashIndex] = NewWordNode;
        return 1;
   }
}

/* Goes through Index and frees all words, WordNodes and DocumentNodes
 * Then frees Index.
 */
int cleanIndex(HashTable *Index){
    for (int x = 0; x < MAX_HASH_SLOT; x++){
        if (Index->table[x] != NULL) {
            struct WordNode *wordList = Index->table[x];
            while (wordList != NULL){
                struct DocumentNode *docList = wordList->page;
                while (docList != NULL){
                    struct DocumentNode *tempDoc = docList;
                    docList = docList->next;
                    free(tempDoc);
                }
                struct WordNode *tempWord = wordList;
                wordList = wordList->next;
                free(tempWord->word);
                free(tempWord);
            }
        }
    }
    free(Index);
    return 1;
}

HashTable *FileToIndex(char *indexfile) 
{
    struct HashTable *Index = malloc(sizeof(HashTable));
    for (int i = 0; i < MAX_HASH_SLOT; i++){
        Index->table[i] = NULL;
    }
    char *token, *wordInfo, *word;
    int docnum, docId, docfreq, offset;
    FILE *fp = fopen(indexfile, "r");
    char *line = calloc(10000, sizeof(char));


    while (fgets(line, 10000, fp) != NULL){
        token = strtok(line, " ");
        word = calloc(75, sizeof(char));
        strcpy(word, token);        
        wordInfo = strtok(NULL, "\n");
        
        sscanf(wordInfo, "%d%n", &docnum, &offset);
        wordInfo += offset; 
        
        while (sscanf(wordInfo, "%d%d%n", &docId, &docfreq, &offset) !=EOF) {
            addWord(word, docId, Index, 1, docfreq);
            wordInfo += offset;
        }
    }
    free(line);
    fclose(fp);
    return Index;
}
