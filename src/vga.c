#include <spede/stdarg.h>
#include <spede/stdio.h>

#include "bit.h"
#include "io.h"
#include "kernel.h"
#include "vga.h"

/**
 * Forward Declarations
 */
void vga_cursor_update(void);

enum vga_color{
    BLACK = 0,
    BLUE = 1,
    GREEN = 2,
    CYAN = 3,
    RED = 4,
    MAGENTA = 5,
    BROWN = 6,
    LIGHT_GREY = 7,
    DARK_GREY = 8,
    LIGHT_BLUE = 9,
    LIGHT_GREEN = 10,
    LIGHT_CYAN = 11,
    LIGHT_RED = 12,
    PINK = 13,
    YELLOW = 14,
    WHITE = 15
}
/**
 * Global variables in this file scope
 */

/**
 * Initializes the VGA driver and configuration
 *  - Defaults variables
 *  - Clears the screen
 */

int current_row = 0;
int current_col = 0;
//int current_bg = BLACK; //Default
//int current_fg = WHITE; //Default
void vga_init(void) {
    kernel_log_info("Initializing VGA driver");
    // Clear the screen
    vga_clear();
}

/**
 * Clears the VGA output and sets the background and foreground colors
 */
void vga_clear(void) {
    // Clear all character data, set the foreground and background colors
    // Set the cursor position to the top-left corner (0, 0)
    int row, col;
    for(row = 0; row < VGA_HEIGHT; row++){
        for (col = 0; col < VGA_WIDTH; col++){
            vga_putc(row, col, BLACK, WHITE, ' ');
        }
    }
}

/**
 * Clears the background color for all characters and sets to the
 * specified value
 *
 * @param bg background color value
 */
void vga_clear_bg(int bg) {
    // Iterate through all VGA memory and set only the background color bits
    int row, col;
    unsigned short *vga_buf = VGA_BASE;
    for (row = 0; row < VGA_HEIGHT; row++){
        for(col = 0; col < VGA_WIDTH; col++){
            int fg = vga_buf[row * VGA_WIDTH + col] & 0x0F;
            vga_buf[row * VGA_WIDTH + col] = VGA_CHAR(bg, fg, ' ');
        }
    }
}

/**
 * Clears the foreground color for all characters and sets to the
 * specified value
 *
 * @param fg foreground color value
 */
void vga_clear_fg(int fg) {
    // Iterate through all VGA memory and set only the foreground color bits.
    int row, col;
    unsigned short *vga_buf = VGA_BASE;
    for (row = 0; row < VGA_HEIGHT; row++){
        for (col = 0; col < VGA_WIDTH; col++){
            int bg = (vga_buf[row * VGA_WIDTH + col] >> 4) & 0x0F;
            vga_buf[row * VGA_WIDTH + col] = VGA_CHAR(bg, fg, ' ');
        }
    }
}

/**
 * Enables the VGA text mode cursor
 */
void vga_cursor_enable(void) {
    // All operations will consist of writing to the address port which
    // register should be set, followed by writing to the data port the value
    // to set for the specified register

    // The cursor will be drawn between the scanlines defined
    // in the following registers:
    //   0x0A Cursor Start Register
    //   0x0B Cursor End Register

    // Bit 5 in the cursor start register (0x0A) will enable or disable the cursor:
    //   0 - Cursor enabled
    //   1 - Cursor disabled

    // The cursor will be displayed as a series of horizontal lines that create a
    // "block" or rectangular symbol. The position/size is determined by the "starting"
    // scanline and the "ending" scanline. Scanlines range from 0x0 to 0xF.

    // In both the cursor start register and cursor end register, the scanline values
    // are specified in bits 0-4

    // To ensure that we do not change bits we are not intending,
    // read the current register state and mask off the bits we
    // want to save

    // Set the cursor starting scanline (register 0x0A, cursor start register)

    // Set the cursor ending scanline (register 0x0B, cursor end register)
    // Ensure that bit 5 is not set so the cursor will be enabled

    // Since we may need to update the vga text mode cursor position in
    // the future, ensure that we track (via software) if the cursor is
    // enabled or disabled

    // Update the cursor location once it is enabled
    outportb(0x3D4, 0x0A);
    char cursor_start = inportb(0x3D5) & 0x1F; // Keep the lower 5 bits
    outportb(0x3D5, cursor_start);

    outportb(0x3D4, 0x0B);
    char cursor_end = inportb(0x3D5) & 0x1F; // Keep the lower 5 bits
    outportb(0x3D5, cursor_end);

    vga_cursor_update();
}

/**
 * Disables the VGA text mode cursor
 */
void vga_cursor_disable(void) {
    // All operations will consist of writing to the address port which
    // register should be set, followed by writing to the data port the value
    // to set for the specified register

    // The cursor will be drawn between the scanlines defined
    // in the following registers:
    //   0x0A Cursor Start Register
    //   0x0B Cursor End Register

    // Bit 5 in the cursor start register (0x0A) will enable or disable the cursor:
    //   0 - Cursor enabled
    //   1 - Cursor disabled

    // Since we are disabling the cursor, we can simply set the bit of interest
    // as we won't care what the current cursor scanline start/stop values are

    // Since we may need to update the vga text mode cursor position in
    // the future, ensure that we track (via software) if the cursor is
    // enabled or disabled   
    outportb(0x3D4, 0x0A);
    outportb(0x3D5, 0x20); // Set bit 5 to disable the cursor

}
/**
 * Indicates if the VGA text mode cursor is enabled or disabled
 */
bool vga_cursor_enabled(void) {
    outportb(0x3D4, 0x0A);
    return !(inportb(0x3D5) & 0x20); // Check if bit 5 is set
}

/**
 * Sets the vga text mode cursor position to the current VGA row/column
 * position if the cursor is enabled
 */
void vga_cursor_update(void) {
    // All operations will consist of writing to the address port which
    // register should be set, followed by writing to the data port the value
    // to set for the specified register

    // The cursor position is represented as an unsigned short (2-bytes). As
    // VGA register values are single-byte, the position representation is
    // split between two registers:
    //   0x0F Cursor Location High Register
    //   0x0E Cursor Location Low Register

    // The Cursor Location High Register is the least significant byte
    // The Cursor Location Low Register is the most significant byte

    // If the cursor is enabled:
        // Calculate the cursor position as an offset into
        // memory (unsigned short value)

        // Set the VGA Cursor Location High Register (0x0F)
        //   Should be the least significant byte (0x??<00>)

        // Set the VGA Cursor Location Low Register (0x0E)
        //   Should be the most significant byte (0x<00>??)
    // Assuming 'current_row' and 'current_col' track the cursor position
    unsigned short position = (current_row * VGA_WIDTH) + current_col;

    // Cursor LOW port to VGA INDEX register
    outportb(0x3D4, 0x0F);
    outportb(0x3D5, (unsigned char)(position & 0xFF));

    // Cursor HIGH port to VGA INDEX register
    outportb(0x3D4, 0x0E);
    outportb(0x3D5, (unsigned char)((position >> 8) & 0xFF));
}

}

/**
 * Sets the current row/column position
 *
 * @param row position (0 to VGA_HEIGHT-1)
 * @param col position (0 to VGA_WIDTH-1)
 * @notes If the input parameters exceed the valid range, the position
 *        will be set to the range boundary (min or max)
 */
void vga_set_rowcol(int row, int col) {
    // Update the text mode cursor (if enabled)
    //checking bounds
    if (row >= VGA_HEIGHT) row = VGA_HEIGHT - 1;
    if (col >= VGA_WIDTH) col = VGA_WIDTH - 1;
    if (row < 0) row = 0;
    if (col < 0) col = 0;

    current_row = row;
    current_col = col;

    //Update text mode cursor whcn enabled
    if (vga_cursor_enabled()){
    vga_cursor_update();
    }
}

/**
 * Gets the current row position
 * @return integer value of the row (between 0 and VGA_HEIGHT-1)
 */
int vga_get_row(void) {
    return current_row;
}

/**
 * Gets the current column position
 * @return integer value of the column (between 0 and VGA_WIDTH-1)
 */
int vga_get_col(void) {
    return current_col;
}

/**
 * Sets the background color.
 *
 * Does not modify any existing background colors, only sets it for
 * new operations.
 *
 * @param bg - background color
 */
int current_bg = BLACK; //default

void vga_set_bg(int bg) {
    current_bg = bg;
}

/**
 * Gets the current background color
 * @return background color value
 */
int vga_get_bg(void) {
    return current_bg;
}

/**
 * Sets the foreground/text color.
 *
 * Does not modify any existing foreground colors, only sets it for
 * new operations.
 *
 * @param color - background color
 */
int current_fg = WHITE; //Default foreground
void vga_set_fg(int fg) {
    current_fg = fg;
}

/**
 * Gets the current foreground color
 * @return foreground color value
 */
int vga_get_fg(void) {
    return current_fg;
}

/**
 * Prints a character on the screen without modifying the cursor or other attributes
 *
 * @param c - Character to print
 */
void vga_setc(unsigned char c) {
    vga_putc_at(current_row, current_col, current_bg, current_fg, c);
}

/**
 * Prints a character on the screen at the current cursor (row/column) position
 *
 * When a character is printed, will do the following:
 *  - Update the row and column positions
 *  - If needed, will wrap from the end of the current line to the
 *    start of the next line
 *  - If the last line is reached, the cursor position will reset to the top-left (0, 0) position
 *  - Special characters are handled as such:
 *    - tab character (\t) prints 'tab_stop' spaces
 *    - backspace (\b) character moves the character back one position,
 *      prints a space, and then moves back one position again
 *    - new-line (\n) should move the cursor to the beginning of the next row
 *    - carriage return (\r) should move the cursor to the beginning of the current row
 *
 * @param c - character to print
 */
void vga_putc(int row, int col, int bg, int fg, unsigned char c) {
        switch (c) {
        case '\n':
            current_row++;
            current_col = 0;
            break;
        case '\r':
            current_col = 0;
            break;
        case '\b':
            if (current_col > 0) current_col--;
            break;
        case '\t':
            current_col = (current_col + 4) & ~(4 - 1);
            break;
        default:
            vga_putc_at(current_row, current_col, current_bg, current_fg, c);
            current_col++;
            break;
    }

    // Handle end of lines and wrap-around
    if (current_col >= VGA_WIDTH) {
        current_col = 0;
        current_row++;
    }
    if (current_row >= VGA_HEIGHT) {
        current_row = 0; // Implement scrolling as needed
    }

    // Update the text mode cursor, if enabled
    if (vga_cursor_enabled()) {
        vga_cursor_update();
    }

    // Handle scecial characters

    // Handle end of lines

    // Wrap-around to the top/left corner

    // Update the text mode cursor, if enabled
}

/**
 * Prints a string on the screen at the current cursor (row/column) position
 *
 * @param s - string to print
 */
void vga_puts(char *s) {
    for (int i = 0; s[i] != '\0'; i++) {
        vga_putc(s[i]);
    }
}

/**
 * Prints a character on the screen at the specified row/column position and
 * with the specified background/foreground colors
 *
 * Does not modify the current row or column position
 * Does not modify the current background or foreground colors
 *
 * @param row the row position (0 to VGA_HEIGHT-1)
 * @param col the column position (0 to VGA_WIDTH-1)
 * @param bg background color
 * @param fg foreground color
 * @param c character to print
 */
void vga_putc_at(int row, int col, int bg, int fg, unsigned char c) {
    if (row >= 0 && row < VGA_HEIGHT && col >= 0 && col < VGA_WIDTH) {
        VGA_BASE[row * VGA_WIDTH + col] = VGA_CHAR(bg, fg, c);
    }
}

/**
 * Prints a string on the screen at the specified row/column position and
 * with the specified background/foreground colors
 *
 * Does not modify the current row or column position
 * Does not modify the current background or foreground colors
 *
 * @param row the row position (0 to VGA_HEIGHT-1)
 * @param col the column position (0 to VGA_WIDTH-1)
 * @param bg background color
 * @param fg foreground color
 * @param s string to print
 */
void vga_puts_at(int row, int col, int bg, int fg, char *s) {
    for (int i = 0; s[i] != '\0'; i++) {
        vga_putc_at(row, col + i, bg, fg, s[i]);
    }
}
