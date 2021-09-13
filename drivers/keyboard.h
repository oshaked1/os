#include "../lib/types.h"

#define KEYBOARD_DATA 0x60

#define NUM_KEYS 84

#ifndef _SPECIAL_KEYS
#define _SPECIAL_KEYS

// scan code which when sent before another scan code, acts as a modifier
#define MODIFIER 0xe0

// all special keys - P suffix means 'pressed', R suffix means 'released'
enum SPECIAL_KEYS{LSHIFT_P=0x2a, RSHIFT_P=0x36, CAPSLOCK_P=0x3a, LSHIFT_R=0xaa,
                RSHIFT_R=0xb6, CAPSLOCK_R=0xba};

enum MODIFIED_SPECIAL_KEYS{CURSOR_UP_P=0x48, CURSOR_LEFT_P=0x4b, CURSOR_RIGHT_P=0x4d,
                CURSOR_DOWN_P=0x50};

#endif

void handle_keyboard_input();