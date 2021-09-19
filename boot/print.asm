print:
    ; save registers to the stack
    pusha

start:
    mov al, [bx] ; 'bx' is the base address for the string

    ; check for null-terminator
    cmp al, 0
    je done

    ; ah = 0x0e - teletype mode
    mov ah, 0x0e

    ; int 0x10 - BIOS video services
    int 0x10 ; 'al' already contains the char

    ; increment pointer and do another iteration
    add bx, 1
    jmp start

done:
    ; restore registers and return
    popa
    ret

; print linefeed and carriage return
print_nl:
    pusha
    
    mov ah, 0x0e
    mov al, 0x0d ; '\r'
    int 0x10
    mov al, 0x0a ; '\n'
    int 0x10
    
    popa
    ret
