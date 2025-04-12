#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define THREAD_COUNT  3
#define MAX_COUNT     100

pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cv = PTHREAD_COND_INITIALIZER;

int count = 1;
int turn = 0;

void* thread(void* arg)
{
    int thread_turn = *(int* )arg;
    
    while (1)
    {
        pthread_mutex_lock(&mtx);
        while (turn != thread_turn)
        {
            pthread_cond_wait(&cv, &mtx);
        }

        printf("thread_%d: %d\n", thread_turn, count++);
        turn = (turn + 1) % THREAD_COUNT;
        if (count > THREAD_COUNT)
            count = 1;

        pthread_mutex_unlock(&mtx);
        pthread_cond_broadcast(&cv);
    }

    return NULL;
}

int main()
{
    pthread_t tid[THREAD_COUNT];
    int thread_turn[THREAD_COUNT];

    for (int i = 0 ; i < THREAD_COUNT; ++i)
    {
        thread_turn[i] = i;
    }

    for (int i = 0; i < THREAD_COUNT; ++i)
    {
        pthread_create(&tid[i], NULL, thread, (void* )&thread_turn[i]);
    }

    for (int i = 0; i < THREAD_COUNT; ++i)
    {
        pthread_join(tid[i], NULL);
    }

    return 0;

}
