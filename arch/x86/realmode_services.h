#include "../../lib/types.h"
#include "interrupt.h"

// FOR VSCODE - actual definitions are passed as arguments at compile time
#ifdef __VSCODE__
#define DISK_LOAD_ADDRESS -1
#endif

#define REALMODE_INPUT_BUFFER 0x7e00
#define REALMODE_MAX_BUFFER   (0x10000-0x7e00)

#ifndef _SERVICES
#define _SERVICES
enum SERVICES{SERVICE_MEMMAP=0};
#endif

#ifndef _MEMMAP_FUNCS
#define _MEMMAP_FUNCS
enum MEMMAP_FUNCS{OBTAIN_MEMMAP=0};
#endif

#define ERROR_BUFFER_TOO_LARGE 1

struct realmode_service_packet
{
    uint8 service;        // AH
    uint8 function;       // AL
    uint32 arg1;          // EBX
    uint32 buf_len;       // ECX
    uint32 arg2;          // EDX
    uint16 input_buffer;  // contents copied from buffer pointed to by ESI
    uint16 output_buffer; // contents copied to buffer pointed to by EDI
} __attribute__((packed));

extern void realmode_service_entry(struct idt_desc *kernel_idt, struct realmode_service_packet pkt);
void realmode_service_call();