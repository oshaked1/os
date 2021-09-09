#include "reset.h"
#include "../../drivers/io.h"

void hardware_warm_reset()
{
    outb(RESET_PORT, WARM_RESET);
}

void hardware_cold_reset()
{
    outb(RESET_PORT, COLD_RESET);
}