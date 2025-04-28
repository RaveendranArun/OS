#include "fifo_shared_mem.h"
#include <string.h>

int fifo_write(fifo_t* fifo, uint8_t* data, uint16_t len)
{
    uint16_t freespace = (fifo->read + BUFFER_SIZE - fifo->write - 1) % BUFFER_SIZE;

    if (len > freespace)
        return -1;

    for (uint16_t iter = 0; iter < len; ++iter)
    {
        fifo->buffer[fifo->write] = data[iter];
        fifo->write = (fifo->write + 1) % BUFFER_SIZE;
    }

    return 0;
}

int fifo_read(fifo_t* fifo, uint8_t data, uint16_t maxsize)
{
    
}