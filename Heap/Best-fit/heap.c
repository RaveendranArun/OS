#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

// Metadata for a heap block.
typedef struct Block
{
    size_t size;        // Usable size in bytes.
    bool isFree;        // Whether the block is free.
    uint8_t pad[7];     // Padding for alignment.
    struct Block* next; // Next block in the heap.
} Block_t;

#define BLOCK_SIZE sizeof(Block_t)
#define HEAP_SIZE 1024

static uint8_t heap[HEAP_SIZE];          // Simulated heap memory.
static Block_t* freeList = (Block_t*)heap; // Start of the block list.

// Initialize the heap with one large free block.
void initHeap()
{
    freeList->size = HEAP_SIZE - BLOCK_SIZE;
    freeList->next = NULL;
    freeList->isFree = true;
}

// Find the best free block for the requested size.
// Best-fit chooses the smallest free block that is still large enough.
Block_t* findBestFit(size_t size)
{
    Block_t* best = NULL;
    Block_t* block = freeList;

    while (block)
    {
        if (block->isFree && block->size >= size)
        {
            if (!best || block->size < best->size)
            {
                best = block;
            }
        }
        block = block->next;
    }

    return best;
}

// Split a block if there is enough space for a new free block.
void splitBlock(Block_t* block, size_t size)
{
    if (block->size >= size + BLOCK_SIZE)
    {
        Block_t* newBlock = (Block_t*)((uint8_t*)block + BLOCK_SIZE + size);
        newBlock->size = block->size - size - BLOCK_SIZE;
        newBlock->next = block->next;
        newBlock->isFree = true;

        block->next = newBlock;
        block->size = size;
    }
}

// Merge adjacent free blocks to reduce fragmentation.
void mergeBlocks()
{
    Block_t* block = freeList;

    while (block && block->next)
    {
        if (block->isFree && block->next->isFree)
        {
            block->size += BLOCK_SIZE + block->next->size;
            block->next = block->next->next;
        }
        else
        {
            block = block->next;
        }
    }
}

// Allocate memory using the best-fit strategy.
void* my_malloc(size_t size)
{
    if (size == 0)
        return NULL;

    Block_t* block = findBestFit(size);
    if (!block)
        return NULL;

    splitBlock(block, size);
    block->isFree = false;

    return (void*)((uint8_t*)block + BLOCK_SIZE);
}

// Free previously allocated memory.
void my_free(void* ptr)
{
    if (!ptr)
        return;

    Block_t* block = (Block_t*)((uint8_t*)ptr - BLOCK_SIZE);
    block->isFree = true;
    mergeBlocks();
}

// Print heap block state for debugging.
void printHeap()
{
    Block_t* block = freeList;
    while (block)
    {
        printf("Block at %p size: %zu free: %d\n", (void*)block, block->size, block->isFree);
        block = block->next;
    }
}

int main()
{
    initHeap();
    printf("Heap before allocation\n");
    printHeap();

    void* p1 = my_malloc(120);
    printf("\nHeap after allocating 120\n");
    printHeap();

    void* p2 = my_malloc(200);
    printf("\nHeap after allocating 200\n");
    printHeap();

    void* p3 = my_malloc(80);
    printf("\nHeap after allocating 80\n");
    printHeap();

    my_free(p2);
    printf("\nHeap after freeing 200\n");
    printHeap();

    void* p4 = my_malloc(100);
    printf("\nHeap after allocating 100\n");
    printHeap();

    my_free(p1);
    my_free(p3);
    my_free(p4);
    printf("\nHeap after freeing all allocations\n");
    printHeap();

    return 0;
}
