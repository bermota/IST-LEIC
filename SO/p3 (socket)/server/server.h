#ifndef SERVER_H
#define SERVER_H

#include "../constants.h"
#include "../tecnicofs-api-constants.h"
#include "fs.h"
#include "sync.h"
#include <errno.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>

/* Type representing an open file. */
typedef struct {
  char *fileName;
  permission mode;
} file_open_t;

/* Type representing the table containing open files. */
typedef struct {
  file_open_t **table;
  int numberOfFilesOpen;
} file_table_t;

file_table_t *new_fileTable();
int addFile(file_table_t *t, permission mode, char *name);
int removeFile(file_table_t *t, int fd);
void free_fileTable(file_table_t *t);
int fileOpenMode(file_table_t *ft, int fd, permission mode);

#endif // SERVER_H