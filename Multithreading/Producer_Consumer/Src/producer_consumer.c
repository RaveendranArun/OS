#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>

#include "queue.h"

#define QUEUE_CAPACITY 5

Queue* q = NULL;

static const char* prod_1 = "TP1";
static const char* prod_2 = "TP2";
static const char* cons_1 = "TC1";
static const char* cons_2 = "TC2";

void* consumer(void* arg)
{
    const char* name = (char* )arg;
    
    printf("%s: Thread waiting to lock the queueu\n", name);
    pthread_mutex_lock(&q->queuMutex);
    printf("%s: Thread locked the queueu\n", name);

    // Check the predicate
    while (isQueueEmpty(q))
    {
        printf("%s: Thread blocks itself, Queue is already empty.\n", name);
        pthread_cond_wait(&q->queueCond, &q->queuMutex);
        printf("%s: Threads wakes up and checking the queue status again\n", name);
    }
 
    // start of C.S of consumer. Consumer must start of consuming elements from full queue only
    //assert(isQueueEmpty(q));
    printf("%s: Elements are available in the queue. Reading the elements\n", name);

    int x;
    while (!isQueueEmpty(q))
    {
        x =  dequeue(q);
        printf("%s: Thread consumes an integer %d, queue size: %d\n", name, x, q->size);
    }

    printf("%s: Thread drains the entire queue, sending signals to blocking threads.\n", name);
    pthread_cond_broadcast(&q->queueCond);

    printf("%s : Thread is releasing the lock\n", name);
    pthread_mutex_unlock(&q->queuMutex);

    printf("%s: Thread finished and exit\n", name);
    return NULL;
}

void* producer(void* arg)
{
    const char* name = (char* )arg;
    
    printf("%s: Thread waiting to lock the queueu\n", name);
    pthread_mutex_lock(&q->queuMutex);
    printf("%s: Thread locked the queueu\n", name);

    // Check the predicate: avoids spurious wakeup
    while (isQueueFull(q))
    {
        printf("%s: Thread blocks itself, Queue is already full.\n", name);
        pthread_cond_wait(&q->queueCond, &q->queuMutex);
        printf("%s: Threads wakes up and checking the queue status again\n", name);
    }
 
    // start of C.S of Producer. Producer must start of filling the items once the queue is empty
    //assert(isQueueFull(q));

    int x;
    while (!isQueueFull(q))
    {
        x =  rand() % 100;
        printf("%s: Thread produced a new integer %d\n", name, x);
        enqueue(q, x);
        printf("%s: Thread pushed the integer %d into the queue, queue size: %d\n", name, x, q->size);
    }

    printf("%s: Thread is filled up the queue, signalling and releasing the lock\n", name);
    pthread_cond_broadcast(&q->queueCond);
    pthread_mutex_unlock(&q->queuMutex);

    printf("%s: Thread finished and exit\n", name);
    return NULL;
}

int main()
{
    pthread_t prod_1_tid, prod_2_tid;
    pthread_t cons_1_tid, cons_2_tid;
    
    srand((unsigned int)(time(NULL) ^ getpid()));  // Mix time and process ID
    
    q = createQueue(QUEUE_CAPACITY);

    pthread_create(&prod_1_tid, NULL, producer, (void* )prod_1);
    pthread_create(&prod_2_tid, NULL, producer, (void* )prod_2);
    
    pthread_create(&cons_1_tid, NULL, consumer, (void* )cons_1);
    pthread_create(&cons_2_tid, NULL, consumer, (void* )cons_2);

    pthread_join(prod_1_tid, NULL);
    pthread_join(prod_2_tid, NULL);
    pthread_join(cons_1_tid, NULL);
    pthread_join(cons_2_tid, NULL);

    return 0;
}
