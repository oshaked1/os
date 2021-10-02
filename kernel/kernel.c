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
#include "../mm/kernel_heap.h"

void kernel_init()
{
    // install ISRs
    log("SYS", LOG_INFO, "INIT", "Installing ISRs");
    install_isrs();

    // install IRQs
    log("SYS", LOG_INFO, "INIT", "Installing IRQs");
    install_irqs();

    // setup PIC
    log("SYS", LOG_INFO, "INIT", "Remapping the PIC");
    setup_pic();

    // setup the IDT
    log("SYS", LOG_INFO, "INIT", "Loading kernel IDT");
    setup_idt();

    // obtain a memory map
    log("SYS", LOG_INFO, "INIT", "Detecting physical memory");
    obtain_bios_memmap();

    // initialize kernel memory regions
    log("SYS", LOG_INFO, "INIT", "Initializing kernel memory layout");
    init_kernel_memmap();
}

void kmain()
{
#ifdef __DEBUG__
    set_log_dst_severity(DST_SCREEN, LOG_DEBUG);
#endif

    // initialize screen before printing anything
    init_screen();

    log("SYS", LOG_NOTICE, "INIT", "Kernel started");

    // perform initialization tasks
    kernel_init();

    // test kmalloc - last few allocations should fail
    int i;
    for(i=0;i<1030;i++)
    {
        void *block = kmalloc(1000);
    }
    debug("done!");
}