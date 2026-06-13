#ifndef SIMPLE_FIFO_H
#define SIMPLE_FIFO_H

#include <pthread.h>
#include <stdbool.h>
#include <stdint.h>

/*
 * Simple FIFO buffer using a circular array.
 * This implementation is thread-safe using a mutex.
 * The FIFO stores raw bytes and supports single-byte and
 * multi-byte enqueue/dequeue operations.
 */
typedef struct {
    uint8_t *buffer;       /* FIFO storage buffer allocated at init */
    int front;             /* Index of the next byte to read */
    int rear;              /* Index of the next byte to write */
    int capacity;          /* Maximum number of bytes in the buffer */
    int size;              /* Current number of stored bytes */
    pthread_mutex_t lock;  /* Protects FIFO state in multi-threaded use */
} fifo_t;

/* Initialize a FIFO of given capacity. Returns 0 on success, -1 on error. */
int fifo_init(fifo_t *fifo, int capacity);

/* Destroy FIFO and free allocated resources. Returns 0 on success. */
int fifo_destroy(fifo_t *fifo);

/* Return true when there are no bytes stored in the FIFO. */
bool fifo_is_empty(fifo_t *fifo);

/* Return true when the FIFO is full and no more bytes can be enqueued. */
bool fifo_is_full(fifo_t *fifo);

/* Enqueue a single byte. Returns 0 on success, -1 if the FIFO is full. */
int fifo_enqueue(fifo_t *fifo, uint8_t item);

/* Dequeue a single byte. Returns 0 on success, -1 if the FIFO is empty. */
int fifo_dequeue(fifo_t *fifo, uint8_t *item);

/*
 * Try to enqueue up to len bytes from data into the FIFO.
 * Returns the number of bytes actually written (0..len).
 * This call does not block.
 */
int fifo_enqueue_bytes(fifo_t *fifo, const uint8_t *data, int len);

/*
 * Try to dequeue up to len bytes from the FIFO into data.
 * Returns the number of bytes actually read (0..len).
 * This call does not block.
 */
int fifo_dequeue_bytes(fifo_t *fifo, uint8_t *data, int len);

#endif // SIMPLE_FIFO_H
