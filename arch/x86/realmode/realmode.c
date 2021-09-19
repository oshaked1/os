#include "print.h"
#include "pm.h"
#include "load_kernel.h"
#include "types.h"

void realmode_main()
{
    prints("%BOOT-5-BOOTLDR: Second stage bootloader started\r\n");

    // load kernel into memory
    load_kernel();
}