#include "../libc/types.h"
#include "../libc/string.h"
#include "../libc/stdlib.h"
#include "../drivers/screen.h"
#include "../arch/x86/interrupt.h"
#include "syslog.h"
#include "init/isr.h"

#define __DEBUG__

void test_handler()
{
    debug("entered test handler");
}

void main()
{
#ifdef __DEBUG__
    set_log_dst_severity(DST_SCREEN, LOG_DEBUG);
#endif

    log("SYS", 5, "INIT", "Kernel started");

    // setup the IDT
    setup_idt();
    log("SYS", 6, "INIT", "IDT has been loaded");

    // install ISRs
    install_isrs();
    log("SYS", 6, "INIT", "ISRs have been installed");

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
    debug("doing int 60, which doesn't exist");
    asm volatile ("int $60");
    debug("returned to kernel");
}