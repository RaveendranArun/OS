#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "shm_ipc.h"

/*
 * Test scenario: Two cores communicating via shared memory IPC.
 * Core 0 sends messages to Core 1, and vice versa.
 */

static void test_shm_ipc_init_destroy(void)
{
    shm_ipc_channel_t channel;
    int rc = shm_ipc_init(&channel, 0, 10);
    assert(rc == 0);
    assert(!shm_ipc_has_message(&channel));
    rc = shm_ipc_destroy(&channel);
    assert(rc == 0);
}

static void test_single_message_passing(void)
{
    shm_ipc_channel_t channel;
    assert(shm_ipc_init(&channel, 0, 10) == 0);

    uint8_t send_data[] = {0x01, 0x02, 0x03, 0x04};
    int rc = shm_ipc_send(&channel, 1, send_data, sizeof(send_data));
    assert(rc == 0);
    assert(shm_ipc_has_message(&channel));

    shm_ipc_message_t msg;
    rc = shm_ipc_recv(&channel, &msg);
    assert(rc == 0);
    assert(msg.sender_core == 0);
    assert(msg.receiver_core == 1);
    assert(msg.message_len == 4);
    assert(memcmp(msg.message, send_data, 4) == 0);
    assert(!shm_ipc_has_message(&channel));

    assert(shm_ipc_destroy(&channel) == 0);
}

static void test_multiple_messages(void)
{
    shm_ipc_channel_t channel;
    assert(shm_ipc_init(&channel, 2, 10) == 0);

    uint8_t data1[] = {0xAA, 0xBB};
    uint8_t data2[] = {0xCC, 0xDD, 0xEE};
    uint8_t data3[] = {0xFF};

    assert(shm_ipc_send(&channel, 0, data1, sizeof(data1)) == 0);
    assert(shm_ipc_send(&channel, 0, data2, sizeof(data2)) == 0);
    assert(shm_ipc_send(&channel, 0, data3, sizeof(data3)) == 0);

    shm_ipc_message_t msg;
    assert(shm_ipc_recv(&channel, &msg) == 0);
    assert(msg.sender_core == 2);
    assert(msg.message_len == 2);
    assert(memcmp(msg.message, data1, 2) == 0);

    assert(shm_ipc_recv(&channel, &msg) == 0);
    assert(msg.message_len == 3);
    assert(memcmp(msg.message, data2, 3) == 0);

    assert(shm_ipc_recv(&channel, &msg) == 0);
    assert(msg.message_len == 1);
    assert(msg.message[0] == 0xFF);

    assert(!shm_ipc_has_message(&channel));
    assert(shm_ipc_destroy(&channel) == 0);
}

static void test_broadcast_message(void)
{
    shm_ipc_channel_t channel;
    assert(shm_ipc_init(&channel, 1, 5) == 0);

    uint8_t data[] = {0x11, 0x22, 0x33};
    int rc = shm_ipc_send(&channel, 0, data, sizeof(data));  /* core_id=0 for broadcast */
    assert(rc == 0);

    shm_ipc_message_t msg;
    rc = shm_ipc_recv(&channel, &msg);
    assert(rc == 0);
    assert(msg.receiver_core == 0);
    assert(msg.sender_core == 1);

    assert(shm_ipc_destroy(&channel) == 0);
}

static void test_statistics(void)
{
    shm_ipc_channel_t channel;
    assert(shm_ipc_init(&channel, 3, 8) == 0);

    uint8_t data[] = {0x12, 0x34};
    uint32_t sent_count, recv_count;

    shm_ipc_get_stats(&channel, &sent_count, &recv_count);
    assert(sent_count == 0 && recv_count == 0);

    assert(shm_ipc_send(&channel, 0, data, sizeof(data)) == 0);
    assert(shm_ipc_send(&channel, 0, data, sizeof(data)) == 0);
    assert(shm_ipc_send(&channel, 0, data, sizeof(data)) == 0);

    shm_ipc_message_t msg;
    assert(shm_ipc_recv(&channel, &msg) == 0);
    assert(shm_ipc_recv(&channel, &msg) == 0);

    shm_ipc_get_stats(&channel, &sent_count, &recv_count);
    assert(sent_count == 3);
    assert(recv_count == 2);

    assert(shm_ipc_destroy(&channel) == 0);
}

static void test_max_message_length(void)
{
    shm_ipc_channel_t channel;
    assert(shm_ipc_init(&channel, 0, 1) == 0);

    uint8_t data[256];
    memset(data, 0xAB, sizeof(data));

    int rc = shm_ipc_send(&channel, 1, data, 256);
    assert(rc == 0);

    shm_ipc_message_t msg;
    rc = shm_ipc_recv(&channel, &msg);
    assert(rc == 0);
    assert(msg.message_len == 256);
    assert(memcmp(msg.message, data, 256) == 0);

    assert(shm_ipc_destroy(&channel) == 0);
}

int main(void)
{
    printf("Running shared memory IPC tests...\n");

    test_shm_ipc_init_destroy();
    printf("  test_shm_ipc_init_destroy passed\n");

    test_single_message_passing();
    printf("  test_single_message_passing passed\n");

    test_multiple_messages();
    printf("  test_multiple_messages passed\n");

    test_broadcast_message();
    printf("  test_broadcast_message passed\n");

    test_statistics();
    printf("  test_statistics passed\n");

    test_max_message_length();
    printf("  test_max_message_length passed\n");

    printf("All shared memory IPC tests passed.\n");
    return EXIT_SUCCESS;
}
