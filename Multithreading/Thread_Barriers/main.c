#include <stdio.h>
#include "thread_barrier.h"

thread_barrier_t barrier;
pthread_t threads[3];

void* callback_fn(void* arg)
{
    thread_barrier_wait(&barrier);
    printf("1st barrier crossed by the thread: %s\n", (char* )arg);

    thread_barrier_wait(&barrier);
    printf("2nd barrier crossed by the thread: %s\n", (char* )arg);

    thread_barrier_wait(&barrier);
    printf("3rd barrier crossed by the thread: %s\n", (char* )arg);

    return NULL;
}

int main()
{
    thread_barrier_init(&barrier, 3);

    static const char* th1 = "th1"; 
    pthread_create(&threads[0], NULL, callback_fn, (void*)th1);
 
    static const char* th2 = "th2"; 
    pthread_create(&threads[0], NULL, callback_fn, (void*)th2);

    static const char* th3 = "th3"; 
    pthread_create(&threads[0], NULL, callback_fn, (void*)th3);

    pthread_join(threads[0], NULL);
    pthread_join(threads[1], NULL);
    pthread_join(threads[2], NULL);

    return 0;
}