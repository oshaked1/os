#include "../../drivers/pic.h"

// PIC1 IRQ lines
#define IRQ0  PIC1_START_INT
#define IRQ1 (PIC1_START_INT + 1)
#define IRQ2 (PIC1_START_INT + 2)
#define IRQ3 (PIC1_START_INT + 3)
#define IRQ4 (PIC1_START_INT + 4)
#define IRQ5 (PIC1_START_INT + 5)
#define IRQ6 (PIC1_START_INT + 6)
#define IRQ7 (PIC1_START_INT + 7)

// PIC2 IRQ lines
#define IRQ8   PIC2_START_INT
#define IRQ9  (PIC2_START_INT + 1)
#define IRQ10 (PIC2_START_INT + 2)
#define IRQ11 (PIC2_START_INT + 3)
#define IRQ12 (PIC2_START_INT + 4)
#define IRQ13 (PIC2_START_INT + 5)
#define IRQ14 (PIC2_START_INT + 6)
#define IRQ15 (PIC2_START_INT + 7)

// hardware IRQ aliases
#define IRQ_TIMER    IRQ0
#define IRQ_KEYBOARD IRQ1
#define IRQ_PIC2     IRQ2
#define IRQ_COM2     IRQ3
#define IRQ_COM1     IRQ4
#define IRQ_LPT2     IRQ5
#define IRQ_FLOPPY   IRQ6
#define IRQ_LPT1     IRQ7
#define IRQ_RT_CLOCK IRQ8

// software IRQs
#define IRQ_REALMODE_SERVICES 0x81

void install_irqs();