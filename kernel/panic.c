#include "../arch/x86/reset.h"
#include "syslog.h"

void kernel_panic()
{
    log("SYS", 0, "PANIC", "KERNEL PANIC");
    hardware_warm_reset();
}