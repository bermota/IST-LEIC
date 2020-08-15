#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "sync.h"
#include "fs.h"
#include "lib/bst.h"

/* Hash function. */
int hash(char* name, int n) {
	if (!name) 
		return -1;
	return (int) name[0] % n;
}

/* Returns unused iNumber. */
int obtainNewInumber(tecnicofs* fs) {
	int newInumber = ++(fs->nextINumber);
	return newInumber;
}

/* Initializes and allocates memory for tecnicoFS. */
tecnicofs* new_tecnicofs(){
	tecnicofs* fs = safeMalloc(sizeof(tecnicofs));

	fs->nextINumber = 0;
	fs->bstRoots = safeMalloc(numberTrees * sizeof(node*));
	fs -> htLocks = safeMalloc(numberTrees * sizeof(LOCK_T));

	for (int i = 0; i < numberTrees; i++){
		fs -> bstRoots[i] = NULL;
		INIT_LOCK(&(fs-> htLocks[i]));
	}

	return fs;
}

/* Frees all of tecnicoFS's related memory. */
void free_tecnicofs(tecnicofs* fs){
	for (int i = 0; i < numberTrees; i++){
		free_tree(fs->bstRoots[i]);
		DESTROY_LOCK(&(fs->htLocks[i]));
	}

	free(fs -> bstRoots);
	free(fs -> htLocks);
	free(fs);
}

/* Creates and inserts a new node on the related hash-index BST. */
void create(tecnicofs* fs, char *name, int inumber){
	int i = hash(name, numberTrees);

	WR_LOCK(&(fs->htLocks[i]));
	fs->bstRoots[i] = insert(fs->bstRoots[i], name, inumber);
	UNLOCK(&(fs->htLocks[i]));
}

/* Removes file and frees memory from the related hash-index BST. */
void delete(tecnicofs* fs, char *name){
	int i = hash(name, numberTrees);

	WR_LOCK(&(fs->htLocks[i]));
	fs->bstRoots[i] = remove_item(fs->bstRoots[i], name);
	UNLOCK(&(fs->htLocks[i]));
}

/* Returns 0 if node not found and its iNumber otherwise. */
int lookup(tecnicofs* fs, char *name){
	int i = hash(name, numberTrees);
	int inumber = 0;

	RD_LOCK(&(fs->htLocks[i]));

	node* searchNode = search(fs->bstRoots[i], name);
	if (searchNode)
		inumber = searchNode->inumber;

	UNLOCK(&(fs->htLocks[i]));
	return inumber;
}

/* Changes oldFileName to newFileName if possible. */
int editFileName(tecnicofs *fs, char* oldFileName, char* newFileName){
	int i = hash(oldFileName, numberTrees);
	int j = hash(newFileName, numberTrees);
	int stop = 0;
	int trylock;
	node* file;
	node* newFileExists;

	if (i != j){ // on different BST's.
		while(!stop){
			WR_LOCK(&(fs->htLocks[i]));
			trylock = TRYLOCK(&(fs->htLocks[j]));
			checkError(trylock && trylock != EBUSY, "Error on trylock.\n");
			if (!trylock)
				stop = 1;
			else if (trylock == EBUSY){
				UNLOCK(&(fs->htLocks[i]));
				usleep(((float) rand()/RAND_MAX)*1000);
			}
		}

		file = search(fs->bstRoots[i], oldFileName);
		newFileExists = search(fs->bstRoots[j], newFileName);
		
		if (!newFileExists && file){
			fs->bstRoots[i] = remove_item(fs->bstRoots[i], oldFileName);
			fs->bstRoots[j] = insert(fs->bstRoots[j], newFileName, file->inumber);	
		}
			
		UNLOCK(&(fs->htLocks[i]));
		UNLOCK(&(fs->htLocks[j]));
	}

	else{ // on the same BST.
		WR_LOCK(&(fs->htLocks[i]));
		file = search(fs->bstRoots[i], oldFileName);
		newFileExists = search(fs->bstRoots[j], newFileName);
		if (!newFileExists && file){
			fs->bstRoots[i] = remove_item(fs->bstRoots[i], oldFileName);
			fs->bstRoots[i] = insert(fs->bstRoots[i], newFileName, file -> inumber);
		}
		UNLOCK(&(fs->htLocks[i]));
	}
	return EXIT_SUCCESS;
	
}


/* Graphical representation of tecnicoFS is printed to fp. */
void print_tecnicofs_tree(FILE * fp, tecnicofs *fs){
	for (int i = 0; i < numberTrees; i++)
		print_tree(fp, fs->bstRoots[i]);
}
