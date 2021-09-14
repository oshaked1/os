; Identical to lesson 13's boot sector, but the %included files have new paths
[org 0x7c00]
REALMODE_OFFSET equ 0x1000
REALMODE_SECTORS equ 4
KERNEL_OFFSET equ 0x2000 ; The same one we used when linking the kernel

    mov [BOOT_DRIVE], dl ; Remember that the BIOS sets us the boot drive in 'dl' on boot
    mov bp, 0x9000
    mov sp, bp

    mov bx, MSG_REAL_MODE
    call print
    call print_nl

    call load_second_stage ; read 2nd stage bootloader from the disk
    call REALMODE_OFFSET
    ;call switch_to_pm ; disable interrupts, load GDT,  etc. Finally jumps to 'BEGIN_PM'
    jmp $ ; Never executed

%include "boot/print.asm"
%include "boot/print_hex.asm"
%include "boot/disk.asm"
%include "boot/gdt.asm"
%include "boot/32bit_print.asm"
%include "boot/switch_pm.asm"

[bits 16]
load_second_stage:
    mov bx, MSG_LOAD_SECOND_STAGE
    call print
    call print_nl

    mov bx, REALMODE_OFFSET
    mov dh, REALMODE_SECTORS
    ;mov bx, KERNEL_OFFSET ; Read from disk and store in 0x1000
    ;mov dh, 32 ; Our future kernel will be larger, make this big
    mov dl, [BOOT_DRIVE]
    call disk_load
    ret

[bits 32]
BEGIN_PM:
    call KERNEL_OFFSET ; Give control to the kernel
    jmp $ ; Stay here when the kernel returns control to us (if ever)


BOOT_DRIVE db 0 ; It is a good idea to store it in memory because 'dl' may get overwritten
MSG_REAL_MODE db "Bootloader started (16-bit real mode)", 0
MSG_LOAD_SECOND_STAGE db "Loading second stage bootloader from disk", 0

; padding
times 510 - ($-$$) db 0
dw 0xaa55
