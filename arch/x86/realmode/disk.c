#include "disk.h"
#include "types.h"
#include "print.h"

// See https://wiki.osdev.org/ATA_in_x86_RealMode_(BIOS) for detaield explanation on the following routines

// disk address packet - must be 4-byte aligned
struct disk_address_packet __attribute__((aligned (4))) addr_pkt;

/**
 * @brief Check whether LBA (Logical Block Addressing) is supported by the BIOS.
 * 
 * @return true LBA is supported
 * @return false LBA is not supported
 */
bool check_lba()
{
    asm volatile ("mov $0x41, %ah");
    asm volatile ("mov $0x55aa, %bx");
    asm volatile ("mov $0x80, %dl");
    asm volatile ("int $0x13");

    // carry flag is set if LBA is not supported
    asm goto ("jc %l[not_supported]" :::: not_supported);

supported:
    return TRUE;

not_supported:
    return FALSE;

}

bool disk_load(uint8 drive_num, uint32 lba, uint16 num_sectors, uint16 segment, uint16 offset)
{
    if (!check_lba())
    {
        prints("%BOOT-2-DISKERR: LBA not supported\r\n");
        return FALSE;
    }

    // set up disk address packet
    addr_pkt.packet_size = 16;
    addr_pkt.always_zero = 0;
    addr_pkt.num_sectors = num_sectors;
    addr_pkt.offset = offset;
    addr_pkt.segment = segment;
    addr_pkt.lba_low = lba;
    addr_pkt.lba_high = 0; // LBA values larger than 2^32 are not supported, because 64-bit arithmetic is complicated in real mode

    // set ds to 0
    asm volatile ("mov $0, %ax");
    asm volatile ("mov %ax, %ds");

    // set si to address packet location
    asm volatile ("mov %0, %%esi" :: "r"(&addr_pkt));

    // ah=0x42 - disk read using LBA
    asm volatile ("mov $0x42, %ah");

    // dl = drive number
    asm volatile ("mov %0, %%dl" :: "r"(drive_num));

    // int 0x13 - BIOS disk services
    asm volatile ("int $0x13");

    // carry flag is set on error
    asm goto ("jc %l[error]" :::: error);
    // ah should be 0 on success
    asm volatile ("test %ah, %ah");
    asm goto ("jnz %l[error]" :::: error);

success:
    return TRUE;

error:
    return FALSE;
}