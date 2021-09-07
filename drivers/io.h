#include "../libc/types.h"

byte port_byte_in (ushort port);
void port_byte_out (ushort port, byte data);
ushort port_word_in (ushort port);
void port_word_out (ushort port, ushort data);
