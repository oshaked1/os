#include "../libc/types.h"
#include "../libc/string.h"
#include "../libc/stdlib.h"
#include "../drivers/screen.h"
#include "syslog.h"

void main()
{
    log("SYS", 5, "INIT", "Kernel started");
}