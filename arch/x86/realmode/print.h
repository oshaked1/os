#include "types.h"

/**
 * @brief Print a character to the screen
 * 
 * @param c The character to print
 */
__attribute__((always_inline)) static inline void printc(char c)
{
    // ah = 0x0e - teletype mode
    asm volatile ("mov $0x0e, %ah");
    // al - character to print
    asm volatile ("mov %0, %%al" :: "r"(c));
    // BIOS int 10h - video services
    asm volatile ("int $0x10");
}

void prints(char *str);
void printh(uint32 n);