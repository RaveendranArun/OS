#ifndef __REC_MUTEX__
#define __REC_MUTEX__

#include <pthread.h>
#include <stdint.h>

typedef struct rec_mutex
{
    /* No of self-locks taken */
    uint16_t self_locks_count;
    /* A Mutex to manipulate the state of this structure in a mutually exclusive way */
    pthread_mutex_t state_mtx;
    /* No of threads waiting for this Mutex lock Grant */
    uint16_t waiting_threads_count;
    /* pthread id of the thread which owns this mutex */
    pthread_t locking_thread;
    /* A CV to make the threads block */
    pthread_cond_t cv;
} rec_mutex_t;

/* Returns 0 on success, -1 on failure */
int rec_mutex_init(rec_mutex_t* r_mtx);

/* Returns 0 on success, -1 on failure */
int rec_mutex_lock(rec_mutex_t* r_mtx);

/* Returns 0 on success, -1 if thread doesn't own lock or on error */
int rec_mutex_unlock(rec_mutex_t* r_mtx);

/* Returns 0 on success, -1 on failure */
int rec_mutex_destroy(rec_mutex_t* r_mtx);

#endif