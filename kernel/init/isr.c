#include "isr.h"
#include "../../libc/types.h"
#include "../../arch/x86/interrupt.h"
#include "../panic.h"

/**
 * @brief General protection exceptions can happen because a myriad of reasons.
 * For now, we are not handling any error.
 * Problem is, whenever a fatal error occurs (e.g. an interrupt with a null IDT entry happened),
 * this exception is raised instead. We cannot simply ignore it, because it is a fatal error,
 * and also the saved instruction pointer is the one that raised the fault (and not the next one)
 * which means we will be stuck in an infinite exception loop.
 * So, this handler will panic the kernel.
 */
void general_protection_exception()
{
    kernel_panic("a general protection exception has occurred");
}

void install_isrs()
{
    register_interrupt_handler(13, general_protection_exception);
}