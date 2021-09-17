KERNEL_OFFSET equ 0x2000
CODE_SEGMENT equ 8
DATA_SEGMENT equ 16

[bits 16]
[extern realmode_main]
call realmode_main
jmp $

global enter_protected_mode
; receives 2 arguments: gdt descriptor address and jump target
enter_protected_mode:
    ; clear interrupts
    cli

    ; load gdt from first argument
    mov eax, [ebp-0x8]
    lgdt [eax]

    ; enter 32-bit protected mode
    mov eax, cr0 ; cr0 contains protected mode flag
    or eax, 0x1    ; set protected mode flag
    mov cr0, eax

    ; far jump to flush CPU pipeline
    jmp CODE_SEGMENT:init_protected_mode
    
[bits 32]
init_protected_mode:
    ; load data segment into segment registers
    mov ax, DATA_SEGMENT
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    ; load jump target into register
    mov eax, [ebp-0xc]

    ; initialize protected mode stack
    mov ebp, 0x90000
    mov esp, ebp

    ; jump to target
    jmp eax
