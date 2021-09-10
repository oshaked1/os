#include "irq.h"
#include "../../arch/x86/interrupt.h"
#include "../../drivers/pic.h"
#include "../../drivers/keyboard.h"

void install_irqs()
{
    register_interrupt_handler(IRQ_KEYBOARD, handle_keyboard_input);
}