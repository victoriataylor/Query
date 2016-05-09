#include <string.h>
#include <stdio.h>
#include <stdlib.h>
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

int main(int argc, char* argv[])
{
	// Check parameters
	if (argc != 3){
		printf("Indexer must recieve 2 parameters: an Index file and a Directory of source files\n");
	}
	if (!IsFile(argv[1])){
		printf("First parameter is not valid file.\n");
		exit(1);
	}
	if (!IsDir(argv[2])){
		printf("Second paramter is not valid directory\n");
		exit(1);
	}


	// Construct the Index
	printf("Constructing Index\n");
	struct HashTable *Index = FileToIndex(argv[1]);
	printf("Index Complete\n");

	//char input[20];
	char *input;
	char string[50];

	printf("QUERY:>");
	// Keep getting input from user until user quits (cntrl-d)
	while (fgets(string, sizeof(string), stdin) != NULL)
	{
		struct List *FinalList =malloc(sizeof(List));
		FinalList->head = NULL;
		FinalList->tail = NULL;

		input = strtok(string, "\n");
		if (ParseInput(input, Index, FinalList)){
			if (IsEmpty(FinalList)){
				printf("No matches found\n");
			}
			else {	
				SortList(FinalList);
				PrintDocuments(FinalList, argv[2]);
			}
		}
		FreeList(FinalList);
		printf("\nQUERY:>");
	}
	cleanIndex(Index);
	return 1;
}

//returns 1 if successful string
//returns 0 if input is illegal
int ParseInput(char *input, HashTable *Index, List *FinalList)
{
	int LastWasOR = 1;
	int LastWasAND = 1;
	struct List *RunningList = malloc(sizeof(List));
	struct List *WordDocs = malloc(sizeof(List));
	RunningList->head = RunningList->tail = WordDocs->head = WordDocs->tail = NULL;
	char *word;

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
			NormalizeWord(word);		//convert to lowercase

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
	FreeList(RunningList);
	return 1;
}


/* Takes two Lists of DocumentNodes. The first is the RunningList and the 
 * second is a new document list that will be compared it. For each document in
 * the RunningList, the NewDocList is checked to see if it also contains the
 * document. If it does, the score of the document in the NewDocList is added
 * to the score of the document in the RunningList. If the document is not found
 * in the NewDocList, then that DocumentNode is removed from the RunningList, and
 * its space is freed. At the end, the NewDocList structure is freed.
*/
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


/*Function takes in the FinalList and the RunningList. For every Document in the
 * RunningList, it checks to see if the same document is already in the FinalList.
 * If it is, it adds the two document frequencies and stores it in the FinalList
 * DocumentNode. It then removes the corresponding RunningList node and frees it.
 * If the document is not in the FinalList, the DocumentNode is removed from the 
 * RunningList and appended to the FinalList.
*/
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



/* Takes in a word and the Index. Searches for the word in the Index.
 * If the word is not found in the Index, then NULL pointer is returned.
 * If the word is in the Index, its DocumentNodes are copied into a new
 * list of DocumentNodes, which is then returned to the user. The nodes
 * are copied so that the original Index list is not altered.
 */
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


int PrintDocuments(List *FinalList, char *FileDirectory)
{
	struct DocumentNode *temp= FinalList->head;
	const size_t path_size = strlen(FileDirectory) + strlen("/") + 20;
	char *path = malloc(path_size);
	char *URL = calloc(200, sizeof(char));
	while (temp != NULL){
    	snprintf(path, path_size, "%s/%d", FileDirectory, temp->doc_id);
    	if (IsFile(path)){
    		FILE *fp = fopen(path, "r");
 	   		fscanf(fp, "%s", URL);
    		printf("DocumentId:%d URL:%s\n", temp->doc_id, URL);
    		temp = temp->next;	
    	}
	}
	free(path);
	free(URL);
	return 1;
}

