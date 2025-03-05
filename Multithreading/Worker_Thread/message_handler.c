#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define NUM_WORKERS   4
#define BUFFER_SIZE   10

typedef struct
{
    void (*handler)(void*, void* );
    void* data;
} Request_t;

typedef struct RequestQueue
{
    Request_t request[BUFFER_SIZE];
    int front;
    int rear;
    int count;
    sem_t sem;
    pthread_mutex_t mutex;
} RequestQueue;

RequestQueue* queue = NULL;
pthread_t worker_id[NUM_WORKERS];

static char* WT[4] = {"WT_1", "WT_2", "WT_3", "WT_4"};

RequestQueue* create_request_queue()
{
    RequestQueue* q = malloc(sizeof(RequestQueue));
    if (!q)
        return NULL;
    else
        return q;
}

void init_queue(RequestQueue* q)
{
    q->count = 0;
    q->front = 0;
    q->rear = 0;
    sem_init(&q->sem, 0, 0);
    pthread_mutex_init(&q->mutex, NULL);
}

// Message handler example
void process_message(void *data, void* name) 
{
    printf("Invoked by %s. Processing message: %s\n", (char* )name, (char *)data);
    sleep(1);
}

void* worker_thread(void* arg)
{
    char* name = (char*)arg;
    while (1)
    {
        /* code */
        sem_wait(&queue->sem);

        pthread_mutex_lock(&queue->mutex);

        Request_t req = queue->request[queue->front];
        
        queue->front = (queue->front + 1) %BUFFER_SIZE;
        queue->count--;

        pthread_mutex_unlock(&queue->mutex);

        req.handler(req.data, name);
    }

    return NULL;
    
}

void create_workers()
{
    for (int i = 0; i < NUM_WORKERS; ++i)
    {
        pthread_create(&worker_id[i], NULL, worker_thread, (void*)WT[i]);
    }
}

void submit_request(void* msg, void (*handler)(void*, void*))
{
    pthread_mutex_lock(&queue->mutex);

    if (queue->count == BUFFER_SIZE)
    {
        printf("Queue full! Dropping message\n");
        return;
    }
    
    queue->request[queue->rear].data = msg;
    queue->request[queue->rear].handler = process_message;
    queue->rear = (queue->rear + 1) % BUFFER_SIZE;
    queue->count++;

    pthread_mutex_unlock(&queue->mutex);
    sem_post(&queue->sem);
}

void simulated_isr(char* msg)
{
    printf("ISR: Message received: %s\n", msg);
    submit_request(msg, process_message);
}

int main()
{
    //1. create queue
    queue =  create_request_queue();

    //2. init queue
    init_queue(queue);

    //3. Create worker threads
    create_workers();

     // Simulate DMA interrupts triggering ISR
     char *messages[] = {"Msg1", "Msg2", "Msg3", "Msg4", "Msg5", "Msg6", "Msg7", "Msg8"};
     int i = 0;
     size_t size = sizeof(messages)/sizeof(messages[0]);
     while (1)
     {
         simulated_isr(messages[i]);
         i = (i+1) % size;
         usleep(500000);  // Simulating time gap between interrupts
     }

    for (int i = 0; i < NUM_WORKERS; i++) 
    {
        pthread_join(worker_id[i], NULL);
    }

    // Destroy semaphore
    sem_destroy(&queue->sem);

    return 0;
}
