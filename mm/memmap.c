#include "memmap.h"
#include "../arch/x86/realmode_services.h"
#include "../kernel/irq.h"
#include "../kernel/syslog.h"
#include "../kernel/panic.h"

// this is not ideal but we assume there won't be more than 20 entries
// from observation, using QEMU no more than 7 entries are returned
struct bios_memory_region bios_memmap[20];

// save number of entries after we obtain the memory map
uint bios_memmap_entries;

// save start and end of free contiguous memory
uint64 freemem_start;
uint64 freemem_end;
uint64 freemem_size;

/* 
The following is an example of the kernel memory layout:

+-------------------------------+ 0x00000000
|        Realmode memory        |
|        +----------------------+ 0x0007e000
|        | Initial kernel stack |
|        +----------------------+ 0x00080000
+-------------------------------+ 0x00100000 - 1MB
|          Kernel image         |
+-------------------------------+ 0x00200000
|          Kernel heap          |
+-------++--------++--------++--+
|       ||        ||        ||  |
|  /\   \/   /\   \/   /\   \/  |
|  ||        ||        ||       |
+--++--------++--------++-------+
|            Stacks             |
+-------------------------------+ 0x08000000 - end of physical memory

It contains 5 regions, which is the size of our memory region array.
*/
#define MEMMAP_ENTRIES 5
memory_region memmap[MEMMAP_ENTRIES];

void obtain_bios_memmap()
{
    uint32 len;

    // set destination buffer
    asm volatile ("mov %0, %%edi" :: "r"(&bios_memmap));

    // set service and function
    asm volatile ("mov %0, %%ah" :: "i"(SERVICE_MEMMAP));
    asm volatile ("mov %0, %%al" :: "i"(OBTAIN_MEMMAP));

    // call real mode services
    asm volatile ("int %0" :: "i"(IRQ_REALMODE_SERVICES));

    // save buffer length
    asm volatile ("mov %%ecx, %0" : "=g"(len));

    // save number of entries
    bios_memmap_entries = len / 24;

    int i;
    for (i = 0; i < bios_memmap_entries; i++)
        debug("Base: 0x%llx\tLength: 0x%llx (%lluKB)\tType: %d\tACPI Attrs: 0x%x",
            bios_memmap[i].base, bios_memmap[i].len, bios_memmap[i].len / 1024,
            bios_memmap[i].type, bios_memmap[i].acpi_attrs);
}

bool address_range_usable(uint64 start, uint64 end)
{
    int i;
    for (i = 0; i < bios_memmap_entries; i++)
    {
        if (bios_memmap[i].type == TYPE_USABLE
            && start >= bios_memmap[i].base
            && end <= bios_memmap[i].base + bios_memmap[i].len)
            return TRUE;
    }
    return FALSE;
}

void find_free_memory()
{
    int largest_index = 0;
    uint64 largest = 0;

    int i;
    for (i = 0; i < bios_memmap_entries; i++)
    {
        if (bios_memmap[i].type == TYPE_USABLE
            && bios_memmap[i].base >= KERNEL_LOAD_ADDRESS
            && bios_memmap[i].len > largest)
        {
            largest = bios_memmap[i].len;
            largest_index = i;
        }
    }

    freemem_start = bios_memmap[largest_index].base;
    freemem_end = freemem_start + bios_memmap[largest_index].len;
    freemem_size = freemem_end - freemem_start;

    // if this memory region contains the kernel image, subtract its size from the size
    // of free memory and update the free memory start address
    if (freemem_start <= KERNEL_LOAD_ADDRESS && KERNEL_LOAD_ADDRESS <= freemem_end)
    {
        freemem_size -= KERNEL_SIZE;
        freemem_start  = KERNEL_LOAD_ADDRESS + KERNEL_SIZE;
    }
}

/**
 * @brief Initialize a memory map of the kernel.
 * NOTE: this routine assumes a traditional memory layout (entire contiguous range
 * from 1MB to a bit before the end of physical memory is available).
 * Also, only up to ~3GB of physical memory can be used as of now
 * (because when memory is larger than this it is split into different regions).
 */
void init_kernel_memmap()
{
    // realmode memory is always at 0x00000000 - 0x00100000
    memmap[0].base = 0;
    memmap[0].len  = 0x100000;
    memmap[0].type = TYPE_REALMODE;

    // initial kernel stack location is determined by compile-time arguments
    memmap[1].base = PROTECTED_MODE_STACK - DEFAULT_STACK_SIZE;
    memmap[1].len  = DEFAULT_STACK_SIZE;
    memmap[1].type = TYPE_STACK;

    // make sure this address range is actually available
    // (it should be considering out current stack is located here)
    if (!address_range_usable(memmap[1].base, PROTECTED_MODE_STACK))
        kernel_panic("Address range from 0x%x to 0x%x is not usable (needed for initial kernel stack)",
                        memmap[1].base, PROTECTED_MODE_STACK);

    // kernel image load address and size is determined by compile-time arguments
    memmap[2].base = KERNEL_LOAD_ADDRESS;
    memmap[2].len  = KERNEL_SIZE;
    memmap[2].type = TYPE_KERNEL_IMAGE;

    // make sure this address range is available (if it is not, something fucky is going on
    // because BIOS successfully loaded the entire image into this address range)
    if (!address_range_usable(KERNEL_LOAD_ADDRESS, KERNEL_LOAD_ADDRESS + KERNEL_SIZE))
        kernel_panic("Address range from 0x%x to 0x%x is not usable (needed for kernel image)",
            KERNEL_LOAD_ADDRESS, KERNEL_LOAD_ADDRESS + KERNEL_SIZE);
    
    // make sure we have enough free memory for kernel heap and stacks
    find_free_memory();
    if (freemem_size < MIN_FREE_MEM)
    {
        uint freemem_kb = (uint)freemem_size / 1024;
        uint req_mem_kb = MIN_FREE_MEM / 1024;
        kernel_panic("Not enough contiguous memory is available (%uKB detected, %uKB required)",
            freemem_kb, req_mem_kb);
    }
    
    // heap starts immediately after kernel image (on a page boudary) and its initial size is 1/10th of available memory
    if (freemem_start % PAGE_SIZE == 0)
        memmap[3].base = freemem_start;
    else
        memmap[3].base = freemem_start + (PAGE_SIZE - (freemem_start % PAGE_SIZE));
    memmap[3].len  = (freemem_size / 10) + (PAGE_SIZE - ((freemem_size / 10) % PAGE_SIZE));
    memmap[3].type = TYPE_KERNEL_HEAP;

    // stack region ends at the end of free memory and its initial size is 1/10th of available memory
    uint64 stack_end = freemem_end - (freemem_end % PAGE_SIZE);
    memmap[4].len  = memmap[3].len;
    memmap[4].base = stack_end - memmap[4].len;
    memmap[4].type = TYPE_STACK;

    // print memory regions for debugging
    int i;
    for (i = 0; i < MEMMAP_ENTRIES; i++)
    {
        debug("Base: 0x%llx\tLength: 0x%llx (%lluKB)\tType: %u",
            memmap[i].base, memmap[i].len, memmap[i].len / 1024, memmap[i].type);
    }
}