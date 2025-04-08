#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include <time.h>

#define BUFFER_SIZE     16
#define CHUNK_SIZE      4
#define TOTAL_DATA_SIZE 64
sem_t data_available;

typedef struct 
{
    uint8_t         buffer[BUFFER_SIZE];
    uint16_t        write_index;
    uint16_t        read_index;
    pthread_mutex_t mtx;
}Queue_t;

Queue_t queue;

static void generate_random_data(uint8_t* data, uint8_t len)
{
    srand(time(NULL));

    for (uint8_t iter = 0; iter < len; ++iter)
    {
        data[iter] = rand() % 0xFF;
        printf("0x%x ", data[iter]);
    }
}

void* dma_thread(void* arg)
{
    uint8_t sample_data[TOTAL_DATA_SIZE];
    generate_random_data(sample_data, TOTAL_DATA_SIZE);
    uint8_t offset = 0;
    uint8_t data_len = TOTAL_DATA_SIZE;

    while (offset < data_len)
    {
        pthread_mutex_lock(&queue.mtx);
        for (uint8_t iter = 0; iter < CHUNK_SIZE && offset < data_len; ++iter)
        {
            queue.buffer[queue.write_index] = sample_data[offset++];
            queue.write_index = (queue.write_index + 1) % BUFFER_SIZE;  
        }
    
        pthread_mutex_unlock(&queue.mtx);
        printf("Signaling the CPU thread\n");
        sem_post(&data_available);

        sleep(1);
    }

    return NULL;
}

void* cpu_thread(void* arg)
{
    volatile uint8_t data[CHUNK_SIZE];

    while (1)
    {
        printf("Wait for new data\n");
        sem_wait(&data_available);

        pthread_mutex_lock(&queue.mtx);
        printf("Received the new chunk\n"); 
        for (uint8_t iter = 0; iter < CHUNK_SIZE; ++iter)
        {
            data[iter] = queue.buffer[queue.read_index];
            queue.read_index = (queue.read_index + 1) % BUFFER_SIZE;
        }

        pthread_mutex_unlock(&queue.mtx);

        printf("Received data: ");
        for (uint8_t iter = 0; iter < CHUNK_SIZE; ++iter)
            printf("0x%x ", data[iter]);
        printf("\n");
    }
}

void init_queue(Queue_t* q)
{
    memset(q->buffer, 0, BUFFER_SIZE);
    q->write_index = 0;
    q->read_index = 0;
    pthread_mutex_init(&q->mtx, NULL);
}


int main()
{
    pthread_t dma_id, cpu_id;

    init_queue(&queue);

    sem_init(&data_available, 0, 0);

    pthread_create(&dma_id, NULL, dma_thread, NULL);
    pthread_create(&cpu_id, NULL, cpu_thread, NULL);


    pthread_join(dma_id, NULL);

    return 0;
}

