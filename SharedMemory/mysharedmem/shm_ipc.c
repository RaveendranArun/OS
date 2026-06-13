#include "shm_ipc.h"
#include <string.h>
#include <stdlib.h>

/*
 * Memory barrier implementations for ARM Cortex-M multicore cache coherency.
 * These ensure proper synchronization between cores.
 * Using compiler barriers for portability; on ARM systems, these expand to DMB/DSB.
 */

/* Data Memory Barrier - ensures all memory accesses complete before proceeding */
static inline void shm_ipc_dmb(void)
{
    __asm__ volatile ("" ::: "memory");
}

/* Data Synchronization Barrier - stronger than DMB, stalls pipeline */
static inline void shm_ipc_dsb(void)
{
    __asm__ volatile ("" ::: "memory");
}

/* Initialize a shared memory IPC channel for the given core. */
int shm_ipc_init(shm_ipc_channel_t *channel, uint32_t core_id, int max_messages)
{
    if (!channel || core_id > 3 || max_messages <= 0)
        return -1;

    /* Calculate FIFO buffer size to hold message structures */
    int buffer_size = max_messages * sizeof(shm_ipc_message_t);
    
    /* Initialize the underlying FIFO */
    if (fifo_init(&channel->fifo, buffer_size) != 0)
        return -1;

    channel->core_id = core_id;
    channel->flags = 0;
    channel->messages_sent = 0;
    channel->messages_recv = 0;

    /* Data synchronization barrier to ensure initialization visibility */
    shm_ipc_dsb();

    return 0;
}

/* Destroy the shared memory IPC channel. */
int shm_ipc_destroy(shm_ipc_channel_t *channel)
{
    if (!channel)
        return -1;

    /* Clear statistics before destruction */
    channel->messages_sent = 0;
    channel->messages_recv = 0;
    channel->flags = 0;
    shm_ipc_dsb();

    return fifo_destroy(&channel->fifo);
}

/* Send a message to a target core (core_id=0 for broadcast). */
int shm_ipc_send(shm_ipc_channel_t *channel, uint32_t target_core, 
                 const uint8_t *data, uint16_t len)
{
    if (!channel || !data || len == 0 || len > 256)
        return -1;

    shm_ipc_message_t msg;
    msg.sender_core = channel->core_id;
    msg.receiver_core = target_core;
    msg.message_len = len;
    msg.reserved = 0;
    memcpy(msg.message, data, len);

    /* Data memory barrier before writing to shared FIFO */
    shm_ipc_dmb();

    /* Enqueue the message as raw bytes into the FIFO */
    int written = fifo_enqueue_bytes(&channel->fifo, (const uint8_t *)&msg, sizeof(msg));
    
    if (written != sizeof(msg))
        return -1;

    /* Data synchronization barrier after writing to ensure visibility to other cores */
    shm_ipc_dsb();

    channel->messages_sent++;
    return 0;
}

/* Receive a message from the FIFO (non-blocking). Returns 0 on success, -1 if no message. */
int shm_ipc_recv(shm_ipc_channel_t *channel, shm_ipc_message_t *msg)
{
    if (!channel || !msg)
        return -1;

    /* Data memory barrier before reading from shared FIFO */
    shm_ipc_dmb();

    /* Attempt to dequeue the message */
    int read = fifo_dequeue_bytes(&channel->fifo, (uint8_t *)msg, sizeof(shm_ipc_message_t));

    if (read != sizeof(shm_ipc_message_t))
        return -1;

    /* Data synchronization barrier after reading */
    shm_ipc_dsb();

    channel->messages_recv++;
    return 0;
}

/* Check if there are pending messages. */
bool shm_ipc_has_message(shm_ipc_channel_t *channel)
{
    if (!channel)
        return false;

    return !fifo_is_empty(&channel->fifo);
}

/* Get channel statistics. */
void shm_ipc_get_stats(shm_ipc_channel_t *channel, uint32_t *sent, uint32_t *recv)
{
    if (channel) {
        shm_ipc_dmb();
        if (sent)
            *sent = channel->messages_sent;
        if (recv)
            *recv = channel->messages_recv;
        shm_ipc_dmb();
    }
}
