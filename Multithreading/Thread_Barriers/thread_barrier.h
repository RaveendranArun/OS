#ifndef __THREAD_BARRIER_H__
#define __THREAD_BARRIER_H__

#include <stdint.h>
#include <stdbool.h>
#include <pthread.h>

typedef struct 
{
    uint32_t threshold_count;
    uint32_t curr_wait_count;
    pthread_cond_t cv;
    pthread_mutex_t mutex;
    bool is_ready_again;
    pthread_cond_t busy_cv;
} thread_barrier_t;

void thread_barrier_init(thread_barrier_t* barrier, uint32_t threshold);

void thread_barrier_wait(thread_barrier_t* barrier);

void thread_barrier_destroy(thread_barrier_t* barrier);

#endif // __THREAD_BARRIER_H__