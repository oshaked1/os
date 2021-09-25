#include "../../lib/types.h"
#include "interrupt.h"

struct realmode_service_packet
{
    uint32 service;       // EAX
    uint32 function;      // EBX
    uint32 input_buffer;  // ESI
    uint32 output_buffer; // EDI
} __attribute__((packed));

extern void realmode_service_entry(struct idt_desc *kernel_idt, struct realmode_service_packet pkt);
void realmode_service_call();