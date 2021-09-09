#include "io.h"

/**
 * Read a byte from the specified port
 */
uchar inb (ushort port) {
    uchar result;
    /* Inline assembler syntax
     * !! Notice how the source and destination registers are switched from NASM !!
     *
     * '"=a" (result)'; set '=' the C variable '(result)' to the value of register e'a'x
     * '"d" (port)': map the C variable '(port)' into e'd'x register
     *
     * Inputs and outputs are separated by colons
     */
    asm volatile ("in %%dx, %%al" : "=a" (result) : "d" (port));
    return result;
}

void outb (ushort port, uchar data) {
    /* Notice how here both registers are mapped to C variables and
     * nothing is returned, thus, no equals '=' in the asm syntax 
     * However we see a comma since there are two variables in the input area
     * and none in the 'return' area
     */
    asm volatile ("out %%al, %%dx" : : "a" (data), "d" (port));
}

ushort inw (ushort port) {
    ushort result;
    asm volatile ("in %%dx, %%ax" : "=a" (result) : "d" (port));
    return result;
}

void outw (ushort port, ushort data) {
    asm volatile ("out %%ax, %%dx" : : "a" (data), "d" (port));
}
