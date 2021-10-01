#include "screen.h"
#include "io.h"
#include "../lib/string.h"

// rows that are locked - backspace from the beginning of a row cannot go back into the row above if it is locked
bool locked_rows[FRAME_BUFFER_ROWS];

/*************** Private functions ***************/

/**
 * @brief Get the current cursor postion
 * 
 * @return uint The cursor position as an index into the frame buffer
 */
uint get_cursor_pos()
{
    outb(SCREEN_CTRL, VGA_CURSOR_HIGH);
    uint8 cursor_high = inb(SCREEN_DATA);

    outb(SCREEN_CTRL, VGA_CURSOR_LOW);
    uint8 cursor_low = inb(SCREEN_DATA);

    return (cursor_high << 8) + cursor_low;
}

/**
 * @brief Set the cursor pos object
 * 
 * @param cursor_pos 
 */
void set_cursor_pos(uint cursor_pos)
{
    outb(SCREEN_CTRL, VGA_CURSOR_HIGH);
    outb(SCREEN_DATA, (cursor_pos & 0xff00) >> 8);

    outb(SCREEN_CTRL, VGA_CURSOR_LOW);
    outb(SCREEN_DATA, cursor_pos & 0xff);
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
        memcpy(CELL_PTR(GET_ROW_POS(row-1)), CELL_PTR(GET_ROW_POS(row)), FRAME_BUFFER_COLS*2);
    
    // clear the last row
    clear_row(FRAME_BUFFER_ROWS-1);

    // copy locked state of rows one row up
    for (row = 1; row <= FRAME_BUFFER_ROWS; row++)
        locked_rows[row -1] = locked_rows[row];
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

    switch(c)
    {
        // handle carriage return by moving the cursor to the beginning of the line
        case '\r':
            cursor_pos = GET_ROW_POS(GET_ROW(cursor_pos));
            set_cursor_pos(cursor_pos);
            break;
        
        // handle line feed by moving the cursor to the beginning of the next line, scrolling the screen if needed
        case '\n':
            locked_rows[GET_ROW(cursor_pos)] = TRUE;
            cursor_pos = GET_ROW_POS(GET_ROW(cursor_pos)+1);
            if (GET_ROW(cursor_pos) >= FRAME_BUFFER_ROWS)
            {
                scroll();
                cursor_pos = LAST_ROW_POS;
            }
            set_cursor_pos(cursor_pos);
            break;

        // handle escape by doing nothing
        case '\e':
            break;

        // handle backspace by moving the cursor one cell back and setting it's contents to none
        case '\b':
            // make sure we aren't returning to the previous row, or if we are, that it is allowed
            if ((GET_ROW(cursor_pos) == GET_ROW(cursor_pos-1)) || locked_rows[GET_ROW(cursor_pos)-1] == FALSE)
            {
                cursor_pos -= 1;
                set_cursor_pos(cursor_pos);
                ushort *buffer = (ushort*)FRAME_BUFFER;
                buffer[cursor_pos] = VGA_CELL(0, color);
            }
            break;
        
        // handle tabs by checking how close we are to a tab boundary and adding spaces accordingly
        case '\t':
            int pos_in_row = cursor_pos % FRAME_BUFFER_COLS;
            int spaces = TAB_SIZE - (pos_in_row % TAB_SIZE);
            
            // we are on a tab boundary - add a full tab worth of spaces
            if (spaces == 0)
                spaces = TAB_SIZE;
            
            // add spaces
            while (spaces--)
                cursor_pos = kputchar_color(' ', color);
            break;

        // printable character
        default:
            // calculate value to write to frame buffer
            ushort value = VGA_CELL(c, color);

            // write to frame buffer
            ushort *buffer = (ushort*)FRAME_BUFFER;
            buffer[cursor_pos] = value;

            // increment cursor position and scroll the screen if it is out of bounds
            cursor_pos++;
            if (GET_ROW(cursor_pos) >= FRAME_BUFFER_ROWS)
            {
                scroll();
                cursor_pos = LAST_ROW_POS;
            }

            set_cursor_pos(cursor_pos);
            break;
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

uint kprint_async(uchar *message)
{
    uint cursor_pos = get_cursor_pos();
    
    byte *current_row = (byte*)CELL_PTR(GET_ROW_POS(GET_ROW(cursor_pos)));
    byte temprow[FRAME_BUFFER_COLS*2];

    // check if row is empty
    bool row_empty = TRUE;
    int i, j;
    for (i = 0; i < FRAME_BUFFER_COLS*2; i += 2)
    {
        // cell is empty if the character is '\0' or a space
        if (current_row[i] != 0 && current_row[i] != ' ')
        {
            row_empty = FALSE;
            break;
        }
    }

    // save contents of current row if needed
    if (!row_empty)
    {
        memcpy(temprow, current_row, FRAME_BUFFER_COLS*2);
        kputchar('\n');
    }

    // save current column in row
    uint col = GET_COL(cursor_pos);

    // input the requested message
    cursor_pos = kprint(message);

    // copy saved row contents to the new empty row if needed
    if (!row_empty)
    {
        current_row = CELL_PTR(GET_ROW_POS(GET_ROW(cursor_pos)));
        memcpy(current_row, temprow, FRAME_BUFFER_COLS*2);
    }

    // move cursor to the same column it was at
    cursor_pos += col;
    set_cursor_pos(cursor_pos);

    return cursor_pos;
}

/**
 * @brief Clear the screen by setting the character of all cells to \x00 and the color to white on black
 * 
 */
uint kcls()
{
    // clear all rows
    int row;
    for (row = 0; row < FRAME_BUFFER_ROWS; row++)
        clear_row(row);

    // set cursor to the beginning of the first row
    uint cursor_pos = GET_ROW_POS(0);
    set_cursor_pos(cursor_pos);
    return cursor_pos;
}

/**
 * @brief Initialize the screen. After BIOS runs, all "empty" cells in the screen buffer are actually spaces.
 * This is a problem because arrow key handling assumes empty cells (that sould not be moved over) are null bytes.
 * Thus we need to empty all these cells.
 * 
 */
uint init_screen()
{
    // clear all rows from the row the cursor is currently on
    uint cursor_pos = get_cursor_pos();
    int row;
    for (row = GET_ROW(cursor_pos); row < FRAME_BUFFER_ROWS; row++)
        clear_row(row);
    
    // set cursor position to beginning of the current row
    cursor_pos = GET_ROW_POS(GET_ROW(cursor_pos));
    set_cursor_pos(cursor_pos);
    return cursor_pos;
}

uint cursor_left()
{
    int cursor_pos = get_cursor_pos();

    // make sure we aren't going out of the screen bounds
    if (cursor_pos - 1 < 0)
        return cursor_pos;

    // make sure we aren't returning to the previous row, or if we are, that it is allowed
    if ((GET_ROW(cursor_pos) == GET_ROW(cursor_pos-1)) || locked_rows[GET_ROW(cursor_pos)-1] == FALSE)
    {
        cursor_pos -= 1;
        set_cursor_pos(cursor_pos);
    }
    return cursor_pos;
}

uint cursor_right()
{
    uint cursor_pos = get_cursor_pos();

    // make sure the location is within the bounds
    if (GET_ROW(cursor_pos) >= FRAME_BUFFER_ROWS)
        return cursor_pos;
    
    // make sure we aren't moving over an empty cell
    byte *cell = CELL_PTR(cursor_pos);
    if (cell[0] != 0)
    {
        cursor_pos++;
        set_cursor_pos(cursor_pos);
    }
    return cursor_pos;
}

uint cursor_up()
{
    // do nothing for now
    uint cursor_pos = get_cursor_pos();
    return cursor_pos;
}

uint cursor_down()
{
    // do nothing for now
    uint cursor_pos = get_cursor_pos();
    return cursor_pos;
}
