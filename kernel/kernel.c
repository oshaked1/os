#include "../lib/types.h"
#include "../lib/string.h"
#include "../lib/stdlib.h"
#include "../lib/stdio.h"
#include "../drivers/screen.h"
#include "../arch/x86/interrupt.h"
#include "syslog.h"
#include "init/isr.h"
#include "init/irq.h"
#include "../drivers/pic.h"

void kmain()
{
#ifdef __DEBUG__
    set_log_dst_severity(DST_SCREEN, LOG_DEBUG);
#endif

    // initialize screen before printing anything
    init_screen();

    log("SYS", 5, "INIT", "Kernel started");

    // install ISRs
    install_isrs();
    log("SYS", 6, "INIT", "ISRs have been installed");

    // install IRQs
    install_irqs();
    log("SYS", 6, "INIT", "IRQs have been installed");

    // setup PIC
    setup_pic();
    log("SYS", 6, "INIT", "PIC has been remapped");

    // setup the IDT
    setup_idt();
    log("SYS", 6, "INIT", "IDT has been loaded");

    // test real mode services
    debug("Performing int 0x%x", IRQ_REALMODE_SERVICES);
    asm volatile ("mov $5, %eax"); // service 5
    asm volatile ("mov $8, %ebx"); // function 8
    asm volatile ("mov $0x1234abcd, %esi"); // input buffer 0x1234abcd
    asm volatile ("mov $0x2345bcde, %edi"); // output buffer 0x2345bcde

    asm volatile ("int %0" :: "i"(IRQ_REALMODE_SERVICES));

    debug("Back from IRQ");
}