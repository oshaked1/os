#include "interrupt.h"
#include "idt.h"
#include "../../drivers/pic.h"

struct idt_entry idt[256];
struct idt_desc idt_desc;
void *interrupt_handlers[256];

void register_interrupt(uint8 interrupt_number, void *handler, DPL dpl)
{
    // create IDT entry
    idt[interrupt_number].offset_high = (uint16)(((size_t)handler >> 16) & 0xffff);
    idt[interrupt_number].offset_low = (uint16)((size_t)handler & 0xffff);

    // present flag always set
    uint8 flags = FLAGS_ALWAYS_ON | FLAGS_PRESENT;
    // DPL is bits 5 and 6
    flags |= (dpl << 5);
    
    idt[interrupt_number].flags = flags;
    idt[interrupt_number].segment = KERNEL_CODE_SEGMENT;
}

void register_interrupt_handler(uint8 interrupt_number, void *handler)
{
    interrupt_handlers[interrupt_number] = handler;
}

void high_level_handler(interrupt_info info)
{
    // load address of handler function into func
    void (*func)() = interrupt_handlers[info.interrupt_number];
    // if the address is 0, then there is no handler registered
    if (func != 0)
        func(info);
    
    // external interrupts need to be acknowledged
    acknowledge_interrupt(info.interrupt_number);
}

// macros for generating a generic handler for a specific interrupt number
#define GENERIC_HANDLER_NOERR(num) ({asm volatile ("cli"); asm volatile ("push $0"); asm volatile ("push $" #num); asm volatile ("jmp generic_interrupt_handler");})
#define GENERIC_HANDLER_ERR(num) ({asm volatile ("cli"); asm volatile ("push $" #num); asm volatile("jmp generic_interrupt_handler");})
__attribute__((naked)) void generic_interrupt_handler()
{
    // save all registers to the stack
    asm volatile ("pusha");

    // save data segment selector to the stack
    asm volatile ("mov %ds, %ax");
    asm volatile ("push %eax");

    // load kernel data segment selector
    asm volatile ("mov %0, %%ax" : : "i"(KERNEL_DATA_SEGMENT));
    asm volatile ("mov %ax, %ds");
    asm volatile ("mov %ax, %es");
    asm volatile ("mov %ax, %fs");
    asm volatile ("mov %ax, %gs");

    // call high level handler
    asm volatile ("call high_level_handler");

    // restore original data segment selector
    asm volatile ("pop %eax");
    asm volatile ("mov %ax, %ds");
    asm volatile ("mov %ax, %es");
    asm volatile ("mov %ax, %fs");
    asm volatile ("mov %ax, %gs");

    // restore all original registers
    asm volatile ("popa");

    // clean up stack from the interrupt number and error code
    asm volatile ("add $8, %esp");

    // re-enable interrupts
    asm volatile ("sti");

    // return from interrupt
    asm volatile ("iret");
}

// ISRs
__attribute__((naked)) void interrupt_handler_0()   {GENERIC_HANDLER_NOERR(0);}
__attribute__((naked)) void interrupt_handler_1()   {GENERIC_HANDLER_NOERR(1);}
__attribute__((naked)) void interrupt_handler_2()   {GENERIC_HANDLER_NOERR(2);}
__attribute__((naked)) void interrupt_handler_3()   {GENERIC_HANDLER_NOERR(3);}
__attribute__((naked)) void interrupt_handler_4()   {GENERIC_HANDLER_NOERR(4);}
__attribute__((naked)) void interrupt_handler_5()   {GENERIC_HANDLER_NOERR(5);}
__attribute__((naked)) void interrupt_handler_6()   {GENERIC_HANDLER_NOERR(6);}
__attribute__((naked)) void interrupt_handler_7()   {GENERIC_HANDLER_NOERR(7);}
__attribute__((naked)) void interrupt_handler_8()   {GENERIC_HANDLER_ERR(8);}
__attribute__((naked)) void interrupt_handler_9()   {GENERIC_HANDLER_NOERR(9);}
__attribute__((naked)) void interrupt_handler_10()  {GENERIC_HANDLER_ERR(10);}
__attribute__((naked)) void interrupt_handler_11()  {GENERIC_HANDLER_ERR(11);}
__attribute__((naked)) void interrupt_handler_12()  {GENERIC_HANDLER_ERR(12);}
__attribute__((naked)) void interrupt_handler_13()  {GENERIC_HANDLER_ERR(13);} // general protection fault
__attribute__((naked)) void interrupt_handler_14()  {GENERIC_HANDLER_ERR(14);}
__attribute__((naked)) void interrupt_handler_15()  {GENERIC_HANDLER_NOERR(15);}
__attribute__((naked)) void interrupt_handler_16()  {GENERIC_HANDLER_NOERR(16);}
__attribute__((naked)) void interrupt_handler_17()  {GENERIC_HANDLER_ERR(17);}
__attribute__((naked)) void interrupt_handler_18()  {GENERIC_HANDLER_NOERR(18);}
__attribute__((naked)) void interrupt_handler_19()  {GENERIC_HANDLER_NOERR(19);}
__attribute__((naked)) void interrupt_handler_20()  {GENERIC_HANDLER_NOERR(20);}
__attribute__((naked)) void interrupt_handler_21()  {GENERIC_HANDLER_NOERR(21);}
__attribute__((naked)) void interrupt_handler_22()  {GENERIC_HANDLER_NOERR(22);}
__attribute__((naked)) void interrupt_handler_23()  {GENERIC_HANDLER_NOERR(23);}
__attribute__((naked)) void interrupt_handler_24()  {GENERIC_HANDLER_NOERR(24);}
__attribute__((naked)) void interrupt_handler_25()  {GENERIC_HANDLER_NOERR(25);}
__attribute__((naked)) void interrupt_handler_26()  {GENERIC_HANDLER_NOERR(26);}
__attribute__((naked)) void interrupt_handler_27()  {GENERIC_HANDLER_NOERR(27);}
__attribute__((naked)) void interrupt_handler_28()  {GENERIC_HANDLER_NOERR(28);}
__attribute__((naked)) void interrupt_handler_29()  {GENERIC_HANDLER_NOERR(29);}
__attribute__((naked)) void interrupt_handler_30()  {GENERIC_HANDLER_ERR(30);}
__attribute__((naked)) void interrupt_handler_31()  {GENERIC_HANDLER_NOERR(31);}

// hardware IRQs
__attribute__((naked)) void interrupt_handler_32()  {GENERIC_HANDLER_NOERR(32);}
__attribute__((naked)) void interrupt_handler_33()  {GENERIC_HANDLER_NOERR(33);} // keyboard input
__attribute__((naked)) void interrupt_handler_34()  {GENERIC_HANDLER_NOERR(34);}
__attribute__((naked)) void interrupt_handler_35()  {GENERIC_HANDLER_NOERR(35);}
__attribute__((naked)) void interrupt_handler_36()  {GENERIC_HANDLER_NOERR(36);}
__attribute__((naked)) void interrupt_handler_37()  {GENERIC_HANDLER_NOERR(37);}
__attribute__((naked)) void interrupt_handler_38()  {GENERIC_HANDLER_NOERR(38);}
__attribute__((naked)) void interrupt_handler_39()  {GENERIC_HANDLER_NOERR(39);}
__attribute__((naked)) void interrupt_handler_40()  {GENERIC_HANDLER_NOERR(40);}
__attribute__((naked)) void interrupt_handler_41()  {GENERIC_HANDLER_NOERR(41);}
__attribute__((naked)) void interrupt_handler_42()  {GENERIC_HANDLER_NOERR(42);}
__attribute__((naked)) void interrupt_handler_43()  {GENERIC_HANDLER_NOERR(43);}
__attribute__((naked)) void interrupt_handler_44()  {GENERIC_HANDLER_NOERR(44);}
__attribute__((naked)) void interrupt_handler_45()  {GENERIC_HANDLER_NOERR(45);}
__attribute__((naked)) void interrupt_handler_46()  {GENERIC_HANDLER_NOERR(46);}
__attribute__((naked)) void interrupt_handler_47()  {GENERIC_HANDLER_NOERR(47);}
__attribute__((naked)) void interrupt_handler_48()  {GENERIC_HANDLER_NOERR(48);}

// software interrupts
__attribute__((naked)) void interrupt_handler_50()  {GENERIC_HANDLER_NOERR(50);}  // test interrupt
__attribute__((naked)) void interrupt_handler_129() {GENERIC_HANDLER_NOERR(129);} // 0x81 - real mode services


void setup_idt()
{
    // register ISRs
    register_interrupt(0,   interrupt_handler_0,   PL0);
    register_interrupt(1,   interrupt_handler_1,   PL0);
    register_interrupt(2,   interrupt_handler_2,   PL0);
    register_interrupt(3,   interrupt_handler_3,   PL0);
    register_interrupt(4,   interrupt_handler_4,   PL0);
    register_interrupt(5,   interrupt_handler_5,   PL0);
    register_interrupt(6,   interrupt_handler_6,   PL0);
    register_interrupt(7,   interrupt_handler_7,   PL0);
    register_interrupt(8,   interrupt_handler_8,   PL0);
    register_interrupt(9,   interrupt_handler_9,   PL0);
    register_interrupt(10,  interrupt_handler_10,  PL0);
    register_interrupt(11,  interrupt_handler_11,  PL0);
    register_interrupt(12,  interrupt_handler_12,  PL0);
    register_interrupt(13,  interrupt_handler_13,  PL0);
    register_interrupt(14,  interrupt_handler_14,  PL0);
    register_interrupt(15,  interrupt_handler_15,  PL0);
    register_interrupt(16,  interrupt_handler_16,  PL0);
    register_interrupt(17,  interrupt_handler_17,  PL0);
    register_interrupt(18,  interrupt_handler_18,  PL0);
    register_interrupt(19,  interrupt_handler_19,  PL0);
    register_interrupt(20,  interrupt_handler_20,  PL0);
    register_interrupt(21,  interrupt_handler_21,  PL0);
    register_interrupt(22,  interrupt_handler_22,  PL0);
    register_interrupt(23,  interrupt_handler_23,  PL0);
    register_interrupt(24,  interrupt_handler_24,  PL0);
    register_interrupt(25,  interrupt_handler_25,  PL0);
    register_interrupt(26,  interrupt_handler_26,  PL0);
    register_interrupt(27,  interrupt_handler_27,  PL0);
    register_interrupt(28,  interrupt_handler_28,  PL0);
    register_interrupt(29,  interrupt_handler_29,  PL0);
    register_interrupt(30,  interrupt_handler_30,  PL0);
    register_interrupt(31,  interrupt_handler_31,  PL0);

    // register IRQs
    register_interrupt(32,  interrupt_handler_32,  PL0);
    register_interrupt(33,  interrupt_handler_33,  PL0);
    register_interrupt(34,  interrupt_handler_34,  PL0);
    register_interrupt(35,  interrupt_handler_35,  PL0);
    register_interrupt(36,  interrupt_handler_36,  PL0);
    register_interrupt(37,  interrupt_handler_37,  PL0);
    register_interrupt(38,  interrupt_handler_38,  PL0);
    register_interrupt(39,  interrupt_handler_39,  PL0);
    register_interrupt(40,  interrupt_handler_40,  PL0);
    register_interrupt(41,  interrupt_handler_41,  PL0);
    register_interrupt(42,  interrupt_handler_42,  PL0);
    register_interrupt(43,  interrupt_handler_43,  PL0);
    register_interrupt(44,  interrupt_handler_44,  PL0);
    register_interrupt(45,  interrupt_handler_45,  PL0);
    register_interrupt(46,  interrupt_handler_46,  PL0);
    register_interrupt(47,  interrupt_handler_47,  PL0);
    register_interrupt(48,  interrupt_handler_48,  PL0);

    // register software interrupts
    register_interrupt(50,  interrupt_handler_50,  PL0);
    register_interrupt(129, interrupt_handler_129, PL0);

    // initialize IDT description
    idt_desc.base = (uint32)&idt;
    idt_desc.size = IDT_SIZE - 1;

    // load the IDT
    asm volatile ("lidt [idt_desc]");

    // enable hardware interrupts
    asm volatile ("sti");
}