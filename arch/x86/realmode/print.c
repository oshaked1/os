#include "print.h"

/**
 * @brief Print a character to the screen
 * 
 * @param c The character to print
 */
void printc(char c)
{
    // ah = 0x0e - teletype mode
    asm volatile ("mov $0x0e, %ah");
    // al - character to print
    asm volatile ("mov %0, %%al" :: "r"(c));
    // BIOS int 10h - video services
    asm volatile ("int $0x10");
}

/**
 * @brief Print a string to the screen
 * 
 * @param str The string to print
 */
void prints(char *str)
{
    int i = 0;
    while (str[i] != 0)
        printc(str[i++]);
}

/**
 * @brief Print a number to the screen in hexadecimal representation
 * 
 * @param n The number to print
 */
void printh(uint32 n)
{
    char str[9]; // max length of string includeing null-terminator

    // length to print - if n <= 0xff, print 2 characters, if 0x100 <= n <= 0xffff, print 4 characters, else print 8 characers
    int len;
    if (n <= 0xff)
        len = 2;
    else if (n <= 0xffff)
        len = 4;
    else
        len = 8;

    // iterate once for every character to output (4 bits)
    char c;
    int i;
    for (i = 0; i < len; i++)
    {
        c = n & 0xf;
        c += '0';

        // character is a letter
        if (c > '9')
            c += 'a' - '9' - 1;
        
        str[len - 1 - i] = c;
        n = n >> 4;
    }

    str[len] = 0;
    prints(str);
}