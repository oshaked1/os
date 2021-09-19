#include "load_kernel.h"
#include "types.h"
#include "disk.h"
#include "print.h"
#include "pm.h"

void load_kernel()
{
    prints("%BOOT-6-BOOTLDR: Loading kernel into memory address 0x");
    printh(KERNEL_LOAD_ADDRESS);
    prints("\r\n");

    uint16 segment = KERNEL_LOAD_ADDRESS / 16;
    uint16 offset = KERNEL_LOAD_ADDRESS % 16;
    bool success = disk_load(BOOT_DRIVE, REALMODE_SECTORS + 1, KERNEL_SECTORS, segment, offset);
    if (!success)
    {
        prints("%BOOT-1-DISKERR: Disk read error while loading kernel\r\n");
        return;
    }

    prints("%BOOT-6-BOOTLDR: Switching into protected mode and launching kernel\r\n");
    switch_protected_mode(KERNEL_LOAD_ADDRESS);
}