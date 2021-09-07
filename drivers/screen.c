#include "screen.h"
#include "io.h"
#include "../libc/string.h"

/*************** Private functions ***************/

/**
 * @brief Get the current cursor postion
 * 
 * @return uint The cursor position as an index into the frame buffer
 */
uint get_cursor_pos()
{
    port_byte_out(SCREEN_CTRL, VGA_CURSOR_HIGH);
    uint8 cursor_high = port_byte_in(SCREEN_DATA);

    port_byte_out(SCREEN_CTRL, VGA_CURSOR_LOW);
    uint8 cursor_low = port_byte_in(SCREEN_DATA);

    return (cursor_high << 8) + cursor_low;
}


/**
 * @brief Set the cursor pos object
 * 
 * @param cursor_pos 
 */
void set_cursor_pos(uint cursor_pos)
{
    port_byte_out(SCREEN_CTRL, VGA_CURSOR_HIGH);
    port_byte_out(SCREEN_DATA, (cursor_pos & 0xff00) >> 8);

    port_byte_out(SCREEN_CTRL, VGA_CURSOR_LOW);
    port_byte_out(SCREEN_DATA, cursor_pos & 0xff);
}

/**
 * @brief Clear a row by setting all cells to character \x00 and color white on black
 * 
 * @param row Row number
 */
void clear_row(uint row)
{
    byte *row_ptr = (byte*)CELL_PTR(GET_ROW_POS(row));
    int i;
    // set the first byte of each word (the character byte) to 0
    for (i = 0; i < FRAME_BUFFER_COLS*2; i += 2)
        row_ptr[i] = 0;
    // set the second byte of each word (the color byte) to black on white
    for (i = 1; i < FRAME_BUFFER_COLS*2; i += 2)
        row_ptr[i] = WHITE_ON_BLACK;
}

/**
 * @brief Scroll the screen one row upwards
 * 
 */
void scroll()
{
    // copy all rows one row up
    int row;
    for (row = 1; row <= FRAME_BUFFER_ROWS; row++)
    {
        memcpy(CELL_PTR(GET_ROW_POS(row-1)), CELL_PTR(GET_ROW_POS(row)), FRAME_BUFFER_COLS*2);
    }
    
    // clear the last row
    clear_row(FRAME_BUFFER_ROWS-1);
}

/*************** Public functions ****************/

/**
 * @brief Print a character to the text-mode screen. 
 * Default color (white on black) is assumed.
 * 
 * @param c The character to print
 * @return uint The new cursor position
 */
uint kputchar(uchar c)
{
    return kputchar_color(c, WHITE_ON_BLACK);
}


/**
 * @brief Print a character to the text mode screen.
 * 
 * @param c The character to print
 * @param color The color of the character
 * @return uint The new cursor position
 */
uint kputchar_color(uchar c, uchar color)
{
    // find location of the cursor
    uint cursor_pos = get_cursor_pos();
    
    // make sure the location is within the bounds - if not, scroll the screen
    if (GET_ROW(cursor_pos) >= FRAME_BUFFER_ROWS)
    {
        scroll();
        cursor_pos = LAST_ROW_POS;
    }

    // handle carriage return by moving the cursor to the beginning of the line
    if (c == '\r')
    {
        cursor_pos = GET_ROW_POS(GET_ROW(cursor_pos));
        set_cursor_pos(cursor_pos);
    }
    
    // handle line feed by moving the cursor to the beginning of the next line, scrolling the screen if needed
    else if (c == '\n')
    {
        cursor_pos = GET_ROW_POS(GET_ROW(cursor_pos)+1);
        if (GET_ROW(cursor_pos) >= FRAME_BUFFER_ROWS)
        {
            scroll();
            cursor_pos = LAST_ROW_POS;
        }
        set_cursor_pos(cursor_pos);
    }

    else
    {
        // calculate value to write to frame buffer
        ushort value = VGA_CELL(c, color);

        // write to frame buffer
        ushort *buffer = (ushort*)FRAME_BUFFER;
        buffer[cursor_pos] = value;

        cursor_pos++;

        // increment cursor position
        set_cursor_pos(cursor_pos);
    }

    return cursor_pos;
}

/**
 * @brief Print a null-terminated string to the text mode screen.
 * Default color (white on black) is assumed.
 * 
 * @param message The null-terminated string to print
 * @return uint The new cursor position
 */
uint kprint(uchar *message)
{
    return kprint_color(message, WHITE_ON_BLACK);
}

/**
 * @brief Print a null-terminated string to the text mode screen.
 * 
 * @param message The null-terminated string to print
 * @param color The color of the string
 * @return uint The new cursor position
 */
uint kprint_color(uchar *message, uchar color)
{
    uchar c;
    size_t i = 0;
    uint pos;
    while ((c=message[i++]) != '\0')
    {
        pos = kputchar_color(c, color);
    }
    return pos;
}

/**
 * @brief Clear the screen by setting the character of all cells to \x00 and the color to white on black
 * 
 */
void kcls()
{
    // clear all rows
    int row;
    for (row = 0; row < FRAME_BUFFER_ROWS; row++)
    {
        clear_row(row);
    }

    // set cursor to the beginning of the first row
    set_cursor_pos(GET_ROW_POS(0));
}