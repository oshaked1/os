#include "types.h"

// FOR VSCODE - actual definitions are passed as arguments at compile time
#ifdef __VSCODE__
#define REALMODE_LOAD_ADDRESS -1
#endif

#define SERVICE_RETURN_ADDRESS (REALMODE_LOAD_ADDRESS + 0x200)

// test service packet - fields will be different later on
struct service_packet
{
    uint32 arg1;
    uint16 arg2;
    uchar arg3;
} __attribute__((packed));