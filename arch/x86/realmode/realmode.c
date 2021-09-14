#include "print.h"
#include "pm.h"

void realmode_main()
{
    prints("Second stage bootloader started\r\n");
    prints("Entering 32-bit protected mode\r\n");
    switch_protected_mode(0x2000);
}