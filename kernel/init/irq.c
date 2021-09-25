#include "irq.h"
#include "../../arch/x86/interrupt.h"

// headers containing IRQ handlers
#include "../../drivers/keyboard.h"
#include "../../arch/x86/realmode_services.h"

void install_irqs()
{
    // register hardware IRQ handlers
    register_interrupt_handler(IRQ_KEYBOARD, handle_keyboard_input);

    // register software IRQ handlers
    register_interrupt_handler(IRQ_REALMODE_SERVICES, realmode_service_call);
}