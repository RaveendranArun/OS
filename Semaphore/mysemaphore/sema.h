#ifndef __SEMA_H__
#define __SEMA_H__

#include <pthread.h>
#include <stdint.h>

// Simple binary/counting semaphore implementation using pthread mutex and cond.
typedef struct sema
{
    int32_t permit_counter;   // current number of available permits
    pthread_mutex_t mutex;    // protects access to permit_counter
    pthread_cond_t cond;      // signals when a permit becomes available
} sema_t;

// Allocate a new semaphore object. Must be initialized with sema_init().
sema_t* sema_get_new_semaphore();

// Initialize semaphore with an initial permit count.
int sema_init(sema_t* sem, int permit_counter_val);

// Acquire one permit. Blocks until a permit is available.
int sema_wait(sema_t* sem);

// Release one permit and wake one waiting thread.
int sema_post(sema_t* sem);

// Destroy semaphore mutex and condition variable.
int sema_destroy(sema_t* sem);

// Return the current permit count.
int sema_get_value(sema_t* sem);

#endif // __SEMA_H__

