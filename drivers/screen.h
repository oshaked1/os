#include "../lib/types.h"

// memory mapped VGA video memory location
#define FRAME_BUFFER 0xb8000

// screen size constants
#define FRAME_BUFFER_ROWS 25
#define FRAME_BUFFER_COLS 80

// macro for defining a memory pointer to a cell position
#define CELL_PTR(pos) (void*)(FRAME_BUFFER + 2*(pos))

// macros for calulating the row and column based on cell position
#define GET_ROW(pos) ((pos) / FRAME_BUFFER_COLS)
#define GET_COL(pos) ((pos) % FRAME_BUFFER_COLS)

// macro for calculating the cell position of a given row and column
#define GET_POS(row, col) ((row) * FRAME_BUFFER_COLS + (col))

// macro for calculating the cell positon of the beginnig of a row
#define GET_ROW_POS(row) GET_POS((row), 0)

// macro for calculating the cell position of the beginning of the last row
#define LAST_ROW_POS GET_ROW_POS(FRAME_BUFFER_ROWS - 1)

// VGA I/O ports
#define SCREEN_CTRL 0x3d4
#define SCREEN_DATA 0x3d5

// VGA I/O control codes for cursor position
#define VGA_CURSOR_HIGH 14
#define VGA_CURSOR_LOW  15

#ifndef _VGA_COLORS
#define _VGA_COLORS
// all 16 VGA colors
enum VGA_COLORS{VGA_BLACK, VGA_BLUE, VGA_GREEN, VGA_CYAN,
                VGA_RED, VGA_MAGENTA, VGA_BROWN, VGA_LIGHT_GRAY,
                VGA_DARK_GRAY, VGA_LIGHT_BLUE, VGA_LIGHT_GREEN, VGA_LIGHT_CYAN,
                VGA_LIGHT_RED, VGA_LIGHT_MAGENTA, VGA_LIGHT_BROWN, VGA_WHITE};
#endif

// macro for generating a VGA color
// for example white foreground on black background: color = COLOR(VGA_WHITE, VGA_BLACK)
#define COLOR(fg, bg) ((((bg) & 0x0f) << 4) + ((fg) & 0x0f))

// shortcut for white on black, for more efficient code when using defaults
#define WHITE_ON_BLACK 0x0f

// macro for generating a VGA frame buffer cell value
#define VGA_CELL(char, color) (((ushort)(color) << 8) + (char))

// number of spaces per tab
#define TAB_SIZE 4

// public kernel functions
uint kputchar(uchar c);
uint kputchar_color(uchar c, uchar color);
uint kprint(uchar *message);
uint kprint_color(uchar *message, uchar color);
uint kprint_async(uchar *message);
uint kprint_at(uchar *message, uint row, uint col);
uint kprint_at_color(uchar *message, uint row, uint col, uchar color);
uint kcls();
uint init_screen();
uint cursor_left();
uint cursor_right();
uint cursor_up();
uint cursor_down();