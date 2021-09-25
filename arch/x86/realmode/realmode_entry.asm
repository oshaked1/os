CODE_SEGMENT equ 8
DATA_SEGMENT equ 16
SERVICE_REQUEST_PACKET_SIZE equ 7

[bits 16]
[extern realmode_main]
    mov [BOOT_DRIVE], dl ; save boot drive in memory
    call realmode_main
    jmp $

; pad to 0x100 bytes after load address, to create static entry address for real mode service calls
times 0x100 - ($-$$) db 0
[bits 32]
[extern service_call_main]
; This is the entry point for all real mode service calls.
; It is located at a static address so the kernel can jump here.
; It receives a request packet as an argument.
; Because it we are in 32-bit protected mode here, our stack is in a different
; location that the one we'll be using in real mode.
; So this entry function copies the request packet to the real mode stack location.
; Afterwards it transitions into real mode and jumps to the service call main function,
; which can access the request packet as an argument because it's on its stack already.
service_call_entry:
    mov ebp, esp

    ; make sure we are copying forward
    cld

    ; set ESI to packet struct address
    lea esi, [ebp+0x4]

    ; set EDI to argument location on real mode stack
    lea edi, [REALMODE_STACK+0x2]

    ; set ECX to number of bytes to copy
    mov ecx, SERVICE_REQUEST_PACKET_SIZE

    ; copy source buffer to destination buffer
    rep movsb

    ; enter real mode with service_call_main as jump target
    push service_call_main
    call enter_real_mode
    jmp $ ; should never execute

[bits 16]
global enter_protected_mode
; receives 2 arguments: gdt descriptor address and jump target
enter_protected_mode:
    mov ebp, esp

    ; disable interrupts
    cli

    ; load gdt from first argument
    mov eax, [ebp+0x2]
    lgdt [eax]

    ; enter 32-bit protected mode
    mov eax, cr0   ; CR0 contains protected mode flag
    or eax, 0x1    ; set protected mode flag
    mov cr0, eax

    ; far jump using code segment selector to set CS
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
    mov eax, [ebp+0x6]

    ; initialize protected mode stack
    mov ebp, PROTECTED_MODE_STACK
    mov esp, ebp

    ; jump to target
    jmp eax

global enter_real_mode
; receives jump target as an argument
enter_real_mode:
    ; NOTE: we are currently in 32-bit protected mode

    mov ebp, esp

    ; disable interrupts
    cli

    ; load GDT
    lgdt [gdt_desc_real]

    ; load jump target into register
    mov edi, [ebp+0x4]

    ; far jump using code segment selector to set CS
    jmp CODE_SEGMENT:init_16_bit_pm

[bits 16]
init_16_bit_pm:
    ; NOTE: we are currently in 16-bit protected mode

    ; load data segment selector into segment registers
    mov eax, DATA_SEGMENT
    mov ds, eax
	mov es, eax
	mov fs, eax
	mov gs, eax
	mov ss, eax

    ; disable paging and set real mode in CR0
    mov eax, cr0
    mov [saved_cr0], eax ; save CR0 for going back into protected mode
    and eax, 0x7ffffffe  ; disable paging (MSB), disable protected mode (LSB)
    mov cr0, eax

    ; far jump to real mode using 0 segment to set CS
    jmp 0:init_real_mode

init_real_mode:
    ; initialize stack
    mov ebp, REALMODE_STACK
    mov esp, ebp

    ; load segment selectors
    mov ax, 0
    mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ss, ax

    ; load real-mode IDT
    lidt [idt_desc_real]

    ; restore interrupts
    sti

    ; jump to target
    jmp edi

gdt_real:
    ; first entry is null
    dq 0

    ; code segment
    dw 0xffff ; length low
    dw 0x0000 ; base low
    db 0x00   ; base middle
    db 0b10011010 ; access
    db 0b00001111 ; granularity
    db 0x00   ; base high

    ; data segment
    dw 0xffff ; length low
    dw 0x0000 ; base low
    db 0x00   ; base middle
    db 0b10010010 ; access
    db 0b00001111 ; granularity
    db 0x00   ; base high

gdt_desc_real:
    dw 24       ; size
    dd gdt_real ; offset

idt_desc_real:
    dw 0x3ff    ; size: (256 entries * 4 bytes each) - 1
    dd 0x0000   ; offset: IDT is already at memory address 0

saved_cr0:
    dd 0 ; save CR0 for going back into protected mode

[bits 32]
global copy_section
[extern load_section]
; copy a section of the kernel from the temporary address it has been loaded into
; (beacause of real mode address limitiations) to its permanent load address
copy_section:
    ; make sure we are copying forward
    cld

    ; set ESI to source buffer
    mov esi, DISK_LOAD_ADDRESS

    ; set EDI to destination buffer
    mov eax, [current_sector]
    mov ebx, 512
    mul ebx
    mov edi, KERNEL_LOAD_ADDRESS
    add edi, eax

    ; set ECX to number of bytes to copy
    mov ecx, 0x10000 ; only 2^16 bytes can be addressed without changing the segment register, this is why the kernel is loaded in sections

    ; copy source buffer to destination buffer
    rep movsb

    ; update current sector
    mov eax, 128
    add [current_sector], eax ; 0x10000 bytes is 128 sectors

    ; load next section
    push load_section
    call enter_real_mode

current_sector:
    dd 0

global BOOT_DRIVE
BOOT_DRIVE: db 0 ; save boot drive in memory for loading the kernel