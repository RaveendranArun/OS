#include "fifo_shared_mem.h"
#include <string.h>

void fifo_init(fifo_t* fifo)
{
    fifo->write = 0;
    fifo->read = 0;
    memset(fifo->buffer, 0, BUFFER_SIZE);
}

int fifo_write(fifo_t* fifo, uint8_t* data, uint16_t len)
{
    uint16_t freespace = (fifo->read + BUFFER_SIZE - fifo->write - 1) % BUFFER_SIZE;

    if (len > freespace)
        return -1;    // not enough space in the fifo

    for (uint16_t iter = 0; iter < len; ++iter)
    {
        fifo->buffer[fifo->write] = data[iter];
        fifo->write = (fifo->write + 1) % BUFFER_SIZE;
    }

    return 0;
}

int fifo_read(fifo_t* fifo, uint8_t* data, uint16_t maxsize)
{
    uint16_t data_len = (fifo->write + BUFFER_SIZE - fifo->read) % BUFFER_SIZE;
    
    if (data_len == 0)
        return -1;    // fifo is empty

    uint16_t read_len = data_len < maxsize ? data_len : maxsize;

    for (uint16_t iter = 0; iter < read_len; ++iter)
    {
        data[iter] = fifo->buffer[fifo->read];
        fifo->read = (fifo->read + 1) % BUFFER_SIZE;
    }

    return read_len;
}