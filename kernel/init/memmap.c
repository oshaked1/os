#include "memmap.h"
#include "../../arch/x86/realmode_services.h"
#include "irq.h"
#include "../syslog.h"

// this is not ideal but we assume there won't be more than 20 entries
// from observation, using QEMU no more than 7 entries are returned
struct memory_region memmap[20];

void obtain_memmap()
{
    uint32 len;

    // set destination buffer
    asm volatile ("mov %0, %%edi" :: "r"(&memmap));

    // set service and function
    asm volatile ("mov %0, %%ah" :: "i"(SERVICE_MEMMAP));
    asm volatile ("mov %0, %%al" :: "i"(OBTAIN_MEMMAP));

    // call real mode services
    asm volatile ("int %0" :: "i"(IRQ_REALMODE_SERVICES));

    // save buffer length
    asm volatile ("mov %%ecx, %0" : "=g"(len));

    int i;
    for (i = 0; i < len / 24; i++)
    {
        // this print doesn't work yet because my vsnprintf implementation does not support 64 bit integers
        //debug("Base: 0x%llx  Length: 0x%llx  Type: %d  ACPI Attrs: 0x%x", memmap[i].base, memmap[i].len, memmap[i].type, memmap[i].acpi_attrs);

        debug("Base High: 0x%x  Base Low: 0x%x  Length High: 0x%x  Length Low: 0x%x  Type: %d  ACPI Attrs: 0x%x", (uint32)(memmap[i].base >> 32) && 0xffffffff,
            (uint32)memmap[i].base & 0xffffffff, (uint32)(memmap[i].len >> 32) & 0xffffffff, (uint32)memmap[i].len & 0xffffffff, memmap[i].type, memmap[i].acpi_attrs);
    }
}