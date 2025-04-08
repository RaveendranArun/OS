#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdint.h>

#define MAX_COUNT 100

pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cv = PTHREAD_COND_INITIALIZER;

int count = 1;

void* odd_thread(void* arg)
{
    while (count < MAX_COUNT)
    {
        pthread_mutex_lock(&mtx);
        while (!(count & 1))
            pthread_cond_wait(&cv, &mtx);
        
        printf("%d ", count++);
        pthread_cond_signal(&cv);
        pthread_mutex_unlock(&mtx);
    }

    return NULL;
}

void* even_thread(void* arg)
{
    while (count < MAX_COUNT)
    {
        pthread_mutex_lock(&mtx);
        while (count & 1)
            pthread_cond_wait(&cv, &mtx);
        
        printf("%d ", count++);
        pthread_cond_signal(&cv);
        pthread_mutex_unlock(&mtx);
    }

    return NULL;
}

int main()
{
    pthread_t odd_id, even_id;

    pthread_create(&odd_id, NULL, odd_thread, NULL);
    pthread_create(&even_id, NULL, even_thread, NULL);

    pthread_join(odd_id, NULL);
    pthread_join(even_id, NULL);


    return 0;
}
