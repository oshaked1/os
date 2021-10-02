#include "../lib/types.h"

#define MIN_BLOCK_SIZE 4096
#define MAX_ORDER 10
#define MAX_BLOCK_SIZE (MIN_BLOCK_SIZE << MAX_ORDER)
#define BLOCK_SIZE(order) (MIN_BLOCK_SIZE << (order))

void *kmalloc(size_t size);