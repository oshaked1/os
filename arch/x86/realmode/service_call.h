#include "types.h"

// FOR VSCODE - actual definitions are passed as arguments at compile time
#ifdef __VSCODE__
#define REALMODE_LOAD_ADDRESS -1
#endif

#define SERVICE_RETURN_ADDRESS (REALMODE_LOAD_ADDRESS + 0x200)

struct service_packet
{
    uint32 service;
    uint32 function;
    uint32 input_buffer;
    uint32 output_buffer;
} __attribute__((packed));