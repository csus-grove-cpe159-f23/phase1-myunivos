/**
 * CPE/CSC 159 Operating System Pragmatics
 * California State University, Sacramento
 *
 * Keyboard driver implementation
 */

#include "vga.h"
#include "keyboard.h"
#include "bit.h"

#define KEY_SHIFT     0x2A
#define KEY_CTRL      0x1D
#define KEY_ALT       0x38
#define KEY_CAPS_LOCK 0x3A
#define KEY_NUMLOCK   0x45
#define KEY_KERNEL_DEBUG (KEY_SHIFT | KEY_CTRL | KEY_ALT)

// Variables to track the status of special keys
static int shift_pressed = 0;
static int ctrl_pressed = 0;
static int alt_pressed = 0;
static int caps_lock_enabled = 0;

// Declare the inportb function
unsigned char inportb(unsigned short port);

/**
 * Initializes keyboard data structures and variables
 */
void keyboard_init() {
    // Additional initialization can be added if needed
}

/**
 * Scans for keyboard input and returns the raw character data
 * @return raw character data from the keyboard
 */
unsigned int keyboard_scan(void) {
    return inportb(0x60);  // Read from the keyboard data port
}

/**
 * Decodes a scancode into an ASCII character code or “special key” definition
 */
unsigned int keyboard_decode(unsigned int scan_code) {
    int key_pressed = !(scan_code & 0x80);
    unsigned int key_code = scan_code & 0x7F;

    switch (key_code) {
        case KEY_SHIFT:
            shift_pressed = key_pressed;
            break;
        case KEY_CTRL:
            ctrl_pressed = key_pressed;
            break;
        case KEY_ALT:
            alt_pressed = key_pressed;
            break;
        case KEY_CAPS_LOCK:
            if (key_pressed) caps_lock_enabled = !caps_lock_enabled;
            break;
        case KEY_NUMLOCK:
            // Handle NUMLOCK key here if needed
            break;
        // Add more cases for other special keys...
        default:
            // Implement custom logic for decoding other keys if needed
            return KEY_NULL;
    }

    return KEY_NULL;
}

/**
 * Polls for a keyboard character to be entered.
 *
 * If a keyboard character data is present, will scan and return
 * the decoded keyboard output.
 *
 * @return decoded character or KEY_NULL (0) for any character
 *         that cannot be decoded
 */
// keyboard.c
unsigned int keyboard_poll(void) {
    if (inportb(0x64) & 0x01) {  // Check if keyboard data is available
        unsigned int scan_code = keyboard_scan();
        unsigned int decoded_key = keyboard_decode(scan_code);
        printf("Scancode: %x, Decoded Key: %x\n", scan_code, decoded_key);  // Add debugging output
        return decoded_key;
    }
    return KEY_NULL;
}


/**
 * Blocks until a keyboard character has been entered
 * @return decoded character entered by the keyboard or KEY_NULL
 *         for any character that cannot be decoded
 */
unsigned int keyboard_getc(void) {
    unsigned int c = KEY_NULL;
    while ((c = keyboard_poll()) == KEY_NULL);
    return c;
}
