#include "../lib/types.h"
#include "io.h"

#define PIC1 0x20
#define PIC2 0xa0

#define PIC1_COMMAND PIC1
#define PIC2_COMMAND PIC2

#define PIC1_DATA (PIC1 + 1)
#define PIC2_DATA (PIC2 + 1)

#define PIC1_START_INT 32
#define PIC1_END_INT (PIC1_START_INT + 7)
#define PIC2_START_INT (PIC1_END_INT + 1)
#define PIC2_END_INT (PIC2_START_INT + 7)

#define PIC_EOI 0x20 // end of interrupt

#define ICW1_ICW4       0x01    /* ICW4 (not) needed */
#define ICW1_SINGLE     0x02    /* Single (cascade) mode */
#define ICW1_INTERVAL4  0x04    /* Call address interval 4 (8) */
#define ICW1_LEVEL      0x08    /* Level triggered (edge) mode */
#define ICW1_INIT       0x10    /* Initialization - required! */
 
#define ICW4_8086       0x01    /* 8086/88 (MCS-80/85) mode */
#define ICW4_AUTO       0x02    /* Auto (normal) EOI */
#define ICW4_BUF_SLAVE  0x08    /* Buffered mode/slave */
#define ICW4_BUF_MASTER 0x0C    /* Buffered mode/master */
#define ICW4_SFNM       0x10    /* Special fully nested (not) */

void setup_pic();

#ifndef _ACK_INT
#define _ACK_INT
__attribute__((always_inline)) static inline void acknowledge_interrupt(uint8 int_num)
{
    if (int_num >= PIC2_START_INT && int_num <= PIC2_END_INT)
    {
        outb(PIC1_COMMAND, PIC_EOI);
        outb(PIC2_COMMAND, PIC_EOI);
    }
    else if (int_num >= PIC1_START_INT && int_num <= PIC1_END_INT)
        outb(PIC1_COMMAND, PIC_EOI);
}
#endif