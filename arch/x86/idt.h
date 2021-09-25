#include "../../lib/types.h"

#define KERNEL_CODE_SEGMENT 8 // second entry in GDT
#define KERNEL_DATA_SEGMENT 16 // third entry in GDT

#define FLAGS_ALWAYS_ON 0b00001110
#define FLAGS_PRESENT 0b10000000

#ifndef _DPL
#define _DPL
typedef enum {PL0, PL1, PL2, PL3}DPL;
#endif

#ifndef _IDT_ENTRY
#define _IDT_ENTRY
struct idt_entry {
    uint16 offset_low;
    uint16 segment;
    uint8 reserved;
    uint8 flags;
    uint16 offset_high;
} __attribute__((packed));
#endif

#ifndef _IDT_DESC
#define _IDT_DESC
struct idt_desc {
    uint16 size;
    uint32 base;
} __attribute__((packed));
#endif

#define IDT_SIZE (256 * sizeof(struct idt_entry))