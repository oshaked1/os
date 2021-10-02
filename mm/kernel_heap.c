#include "kernel_heap.h"
#include "memmap.h"
#include "../kernel/syslog.h"
#include "../lib/types.h"

// this header is located at beginning of each heap block
typedef struct block_hdr
{
    struct block_hdr *next_free;
    struct block_hdr *prev_free;
    uint8 order;
    bool free;
} block_hdr;

// start of heap
void *heap_base = NULL;

// array that holds a linked list of free blocks for every block size order
block_hdr *free_blocks[MAX_ORDER+1];

/**
 * @brief Initialize the heap by emptying the lists of free blocks,
 * except for the highest order which has a free block that spans the entire heap.
 * 
 */
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
    free_blocks[MAX_ORDER]->free = TRUE;
}

/**
 * @brief Find the smallest free block available.
 * 
 * @param order The minimum order of the block we're searching for
 * @return block_hdr* A pointer to the block that was found
 */
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

/**
 * @brief Marks a block as used and removes it from the free list.
 * 
 * @param block A pointer to the block
 */
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
    else if (block->prev_free == NULL && block->next_free != NULL)
    {
        free_blocks[block->order] = block->next_free;
        block->next_free->prev_free = NULL;
        block->next_free = NULL;
    }

    // this block is the last in the list
    else if (block->next_free == NULL && block->prev_free != NULL)
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

    // mark the block as not free
    block->free = FALSE;
}

/**
 * @brief Marks a block as free and adds it to the free list.
 * 
 * @param block A pointer to the block
 */
void add_to_free_list(block_hdr *block)
{
#ifdef __DEBUG_KERNEL_HEAP__
    log("MM", LOG_DEBUG, "HEAP", "Adding block of order %d at address 0x%p to free list",
        (int)block->order, block);
#endif

    uint8 order = block->order;

    // reset the pointers of the block
    block->prev_free = NULL;
    block->next_free = NULL;

    // attach the block to the list
    if (free_blocks[order] != NULL)
    {
        free_blocks[order]->prev_free = block;
        block->next_free = free_blocks[order];
    }
    free_blocks[order] = block;

    // mark the block as free
    block->free = TRUE;
}

/**
 * @brief Splits a block into two, marking the original as used and the
 * two new blocks as free.
 * 
 * @param block A pointer to the block
 * @return block_hdr* A pointer to the left new block
 */
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

/**
 * @brief Find whether the given block is the left buddy.
 * 
 * @param block A pointer to the block
 * @return true The block is the left buddy
 * @return false The block is the right buddy
 */
bool is_left(block_hdr *block)
{
    // if the block is on the left, the modulos of its offset from
    // the heap start by the size of the parent block should be 0
    return ((size_t)block - (size_t)heap_base) % (BLOCK_SIZE(block->order + 1)) == 0;
}

/**
 * @brief Find whether the block is valid.
 * 
 * @param block A pointer to the block
 * @return true The block is valid
 * @return false The block is invalid
 */
bool is_valid(block_hdr *block)
{
    // the block is out of the heap bounds
    if ((size_t)block < (size_t)heap_base
        || (size_t)block >= (size_t)heap_base + BLOCK_SIZE(MAX_ORDER))
        return FALSE;

    // the modulos of the offset from the heap start by the block size should be 0
    return ((size_t)block - (size_t)heap_base) % BLOCK_SIZE(block->order) == 0;
}

/**
 * @brief Get the buddy of the given block.
 * 
 * @param block A pointer to the block
 * @return block_hdr* A pointer to the buddy block
 */
block_hdr *get_buddy(block_hdr *block)
{
    block_hdr *buddy;

    // this is the left block - buddy is on the right
    if (is_left(block))
        buddy = (block_hdr*)((size_t)block + BLOCK_SIZE(block->order));
    // buddy is on the left
    else
        buddy = (block_hdr*)((size_t)block - BLOCK_SIZE(block->order));
    
    return buddy;
}

/**
 * @brief Coalesce a block with its buddy, marking the two as used
 * and the new coalesced block as free.
 * 
 * @param block A pointer to the block that is to be coalesced with its buddy
 * @return block_hdr* A pointer to the new coalesced block
 */
block_hdr *coalesce_block(block_hdr *block)
{
    // make sure the block isn't of the highest order
    if (block->order == MAX_ORDER)
        return block;

#ifdef __DEBUG_KERNEL_HEAP__
    block_hdr *buddy = get_buddy(block);
    log("MM", LOG_DEBUG, "HEAP", "Coalescing blocks of order %d at 0x%p and 0x%p",
        (int)block->order, block, buddy);
#endif

    // remove this block and its buddy from the free list
    remove_from_free_list(block);
    remove_from_free_list(get_buddy(block));

    // if this is the right block, change the pointer to the left block
    if (!is_left(block))
        block = (block_hdr*)((size_t)block - BLOCK_SIZE(block->order));
    
    // increment the order of this block, and set it as free
    block->order++;
    add_to_free_list(block);

    // return the updated block
    return block;
}

/**
 * @brief Allocate memory on the kernel heap.
 * We use a memory allocation method that is based on the buddy memory allocation algorithm
 * (see https://en.wikipedia.org/wiki/Buddy_memory_allocation)
 * The heap is managed in much the same way as the buddy system, but the data structures
 * and routines used to implement it are different.
 * 
 * @param size The requested size to allocate
 * @return void* A pointer to the allocated block (this actually points to after the block header)
 */
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

/**
 * @brief Frees a block previously allocated by kmalloc.
 * 
 * @param ptr A pointer to the block, as obtained by kmalloc
 */
void kfree(const void *ptr)
{
    // make sure the pointer is not NULL
    if (ptr == NULL)
        return;
    
    // get the block that is pointed to
    block_hdr *block = (block_hdr*)((size_t)ptr - sizeof(block_hdr));

    // invalid block
    if (block->order > MAX_ORDER || !is_valid(block))
    {
        log("MM", LOG_WARNING, "HEAP", "Attempt to free invalid block at address 0x%p", block);
        return;
    }

    // free the block
    add_to_free_list(block);

    // remember the original order
    uint8 orig_order = block->order;

    // try coalescing the block
    block_hdr *current_block = block;
    while (current_block->order < MAX_ORDER)
    {
        // buddy is not free - can't coalesce any further
        if (!(get_buddy(current_block)->free))
            break;
        
        // coalesce the block with its buddy
        current_block = coalesce_block(current_block);
    }

#ifdef __DEBUG_MEMORY__
        log("MM", LOG_DEBUG, "HEAP", "Freed block of order %d at address 0x%p",
            (int)orig_order, block);
#endif
    return;
}