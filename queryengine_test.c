// Filename: Test cases for query.c
// Description: A unit test for query
// 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "../util/list.h"
#include "../util/common.h"
#include "../util/hashtable.h"
#include "../util/web.h"
#include "../util/file.h"

List *CopyDocList(char *word, HashTable *Index);
int ParseInput(char *input, HashTable *Index, List *FinalList);
int ANDDocs(List *RunningList, List *NewDocList);
int ORDocs(List *FinalList, List *RunningList);
int PrintDocuments(List *FinalList, char *FileDirectory);

// Useful MACROS for controlling the unit tests.

// each test should start by setting the result count to zero

#define START_TEST_CASE  int rs=0

// check a condition and if false print the test condition failed
// e.g., SHOULD_BE(dict->start == NULL)
// note: the construct "#x" below is the sstringification preprocessor operator that
//       converts the argument x into a character string constant

#define SHOULD_BE(x) if (!(x))  {rs=rs+1; \
    printf("Line %d test [%s] Failed\n", __LINE__,#x); \
  }


// return the result count at the end of a test

#define END_TEST_CASE return rs



#define RUN_TEST(x, y) if (!x()) {              \
    printf("Test %s passed\n", y);              \
} else {                                        \
    printf("Test %s failed\n", y);              \
    cnt = cnt + 1;                              \
}
    

// Test case: CopyDocList:1
// This test case calls CopyDocList for the condition where the word is not in
// the Index

List *CopyDocList(char *word, HashTable *Index)
{
  int HashIndex = JenkinsHash(word, MAX_HASH_SLOT);
  struct WordNode *search = Index->table[HashIndex];
  
  // If nothing in the hash-slot, the word is not in the Index
  if (search == NULL){
    return NULL;
  }
  else {
    while (search != NULL){

      // If the word is found, transfer its Document data
      if (strcmp(search->word, word) == 0){
        
        // Get pointer to the first document to be copied
        DocumentNode *temp = search->page;
        
        // Create a document list
        struct List *DocumentList = malloc(sizeof(List));
        DocumentList->head = DocumentList->tail = NULL;

        // Iterate through list and transfer data
        while (temp != NULL){
          struct DocumentNode *NewDoc = malloc(sizeof(DocumentNode));
          NewDoc->freq = temp->freq;
          NewDoc->doc_id = temp->doc_id;
          AppendList(NewDoc, DocumentList);
          temp = temp->next;

        }
        return DocumentList;
      }
      search = search->next;
    }
    return NULL;
  }
}

int ORDocs(List *FinalList, List *RunningList)
{
  struct DocumentNode *current;
  struct DocumentNode *final;

  current = RunningList->head;
  while (current != NULL){
    int match = 0;
    final = FinalList->head;
    while (final != NULL){
      if (current->doc_id == final->doc_id) {
        final->freq = final->freq + current->freq;
        match = 1;
        break;
      }
      final = final->next;
    }
    DocumentNode *temp = current;
    current = current->next;
    RemoveNode(temp, RunningList);
    if (!match){
      AppendList(temp, FinalList);
    }
    else if (match){
      free(temp);
    }
  }
  // DocumentNodes in RunningList are now all either moved to FinalList or are freed
  // Reset Runninglist and make empty by resetting its head and tail
  RunningList->head = NULL;
  RunningList->tail = NULL;

  return 1;
}


int ANDDocs(List *RunningList, List *NewDocList)
{

  struct DocumentNode *current = RunningList->head;

  //Iterate through the RunningList
  while (current != NULL){
    int match = 0;

    // Check to see if the doc_id matches any doc_ids in the new list
    DocumentNode *comparable = NewDocList->head;
    while (comparable != NULL){

      // If there is a match, add their scores together
      if (current->doc_id == comparable->doc_id){
        current->freq = current->freq + comparable->freq;
        match = 1;
        break;
      }
      comparable = comparable->next;
    }
    DocumentNode *temp = current;
    current = current->next;
    if (!match){
      RemoveNode(temp, RunningList);
      free(temp);
    }
  }
  FreeList(NewDocList);
  return 1;
}

int ParseInput(char *input, HashTable *Index, List *FinalList)
{
  int LastWasOR = 1;
  int LastWasAND = 1;
  struct List *RunningList = malloc(sizeof(List));
  struct List *WordDocs = malloc(sizeof(List));
  RunningList->head = RunningList->tail = WordDocs->head = WordDocs->tail = NULL;
  char *word = calloc(15, sizeof(char));

  word = strtok(input, " ");

  while (word != NULL) {

    //if the word = OR, then push WorkingList into the RunningList
    if (strcmp(word, "OR") == 0){
      if (LastWasOR || LastWasAND){
        printf("Invalid operator placement in query.\n");
        return 0;
      }
      ORDocs(FinalList, RunningList);
      LastWasOR = 1;
    }

    // If the word is AND, mkae sure it wasn't preceded by another operator
    // and turn the AND switch on
    else if (strcmp(word, "AND") == 0){
      if (LastWasOR || LastWasAND){
        printf("Invalid operator placement in query.\n");
        return 0;
      }
      LastWasAND = 1;
    }

    // If the input is a word and not an operator
    else{ 
      NormalizeWord(word);    //convert to lowercase

      // If the last operator was OR, copy the word list right into the RunningList
      // (Otherwise new words would be compared with an empty list)
      if (LastWasOR){
        RunningList = CopyDocList(word, Index);
        if (!RunningList){
          printf("Word %s not found in any files\n", word);
          return 0;
        }
      }

      // Otherwise AND it with the RunningList
      else {
        WordDocs = CopyDocList(word, Index);
        if (!WordDocs){
          printf("Word %s not found in any files\n", word);
          return 0;
        }
        ANDDocs(RunningList, WordDocs);
      }

      LastWasAND = 0;
      LastWasOR = 0;
    }
    word = strtok(NULL, " ");
  }
    //Must push remaining items on RunningList into FinalList
  ORDocs(FinalList, RunningList);
  return 1;
}

//Test Case: CopyDocList:1
//Makes sure that CopyDocList properly returns when the search is not in the Index

int TestCopyDocList1() {
  START_TEST_CASE;
  struct HashTable *Index = FileToIndex("../indexer/src/indexer.dat");
  struct List *TestList = CopyDocList("DefinitelyNotinIndex", Index);
  
  SHOULD_BE(!TestList);
  cleanIndex(Index);
  END_TEST_CASE;
}

// Test case: CopyDocList:2
// This test case calls CopyDocList() but has an empty Index

int TestCopyDocList2() {
  START_TEST_CASE;
  struct HashTable *Index = malloc(sizeof(HashTable));
  struct List *TestList = CopyDocList("moose", Index);
  SHOULD_BE(!TestList);
  END_TEST_CASE;
}


// Test case:ANDDocs1
// When the lists are mutually exclusive- FinalList should be empty
int TestANDDocs1() {
  START_TEST_CASE;

  struct List *FinalList = malloc(sizeof(List));
  struct List *RunningList = malloc(sizeof(List));

  struct DocumentNode *one = malloc(sizeof(DocumentNode));
  struct DocumentNode *two = malloc(sizeof(DocumentNode));
  struct DocumentNode *three = malloc(sizeof(DocumentNode));
  struct DocumentNode *four = malloc(sizeof(DocumentNode));

  one->doc_id = 1;
  two->doc_id = 2;
  three->doc_id =3;
  four->doc_id = 4;

  AppendList(one, FinalList);
  AppendList(two, FinalList);

  AppendList(three, RunningList);
  AppendList(four, RunningList);

  ANDDocs(FinalList, RunningList);
  SHOULD_BE(FinalList->head == NULL);

  END_TEST_CASE;
}


// Test case:ANDDocs2
// When nodes match freq should be added together

int TestANDDocs2() {
  START_TEST_CASE;
  struct List *FinalList = malloc(sizeof(List));
  struct List *RunningList = malloc(sizeof(List));
  RunningList->head = RunningList->tail = FinalList->head = FinalList->tail = NULL;

  struct DocumentNode *one = malloc(sizeof(DocumentNode));
  struct DocumentNode *two = malloc(sizeof(DocumentNode));
  struct DocumentNode *three = malloc(sizeof(DocumentNode));
  struct DocumentNode *four = malloc(sizeof(DocumentNode));

  struct DocumentNode *one2 = malloc(sizeof(DocumentNode));
  struct DocumentNode *two2 = malloc(sizeof(DocumentNode));
  struct DocumentNode *three2 = malloc(sizeof(DocumentNode));
  struct DocumentNode *four2 = malloc(sizeof(DocumentNode));
  
  one->doc_id = 1;
  two->doc_id = 2;
  three->doc_id =3;
  four->doc_id = 4;

  one2->doc_id = 1;
  two2->doc_id = 2;
  three2->doc_id =3;
  four2->doc_id = 4;

  one->freq = 1;
  two->freq = 1;
  three->freq = 1;
  four->freq = 1;
  one2->freq = 1;
  two2->freq = 1;
  three2->freq = 1;
  four2->freq = 1;

  AppendList(one, FinalList);
  AppendList(two, FinalList);
  AppendList(three, FinalList);
  AppendList(four, FinalList);

  
  AppendList(one2, RunningList);
  AppendList(two2, RunningList);
  AppendList(three2, RunningList);
  AppendList(four2, RunningList);

  ANDDocs(FinalList, RunningList);

  SHOULD_BE(one->freq == 2);
  SHOULD_BE(two->freq == 2);
  SHOULD_BE(three->freq == 2);
  SHOULD_BE(four->freq == 2);
  END_TEST_CASE;
}


// Test case:ORDoc1
// Checking to make sure that OrDoc runs properly, getting rid of the right 
// nodes and transferring data when there are matches and nonmatches in documents

int TestORDocs1() {
  START_TEST_CASE;
  struct List *FinalList = malloc(sizeof(List));
  struct List *RunningList = malloc(sizeof(List));
  RunningList->head = RunningList->tail = FinalList->head = FinalList->tail = NULL;

  struct DocumentNode *one = malloc(sizeof(DocumentNode));
  struct DocumentNode *two = malloc(sizeof(DocumentNode));
  struct DocumentNode *three = malloc(sizeof(DocumentNode));
  struct DocumentNode *four = malloc(sizeof(DocumentNode));


  
  one->doc_id = 10;
  two->doc_id = 20;
  three->doc_id = 10;
  four->doc_id = 30;

  one->freq = 1;
  two->freq = 1;
  three->freq = 1;
  four->freq = 1;

  AppendList(one, FinalList);
  AppendList(two, FinalList);
  AppendList(three, RunningList);
  AppendList(four, RunningList);

  ORDocs(FinalList, RunningList);

  SHOULD_BE(one->freq == 2);
  SHOULD_BE(two->freq == 1);
  SHOULD_BE(four->freq == 1);
  SHOULD_BE(RunningList->head == NULL);
  SHOULD_BE(RunningList->tail == NULL);

  END_TEST_CASE;
}



int main(int argc, char** argv) {
  int cnt = 0;
  RUN_TEST(TestCopyDocList2, "CopyDocList Test case 2");
  RUN_TEST(TestANDDocs1, "ANDDocs Test case 1");
  RUN_TEST(TestANDDocs2, "ANDDocs Test case 2");
  RUN_TEST(TestORDocs1, "ORDocs Test case 1");

  if (!cnt) {
    printf("All passed!\n"); return 0;
  } else {
    printf("Some fails!\n"); return 1;
  }
}
