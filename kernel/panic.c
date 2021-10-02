#include "../arch/x86/reset.h"
#include "syslog.h"
#include "../lib/stdio.h"
#include "../lib/stdarg.h"

void kernel_panic(const char *msg, ...)
{
    char str[MAX_LOG_LENGTH];
    va_list args;
    va_start(args, msg);
    vsnprintf(str, MAX_LOG_LENGTH, msg, args);
    va_end(args);
    log("SYS", LOG_EMERG, "PANIC", "KERNEL PANIC: %s", str);
    hardware_warm_reset();
}