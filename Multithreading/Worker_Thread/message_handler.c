#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define NUM_WORKERS   4
#define BUFFER_SIZE   10

// A single request stores a handler and the payload for that handler.
typedef struct
{
    void (*handler)(void*, void*);
    void* data;
} Request_t;

// RequestQueue implements a fixed-size circular buffer for pending requests.
// It also carries the synchronization primitives used by producers and workers.
typedef struct RequestQueue
{
    Request_t request[BUFFER_SIZE];
    int front;               // index of the next request to consume
    int rear;                // index where the next request will be inserted
    int count;               // number of requests currently in the queue
    sem_t sem;               // counts available requests for workers
    pthread_mutex_t mutex;   // protects queue state during enqueue/dequeue
} RequestQueue;

RequestQueue* queue = NULL;
pthread_t worker_id[NUM_WORKERS];

static char* WT[NUM_WORKERS] = {"WT_1", "WT_2", "WT_3", "WT_4"};

// Allocate the request queue control structure.
RequestQueue* create_request_queue()
{
    RequestQueue* q = malloc(sizeof(RequestQueue));
    if (!q)
        return NULL;
    return q;
}

// Initialize queue indices, counter, semaphore and mutex.
void init_queue(RequestQueue* q)
{
    q->count = 0;
    q->front = 0;
    q->rear = 0;
    sem_init(&q->sem, 0, 0);          // start with zero available requests
    pthread_mutex_init(&q->mutex, NULL); // default mutex attributes
}

// Example request handler. In a real system this could be a message parser, logger, etc.
void process_message(void *data, void* name)
{
    printf("Invoked by %s. Processing message: %s\n", (char*)name, (char*)data);
    sleep(1); // simulate work done by the handler
}

// Worker thread loop: wait for a request, dequeue it, then process it.
void* worker_thread(void* arg)
{
    char* name = (char*)arg;

    while (1)
    {
        // Block until at least one request is available.
        sem_wait(&queue->sem);

        // Remove a request from the queue under mutex protection.
        pthread_mutex_lock(&queue->mutex);

        Request_t req = queue->request[queue->front];
        queue->front = (queue->front + 1) % BUFFER_SIZE;
        queue->count--;

        pthread_mutex_unlock(&queue->mutex);

        // Execute the stored handler outside the critical section.
        req.handler(req.data, name);
    }

    return NULL;
}

// Create the pool of worker threads.
void create_workers()
{
    for (int i = 0; i < NUM_WORKERS; ++i)
    {
        pthread_create(&worker_id[i], NULL, worker_thread, (void*)WT[i]);
    }
}

// Enqueue a new request for the worker threads.
void submit_request(void* msg, void (*handler)(void*, void*))
{
    pthread_mutex_lock(&queue->mutex);

    if (queue->count == BUFFER_SIZE)
    {
        // Do not drop the mutex when the queue is full.
        printf("Queue full! Dropping message\n");
        pthread_mutex_unlock(&queue->mutex);
        return;
    }

    queue->request[queue->rear].data = msg;
    queue->request[queue->rear].handler = handler;
    queue->rear = (queue->rear + 1) % BUFFER_SIZE;
    queue->count++;

    pthread_mutex_unlock(&queue->mutex);

    // Notify a waiting worker that a request is ready.
    sem_post(&queue->sem);
}

// Simulate an ISR-like producer that receives a message and enqueues it.
void simulated_isr(char* msg)
{
    printf("ISR: Message received: %s\n", msg);
    submit_request(msg, process_message);
}

int main()
{
    // Allocate and initialize the shared request queue.
    queue = create_request_queue();
    init_queue(queue);

    // Start worker threads that will process requests.
    create_workers();

    // Simulate repeated interrupt-driven message arrivals.
    char *messages[] = {"Msg1", "Msg2", "Msg3", "Msg4", "Msg5", "Msg6", "Msg7", "Msg8"};
    int i = 0;
    size_t size = sizeof(messages)/sizeof(messages[0]);

    while (1)
    {
        simulated_isr(messages[i]);
        i = (i + 1) % size;
        usleep(500000);  // pause to simulate time between interrupts
    }

    // The program never reaches this point in the current design,
    // but a clean shutdown would join workers and destroy resources.
    for (int i = 0; i < NUM_WORKERS; i++)
    {
        pthread_join(worker_id[i], NULL);
    }

    sem_destroy(&queue->sem);
    return 0;
}
