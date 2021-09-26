#include "types.h"

// FOR VSCODE - actual definitions are passed as arguments at compile time
#ifdef __VSCODE__
#define REALMODE_LOAD_ADDRESS -1
#endif

#define SERVICE_RETURN_ADDRESS (REALMODE_LOAD_ADDRESS + 0x200)

struct service_packet
{
    uint8 service;
    uint8 function;
    uint32 arg1;
    uint32 buf_len;
    uint32 arg2;
    uint16 input_buffer;
    uint16 output_buffer;
} __attribute__((packed));

extern uint32 saved_eax, saved_ebx, saved_ecx, saved_edx, saved_esi, saved_edi;