#include "fifo.h"
#include <stdlib.h>
#include <string.h>

/*
 * Simple circular FIFO implementation.
 *
 * The buffer is managed as a ring: `front` points to the next byte to read,
 * `rear` points to the next byte position for writing, and `size` tracks the
 * number of bytes currently stored. The FIFO is protected by a mutex so that
 * multiple threads can safely use the same FIFO instance.
 */

/*
 * Initialize the FIFO structure.
 * Allocate a buffer of `capacity` bytes, reset indices and size, and initialize the mutex.
 * The FIFO is empty after successful initialization.
 */
int fifo_init(fifo_t *fifo, int capacity)
{
    if (!fifo || capacity <= 0)
        return -1;

    fifo->buffer = (uint8_t *)malloc((size_t)capacity * sizeof(uint8_t));
    if (!fifo->buffer)
        return -1;

    fifo->capacity = capacity;
    fifo->front = 0;   /* next read position */
    fifo->rear = 0;    /* next write position */
    fifo->size = 0;    /* stored byte count */
    
    /* Initialize the mutex used to serialize access. */
    if (pthread_mutex_init(&fifo->lock, NULL) != 0) {
        free(fifo->buffer);
        fifo->buffer = NULL;
        return -1;
    }


    return 0;
}

/*
 * Clean up the FIFO.
 * Destroy the mutex and free the allocated buffer.
 * After destroy, the FIFO is no longer usable until reinitialized.
 */
int fifo_destroy(fifo_t *fifo)
{
    if (!fifo)
        return -1;

    pthread_mutex_destroy(&fifo->lock);
    free(fifo->buffer);
    fifo->buffer = NULL;
    fifo->capacity = 0;
    fifo->front = 0;
    fifo->rear = 0;
    fifo->size = 0;
    return 0;
}

/*
 * Internal helper: check empty state without locking (assumes lock is held).
 */
static bool fifo_is_empty_unlocked(fifo_t *fifo)
{
    return (fifo->size == 0);
}

/*
 * Internal helper: check full state without locking (assumes lock is held).
 */
static bool fifo_is_full_unlocked(fifo_t *fifo)
{
    return (fifo->size == fifo->capacity);
}

/*
 * Check whether the FIFO contains no bytes.
 * Returns true if empty, false otherwise.
 */
bool fifo_is_empty(fifo_t *fifo)
{
    if (!fifo)
        return true;

    pthread_mutex_lock(&fifo->lock);
    bool result = fifo_is_empty_unlocked(fifo);
    pthread_mutex_unlock(&fifo->lock);
    return result;
}

/*
 * Check whether the FIFO is full and cannot accept more bytes.
 */
bool fifo_is_full(fifo_t *fifo)
{
    if (!fifo)
        return false;

    pthread_mutex_lock(&fifo->lock);
    bool result = fifo_is_full_unlocked(fifo);
    pthread_mutex_unlock(&fifo->lock);
    return result;
}

/*
 * Enqueue a single byte into the FIFO.
 * If the FIFO is full, the call returns -1 without blocking.
 * This operation advances the rear index and increases the stored size on success.
 */
int fifo_enqueue(fifo_t *fifo, uint8_t item)
{
    if (!fifo || !fifo->buffer)
        return -1;

    pthread_mutex_lock(&fifo->lock);

    /* If the FIFO is full, we do not block; return failure. */
    if (fifo_is_full_unlocked(fifo)) {
        pthread_mutex_unlock(&fifo->lock);
        return -1;
    }

    /* Store the new byte at rear and advance the rear index. */
    fifo->buffer[fifo->rear] = item;
    fifo->rear = (fifo->rear + 1) % fifo->capacity;
    fifo->size++;

    pthread_mutex_unlock(&fifo->lock);
    return 0;
}

/*
 * Dequeue a single byte from the FIFO.
 * If the FIFO is empty, the call returns -1 without blocking.
 * This operation advances the front index and decreases the stored size on success.
 */
int fifo_dequeue(fifo_t *fifo, uint8_t *item)
{
    if (!fifo || !fifo->buffer || !item)
        return -1;

    pthread_mutex_lock(&fifo->lock);

    if (fifo_is_empty_unlocked(fifo)) {
        pthread_mutex_unlock(&fifo->lock);
        return -1;
    }

    *item = fifo->buffer[fifo->front];
    fifo->front = (fifo->front + 1) % fifo->capacity;
    fifo->size--;

    pthread_mutex_unlock(&fifo->lock);
    return 0;
}

/*
 * Non-blocking enqueue of up to len bytes from data into the FIFO.
 * Returns the number of bytes actually written (0..len).
 */
int fifo_enqueue_bytes(fifo_t *fifo, const uint8_t *data, int len)
{
    if (!fifo || !fifo->buffer || !data || len <= 0)
        return -1;

    pthread_mutex_lock(&fifo->lock);

    /* Write as many bytes as the FIFO can currently accept. */
    int available = fifo->capacity - fifo->size;
    int to_write = len < available ? len : available;
    if (to_write > 0) {
        /* Copy the first contiguous chunk until the end of the buffer. */
        int first_chunk = fifo->capacity - fifo->rear;
        if (first_chunk > to_write)
            first_chunk = to_write;

        memcpy(&fifo->buffer[fifo->rear], data, (size_t)first_chunk);
        fifo->rear = (fifo->rear + first_chunk) % fifo->capacity;
        fifo->size += first_chunk;

        /* If the data wraps around the end of the buffer, write the remainder at the beginning. */
        int second_chunk = to_write - first_chunk;
        if (second_chunk > 0) {
            memcpy(&fifo->buffer[fifo->rear], data + first_chunk, (size_t)second_chunk);
            fifo->rear = (fifo->rear + second_chunk) % fifo->capacity;
            fifo->size += second_chunk;
        }
    }

    pthread_mutex_unlock(&fifo->lock);
    return to_write;
}

/*
 * Non-blocking dequeue of up to len bytes from the FIFO into data.
 * Returns the number of bytes actually read (0..len).
 */
int fifo_dequeue_bytes(fifo_t *fifo, uint8_t *data, int len)
{
    if (!fifo || !fifo->buffer || !data || len <= 0)
        return -1;

    pthread_mutex_lock(&fifo->lock);

    /* Read as many bytes as are available, up to the requested length. */
    int available = fifo->size;
    int to_read = len < available ? len : available;
    if (to_read > 0) {
        /* Copy the first contiguous chunk until the end of the buffer. */
        int first_chunk = fifo->capacity - fifo->front;
        if (first_chunk > to_read)
            first_chunk = to_read;

        memcpy(data, &fifo->buffer[fifo->front], (size_t)first_chunk);
        fifo->front = (fifo->front + first_chunk) % fifo->capacity;
        fifo->size -= first_chunk;

        /* If the data wraps around, copy the remainder from the buffer start. */
        int second_chunk = to_read - first_chunk;
        if (second_chunk > 0) {
            memcpy(data + first_chunk, &fifo->buffer[fifo->front], (size_t)second_chunk);
            fifo->front = (fifo->front + second_chunk) % fifo->capacity;
            fifo->size -= second_chunk;
        }
    }

    pthread_mutex_unlock(&fifo->lock);
    return to_read;
}

