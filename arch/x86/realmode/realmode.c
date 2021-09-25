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
    prints("Entered service call main!\r\n");
    prints("arg1 is 0x");
    printh(pkt.arg1);
    prints("\r\narg2 is 0x");
    printh(pkt.arg2);
    prints("\r\narg3 is '");
    printc(pkt.arg3);
    prints("'\r\n");

    // return to protected mode with service_call_return as jump target
    switch_protected_mode(SERVICE_RETURN_ADDRESS);
}