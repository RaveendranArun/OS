#include "sema.h"

#include <stdlib.h>

// Simple semaphore implementation using a mutex and condition variable.
// permit_counter tracks available permits. sema_wait() blocks until permits
// are available, and sema_post() releases a permit and wakes a waiting thread.
// The object returned by sema_get_new_semaphore() must be initialized with
// sema_init() before it is used.
sema_t* sema_get_new_semaphore()
{
    return (sema_t*)calloc(1, sizeof(sema_t));
}

// Initialize a semaphore with the given permit count.
// permit_counter is the initial number of available permits.
int sema_init(sema_t* sem, int permit_counter)
{
    if (!sem)
        return -1;

    sem->permit_counter = permit_counter;

    int rc = pthread_mutex_init(&sem->mutex, NULL);
    if (rc != 0)
        return rc;

    rc = pthread_cond_init(&sem->cond, NULL);
    if (rc != 0)
    {
        pthread_mutex_destroy(&sem->mutex);
        return rc;
    }

    return 0;
}

// Wait for a semaphore permit to become available.
// This function blocks until permit_counter > 0, then decrements it.
int sema_wait(sema_t* sem)
{
    if (!sem)
        return -1;

    int rc = pthread_mutex_lock(&sem->mutex);
    if (rc != 0)
        return rc;

    while (sem->permit_counter == 0)
    {
        rc = pthread_cond_wait(&sem->cond, &sem->mutex);
        if (rc != 0)
            break;
    }

    if (rc == 0)
        sem->permit_counter--;

    int unlock_rc = pthread_mutex_unlock(&sem->mutex);
    return rc != 0 ? rc : unlock_rc;
}

// Release a semaphore permit and notify one waiting thread.
int sema_post(sema_t* sem)
{
    if (!sem)
        return -1;

    int rc = pthread_mutex_lock(&sem->mutex);
    if (rc != 0)
        return rc;

    sem->permit_counter++;
    rc = pthread_cond_signal(&sem->cond);

    int unlock_rc = pthread_mutex_unlock(&sem->mutex);
    return rc != 0 ? rc : unlock_rc;
}

// Return the current permit count, safe under protection of the mutex.
int sema_get_value(sema_t* sem)
{
    if (!sem)
        return -1;

    int rc = pthread_mutex_lock(&sem->mutex);
    if (rc != 0)
        return -1;

    int value = sem->permit_counter;
    pthread_mutex_unlock(&sem->mutex);
    return value;
}

// Destroy the semaphore resources.
int sema_destroy(sema_t* sem)
{
    if (!sem)
        return -1;

    sem->permit_counter = 0;
    pthread_mutex_destroy(&sem->mutex);
    pthread_cond_destroy(&sem->cond);
    return 0;
}

