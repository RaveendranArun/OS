#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

typedef struct Block
{
    struct Block* next;
    size_t size;
    bool isFree;
    uint8_t pad[7];
}Block_t;

#define BLOCK_SIZE    sizeof(Block_t)
#define HEAP_SIZE     1024

static uint8_t heap[HEAP_SIZE];
static Block_t* freeList = (Block_t* )heap;

void initHeap()
{
    freeList->size = HEAP_SIZE - BLOCK_SIZE;
    freeList->next = NULL;
    freeList->isFree = true;
}

// Find a free block using first-fit strategy
Block_t* findFreeBlock(size_t size)
{
    Block_t* block = freeList;
    
    while (block != NULL)
    {
        /* code */
        if (block->isFree && block->size >= size)
        {
            return block; // Return first suitable block
        }
        block = block->next;
    }
    return NULL; // No suitable block found
}

void splitBlock(Block_t* block, size_t size)
{
    if (block->size >= (size + BLOCK_SIZE))                                      // Ensure the space for new block
    {
        Block_t* newBlock = (Block_t*)((uint8_t* )block + BLOCK_SIZE + size);    // Get the new block
        newBlock->size = block->size - size - BLOCK_SIZE;                        // free space for the new block
        newBlock->next = block->next;                                            // Next block 
        newBlock->isFree = true;                                                 // Marking it as free block

        block->next = newBlock;                                                  // update the current blocks next block
        block->size = size;                                                      // Update the current blocks size
        block->isFree = false;                                                   // Marking the current block as allocated
    }
}

// Merge adjacent free blocks to reduce fragmentation
void mergeBlocks()
{
    Block_t* block = (Block_t* )freeList;
    
    while (block && block->next)
    {
        if (block->isFree && block->next->isFree)   // Merge two free blocks
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

void* my_malloc(size_t size)
{
    // 1. If the requested size is zero, return NULL
    if (size <= 0)
        return NULL;

    // 1. Find the free block
    Block_t* block = findFreeBlock(size);
    if (!block)
        return NULL;
        
    // 2. Split the free block, if its size is larger than the requested size
    splitBlock(block, size);
    block->isFree = false; // allocated;

    return (void* )((uint8_t*)block + BLOCK_SIZE); // Return the pointer to user memory (after the meta data)
}

// Free allocated memory
void my_free(void* ptr)
{
    if (!ptr)
        return;

    Block_t* block = (Block_t* )((uint8_t* )ptr - BLOCK_SIZE);
    block->isFree = true;

    mergeBlocks(); // Try merging the blocks
}

void printHeap()
{
    Block_t* block = freeList;
    while (block)
    {
        /* code */
        printf("Block at %p size: %zu, Free: %d\n", (void* )block, block->size, block->isFree);
        block = block->next;
    }
}

int main()
{
    initHeap();
    printf("Heap before allocation\n");
    printHeap();

    void* ptr1 = my_malloc(100);
    printf("Heap after allocating 100\n");
    printHeap();

    void* ptr2 = my_malloc(200);
    printf("Heap after allocating 200\n");
    printHeap();

    my_free(ptr1);
    ptr1 = NULL;
    printf("\nHeap after freeing 100\n");
    printHeap();

    ptr1 = my_malloc(50);
    printf("Heap after allocating 50\n");
    printHeap();

    my_free(ptr2);
    printf("\nHeap after freeing 200 allocation\n");
    printHeap();

    my_free(ptr1);
    printf("\nHeap after freeing 50 allocation\n");
    printHeap();

    return 0;
}