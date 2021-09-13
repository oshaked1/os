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

#include "../drivers/keyboard.h"

#define __DEBUG__

void kmain()
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

    // install IRQs
    install_irqs();
    log("SYS", 6, "INIT", "IRQs have been installed");

    // setup PIC
    setup_pic();
    log("SYS", 6, "INIT", "PIC has been remapped");
}