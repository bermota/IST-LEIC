#ifndef FS_H
#define FS_H
#include "lib/bst.h"

/* tecnicoFS is a BST representing file names in a directory. */
typedef struct tecnicofs {
    node* bstRoot; /* Pointer to root node of tecnicoFS's BST. */
    int nextINumber; /* iNumber is an ID unique to each file. */
} tecnicofs;

int obtainNewInumber(tecnicofs* fs);
tecnicofs* new_tecnicofs();
void free_tecnicofs(tecnicofs* fs);
void create(tecnicofs* fs, char* name, int inumber);
void delete (tecnicofs* fs, char* name);
int lookup(tecnicofs* fs, char* name);
void print_tecnicofs_tree(FILE* fp, tecnicofs* fs);

#endif
