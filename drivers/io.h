#include "../lib/types.h"

#ifndef _IO_FUNCS
#define _IO_FUNCS

__attribute__((always_inline)) static inline byte inb(uint16 port)
{
    byte result;
    asm volatile ("in %%dx, %%al" : "=a" (result) : "d" (port));
    return result;
}

__attribute__((always_inline)) static inline void outb(uint16 port, byte data)
{
    asm volatile ("out %%al, %%dx" : : "a" (data), "d" (port));
}

__attribute__((always_inline)) static inline uint16 inw(uint16 port)
{
    uint16 result;
    asm volatile ("in %%dx, %%ax" : "=a" (result) : "d" (port));
    return result;
}

__attribute__((always_inline)) static inline void outw(uint16 port, uint16 data)
{
    asm volatile ("out %%ax, %%dx" : : "a" (data), "d" (port));
}

__attribute__((always_inline)) static inline void io_wait()
{
    outb(0x80, 0);
}

#endif