#include "sema.h"

#include <stdbool.h>
#include <stdlib.h>

sema_t* sema_get_new_semaphore()
{
    sema_t* sem = (sema_t* )malloc(sizeof(sema_t));
    if (sem)
        return sem;
    else
        return NULL;
}

int sema_init(sema_t* sem, int permit_counter)
{
    sem->permit_counter = permit_counter;
    pthread_mutex_init(&sem->mutex, NULL);
    pthread_cond_init(&sem->cond, NULL);
}

int sema_wait(sema_t* sem)
{
    pthread_mutex_lock(&sem->mutex);
    sem->permit_counter--;
    if (sem->permit_counter < 0)
    {
        pthread_cond_wait(&sem->cond, &sem->mutex);
    }
    pthread_mutex_unlock(&sem->mutex);
}

int sema_post(sema_t* sem)
{
    bool any_thread_waiting;
    
    pthread_mutex_lock(&sem->mutex);
    any_thread_waiting = sem->permit_counter < 0 ? true : false;
    sem->permit_counter++;
    if (any_thread_waiting)
    {
        pthread_cond_signal(&sem->cond);
    }
    pthread_mutex_unlock(&sem->mutex);
}

int sema_destroy(sema_t* sem)
{
    sem->permit_counter = 0;
    pthread_mutex_unlock(&sem->mutex);
    pthread_mutex_destroy(&sem->mutex);
    pthread_cond_destroy(&sem->cond);
}

