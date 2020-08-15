#include "sync.h"
#include <stdio.h>
#include <stdlib.h>

/* Allocates memory according to the given size in a safe way. */
void *safeMalloc(size_t size) {
  void *res = malloc(size);
  if (!res) {
    perror("Could not allocate memory.\n");
    exit(EXIT_FAILURE);
  }

  return res;
}

/* If condition is not 0 prints message. */
int checkError(int condition, char *message) {
  if (condition) {
    perror(message);
    exit(EXIT_FAILURE);
  }
  return EXIT_SUCCESS;
}

/* Does nothing. */
int doNothing(void *x) {
  (void)x;
  return 0;
}
