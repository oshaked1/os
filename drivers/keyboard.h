#include "../libc/types.h"

#define KEYBOARD_DATA 0x60

#define NUM_KEYS 84

#ifndef _SPECIAL_KEYS
#define _SPECIAL_KEYS

// all special keys - P suffix means 'pressed', R suffix means 'released'
enum SPECIAL_KEYS{LSHIFT_P=0x2a, RSHIFT_P=0x36, CAPSLOCK_P=0x3a, LSHIFT_R=0xaa,
                RSHIFT_R=0xb6, CAPSLOCK_R=0xba};

#endif

void handle_keyboard_input();