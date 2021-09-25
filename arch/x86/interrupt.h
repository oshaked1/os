#include "../../lib/types.h"
#include "idt.h"

#ifndef _INTERRUPT_INFO
#define _INTERRUPT_INFO
typedef struct interrupt_info
{
    uint32 ds; // data segment selector
    uint32 edi, esi, ebp, esp, ebx, edx, ecx, eax; // pushed by pusha
    uint32 interrupt_number, error_code; // pushed by specific interrupt handler
    uint32 eip, cs, eflags, useresp, ss; // pushed by the processor automatically
} interrupt_info;
#endif

void setup_idt();
void register_interrupt_handler(uint8 interrupt_number, void *handler);