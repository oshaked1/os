#include "print.h"
#include "load_kernel.h"

void realmode_main()
{
    prints("%BOOT-5-BOOTLDR: Second stage bootloader started\r\n");

    // load kernel into memory
    load_kernel();
}