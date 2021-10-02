#include "kernel_heap.h"
#include "memmap.h"
#include "../kernel/syslog.h"
#include "../lib/types.h"

typedef struct block_hdr
{
    struct block_hdr *next_free;
    struct block_hdr *prev_free;
    uint8 order;
} block_hdr;

// start of heap
void *heap_base = NULL;

// array that holds a linked list of free blocks for every block size order
block_hdr *free_blocks[MAX_ORDER+1];

void heap_init()
{
    log("MM", LOG_INFO, "HEAP", "Initializing kernel heap");
    heap_base = kernel_heap_base();

    // initialize free block lists
    int i;
    for(i = 0; i < MAX_ORDER - 1; i++)
        free_blocks[i] = NULL;

    // highest order has a single free block which is the entire heap
    free_blocks[MAX_ORDER] = (block_hdr*)heap_base;
    free_blocks[MAX_ORDER]->next_free = NULL;
    free_blocks[MAX_ORDER]->prev_free = NULL;
    free_blocks[MAX_ORDER]->order = MAX_ORDER;
}

block_hdr *find_smallest_free(uint8 order)
{
    uint8 i;
    for (i = order; i <= MAX_ORDER; i++)
    {
        if (free_blocks[i] != NULL)
            return free_blocks[i];
    }
    return NULL;
}

void remove_from_free_list(block_hdr *block)
{
#ifdef __DEBUG_KERNEL_HEAP__
    log("MM", LOG_DEBUG, "HEAP", "Removing block of order %d at address 0x%p from free list",
        (int)block->order, block);
#endif

    // this is the only free block of this order - simply remove it from the list of free blocks
    if (block->next_free == NULL && block->prev_free == NULL)
        free_blocks[block->order] = NULL;
    
    // this block is the first in the list - point the list start to the next free block
    if (block->prev_free == NULL && block->next_free != NULL)
    {
        free_blocks[block->order] = block->next_free;
        block->next_free->prev_free = NULL;
        block->next_free = NULL;
    }

    // this block is the last in the list
    if (block->next_free == NULL && block->prev_free != NULL)
    {
        block->prev_free->next_free = NULL;
        block->prev_free = NULL;
    }

    // the block is in the middle of the list
    else
    {
        block->prev_free->next_free = block->next_free;
        block->next_free->prev_free = block->prev_free;
        block->next_free = NULL;
        block->prev_free = NULL;
    }
}

void add_to_free_list(block_hdr *block)
{
#ifdef __DEBUG_KERNEL_HEAP__
    log("MM", LOG_DEBUG, "HEAP", "Adding block of order %d at address 0x%p to free list",
        (int)block->order, block);
#endif

    uint8 order = block->order;

    free_blocks[order]->prev_free = block;
    block->prev_free = NULL;
    block->next_free = free_blocks[order];
    free_blocks[order] = block;
}

block_hdr *split_block(block_hdr *block)
{
    // make sure the block is splittable
    if (block->order == 0)
        return block;

#ifdef __DEBUG_KERNEL_HEAP__
    log("MM", LOG_DEBUG, "HEAP", "Splitting block of order %d at address 0x%p",
        (int)block->order, block);
#endif
    
    // remove block from the free list
    remove_from_free_list(block);

    uint8 neworder = block->order - 1;
    
    // initialize 'right' child block as free
    block_hdr *right = (block_hdr*)((size_t)block + BLOCK_SIZE(block->order) / 2);
    right->order = neworder;

    // add it to the free list
    add_to_free_list(right);

    // add 'left' child block to free list and return it
    block->order = neworder;
    add_to_free_list(block);
    return block;
}

void *kmalloc(size_t size)
{
    // heap base is NULL which means this is the first call to kmalloc
    if (heap_base == NULL)
        heap_init();

    // find which block order we need for this size
    // TODO: implement this using floating point math:
    // order = (int)log2(size) - log2(MIN_BLOCK_SIZE)
    uint8 i, order;
    for (i = 0; i <= MAX_ORDER; i++)
    {
        if (BLOCK_SIZE(i) - sizeof(block_hdr) >= size)
        {
            order = i;
            break;
        }
    }
    // no order is large enough for this request
    if (i > MAX_ORDER)
    {
        log("MM", LOG_INFO, "HEAP", "Cannot satisfy request for %lu bytes - maximum valid request size is %lu",
            size, BLOCK_SIZE(MAX_ORDER));
        return NULL;
    }
    
    // find smallest free block
    block_hdr *smallest_free = find_smallest_free(order);

    // no block found that can satisfy this request
    if (smallest_free == NULL)
    {
        log("MM", LOG_ERR, "HEAP", "Cannot find block to satisfy request for %lu bytes (order %d)",
            size, (int)order);
        return NULL;
    }

    block_hdr *allocated;

    // smallest free block is of the correct order - allocate it
    if (smallest_free->order == order)
    {
        remove_from_free_list(smallest_free);
        allocated = smallest_free;
    }

    // the block is of a higher order - split it until we reach the needed order
    block_hdr *current_block = smallest_free;
    while(current_block->order > order)
        current_block = split_block(current_block);
    
    // remove the block from the free list
    remove_from_free_list(current_block);
    allocated = current_block;

    // return a pointer to the start of the data
#ifdef __DEBUG_MEMORY__
    log("MM", LOG_DEBUG, "HEAP", "Allocated block of order %d at address 0x%p for request of size %lu",
        (int)allocated->order, allocated, size);
#endif
    return (void*)allocated + sizeof(block_hdr);
}