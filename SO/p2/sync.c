#include "sync.h"
#include <stdio.h>
#include <stdlib.h>

/* Allocates memory according to the given size in a safe way. */
void *safeMalloc(size_t size)
{
    void *res = malloc(size);
    if (!res) {
        perror("Could not allocate memory.\n");
        exit(EXIT_FAILURE);
    }

    return res;
}


/* Runs fn on a positive number of threads.
 * Given function MT-safe. */
void runThreads(int numberThreads, void* (fn)(void*))
{
    int i, length;

    // Forces a single thread when sequential.
    length = MULTITHREADED ? numberThreads : 1;

    pthread_t* tid = safeMalloc(length * sizeof(pthread_t));

    for (i = 0; i < length; i++)
        checkError(pthread_create(&tid[i], 0, fn, NULL), "Error.\n");

    for (i = 0; i < length; i++)
        checkError(pthread_join(tid[i], NULL), "Error waiting for task.\n");

    free(tid);
}

/* If condition is not 0 prints message. */
int checkError(int condition, char *message){
    if (condition){
        perror(message);
        exit(EXIT_FAILURE);
    }
    return EXIT_SUCCESS;
}

/* Does nothing. */
int doNothing(void* x){
    (void)x;
    return 0;
}
