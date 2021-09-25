#include "../../lib/types.h"
#include "interrupt.h"

struct realmode_service_packet
{
    uint32 arg1;
    uint16 arg2;
    uchar arg3;
} __attribute__((packed));

extern void realmode_service_entry(struct idt_desc *kernel_idt, struct realmode_service_packet pkt);
void realmode_service_call();