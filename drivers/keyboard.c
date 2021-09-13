#include "keyboard.h"
#include "../lib/types.h"
#include "io.h"
#include "screen.h"

/* This driver currently has a bug (that seems to actually be caused by QEMU).
Whenever both SHIFT keys are pressed, they kind of "cancel out" each other and the result is as if only one were ever pressed
and never released. This means that following such a double press, the driver thinks SHIFT is pressed even though it's not.
This can easily be undone by simply pressing one of the SHIFT keys alone. For the record, CTRL behaves the same way.

I added a debug message each time this interrupt handler is invoked that prints the received scan code to see what's up.
Turns out when one SHIFT is pressed, pressing the other one WILL NOT REGISTER AT ALL (or at least no interrupt is received).

I tried to find this issue online but to no avail :(
I even sanity checked the behavior of my host OS (Windows), and it works as expected.
For now i'm not bothering on finding a fix for this
*/

const char keys[NUM_KEYS]      = "\0\e1234567890-=\b\tqwertyuiop[]\n\0asdfghjkl;\'`\0\\zxcvbnm,./\0*\0 \0\0\0\0\0\0\0\0\0\0\0\0\000789-456+1230.";
const char keys_caps[NUM_KEYS] = "\0\e!@#$%^&*()_+\b\0QWERTYUIOP{}\n\0ASDFGHJKL:\"~\0|ZXCVBNM<>?\0*\0 \0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0-\0\0\0+\0\0\0\0\0";

bool shift_pressed = FALSE;
bool caps_lock = FALSE;

// many key presses send the scan code 0xe0 before sending a second scan code,
// thus it acts as a modifier
bool modify = FALSE;

void handle_keyboard_input()
{
    // read the scan code from the keyboard I/O port
    uint8 scan_code = inb(KEYBOARD_DATA);

    // handle modifier key
    if (scan_code == MODIFIER)
    {
        modify = TRUE;
        return;
    }

    if (!modify)
    {
        switch (scan_code)
        {
            // shift pressed
            case LSHIFT_P:
            case RSHIFT_P:
                shift_pressed = TRUE;
                break;
            
            // shift released
            case LSHIFT_R:
            case RSHIFT_R:
                shift_pressed = FALSE;
                break;
            
            // caps lock pressed - just invert the current state
            case CAPSLOCK_P:
                caps_lock = !caps_lock;
                break;
            
            // not a special key, check if printable and if so print
            default:
            {
                if (scan_code > 0 && scan_code < NUM_KEYS)
                {
                    // check which keyset to use
                    const char *keyset = (caps_lock ^ shift_pressed)? keys_caps: keys;
                    
                    // make sure the character is supposed to be printed
                    if (keyset[scan_code] != 0)
                        kputchar(keyset[scan_code]);
                }
                break;
            }
        }
    }

    else
    {
        switch (scan_code)
        {
            case CURSOR_LEFT_P:
                cursor_left();
                break;
            
            case CURSOR_RIGHT_P:
                cursor_right();
                break;
            
            case CURSOR_UP_P:
                cursor_up();
                break;
            
            case CURSOR_DOWN_P:
                cursor_down();
                break;
            
            default:
                break;
        }
    }

    // unset modifier
    if (scan_code != MODIFIER)
        modify = FALSE;
}