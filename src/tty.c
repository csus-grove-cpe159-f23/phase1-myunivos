/**
 * CPE/CSC 159 - Operating System Pragmatics
 * California State University, Sacramento
 *
 * TTY Definitions
 */

#include <spede/string.h>

#include "kernel.h"
#include "timer.h"
#include "tty.h"
#include "vga.h"

// TTY Table
struct tty_t tty_table[TTY_MAX];

// Current Active TTY
struct tty_t *active_tty;

/**
 * Sets the active TTY to the selected TTY number
 * @param tty - TTY number
 */
void tty_select(int n) {
    // Set the active tty to point to the entry in the tty table
    // if a new tty is selected, the tty should trigger a refresh
}

/**
 * Refreshes the tty if needed
 */
void tty_refresh(void) {
    if (!active_tty) {
        kernel_panic("No TTY is selected!");
        return;
    }

    // If the TTY needs to be refreshed, copy the tty buffer
    // to the VGA output.
    // ** hint: use vga_putc_at() since you are setting specific characters
    //          at specific locations
    // Reset the tty's refresh flag so we don't refresh unnecessarily
    if (active_tty->refresh) {
        for (int y = 0; y < TTY_HEIGHT; y++) {
            for (int x = 0; x < TTY_WIDTH; x++) {
                char c = active_tty->buf[y * TTY_WIDTH + x];
                vga_putc_at(x, y, active_tty->color_fg, active_tty->color_bg, c);
            }
        }
        active_tty->refresh = 0;  // Reset refresh flag
    }
}

/**
 * Updates the active TTY with the given character
 */
void tty_update(char c) {
    if (!active_tty) {
        return;
    }

    // Since this is a virtual wrapper around the VGA display, treat each
    // input character as you would for the VGA output
    //   Adjust the x/y positions as necessary
    //   Handle scrolling at the bottom

    // Instead of writing to the VGA display directly, you will write
    // to the tty buffer.
    //
    // If the screen should be updated, the refresh flag should be set
    // to trigger the the VGA update via the tty_refresh callback
        switch (c) {
        case '\n':  // New Line
            active_tty->pos_y++;
            active_tty->pos_x = 0;
            break;
        case '\r':  // Carriage Return
            active_tty->pos_x = 0;
            break;
        case '\b':  // Backspace
            if (active_tty->pos_x > 0) {
                active_tty->pos_x--;
            }
            break;
        case '\t':  // Horizontal Tab
            active_tty->pos_x = (active_tty->pos_x + 8) & ~(8 - 1);
            break;
        default:    // Printable Characters
            if (active_tty->pos_x >= TTY_WIDTH) {
                active_tty->pos_x = 0;
                active_tty->pos_y++;
            }
            if (active_tty->pos_y >= TTY_HEIGHT) {
                // Implement scrolling or wrap around
                active_tty->pos_y = 0;
            }
            active_tty->buf[active_tty->pos_y * TTY_WIDTH + active_tty->pos_x] = c;
            active_tty->pos_x++;
            break;
    }
        active_tty->refresh = 1; //for refresh
}

/**
 * Initializes all TTY data structures and memory
 * Selects TTY 0 to be the default
 */
void tty_init(void) {
    kernel_log_info("tty: Initializing TTY driver");

    // Initialize the tty_table
    for (int i = 0; i < TTY_MAX; i++){
        tty_table[i].id = i;
        memset(tty_table[i].buf, ' ', TTY_BUF_SIZE);  // Fill buffer with spaces
        tty_table[i].refresh = 1;  // Mark for initial refresh
        tty_table[i].color_bg = VGA_COLOR_BLACK;
        tty_table[i].color_fg = VGA_COLOR_WHITE;
        tty_table[i].pos_x = 0;
        tty_table[i].pos_y = 0;
        tty_table[i].pos_scroll = 0;

    }

    // Select tty 0 to start with
    active_tty = &tty_table[0];
    // Register a timer callback to update the screen on a regular interval
}

