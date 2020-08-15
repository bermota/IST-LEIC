#ifndef FS_H
#define FS_H
#include "../constants.h"
#include "../tecnicofs-api-constants.h"
#include "lib/bst.h"
#include "lib/inodes.h"
#include "sync.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

int numBuckets;

#define MAX_FILES_OPEN 5
#define MAX_FILENAME_LEN 1023

typedef struct tecnicofs {
  node **bstRoots;
  LOCK_T *htLocks;
} tecnicofs;

tecnicofs *new_tecnicofs();
void free_tecnicofs(tecnicofs *fs);
int create(tecnicofs *fs, char *name, uid_t owner, permission ownerPerm,
           permission othersPerm);
int delete (tecnicofs *fs, char *name, uid_t uid);
int lookup(tecnicofs *fs, char *name);
int editFileName(tecnicofs *fs, char *name, char *newName, uid_t uid);
int openFile(tecnicofs *fs, char *name, permission mode, uid_t uid);
int closeFile(tecnicofs *fs, char *name);
int fileWrite(tecnicofs *fs, char *name, char *buffer);
int readFile(tecnicofs *fs, char *name, int len, char *buffer);
void print_tecnicofs_tree(FILE *fp, tecnicofs *fs);

#endif /* FS_H */
