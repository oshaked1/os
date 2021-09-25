#include "realmode_services.h"
#include "../../kernel/syslog.h"

#include "../../drivers/screen.h"

void realmode_service_call()
{
    debug("Entering real mode for service call");

    struct realmode_service_packet pkt;
    pkt.arg1 = 0x1234abcd;
    pkt.arg2 = 0xc0de;
    pkt.arg3 = '!';
    
    // clear screen beacause real mode debugging output overwrites text written by the kernel
    // (BIOS does not use the cursor to track print location)
    kcls();
    
    realmode_service_entry(pkt);
}