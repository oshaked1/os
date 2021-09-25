REALMODE_SERVICE_ENTRY_ADDRESS equ (REALMODE_LOAD_ADDRESS + 0x100)

[bits 32]
[extern kmain]
    call kmain
    jmp $

global realmode_service_entry
realmode_service_entry:
    jmp REALMODE_SERVICE_ENTRY_ADDRESS
    jmp $