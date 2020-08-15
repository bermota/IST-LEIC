#include "fs.h"

/* Hash function. */
int hash(char *name, int n) {
  if (!name)
    return -1;
  return (int)name[0] % n;
}

/* Initializes and allocates memory for tecnicoFS. */
tecnicofs *new_tecnicofs() {
  tecnicofs *fs = safeMalloc(sizeof(tecnicofs));

  inode_table_init();
  fs->bstRoots = safeMalloc(numBuckets * sizeof(node *));
  fs->htLocks = safeMalloc(numBuckets * sizeof(LOCK_T));

  for (int i = 0; i < numBuckets; i++) {
    fs->bstRoots[i] = NULL;
    INIT_LOCK(&(fs->htLocks[i]));
  }

  return fs;
}

/* Frees all of tecnicoFS's related memory. */
void free_tecnicofs(tecnicofs *fs) {
  inode_table_destroy();
  for (int i = 0; i < numBuckets; i++) {
    free_tree(fs->bstRoots[i]);
    DESTROY_LOCK(&(fs->htLocks[i]));
  }

  free(fs->bstRoots);
  free(fs->htLocks);
  free(fs);
}

/* Creates and inserts a new node. Returns 0 on success and error code
 * otherwise. */
int create(tecnicofs *fs, char *name, uid_t owner, permission ownerPerm,
           permission othersPerm) {
  int inumber;
  int i = hash(name, numBuckets);
  WR_LOCK(&(fs->htLocks[i]));

  if (search(fs->bstRoots[i], name)) {
    UNLOCK(&(fs->htLocks[i]));
    return TECNICOFS_ERROR_FILE_ALREADY_EXISTS;
  }

  if ((inumber = inode_create(owner, ownerPerm, othersPerm)) < 0) {
    UNLOCK(&(fs->htLocks[i]));
    return TECNICOFS_ERROR_OTHER;
  }

  fs->bstRoots[i] = insert(fs->bstRoots[i], name, inumber);
  UNLOCK(&(fs->htLocks[i]));
  return EXIT_SUCCESS;
}

/* Removes and deletes a file with the given name. Returns 0 on success and
   error code otherwise. */
int delete (tecnicofs *fs, char *name, uid_t uid) {
  node *searchNode = NULL;
  int i = hash(name, numBuckets);
  uid_t owner;

  WR_LOCK(&(fs->htLocks[i]));

  if (!(searchNode = search(fs->bstRoots[i], name))) {
    UNLOCK(&(fs->htLocks[i]));
    return TECNICOFS_ERROR_FILE_NOT_FOUND;
  }

  if (inode_get(searchNode->inumber, &owner, NULL, NULL, NULL, 0) < 0) {
    UNLOCK(&(fs->htLocks[i]));
    return TECNICOFS_ERROR_OTHER;
  }

  if (owner != uid)
    return TECNICOFS_ERROR_PERMISSION_DENIED;

  if (inode_isOpen(searchNode->inumber)) {
    UNLOCK(&(fs->htLocks[i]));
    return TECNICOFS_ERROR_FILE_IS_OPEN;
  }

  if (inode_delete(searchNode->inumber) < 0)
    return TECNICOFS_ERROR_OTHER;

  fs->bstRoots[i] = remove_item(fs->bstRoots[i], name);

  UNLOCK(&(fs->htLocks[i]));

  return EXIT_SUCCESS;
}

/* Returns UNEXISTENT if node not found and its iNumber otherwise. */
int lookup(tecnicofs *fs, char *name) {
  int i = hash(name, numBuckets);
  node *searchNode = search(fs->bstRoots[i], name);

  return searchNode ? searchNode->inumber : UNEXISTENT;
}

/* Changes oldFileName to newFileName if possible. */
int editFileName(tecnicofs *fs, char *oldFileName, char *newFileName,
                 uid_t uid) {
  int i = hash(oldFileName, numBuckets);
  int j = hash(newFileName, numBuckets);
  int stop = 0;
  int trylock;
  node *file;
  uid_t owner;

  if (i != j) { // on different BST's.
    while (!stop) {
      WR_LOCK(&(fs->htLocks[i]));
      trylock = TRYLOCK(&(fs->htLocks[j]));
      checkError(trylock && trylock != EBUSY, "Error on trylock.\n");
      if (!trylock)
        stop = 1;
      else if (trylock == EBUSY) {
        UNLOCK(&(fs->htLocks[i]));
        usleep(((float)rand() / RAND_MAX) * 1000);
      }
    }

    if (!(file = search(fs->bstRoots[i], oldFileName))) {
      UNLOCK(&(fs->htLocks[i]));
      UNLOCK(&(fs->htLocks[j]));
      return TECNICOFS_ERROR_FILE_NOT_FOUND;
    }

    else if (search(fs->bstRoots[j], newFileName)) {
      UNLOCK(&(fs->htLocks[i]));
      UNLOCK(&(fs->htLocks[j]));
      return TECNICOFS_ERROR_FILE_ALREADY_EXISTS;
    }

    if (inode_get(file->inumber, &owner, NULL, NULL, NULL, 0) < 0) {
      UNLOCK(&(fs->htLocks[i]));
      UNLOCK(&(fs->htLocks[j]));
      return TECNICOFS_ERROR_OTHER;
    }

    if (owner != uid)
      return TECNICOFS_ERROR_PERMISSION_DENIED;

    else if (inode_isOpen(file->inumber)) {
      UNLOCK(&(fs->htLocks[i]));
      UNLOCK(&(fs->htLocks[j]));
      return TECNICOFS_ERROR_FILE_IS_OPEN;
    }

    fs->bstRoots[i] = remove_item(fs->bstRoots[i], oldFileName);
    fs->bstRoots[j] = insert(fs->bstRoots[j], newFileName, file->inumber);

    UNLOCK(&(fs->htLocks[i]));
    UNLOCK(&(fs->htLocks[j]));
  }

  else { // on the same BST.
    WR_LOCK(&(fs->htLocks[i]));

    if (!(file = search(fs->bstRoots[i], oldFileName))) {
      UNLOCK(&(fs->htLocks[i]));
      return TECNICOFS_ERROR_FILE_NOT_FOUND;
    }

    else if (search(fs->bstRoots[j], newFileName)) {
      UNLOCK(&(fs->htLocks[i]));
      return TECNICOFS_ERROR_FILE_ALREADY_EXISTS;
    }

    else if (inode_isOpen(file->inumber)) {
      UNLOCK(&(fs->htLocks[i]));
      return TECNICOFS_ERROR_FILE_IS_OPEN;
    }

    fs->bstRoots[i] = insert(fs->bstRoots[i], newFileName, file->inumber);
    fs->bstRoots[i] = remove_item(fs->bstRoots[i], oldFileName);

    UNLOCK(&(fs->htLocks[i]));
  }
  return EXIT_SUCCESS;
}

/* Returns 1 or 0 depending on the given arguments to check the permissions. */
int hasPermission(int isOwner, permission ownerPerm, permission othersPerm,
                  permission mode) {
  permission perm = (isOwner ? ownerPerm : othersPerm);
  return perm == RW ? 1 : perm == mode;
}

/* Opens file with the given mode. Returns 0 on success and error code
 * otherwise. */
int openFile(tecnicofs *fs, char *name, permission mode, uid_t uid) {
  int i = hash(name, numBuckets);
  int inumber;
  node *searchNode;
  permission ownerPerm, othersPerm;
  uid_t owner;
  WR_LOCK(&(fs->htLocks[i]));

  if ((searchNode = search(fs->bstRoots[i], name)) == NULL) {
    UNLOCK(&(fs->htLocks[i]));
    return TECNICOFS_ERROR_FILE_NOT_FOUND;
  }
  inumber = searchNode->inumber;

  if (inode_get(inumber, &owner, &ownerPerm, &othersPerm, NULL, 0) < 0) {
    UNLOCK(&(fs->htLocks[i]));
    return TECNICOFS_ERROR_OTHER;
  }

  if (!hasPermission(owner == uid, ownerPerm, othersPerm, mode)) {
    UNLOCK(&(fs->htLocks[i]));
    return TECNICOFS_ERROR_PERMISSION_DENIED;
  }

  inode_open(inumber);
  UNLOCK(&(fs->htLocks[i]));
  return EXIT_SUCCESS;
}

/* Returns 0 if the file with the given fd was successfully closed and an error
   otherwise. */
int closeFile(tecnicofs *fs, char *name) {
  int inumber;

  if (!name)
    return TECNICOFS_ERROR_FILE_NOT_OPEN;

  inumber = lookup(fs, name);

  if (inumber == -1)
    return TECNICOFS_ERROR_FILE_NOT_FOUND;

  if (!inode_isOpen(inumber))
    return TECNICOFS_ERROR_FILE_NOT_OPEN;

  inode_close(inumber);
  return EXIT_SUCCESS;
}

/* Writes buffer into the file with the given name. Returns 0 on success and
   error code otherwise. */
int fileWrite(tecnicofs *fs, char *name, char *buffer) {
  if (inode_set(lookup(fs, name), buffer, strlen(buffer)) < 0)
    return TECNICOFS_ERROR_OTHER;

  return EXIT_SUCCESS;
}

/* Reads from the file with the given name to buffer to a maximum of len chars.
   Returns 0 on success and error code otherwise. */
int readFile(tecnicofs *fs, char *name, int len, char *buffer) {
  int inumber = lookup(fs, name);

  if (inumber == UNEXISTENT)
    return TECNICOFS_ERROR_FILE_NOT_FOUND;

  if (inode_get(inumber, NULL, NULL, NULL, buffer, len - 1) < 0)
    return TECNICOFS_ERROR_OTHER;

  return strlen(buffer);
}

/* Graphical representation of tecnicoFS is printed to fp. */
void print_tecnicofs_tree(FILE *fp, tecnicofs *fs) {
  for (int i = 0; i < numBuckets; i++)
    print_tree(fp, fs->bstRoots[i]);
}
