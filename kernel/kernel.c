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
    uint32 in_buf[2];
    in_buf[0] = 0xdeadbeef;
    in_buf[1] = 0xcafebabe;
    uint32 *out_buf;
    asm volatile ("mov $5, %ah"); // service 5
    asm volatile ("mov $8, %al"); // function 8
    asm volatile ("mov $8, %ecx"); // input buffer length
    asm volatile ("mov %0, %%esi" :: "r"(&in_buf) : "eax"); // input buffer
    asm volatile ("mov %0, %%edi" :: "r"(&out_buf): "eax"); // output buffer

    asm volatile ("int %0" :: "i"(IRQ_REALMODE_SERVICES));
    uint32 ecx;
    asm volatile ("mov %%ecx, %0" : "=g"(ecx));

    debug("Back from IRQ");
    debug("ecx: 0x%x, out[0]: 0x%x, out[1]: 0x%x", ecx, out_buf[0], out_buf[1]);
}