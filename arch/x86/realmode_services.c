#include "realmode_services.h"
#include "interrupt.h"
#include "../../kernel/syslog.h"

#include "../../drivers/screen.h"

void realmode_service_call()
{
    debug("Entering real mode for service call");

    struct realmode_service_packet pkt;
    pkt.arg1 = 0x1EE7C0DE;
    pkt.arg2 = 0xCAFE;
    pkt.arg3 = '!';

    debug("Passing service packet with args 0x%x, 0x%x, '%c'", pkt.arg1, pkt.arg2, pkt.arg3);
    
    // clear screen beacause real mode debugging output overwrites text written by the kernel
    // (BIOS does not use the cursor to track print location)
    kcls();
    
    extern struct idt_desc idt_desc;
    realmode_service_entry(&idt_desc, pkt);

    debug("Back in the kernel!");
}