#include "multithreading.h"
#include <pthread.h>

/* Runs fn on a positive number of threads.
 * Given function MT-safe. */
void threadPool(int numberThreads, void* (*fn)(void*))
{
    int i, length;

    /* Forces a single thread when sequential */
    length = MULTITHREADED ? numberThreads : 1;

    pthread_t* tid = malloc(length * sizeof(pthread_t));

    CHECK_ERROR(!tid, "Could not allocate memory.\n");

    for (i = 0; i < length; i++)
        pthread_create(&tid[i], 0, (void*)fn, NULL);

    for (i = 0; i < length; i++)
        CHECK_ERROR(pthread_join(tid[i], NULL), "Error waiting for task.\n");

    free(tid);
}