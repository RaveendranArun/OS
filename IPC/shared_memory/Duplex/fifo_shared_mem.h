#ifndef __FIFO_SHARED_MEM_H__
#define __FIFO_SHARED_MEM_H__

#define BUFFER_SIZE 256

typedef struct fifo_t
{
    uint16_t write;
    uint16_t read;
    uint8_t buffer[BUFFER_SIZE;
}fifo_t;

typedef struct shm_fifo_t 
{
    fifo_t shm_a2b_fifo;
    fifo_t shm_b2a_fifo;
}shared_memory_t;

void fifo_write(fifo_t* fifo, uint8_t* data, uint16_t len);

void fifo_read(fifo_t* fifo, uint8_t* data, uint16_t maxLen);

#endif // __FIFO_SHARED_MEM_H__