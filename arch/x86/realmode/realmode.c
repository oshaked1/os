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
#endif

    switch (pkt.service)
    {
        case SERVICE_MEMMAP:
            switch (pkt.function)
            {
                case OBTAIN_MEMMAP:
                    obtain_memmap(pkt);
                    break;
            }
            break;
        
        default:
            break;
    }

    // return to protected mode with service_call_return as jump target
    switch_protected_mode(SERVICE_RETURN_ADDRESS);
}