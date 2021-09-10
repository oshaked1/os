#include "keyboard.h"
#include "../libc/types.h"
#include "io.h"
#include "screen.h"

const char keys[NUM_KEYS] = "\0\e1234567890-=\b\tQWERTYUIOP[]\n\0ASDFGHJKL;\'`\0\\ZXCVBNM,./\0*\0 \0\0\0\0\0\0\0\0\0\0\0\0\000789-456+1230.";

void handle_keyboard_input()
{
    // read the scan code from the keyboard I/O port
    uint8 scan_code = inb(KEYBOARD_DATA);

    if (scan_code > 0 && scan_code < NUM_KEYS)
    {
        // make sure the character is supposed to be printed
        if (keys[scan_code] != 0)
            kputchar(keys[scan_code]);
    }
}