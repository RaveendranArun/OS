#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "fifo.h"

static void test_init_destroy(void)
{
    fifo_t fifo;
    int rc = fifo_init(&fifo, 8);
    assert(rc == 0);
    assert(fifo_is_empty(&fifo));
    assert(!fifo_is_full(&fifo));
    rc = fifo_destroy(&fifo);
    assert(rc == 0);
}

static void test_single_byte_operations(void)
{
    fifo_t fifo;
    assert(fifo_init(&fifo, 4) == 0);

    assert(fifo_enqueue(&fifo, 0x11) == 0);
    assert(fifo_enqueue(&fifo, 0x22) == 0);
    assert(fifo_enqueue(&fifo, 0x33) == 0);
    assert(fifo_enqueue(&fifo, 0x44) == 0);
    assert(fifo_is_full(&fifo));
    assert(fifo_enqueue(&fifo, 0x55) == -1);

    uint8_t value;
    assert(fifo_dequeue(&fifo, &value) == 0);
    assert(value == 0x11);
    assert(fifo_dequeue(&fifo, &value) == 0);
    assert(value == 0x22);
    assert(fifo_dequeue(&fifo, &value) == 0);
    assert(value == 0x33);
    assert(fifo_dequeue(&fifo, &value) == 0);
    assert(value == 0x44);
    assert(fifo_is_empty(&fifo));
    assert(fifo_dequeue(&fifo, &value) == -1);

    assert(fifo_destroy(&fifo) == 0);
}

static void test_multi_byte_operations(void)
{
    fifo_t fifo;
    assert(fifo_init(&fifo, 6) == 0);

    uint8_t write_data[] = {0xAA, 0xBB, 0xCC, 0xDD};
    int written = fifo_enqueue_bytes(&fifo, write_data, sizeof(write_data));
    assert(written == 4);
    assert(!fifo_is_empty(&fifo));
    assert(!fifo_is_full(&fifo));

    uint8_t read_data[4] = {0};
    int read = fifo_dequeue_bytes(&fifo, read_data, 4);
    assert(read == 4);
    assert(memcmp(write_data, read_data, 4) == 0);
    assert(fifo_is_empty(&fifo));

    /* Partial enqueue when not enough room. Capacity is 6, so only 3 bytes fit. */
    assert(fifo_enqueue(&fifo, 0x10) == 0);
    assert(fifo_enqueue(&fifo, 0x20) == 0);
    assert(fifo_enqueue(&fifo, 0x30) == 0);
    assert(fifo_enqueue_bytes(&fifo, write_data, sizeof(write_data)) == 3);
    assert(fifo_is_full(&fifo));

    uint8_t expected[6] = {0x10, 0x20, 0x30, 0xAA, 0xBB, 0xCC};
    memset(read_data, 0, sizeof(read_data));
    read = fifo_dequeue_bytes(&fifo, read_data, 6);
    assert(read == 6);
    assert(memcmp(read_data, expected, 6) == 0);
    assert(fifo_is_empty(&fifo));

    assert(fifo_destroy(&fifo) == 0);
}

static void test_edge_conditions(void)
{
    fifo_t fifo;
    assert(fifo_init(&fifo, 3) == 0);

    uint8_t data[3];
    int rc = fifo_enqueue_bytes(&fifo, data, 0);
    assert(rc == -1);
    rc = fifo_dequeue_bytes(&fifo, data, 0);
    assert(rc == -1);

    assert(fifo_destroy(&fifo) == 0);
}

int main(void)
{
    printf("Running simple FIFO tests...\n");

    test_init_destroy();
    printf("  test_init_destroy passed\n");

    test_single_byte_operations();
    printf("  test_single_byte_operations passed\n");

    test_multi_byte_operations();
    printf("  test_multi_byte_operations passed\n");

    test_edge_conditions();
    printf("  test_edge_conditions passed\n");

    printf("All simple FIFO tests passed.\n");
    return EXIT_SUCCESS;
}
