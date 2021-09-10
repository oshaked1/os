#include "isr.h"
#include "../../libc/types.h"
#include "../../libc/stdio.h"
#include "../../arch/x86/interrupt.h"
#include "../panic.h"

/**
 * @brief General protection faults can happen because a myriad of reasons.
 * For now, we are not handling any error.
 * Problem is, whenever a fatal error occurs (e.g. an interrupt with a null IDT entry happened),
 * this exception is raised instead. We cannot simply ignore it, because it is a fatal error,
 * and also the saved instruction pointer is the one that raised the fault (and not the next one)
 * which means we will be stuck in an infinite exception loop.
 * So, this handler will panic the kernel.
 */
void general_protection_fault(interrupt_info info)
{
    char str[100];
    snprintf(str, 100, "a general protection fault has occurred at address 0x%x", info.eip);
    kernel_panic(str);
}

void install_isrs()
{
    register_interrupt_handler(13, general_protection_fault);
}