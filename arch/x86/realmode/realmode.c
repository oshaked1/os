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

// currently only shows debug output and returns
void service_call_main(service_packet pkt)
{
#ifdef __REALMODE_DEBUG__
    prints("Entered service call main!\r\n");
    prints("service is 0x");
    printh(pkt.service);
    prints("\r\nfunction is 0x");
    printh(pkt.function);
    prints("\r\ninput buffer at 0x");
    printh(pkt.input_buffer);
    uint32 *in_buf = (uint32*)(uint32)pkt.input_buffer;
    prints(" contains 0x");
    printh(in_buf[0]);
    prints(", 0x");
    printh(in_buf[1]);
    prints("\r\ndestination buffer is at 0x");
    printh(pkt.output_buffer);
    prints("\r\n");

    // set output registers
    saved_eax = 0x1234;
    saved_ebx = 0xabcd1234;
    saved_ecx = 8; // output buffer length
    
    // fill output buffer
    uint32 *out_buf = (uint32*)(uint32)pkt.output_buffer;
    out_buf[0] = 0xdeadc0de;
    out_buf[1] = 0xdefeca8e;
#endif

    // return to protected mode with service_call_return as jump target
    switch_protected_mode(SERVICE_RETURN_ADDRESS);
}