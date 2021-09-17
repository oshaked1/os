#include "pm.h"

#include "print.h"

struct gdt_entry gdt[3]; // GDT with 3 entries: first is null, second is code segment, third is data data
struct gdt_desc gdt_desc; // GDT descriptor

void init_gdt()
{
    // first GDT entry - null (all zeros)
    gdt[0].length_low = 0;
    gdt[0].base_low = 0;
    gdt[0].base_middle = 0;
    gdt[0].access = 0;
    gdt[0].granularity = 0;
    gdt[0].base_high = 0;

    // second GDT entry - code segment (spans entire linear address range)
    gdt[1].length_low = 0xffff;
    gdt[1].base_low = 0x0000;
    gdt[1].base_middle = 0x00;
    gdt[1].access = PRESENT | PL0 | DESCRIPTOR_TYPE_NORMAL | TYPE_CODE;
    gdt[1].granularity = GRANULARITY_PAGE | OPERAND_SIZE_32 | SEGMENT_LENGTH_MAX;
    gdt[1].base_high = 0x00;

    // third GDT entry - data segment (spans entire linear address range)
    gdt[2].length_low = 0xffff;
    gdt[2].base_low = 0x0000;
    gdt[2].base_middle = 0x00;
    gdt[2].access = PRESENT | PL0 | DESCRIPTOR_TYPE_NORMAL | TYPE_DATA;
    gdt[2].granularity = GRANULARITY_PAGE | OPERAND_SIZE_32 | SEGMENT_LENGTH_MAX;
    gdt[2].base_high = 0x00;

    // setup gdt descriptor
    gdt_desc.size = sizeof(struct gdt_entry) * 3 - 1;
    gdt_desc.offset = (uint32)gdt;
}

void switch_protected_mode(uint32 jump_target)
{
    // initialize GDT
    init_gdt();
    printh((uint16)&gdt_desc);

    // call protected mode entry
    enter_protected_mode((uint32)&gdt_desc, jump_target);
}