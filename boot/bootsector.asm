[org 0x7c00]
REALMODE_OFFSET equ 0x1000
REALMODE_SECTORS equ 8

begin:
    mov [BOOT_DRIVE], dl ; BIOS sets the boot drive in 'dl' - store it in memory for later

    ; initialize real mode stack
    mov bp, 0x9000
    mov sp, bp

    ; print bootloader start message
    mov bx, MSG_BOOTLDR
    call print
    call print_nl

    call load_second_stage ; read 2nd stage bootloader from the disk
    mov dl, [BOOT_DRIVE] ; save boot drive for 2nd stage bootloader
    call REALMODE_OFFSET ; enter 2nd stage bootloader
    jmp $ ; never executed

; include utilities
%include "boot/print.asm"
%include "boot/print_hex.asm"
%include "boot/disk.asm"

load_second_stage:
    ; print 2nd stage loading message
    mov bx, MSG_LOAD_SECOND_STAGE
    call print
    mov dx, REALMODE_OFFSET
    call print_hex
    call print_nl

    ; disk read
    mov bx, REALMODE_OFFSET
    mov dh, REALMODE_SECTORS
    mov dl, [BOOT_DRIVE]
    call disk_load
    ret

BOOT_DRIVE db 0 ; reserve memory for boot drive
MSG_BOOTLDR db "%BOOT-5-BOOTLDR: Bootloader started", 0
MSG_LOAD_SECOND_STAGE db "%BOOT-6-BOOTLDR: Loading second stage bootloader into memory address ", 0

; padding and boot sector magic
times 510 - ($-$$) db 0
dw 0xaa55
