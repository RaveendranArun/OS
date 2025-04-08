#ifndef __SEMA_H__
#define __SEMA_H__

#include <pthread.h>
#include <stdint.h>

typedef struct sema
{
    int32_t permit_counter;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
}sema_t;

sema_t* sema_get_new_semaphore();
int sema_init(sema_t* sem, int permit_counter_val);
int sema_wait(sema_t* sem);
int sema_post(sema_t* sem);
int sema_destroy(sema_t* sem);
int sema_get_value(sema_t* sem);

#endif // __SEMA_H__
    
