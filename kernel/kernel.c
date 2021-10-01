#include "../lib/types.h"
#include "../lib/string.h"
#include "../lib/stdlib.h"
#include "../lib/stdio.h"
#include "../drivers/screen.h"
#include "../arch/x86/interrupt.h"
#include "syslog.h"
#include "isr.h"
#include "irq.h"
#include "../drivers/pic.h"
#include "../mm/memmap.h"

void kernel_init()
{
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

    // obtain a memory map
    log("SYS", 6, "INIT", "Detecting physical memory");
    obtain_bios_memmap();

    // initialize kernel memory regions
    log("SYS", 6, "INIT", "Initializing kernel memory layout");
    init_kernel_memmap();
}

void kmain()
{
#ifdef __DEBUG__
    set_log_dst_severity(DST_SCREEN, LOG_DEBUG);
#endif

    // initialize screen before printing anything
    init_screen();

    log("SYS", 5, "INIT", "Kernel started");

    // perform initialization tasks
    kernel_init();
}