#include "types.h"

extern uint8 BOOT_DRIVE;

struct disk_address_packet{
    uint8 packet_size;  // struct size - always 16
    uint8 always_zero;  // always 0
    uint16 num_sectors; // number of sectors to transfer
    uint16 offset;      // memory offset to transfer to
    uint16 segment;     // memory segment to transfer to
    uint32 lba_low;     // lower 32 bits of 48-bit LBA
    uint32 lba_high;    // upper 16 bits of 48-bit LBA
} __attribute__((packed));

bool disk_load(uint8 drive_num, uint32 lba, uint16 num_sectors, uint16 segment, uint16 offset);