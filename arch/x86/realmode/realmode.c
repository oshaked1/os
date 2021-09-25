#include "print.h"
#include "load_kernel.h"
#include "service_call.h"
#include "pm.h"

void realmode_main()
{
    prints("%BOOT-5-BOOTLDR: Second stage bootloader started\r\n");

    // load kernel into memory
    load_kernel();
}

void service_call_main(struct service_packet pkt)
{
#ifdef __REALMODE_DEBUG__
    prints("Entered service call main!\r\n");
    prints("service is 0x");
    printh(pkt.service);
    prints("\r\nfunction is 0x");
    printh(pkt.function);
    prints("\r\nsource buffer is 0x");
    printh(pkt.input_buffer);
    prints("\r\ndestination buffer is 0x");
    printh(pkt.output_buffer);
    prints("\r\n");
#endif

    // return to protected mode with service_call_return as jump target
    switch_protected_mode(SERVICE_RETURN_ADDRESS);
}