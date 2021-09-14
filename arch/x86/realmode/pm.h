#include "types.h"

// Please refer to https://wiki.osdev.org/Global_Descriptor_Table for deatiled GDT info

// access flags
#define PRESENT 0b10000000
#define PL0 0
#define DESCRIPTOR_TYPE_NORMAL 0b00010000
#define TYPE_CODE 0b00001010
#define TYPE_DATA 0b00000010

// granularity flags
#define GRANULARITY_PAGE 0b10000000
#define OPERAND_SIZE_32  0b01000000
#define SEGMENT_LENGTH_MAX 0b00001111

struct gdt_entry
{
    uint16 length_low; // segment length, bits 0-15
    uint16 base_low;   // segment base, bits 0-15
    uint8 base_middle; // segment base, bits 16-23

    uint8 access;      // access flags:
                       // +---+---+------------+
                       // | 7 | P | Present    |
                       // +---+---+------------+
                       // | 6 | D | Descriptor |
                       // |   | P | Privilege  |
                       // | 5 | L | Level      |
                       // +---+---+------------+
                       // | 4 | D | Descriptor |
                       // |   | T | Type       |
                       // +---+---+------------+
                       // | 3 | T | Segment    |
                       // | 2 | Y | Type:      |
                       // | 1 | P | Code /     |
                       // | 0 | E | Data       |
                       // +---+---+------------+

    uint8 granularity; // granularity flags, segment length bits 16-19
                       // +---+---+--------------+
                       // | 7 | G | Granularity  |
                       // +---+---+--------------+
                       // | 6 | D | Operand Size |
                       // +---+---+--------------+
                       // | 5 | 0 | Always 0     |
                       // +---+---+--------------+
                       // | 4 | A | Always 0     |
                       // +---+---+--------------+
                       // | 3 | L |              |
                       // |   | E | Segment      |
                       // | 2 | N | Length       |
                       // |   | G | Bits         |
                       // | 1 | T | 16-19        |
                       // |   | H |              |
                       // | 0 |   |              |
                       // +---+---+--------------+

    uint8 base_high;   // segment base, bits 24-31
} __attribute__((packed));

struct gdt_desc
{
    uint16 size;
    uint32 offset;
} __attribute__((packed));

void switch_protected_mode(uint32 jump_target);
extern void enter_protected_mode(uint32 gdt_desc, uint32 jump_target);