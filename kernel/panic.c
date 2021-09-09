#include "../arch/x86/reset.h"
#include "syslog.h"

void kernel_panic(char *msg)
{
    log("SYS", 0, "PANIC", "KERNEL PANIC: %s", msg);
    hardware_warm_reset();
}