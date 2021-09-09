#include "serial.h"
#include "io.h"
#include "../libc/string.h"

void set_baud_rate(ushort com, uint baud)
{
    // set DLAB
    uint8 line_ctrl = port_byte_in(SERIAL_LINE_CTRL(com));
    port_byte_out(SERIAL_LINE_CTRL(com), line_ctrl | 0b10000000);

    // send baud rate divisor
    port_byte_out(SERIAL_DATA(com), (DIVISOR(baud) & 0xff));
    port_byte_out(SERIAL_INT_ENABLE(com), (DIVISOR(baud) >> 8) & 0xff);

    // clear DLAB
    port_byte_out(SERIAL_LINE_CTRL(com), line_ctrl & 0b01111111);
}

void init_serial(ushort com)
{
    // set baud rate to default
    set_baud_rate(com, DEFAULT_BAUD);

    // set data bits to default
    uint8 line_ctrl = port_byte_in(SERIAL_LINE_CTRL(com));
    line_ctrl = (line_ctrl & (~DATA_BITS_MASK)) | DEFAULT_DATA_BITS;

    // set stop bits to default
    line_ctrl = (line_ctrl & (~STOP_BITS_MASK)) | DEFAULT_STOP_BITS;

    // set parity to default
    line_ctrl = (line_ctrl & (~PARITY_MASK)) | DEFAULT_PARITY;

    // update line control
    port_byte_out(SERIAL_LINE_CTRL(com), line_ctrl);

    // disable all interrupts
    port_byte_out(SERIAL_INT_ENABLE(com), INT_NONE);
}

void serial_sendb(ushort com, byte data)
{
    port_byte_out(SERIAL_DATA(com), data);
}

void serial_send(ushort com, byte *data, size_t len)
{
    size_t i;
    for (i = 0; i < len; i++)
        serial_sendb(com, data[i]);
}

void serial_sends(ushort com, char *data)
{
    size_t len = strlen(data);
    serial_send(com, data, len);
}
