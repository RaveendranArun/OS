# Shared Memory IPC for ARM Cortex M4/M7 Multicore Communication

## Overview

This module provides inter-process communication (IPC) using shared memory and FIFO queues for ARM Cortex-M multicore environments (M4/M7). It handles cache coherency and memory ordering to ensure reliable message passing between cores.

## Features

- **Non-blocking FIFO-based messaging**: Fast, lock-free communication using circular FIFO buffers
- **Cache coherency**: Uses memory barriers (DMB/DSB) to ensure data visibility across cores
- **Memory ordering**: Proper synchronization barriers prevent memory reordering issues
- **Flexible message format**: Each message includes sender core ID, receiver core ID, and up to 256 bytes of payload
- **Broadcast support**: Messages can be sent to specific cores or broadcast (core_id=0)
- **Statistics tracking**: Per-channel counters for sent and received messages

## Architecture

### Data Structure: `shm_ipc_message_t`

```c
typedef struct {
    uint32_t sender_core;    /* Source core ID (0-3) */
    uint32_t receiver_core;  /* Target core ID (0 for broadcast) */
    uint16_t message_len;    /* Message payload length (1-256 bytes) */
    uint16_t reserved;       /* Alignment padding */
    uint8_t message[256];    /* Message payload */
} shm_ipc_message_t;
```

### Data Structure: `shm_ipc_channel_t`

```c
typedef struct {
    fifo_t fifo;             /* Underlying FIFO queue */
    uint32_t core_id;        /* Channel's associated core ID */
    volatile uint32_t flags; /* Synchronization flags */
    uint32_t messages_sent;  /* Statistics counter */
    uint32_t messages_recv;  /* Statistics counter */
} shm_ipc_channel_t;
```

## Usage Example

### Initialization

```c
#include "shm_ipc.h"

/* Create shared memory region */
shm_ipc_channel_t channel;
shm_ipc_init(&channel, core_id, max_messages);
```

### Sending Messages

```c
uint8_t data[] = {0x01, 0x02, 0x03};
shm_ipc_send(&channel, target_core_id, data, sizeof(data));
```

### Receiving Messages

```c
shm_ipc_message_t msg;
if (shm_ipc_recv(&channel, &msg) == 0) {
    printf("Received from core %u: %u bytes\n", msg.sender_core, msg.message_len);
}
```

### Checking for Messages

```c
while (shm_ipc_has_message(&channel)) {
    shm_ipc_message_t msg;
    shm_ipc_recv(&channel, &msg);
    /* Process message */
}
```

### Getting Statistics

```c
uint32_t sent, recv;
shm_ipc_get_stats(&channel, &sent, &recv);
printf("Sent: %u, Received: %u\n", sent, recv);
```

## Memory Coherency Handling

The implementation uses memory barriers to ensure cache coherency across ARM Cortex-M cores:

- **DMB (Data Memory Barrier)**: Used before/after FIFO access to ensure memory visibility
- **DSB (Data Synchronization Barrier)**: Used after critical operations for stronger synchronization

### On ARM Cortex-M Targets

For ARM compilation, replace the portable barriers in `shm_ipc.c` with ARM-specific barriers:

```c
/* ARM Cortex-M DMB (ISH = Inner Shareable) */
#define shm_ipc_dmb() __asm__ volatile ("dmb ish" ::: "memory")

/* ARM Cortex-M DSB (ISH = Inner Shareable) */
#define shm_ipc_dsb() __asm__ volatile ("dsb ish" ::: "memory")
```

## Thread Safety

- **Lock-free operation**: Uses non-blocking FIFO operations
- **Cache-safe**: Memory barriers ensure no stale data is read
- **Reordering prevention**: Barriers prevent both speculative execution and compiler reordering

## Testing

Run the test suite to verify functionality:

```bash
gcc -Wall -Wextra -pthread -std=c99 -O2 fifo.c shm_ipc.c shm_ipc_test.c -o shm_ipc_test
./shm_ipc_test
```

Test coverage:
- Initialization and cleanup
- Single message passing
- Multiple consecutive messages
- Broadcast messages (receiver_core = 0)
- Statistics tracking
- Maximum message length (256 bytes)

## Limitations

- Max 256 bytes per message (configurable in struct definition)
- Fixed buffer size specified at initialization
- Non-blocking operations (no wait-on-empty semantics)
- Designed for up to 4 cores (ARM Cortex-M multicore limit)

## Files

- `shm_ipc.h`: Public API header
- `shm_ipc.c`: Implementation with cache coherency barriers
- `shm_ipc_test.c`: Comprehensive test suite
- `fifo.h`, `fifo.c`: Underlying FIFO implementation
- `README.md`: This documentation

## Performance Considerations

- **Non-blocking**: IPC operations do not block waiting for space or data
- **Lock-free**: No mutex contention, suitable for real-time systems
- **Cache-efficient**: FIFO circular buffer minimizes memory fragmentation
- **Barrier overhead**: Memory barriers add ~5-10 cycles on ARM (low impact for real-time messaging)

## Multicore Scenario

Consider two ARM cores communicating:

```
Core 0                           Core 1
────────────────────────────────────────
1. Prepare message
2. DMB barrier
3. Write to FIFO  ──────────────>  4. DMB barrier
                                  5. Read from FIFO
                                  6. Process message
```

The barriers ensure that Core 1 sees Core 0's message data completely before acting on it.
