#include "../lib/types.h"

// FOR VSCODE - actual definitions are passed as arguments at compile time
#ifdef __VSCODE__
#define PROTECTED_MODE_STACK -1
#define KERNEL_LOAD_ADDRESS -1
#define KERNEL_SIZE -1
#endif

#define DEFAULT_STACK_SIZE 0x2000 // 8KB
#define MIN_FREE_MEM 0x1000000 // 16MB
#define PAGE_SIZE 4096 // 4KB
#define KERNEL_HEAP_SIZE 0x400000 // 4MB

#ifndef _BIOS_REGION_TYPE
#define _BIOS_REGION_TYPE
enum BIOS_REGION_TYPE{TYPE_USABLE=1, TYPE_RESERVED=2, TYPE_ACPI_RECLAIMABLE=3, TYPE_ACPI_NVS=4, TYPE_BAD=5};
#endif

#ifndef _BIOS_MEMORY_REGION
#define _BIOS_MEMORY_REGION
struct bios_memory_region
{
    uint64 base;
    uint64 len;
    uint32 type;
    uint32 acpi_attrs;
} __attribute__((packed));
#endif

#ifndef _KERNEL_REGION_TYPE
#define _KERNEL_REGION_TYPE
typedef enum KERNEL_REGION_TYPE{TYPE_REALMODE, TYPE_STACK, TYPE_KERNEL_IMAGE, TYPE_KERNEL_HEAP, TYPE_PAGE_FRAME_POOL} region_type;
#endif

#ifndef _KERNEL_MEMORY_REGION
#define _KERNEL_MEMORY_REGION
typedef struct memory_region
{
    uint64 base;
    uint64 len;
    region_type type;
} memory_region;
#endif

void obtain_bios_memmap();
void init_kernel_memmap();
void *kernel_heap_base();