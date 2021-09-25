#include "realmode_services.h"
#include "interrupt.h"
#include "../../kernel/syslog.h"

void realmode_service_call(interrupt_info info)
{
    // set up service request packet
    struct realmode_service_packet pkt;
    pkt.service = info.eax;
    pkt.function = info.ebx;
    pkt.input_buffer = info.esi;
    pkt.output_buffer = info.edi;

#ifdef __DEBUG__
    debug("Entering real mode service 0x%x function 0x%x", pkt.service, pkt.function);
#endif

#ifdef __REALMODE_DEBUG__
#include "../../drivers/screen.h"
    // clear screen so that real mode prints don't overwrite already printed lines
    kcls();
#endif

    // get IDT descriptor for real mode service call
    extern struct idt_desc idt_desc;

    // enter real mode service handler
    realmode_service_entry(&idt_desc, pkt);

#ifdef __DEBUG__
    debug("Back from real mode service 0x%x function 0x%x", pkt.service, pkt.function);
#endif
}