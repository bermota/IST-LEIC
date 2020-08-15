#include "fs.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Returns unused iNUmber */
int obtainNewInumber(tecnicofs* fs)
{
    int newInumber = ++(fs->nextINumber);
    return newInumber;
}

/* Inicializes and allocates memory for tecnicoFS */
tecnicofs* new_tecnicofs()
{
    tecnicofs* fs = malloc(sizeof(tecnicofs));
    if (!fs) {
        perror("failed to allocate tecnicofs");
        exit(EXIT_FAILURE);
    }
    fs->bstRoot = NULL;
    fs->nextINumber = 0;
    return fs;
}

/* Frees ALL of TecnicoFS's related memory */
void free_tecnicofs(tecnicofs* fs)
{
    free_tree(fs->bstRoot);
    free(fs);
}

/* Creates and inserts a new node (=file) on tecnicoFS BST */
void create(tecnicofs* fs, char* name, int inumber)
{
    fs->bstRoot = insert(fs->bstRoot, name, inumber);
}

/* Removes file and frees memory from tecnicoFS bst.
 * Does not decrease nextINumber */
void delete (tecnicofs* fs, char* name)
{
    fs->bstRoot = remove_item(fs->bstRoot, name);
}

/* Returns 0 if node not found,
 * iNumber of node with same name as key if found. */
int lookup(tecnicofs* fs, char* name)
{
    node* searchNode = search(fs->bstRoot, name);
    if (searchNode) {
        return searchNode->inumber;
    }
    return EXIT_SUCCESS;
}

/* Graphical (textual) representation of tecnicoFS BST is printed to a file */
void print_tecnicofs_tree(FILE* fp, tecnicofs* fs)
{
    print_tree(fp, fs->bstRoot);
}
