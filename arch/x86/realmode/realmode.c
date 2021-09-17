#include "print.h"
#include "pm.h"

void realmode_main()
{
    prints("Second stage bootloader started\r\n");

    // jump to protected mode - no code at this address, but single-step debugging shows that this works
    prints("Switching to protected mode\r\n");
    switch_protected_mode(0x3000);
}