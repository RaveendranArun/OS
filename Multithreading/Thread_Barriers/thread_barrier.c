#include "thread_barrier.h"

void thread_barrier_init(thread_barrier_t* barrier, uint32_t threshold)
{
    barrier->threshold_count = threshold;
    barrier->curr_wait_count = 0;
    pthread_cond_init(&barrier->cv, NULL);
    pthread_mutex_init(&barrier->mutex, NULL);
    barrier->is_ready_again = true;
    pthread_cond_init(&barrier->busy_cv, NULL);
}

void thread_barrier_wait(thread_barrier_t* barrier)
{
    pthread_mutex_lock(&barrier->mutex);

    while (barrier->is_ready_again == false)
    {
        pthread_cond_wait(&barrier->busy_cv, &barrier->mutex);
    }

    if (barrier->curr_wait_count + 1 == barrier->threshold_count)
    {
        barrier->is_ready_again = false;
        pthread_cond_signal(&barrier->cv);
        pthread_mutex_unlock(&barrier->mutex);
        return;
    }

    barrier->curr_wait_count++;
    pthread_cond_wait(&barrier->cv, &barrier->mutex);
    barrier->curr_wait_count--;
    
    if (barrier->curr_wait_count == 0)
    {
        barrier->is_ready_again = true;
        pthread_cond_broadcast(&barrier->busy_cv);
    }
    else
    {
        pthread_cond_signal(&barrier->cv);
    }

    pthread_mutex_unlock(&barrier->mutex);
}

void thread_barrier_destroy(thread_barrier_t* barrier)
{
    barrier->threshold_count = 0;
    barrier->curr_wait_count = 0;
    pthread_cond_destroy(&barrier->cv);
    pthread_mutex_destroy(&barrier->mutex);
    barrier->is_ready_again = true;
    pthread_cond_destroy(&barrier->busy_cv);
}
