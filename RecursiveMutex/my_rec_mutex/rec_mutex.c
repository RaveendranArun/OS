#include <pthread.h>
#include "rec_mutex.h"

int rec_mutex_init(rec_mutex_t* r_mtx)
{
    if (!r_mtx)
        return -1;
    
    r_mtx->locking_thread = 0;
    r_mtx->waiting_threads_count = 0;
    r_mtx->self_locks_count = 0;
    
    if (pthread_mutex_init(&r_mtx->state_mtx, NULL) != 0)
        return -1;
    
    if (pthread_cond_init(&r_mtx->cv, NULL) != 0) {
        pthread_mutex_destroy(&r_mtx->state_mtx);
        return -1;
    }
    
    return 0;
}

int rec_mutex_lock(rec_mutex_t *r_mtx)
{
    if (!r_mtx)
        return -1;
    
    if (pthread_mutex_lock(&r_mtx->state_mtx) != 0)
        return -1;

    pthread_t current_thread = pthread_self();

    /* Case 1: When r_mtx object is not already locked */
    if (r_mtx->self_locks_count == 0)
    {
        r_mtx->self_locks_count = 1;
        r_mtx->locking_thread = current_thread;
        pthread_mutex_unlock(&r_mtx->state_mtx);
        return 0;
    }

    /* Case 2: When r_mtx is locked by self already */
    if (r_mtx->locking_thread == current_thread)
    {
        r_mtx->self_locks_count++;
        pthread_mutex_unlock(&r_mtx->state_mtx);
        return 0;
    }

    /* Case 3: When this r_mtx object is locked by some other thread */
    while (r_mtx->locking_thread && r_mtx->locking_thread != current_thread)
    {
        r_mtx->waiting_threads_count++;
        if (pthread_cond_wait(&r_mtx->cv, &r_mtx->state_mtx) != 0) {
            r_mtx->waiting_threads_count--;
            pthread_mutex_unlock(&r_mtx->state_mtx);
            return -1;
        }
        r_mtx->waiting_threads_count--;
    }

    r_mtx->self_locks_count = 1;
    r_mtx->locking_thread = current_thread;
    if (pthread_mutex_unlock(&r_mtx->state_mtx) != 0)
        return -1;
    
    return 0;
}

int rec_mutex_unlock(rec_mutex_t* r_mtx)
{
    if (!r_mtx)
        return -1;
    
    if (pthread_mutex_lock(&r_mtx->state_mtx) != 0)
        return -1;

    pthread_t current_thread = pthread_self();

    /* Case 1: When r_mtx object is not already locked */
    if (r_mtx->self_locks_count == 0)
    {
        pthread_mutex_unlock(&r_mtx->state_mtx);
        return -1;  /* Error: not locked */
    }

    /* Case 2: When r_mtx is locked by self */
    if (r_mtx->locking_thread == current_thread)
    {
        r_mtx->self_locks_count--;
        if (r_mtx->self_locks_count > 0)
        {
            pthread_mutex_unlock(&r_mtx->state_mtx);
            return 0;
        }

        if (r_mtx->waiting_threads_count > 0)
        {
            pthread_cond_broadcast(&r_mtx->cv);
        }

        r_mtx->locking_thread = 0;
        if (pthread_mutex_unlock(&r_mtx->state_mtx) != 0)
            return -1;
        
        return 0;
    }

    /* Case 3: When this r_mtx object is locked by some other thread */
    pthread_mutex_unlock(&r_mtx->state_mtx);
    return -1;  /* Error: thread doesn't own lock */
}

int rec_mutex_destroy(rec_mutex_t *r_mtx)
{
    if (!r_mtx)
        return -1;
    
    /* Can only destroy if not locked */
    if (r_mtx->self_locks_count > 0)
        return -1;  /* Error: mutex is still locked */
    
    r_mtx->locking_thread = 0;
    r_mtx->waiting_threads_count = 0;
    r_mtx->self_locks_count = 0;
    
    if (pthread_mutex_destroy(&r_mtx->state_mtx) != 0)
        return -1;
    
    if (pthread_cond_destroy(&r_mtx->cv) != 0)
        return -1;
    
    return 0;
}
