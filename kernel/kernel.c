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
    log("SYS", 6, "INIT", "Installing ISRs");
    install_isrs();

    // install IRQs
    log("SYS", 6, "INIT", "Installing IRQs");
    install_irqs();

    // setup PIC
    log("SYS", 6, "INIT", "Remapping the PIC");
    setup_pic();

    // setup the IDT
    log("SYS", 6, "INIT", "Loading kernel IDT");
    setup_idt();

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