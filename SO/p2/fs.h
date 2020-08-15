#ifndef FS_H
#define FS_H
#include "lib/bst.h"
#include "sync.h"

int numberTrees;

typedef struct tecnicofs {
    node **bstRoots;
    int nextINumber;
    LOCK_T *htLocks;
} tecnicofs;

int obtainNewInumber(tecnicofs* fs);
tecnicofs* new_tecnicofs();
void free_tecnicofs(tecnicofs* fs);
void create(tecnicofs* fs, char *name, int inumber);
void delete(tecnicofs* fs, char *name);
int lookup(tecnicofs* fs, char *name);
int editFileName(tecnicofs* fs, char *name, char *newName);
void print_tecnicofs_tree(FILE * fp, tecnicofs *fs);
int hash(char* name, int n);

#endif /* FS_H */
