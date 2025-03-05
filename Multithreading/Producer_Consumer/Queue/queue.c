#include <stdio.h>
#include <stdlib.h>
#include "queue.h"

Queue* createQueue(uint8_t capacity)
{
    Queue* q = (Queue* )malloc(sizeof(Queue));
    if (q == NULL)
    {
        printf("Queue creation failed\n");
        return NULL;
    }

    q->front = 0;
    q->rear = 0;
    q->capacity = capacity;
    q->buf = (int* )malloc(sizeof(int) * q->capacity);
    pthread_mutex_init(&q->queuMutex, NULL);
    pthread_cond_init(&q->queueCond, NULL);

    return q;
}

uint8_t isQueueFull(Queue* q)
{
    if ((q != NULL) && (q->size == q->capacity))
        return 1;
    else
        return 0;
}

uint8_t isQueueEmpty(Queue* q)
{
    if ((q != NULL) && (q->size == 0))
        return 1;
    else
        return 0;
}

void enqueue(Queue* q, int x)
{
    if (isQueueFull(q))
    {
        printf("Queue is full. Unable to insert new item\n");
        return;
    }

    q->buf[q->rear] = x;
    q->rear = (q->rear + 1) % q->capacity;
    q->size = q->size + 1;
}

int dequeue(Queue* q)
{
    if (isQueueEmpty(q))
    {
        printf("Queue is empty\n");
        return -1;
    }

    int item = q->buf[q->front];
    q->front = (q->front + 1) % q->capacity;
    q->size--;

    return item;
}