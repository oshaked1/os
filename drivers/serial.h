#include "../libc/types.h"

// baud rate definitions
#define CLOCK_RATE   115200
#define DEFAULT_BAUD 9600
#define MAX_BAUD CLOCK_RATE
#define DIVISOR(baud) (CLOCK_RATE / (baud))

// data bits definitions
#define DATA_BITS_MASK 0b00000011
#define DATA_BITS_8 0b00000011
#define DATA_BITS_7 0b00000010
#define DATA_BITS_6 0b00000001
#define DATA_BITS_5 0b00000000
#define DEFAULT_DATA_BITS DATA_BITS_8

// stop bits definitions
#define STOP_BITS_MASK 0b00000100
#define STOP_BITS_1 0b00000000
#define STOP_BITS_2 0b00000100
#define DEFAULT_STOP_BITS STOP_BITS_1

// parity definitions
#define PARITY_MASK  0b00111000
#define PARITY_NONE  0b00000000
#define PARITY_ODD   0b00001000
#define PARITY_EVEN  0b00011000
#define PARITY_MARK  0b00101000
#define PARITY_SPACE 0b00111000
#define DEFAULT_PARITY PARITY_NONE

// interrupt definitions
#define INT_DATA_AVAIL  0b00000001
#define INT_TRANS_EMPTY 0b00000010
#define INT_BREAK_ERR   0b00000100
#define INT_STATUS_CHG  0b00001000
#define INT_NONE        0b00000000

// line status definitions
#define LINE_DR   0b00000001 // data ready
#define LINE_OE   0b00000010 // overrun error
#define LINE_PE   0b00000100 // parity error
#define LINE_FE   0b00001000 // framing error
#define LINE_BI   0b00010000 // break indicator
#define LINE_THRE 0b00100000 // transmitter holding register empty
#define LINE_TEMT 0b01000000 // transmitter empty
#define LINE_IMPENDING_ERR 0b10000000 // impending error

// base port address definitions
#define COM1 0x3f8
#define COM2 0x2f8
#define COM3 0x3e8
#define COM4 0x2e8

// port offset definitions
#define SERIAL_DATA(com) com
#define SERIAL_INT_ENABLE(com) ((com) + 1)
#define SERIAL_FIFO(com) ((com) + 2)
#define SERIAL_LINE_CTRL(com) ((com) + 3)
#define SERIAL_MODEM_CTRL(com) ((com) + 4)
#define SERIAL_LINE_STATUS(com) ((com) + 5)
#define SERIAL_MODEM_STATUS(com) ((com) + 6)
#define SERIAL_SCRATCH(com) ((com) + 7)

void set_baud_rate(ushort com, uint baud);
void init_serial(ushort com);
void serial_send(ushort com, byte *data, size_t len);
void serial_sends(ushort com, char *data);
byte serial_recv(ushort com);