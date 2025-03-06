#include "thread_barrier.h"

// Initialize the thread barrier
void thread_barrier_init(thread_barrier_t* barrier, uint32_t threshold)
{
    barrier->threshold_count = threshold;            // Set the number of threads that must reach the barrier
    barrier->curr_wait_count = 0;                    // Initialize the waiting thread count to 0
    pthread_cond_init(&barrier->cv, NULL);           // Initialize the condition variable for synchronization
    pthread_mutex_init(&barrier->mutex, NULL);       // Initialize the mutex for thread synchronization
    barrier->is_ready_again = true;                  // Flag to indicate if the barrier is ready for reuse
    pthread_cond_init(&barrier->busy_cv, NULL);      // Condition variable to prevent premature entry
}

// Wait at the barrier until all threads reach it
void thread_barrier_wait(thread_barrier_t* barrier)
{
    pthread_mutex_lock(&barrier->mutex);            // Lock the mutex before modifying shared data

    // Wait until the barrier is ready to be used again
    while (barrier->is_ready_again == false)
    {
        pthread_cond_wait(&barrier->busy_cv, &barrier->mutex);
    }

    // If this is the last thread needed to reach the threshold, wake all waiting threads
    if (barrier->curr_wait_count + 1 == barrier->threshold_count)
    {
        barrier->is_ready_again = false;            // Mark the barrier as not ready for new threads
        pthread_cond_signal(&barrier->cv);          // Wake one waiting thread
        pthread_mutex_unlock(&barrier->mutex);      // Unlock and return
        return;
    }

    barrier->curr_wait_count++;                     // Increment the count of waiting threads
    pthread_cond_wait(&barrier->cv, &barrier->mutex); // Wait until signaled

    barrier->curr_wait_count--;                     // Decrement the count as the thread proceeds

    // If all threads have left the barrier, reset for the next cycle
    if (barrier->curr_wait_count == 0)
    {
        barrier->is_ready_again = true;            // Reset the barrier for the next use
        pthread_cond_broadcast(&barrier->busy_cv); // Wake all threads waiting to enter
    }
    else
    {
        pthread_cond_signal(&barrier->cv);         // Wake the next waiting thread
    }

    pthread_mutex_unlock(&barrier->mutex);         // Unlock the mutex before exiting
}

// Destroy the thread barrier and clean up resources
void thread_barrier_destroy(thread_barrier_t* barrier)
{
    barrier->threshold_count = 0;
    barrier->curr_wait_count = 0;
    pthread_cond_destroy(&barrier->cv);           // Destroy the condition variable
    pthread_mutex_destroy(&barrier->mutex);       // Destroy the mutex
    barrier->is_ready_again = true;               // Reset the flag
    pthread_cond_destroy(&barrier->busy_cv);      // Destroy the second condition variable
}
