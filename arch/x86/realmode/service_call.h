#include "types.h"

// test service packet - fields will be different later on
struct service_packet
{
    uint32 arg1;
    uint16 arg2;
    uchar arg3;
} __attribute__((packed));