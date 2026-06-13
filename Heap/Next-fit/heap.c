#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

// Metadata for each heap block.
// Each block stores its usable size, free state, and link to the next block.
typedef struct Block
{
    size_t size;        // Number of usable bytes after this header.
    bool isFree;        // Whether the block is currently free.
    uint8_t pad[7];     // Padding for alignment on 64-bit systems.
    struct Block* next; // Next block in the heap-linked list.
} Block_t;

#define BLOCK_SIZE sizeof(Block_t)
#define HEAP_SIZE 1024

static uint8_t heap[HEAP_SIZE];            // Underlying heap memory region.
static Block_t* freeList = (Block_t*)heap; // Pointer to the first block in the heap.
static Block_t* lastAlloc = NULL;          // Last allocation point used by next-fit.

// Initialize the heap with a single free block covering the entire region.
void initHeap()
{
    freeList->size = HEAP_SIZE - BLOCK_SIZE;
    freeList->next = NULL;
    freeList->isFree = true;
    lastAlloc = freeList; // Next-fit starts scanning from this block.
}

// Find a free block using the next-fit strategy.
// Search begins at lastAlloc and wraps back to the heap start.
Block_t* findFreeBlock(size_t size)
{
    if (!lastAlloc)
        return NULL;

    Block_t* block = lastAlloc;
    Block_t* start = block;

    do
    {
        if (block->isFree && block->size >= size)
            return block; // Found a suitable free block.

        // Move to the next block or wrap back to the first block.
        block = block->next ? block->next : freeList;
    } while (block != start);

    return NULL; // No suitable block exists.
}

// Split a free block when it is larger than needed.
// The remaining space becomes a new free block.
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
            // Combine current block with next block.
            block->size += BLOCK_SIZE + block->next->size;
            block->next = block->next->next;
        }
        else
        {
            block = block->next;
        }
    }
}

// Allocate memory from the heap.
void* my_malloc(size_t size)
{
    if (size == 0)
        return NULL;

    Block_t* block = findFreeBlock(size);
    if (!block)
        return NULL; // No free block large enough.

    splitBlock(block, size);
    block->isFree = false;

    // Update lastAlloc for next-fit.
    lastAlloc = block->next ? block->next : freeList;

    // Return pointer to the usable memory area after the header.
    return (void*)((uint8_t*)block + BLOCK_SIZE);
}

// Free previously allocated memory.
void my_free(void* ptr)
{
    if (!ptr)
        return;

    // Find the block header from the user pointer.
    Block_t* block = (Block_t*)((uint8_t*)ptr - BLOCK_SIZE);
    block->isFree = true;
    mergeBlocks(); // Combine any adjacent free blocks.
}

// Print the heap layout for debugging.
void printHeap()
{
    Block_t* block = freeList;
    while (block)
    {
        printf("Block at %p size: %zu, Free: %d\n", (void*)block, block->size, block->isFree);
        block = block->next;
    }
}

int main()
{
    initHeap();
    printf("Heap before allocation\n");
    printHeap();

    void* p1 = my_malloc(100);
    printf("\nHeap after allocating 100\n");
    printHeap();

    void* p2 = my_malloc(200);
    printf("\nHeap after allocating 200\n");
    printHeap();

    my_free(p1);
    printf("\nHeap after freeing 100\n");
    printHeap();

    void* p3 = my_malloc(50);
    printf("\nHeap after allocating 50\n");
    printHeap();

    my_free(p2);
    my_free(p3);
    printf("\nHeap after freeing remaining blocks\n");
    printHeap();

    return 0;
}
