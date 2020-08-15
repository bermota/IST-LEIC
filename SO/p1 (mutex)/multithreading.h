#ifndef MULTITHREADING_H
#define MULTITHREADING_H

#include <ctype.h>
#include <getopt.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

void threadPool(int numThreads, void* (*start_routine)(void*));

/* Prints error message Y in case X is not 0. */
#define CHECK_ERROR(X, Y)   \
    if (X) {                \
        perror(Y);          \
        exit(EXIT_FAILURE); \
    }

/* Particular case of CHECK_ERROR for pthread errors. */
#define SAFE_FN(X) CHECK_ERROR(X, "Fatal pthread lock error. Terminating.\n")

#ifdef MUTEX

#define MULTITHREADED 1 /* Program runs in multithreading. */
#define DECLARE_LOCK(X) pthread_mutex_t X
#define INIT_LOCK(X) SAFE_FN(pthread_mutex_init(&X, NULL))
#define RD_LOCK(X) SAFE_FN(pthread_mutex_lock(&X))
#define WR_LOCK(X) SAFE_FN(pthread_mutex_lock(&X))
#define UNLOCK(X) SAFE_FN(pthread_mutex_unlock(&X))
#define DESTROY_LOCK(X) SAFE_FN(pthread_mutex_destroy(&X))

#elif RWLOCK

#define MULTITHREADED 1 /* Program runs in multithreading. */
#define DECLARE_LOCK(X) pthread_rwlock_t X
#define INIT_LOCK(X) SAFE_FN(pthread_rwlock_init(&X, NULL))
#define RD_LOCK(X) SAFE_FN(pthread_rwlock_rdlock(&X))
#define WR_LOCK(X) SAFE_FN(pthread_rwlock_wrlock(&X))
#define UNLOCK(X) SAFE_FN(pthread_rwlock_unlock(&X))
#define DESTROY_LOCK(X) SAFE_FN(pthread_rwlock_destroy(&X))

#else

#define MULTITHREADED 0 /* Program runs sequentially. */
#define DECLARE_LOCK(X)
#define INIT_LOCK(X)
#define RD_LOCK(X)
#define WR_LOCK(X)
#define UNLOCK(X)
#define DESTROY_LOCK(X)

#endif

#endif