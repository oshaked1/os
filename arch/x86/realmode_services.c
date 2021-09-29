#include "realmode_services.h"
#include "interrupt.h"
#include "../../lib/string.h"
#include "../../kernel/syslog.h"

/**
 * @brief This function serves as an interrupt handler for realmode service calls.
 * A real mode service call uses the following registers to pass information to the real mode service handler:
 *  - AH  : the requested service
 *  - AL  : the requested function within the service
 *  - EBX : a general argument to the service handler
 *  - ECX : the length of the input buffer, if applicable
 *  - EDX : another general argument
 *  - ESI : the memory address of the input buffer, if applicable
 *  - EDI : the memory address of the output buffer, if applicable
 * 
 * The contents of these registers are placed in a request packet which is passed to the real mode handler
 * on the kernel stack (the packet is copied to the real mode stack by the service request entry function).
 * 
 * Because the input and output buffer locations may reside in addresses that are not accessible in real mode,
 * this interrupt handler copies the input buffer to an accessible location, and passes the updated input buffer
 * address as well as an updated output buffer address that is accessible in real mode in the request packet.
 * After returning from real mode, it copies the output buffer from the real mode accessible location to the
 * location specified in the original request.
 * 
 * The real mode service handler may return values in the general purpose registers, which can be used by the
 * caller. EDX, as a convention, contains an error code, while ECX contains the length of the output buffer.
 * 
 * @param info The interrupt info struct that is passed by the interrupt handler
 */
void realmode_service_call(interrupt_info *info)
{
    // set up service request packet
    struct realmode_service_packet pkt;
    pkt.service = (info->eax >> 8) & 0xff;
    pkt.function = info->eax & 0xff;
    pkt.arg1 = info->ebx;
    pkt.arg2 = info->edx;

    // check if the requested function receives an input buffer
    switch (pkt.service)
    {
        // placeholder - currently no services receive an input buffer
        case 0xff:
        case 5: // DEBUG SERVICE
            goto input_buffer;

        // no input buffer
        default:
            goto no_input_buffer;
    }

input_buffer:
    pkt.buf_len = info->ecx;

    // if input buffer length is too large, set EDX (error code) and return
    if (pkt.buf_len > REALMODE_MAX_BUFFER)
    {
        info->edx = ERROR_BUFFER_TOO_LARGE;
        return;
    }

    // copy input buffer to real mode input buffer location
    void *realmode_input_buf = (void*)REALMODE_INPUT_BUFFER;
    memcpy(realmode_input_buf, (void*)info->esi, pkt.buf_len);

    // set real mode buffer locations
    pkt.input_buffer = REALMODE_INPUT_BUFFER;

    // if there is an input buffer, the output buffer will come right after
    pkt.output_buffer = REALMODE_INPUT_BUFFER + pkt.buf_len;

    goto enter_realmode;

no_input_buffer:
    // set up default values
    pkt.buf_len = 0;
    pkt.input_buffer = 0;

    // if there is no input buffer, the output buffer will be at the default real mode buffer location
    pkt.output_buffer = REALMODE_INPUT_BUFFER;

enter_realmode:
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

    // save registers
    asm volatile ("mov %%eax, %0" : "=g"(info->eax));
    asm volatile ("mov %%ebx, %0" : "=g"(info->ebx));
    asm volatile ("mov %%ecx, %0" : "=g"(info->ecx));
    asm volatile ("mov %%edx, %0" : "=g"(info->edx));
    asm volatile ("mov %%esi, %0" : "=g"(info->esi));
    asm volatile ("mov %%edi, %0" : "=g"(info->edi));

#ifdef __DEBUG__
    debug("Back from real mode service 0x%x function 0x%x", pkt.service, pkt.function);
#endif

    // check if the requested function generated an output buffer
    switch (pkt.service)
    {
        // output buffer
        case 5: // DEBUG SERVICE
        case SERVICE_MEMMAP:
            goto output_buffer;
        
        // no output buffer
        default:
            goto end;
    }

output_buffer:
    // copy real mode output buffer to location specified in the original request
    memcpy((void*)info->edi, (void*)(size_t)pkt.output_buffer, info->ecx);

end:
#ifdef __DEBUG__
    debug("output registers: eax: 0x%x, ebx: 0x%x, ecx: 0x%x, edx: 0x%x, esi: 0x%x, edi: 0x%x",info->eax,info->ebx,info->ecx,info->edx,info->esi,info->edi);
#endif
    return;
}