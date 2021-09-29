#include "../../lib/types.h"

#ifndef _REGION_TYPE
#define _REGION_TYPE
enum REGION_TYPE{TYPE_USABLE=1, TYPE_RESERVED=2, TYPE_ACPI_RECLAIMABLE=3, TYPE_ACPI_NVS=4, TYPE_BAD=5};
#endif

#ifndef _MEMORY_REGION
#define _MEMORY_REGION
struct memory_region
{
    uint64 base;
    uint64 len;
    uint32 type;
    uint32 acpi_attrs;
} __attribute__((packed));
#endif

void obtain_memmap();