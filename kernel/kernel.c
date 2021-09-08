#include "../libc/types.h"
#include "../libc/string.h"
#include "../libc/stdlib.h"
#include "../drivers/screen.h"
#include "../arch/x86/interrupt.h"
#include "syslog.h"

void test_handler()
{
    debug("entered test handler");
}

void main()
{
    log("SYS", 5, "INIT", "Kernel started");

    // setup the IDT
    setup_idt();
    log("SYS", 6, "INIT", "IDT has been loaded");

    // show that interrupts work
    debug("registering handler for int 50");
    register_interrupt_handler(50, test_handler);
    debug("doing int 3");
    asm volatile ("int $3");
    debug("returned to kernel");
    debug("doing int 15");
    asm volatile ("int $15");
    debug("returned to kernel");
    debug("doing int 50");
    asm volatile ("int $50");
    debug("returned to kernel");
}