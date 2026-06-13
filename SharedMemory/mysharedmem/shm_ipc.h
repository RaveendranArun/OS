#ifndef SHM_IPC_H
#define SHM_IPC_H

#include <stdint.h>
#include <stdbool.h>
#include "fifo.h"

/*
 * Shared Memory IPC for ARM Cortex M4/M7 multicore communication.
 * 
 * This implementation provides inter-process communication using FIFO
 * with proper cache coherency and memory ordering for ARM multicore environments.
 * 
 * Usage:
 *   - Place shm_ipc_channel_t in a shared memory region accessible to all cores
 *   - Each core pins itself and opens a channel with its core ID
 *   - Use shm_ipc_send() and shm_ipc_recv() for message passing
 *   - Memory barriers ensure cache coherency across cores
 */

typedef struct {
    uint32_t sender_core;    /* Core ID that sent the message */
    uint32_t receiver_core;  /* Target core ID (0 for broadcast) */
    uint16_t message_len;    /* Length of the message data */
    uint16_t reserved;       /* Padding for alignment */
    uint8_t message[256];    /* Message payload (max 256 bytes) */
} shm_ipc_message_t;

typedef struct {
    fifo_t fifo;             /* FIFO for message passing */
    uint32_t core_id;        /* ID of the core using this channel */
    volatile uint32_t flags; /* Status flags for synchronization */
    uint32_t messages_sent;  /* Statistics: total messages sent */
    uint32_t messages_recv;  /* Statistics: total messages received */
} shm_ipc_channel_t;

/* Initialize a shared memory IPC channel for the given core. */
int shm_ipc_init(shm_ipc_channel_t *channel, uint32_t core_id, int max_messages);

/* Destroy the shared memory IPC channel. */
int shm_ipc_destroy(shm_ipc_channel_t *channel);

/* Send a message to a target core (core_id=0 for broadcast). */
int shm_ipc_send(shm_ipc_channel_t *channel, uint32_t target_core, 
                 const uint8_t *data, uint16_t len);

/* Receive a message from the FIFO (non-blocking). Returns 0 on success, -1 if no message. */
int shm_ipc_recv(shm_ipc_channel_t *channel, shm_ipc_message_t *msg);

/* Check if there are pending messages. */
bool shm_ipc_has_message(shm_ipc_channel_t *channel);

/* Get channel statistics. */
void shm_ipc_get_stats(shm_ipc_channel_t *channel, uint32_t *sent, uint32_t *recv);

#endif // SHM_IPC_H
