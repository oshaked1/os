#include "pic.h"
#include "../lib/types.h"
#include "io.h"

void pic_remap(uint8 pic1_start_int, uint8 pic2_start_int)
{
    byte a1, a2;

    a1 = inb(PIC1_DATA);                        // save masks
    a2 = inb(PIC2_DATA);

    outb(PIC1_COMMAND, ICW1_INIT | ICW1_ICW4);  // starts the initialization sequence (in cascade mode)
    io_wait();
    outb(PIC2_COMMAND, ICW1_INIT | ICW1_ICW4);
    io_wait();
    outb(PIC1_DATA, pic1_start_int);            // ICW2: Master PIC vector offset
    io_wait();
    outb(PIC2_DATA, pic2_start_int);            // ICW2: Slave PIC vector offset
    io_wait();
    outb(PIC1_DATA, 4);                         // ICW3: tell Master PIC that there is a slave PIC at IRQ2 (0000 0100)
    io_wait();
    outb(PIC2_DATA, 2);                         // ICW3: tell Slave PIC its cascade identity (0000 0010)
    io_wait();

    outb(PIC1_DATA, ICW4_8086);
    io_wait();
    outb(PIC2_DATA, ICW4_8086);
    io_wait();

    outb(PIC1_DATA, a1);   // restore saved masks.
    outb(PIC2_DATA, a2);
}

void setup_pic()
{
    pic_remap(PIC1_START_INT, PIC2_START_INT);
}