#include "../../libc/types.h"
#include "../../kernel/syslog.h"
#include "../../libc/stdlib.h"
#include "idt.h"

struct interrupt_info
{
    uint32 ds; // data segment selector
    uint32 edi, esi, ebp, esp, ebx, edx, ecx, eax; // pushed by pusha
    uint32 interrupt_number, error_code; // pushed by specific interrupt handler
    uint32 eip, cs, eflags, useresp, ss; // pushed by the processor automatically
};

void setup_idt();
void register_interrupt_handler(uint8 interrupt_number, void *handler);