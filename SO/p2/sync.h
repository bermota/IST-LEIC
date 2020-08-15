#ifndef SYNC_H
#define SYNC_H
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>
#include <string.h>
#include <errno.h>

void runThreads(int numberThreads, void* (fn)(void*));
int checkError(int condition, char *message);
void *safeMalloc(size_t size);
int doNothing(void *x);

/* Particular case of checkError for pthread errors. */
#define PTHREAD_ERROR		"Fatal pthread lock error. Terminating.\n"
#define SAFE_PTHREAD(X)     checkError(X, PTHREAD_ERROR)

/* Particular case of checkError for semaphores. */
#define SEMAPHORE_ERROR     "Fatal semaphore error. Terminating.\n"
#define SAFE_SEM(X)         checkError(X, SEMAPHORE_ERROR)

#define SEM_INIT(X, Y, Z)   SAFE_SEM(sem_init(X, Y, Z))
#define WAIT(X)             SAFE_SEM(sem_wait(X))
#define POST(X)             SAFE_SEM(sem_post(X))
#define SEM_DESTROY(X)      SAFE_SEM(sem_destroy(X))


#ifdef MUTEX

#define MULTITHREADED       1 /* Program runs in multithreading. */
#define LOCK_T 			    pthread_mutex_t
#define INIT_LOCK(X) 		SAFE_PTHREAD(pthread_mutex_init(X, NULL))
#define RD_LOCK(X) 		    SAFE_PTHREAD(pthread_mutex_lock(X))
#define WR_LOCK(X) 		    SAFE_PTHREAD(pthread_mutex_lock(X))
#define UNLOCK(X) 		    SAFE_PTHREAD(pthread_mutex_unlock(X))
#define DESTROY_LOCK(X) 	SAFE_PTHREAD(pthread_mutex_destroy(X))
#define TRYLOCK(X)          pthread_mutex_trylock(X)

#elif RWLOCK

#define MULTITHREADED       1 /* Program runs in multithreading. */
#define LOCK_T 			    pthread_rwlock_t
#define INIT_LOCK(X) 		SAFE_PTHREAD(pthread_rwlock_init(X, NULL))
#define RD_LOCK(X) 		    SAFE_PTHREAD(pthread_rwlock_rdlock(X))
#define WR_LOCK(X) 		    SAFE_PTHREAD(pthread_rwlock_wrlock(X))
#define UNLOCK(X) 		    SAFE_PTHREAD(pthread_rwlock_unlock(X))
#define DESTROY_LOCK(X) 	SAFE_PTHREAD(pthread_rwlock_destroy(X))
#define TRYLOCK(X)          pthread_rwlock_trywrlock(X)

#else

#define MULTITHREADED 		0 /* Program runs sequentially. */
#define LOCK_T 			    void*
#define INIT_LOCK(X) 		doNothing(X)
#define RD_LOCK(X) 		    doNothing(X)
#define WR_LOCK(X) 		    doNothing(X)
#define UNLOCK(X) 		    doNothing(X)
#define DESTROY_LOCK(X) 	doNothing(X)
#define TRYLOCK(X)          doNothing(X)

#endif
#endif /* SYNC_H */

