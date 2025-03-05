#ifndef __QUEUE_H__
#define __QUEUE_H__

#include <stdint.h>
#include <pthread.h>

#define  BUFFER_SIZE 5

typedef struct Queue
{   
    uint8_t front;
    uint8_t rear;
    uint8_t size;
    uint8_t capacity;
    int* buf;
    pthread_mutex_t queuMutex;
    pthread_cond_t queueCond;
}Queue;

Queue* createQueue(uint8_t capacity);
void enqueue(Queue* q, int32_t x);
int dequeue(Queue* q);
uint8_t isQueueEmpty(Queue* q);
uint8_t isQueueFull(Queue* q);

#endif // __QUEUE_H__