#include "service_call.h"
#include "print.h"

// See https://wiki.osdev.org/Detecting_Memory_(x86) for detaield explanation on the following routines

void obtain_memmap(service_packet pkt)
{
    uint16 current_offset = 0;
    uint32 eax, ebx = 0;
    while (TRUE)
    {
#ifdef __REALMODE_DEBUG__
        prints("current offset is 0x");
        printh(current_offset);
        prints("\r\n");
#endif
        // point ES:DI to the output buffer
        asm volatile ("mov $0, %eax");
        asm volatile ("mov %eax, %es");
        asm volatile ("mov %0, %%edi" :: "r"(pkt.output_buffer + current_offset));

        // set EBX
        asm volatile ("mov %0, %%ebx" :: "r"(ebx));

        // set EDX to 0x534D4150 ("SMAP")
        asm volatile ("mov $0x534d4150, %edx");

        // set EAX to 0xE820 (specific memory map function)
        asm volatile ("mov $0xe820, %eax");

        // set ECX to 24
        asm volatile ("mov $24, %ecx");

        // perform int 0x15 (BIOS memory services)
        asm volatile ("int $0x15");

        // save EAX and EBX
        asm volatile ("mov %%eax, %0" : "=g"(eax) :: "ebx");
        asm volatile ("mov %%ebx, %0" : "=g"(ebx));

        // if carry flag is set, this entry is not valid
        asm goto ("jc %l[done]" :::: done);

        // make sure EAX is 0x534D4150 ("SMAP")
        if (eax != 0x534d4150)
            break;

        // update current offset
        current_offset += 24;

        if (ebx == 0)
            break;
    }

done:
    // place buffer length in ECX
    saved_ecx = current_offset;
    // set error code to 0
    saved_edx = 0;
    return;
}