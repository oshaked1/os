#include "load_kernel.h"
#include "types.h"
#include "disk.h"
#include "print.h"
#include "pm.h"

uint32 current_sector; // save current sector for next load iterations

/**
 * @brief Load a section of the kernel into its permanent location. A section is 2^16 bytes.
 * 
 */
void load_section()
{
    // loading finished - jump to kernel entry
    if (current_sector >= KERNEL_SECTORS)
    {
        prints("%BOOT-6-BOOTLDR: Switching into protected mode and launching kernel\r\n");
        switch_protected_mode(KERNEL_LOAD_ADDRESS);
    }

    // temporary disk load address for real mode BIOS routine
    uint16 segment = DISK_LOAD_ADDRESS / 16;
    uint16 offset = DISK_LOAD_ADDRESS % 16;
    
    // calculate number of sectors to read
    uint16 num_sectors;
    if (KERNEL_SECTORS - current_sector > MAX_SECTORS_PER_READ)
        num_sectors = MAX_SECTORS_PER_READ;
    else
        num_sectors = KERNEL_SECTORS - current_sector;

    // perform disk load
    bool success = disk_load(BOOT_DRIVE, REALMODE_SECTORS + 1 + current_sector, num_sectors, segment, offset);

    // disk load failure
    if (!success)
    {
        prints("%BOOT-1-DISKERR: Disk read error while loading kernel\r\n");
        return;
    }

#ifdef __DEBUG__
    prints("copying sectors 0x");
    printh(current_sector);
    prints(" to 0x");
    printh(current_sector + num_sectors);
    prints("\r\n");
#endif
    // update current sector
    current_sector += num_sectors;

    // copy the section from its temporary location to its permanent location
    // using the 32 bit protected mode "copy_section" assembly routine
    switch_protected_mode((uint16)copy_section);
}

/**
 * @brief Load the kernel into memory. Because the kernel may be large, we need to load it in sections of 2^16 bytes.
 * This is because the disk load BIOS routine uses real mode addressing which means that only 2^16 bytes are accessible
 * using a single segment index.
 * Additionaly, we want to load the kernel into an address which isn't accessible from real mode at all, which means that each
 * section will be loaded into a real mode accessible address, and then copied to its permanent address from 32 bit protected mode.
 * 
 */
void load_kernel()
{
    prints("%BOOT-6-BOOTLDR: Loading kernel into memory address 0x");
    printh(KERNEL_LOAD_ADDRESS);
    prints("\r\n");

    current_sector = 0;
    load_section();
}