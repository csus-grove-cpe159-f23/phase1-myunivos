/**
 * CPE/CSC 159 Operating System Pragmatics
 * California State University, Sacramento
 *
 * Keyboard driver implementation
 */
#include "io.h"
#include "kernel.h"
#include "vga.h"
#include "keyboard.h"
#include "bit.h"

#define KEY_SHIFT     0x2A
#define KEY_CTRL      0x1D
#define KEY_ALT       0x38
#define KEY_CAPS_LOCK 0x3A
#define KEY_NUMLOCK 0x45 // Assuming 0x45 is the scancode for Num Lock
#define KEY_KERNEL_DEBUG (KEY_SHIFT | KEY_CTRL | KEY_ALT)

// Variables to track the status of special keys
static int shift_pressed = 0;
static int ctrl_pressed = 0;
static int alt_pressed = 0;
static int caps_lock_enabled = 0;
static int numlock_active = 0; 

//ASCII mapping for scancodes

// Declare the inportb function
unsigned char inportb(unsigned short port);

/**
 * Initializes keyboard data structures and variables
 */
void keyboard_init() {
    kernel_log_info("Initializing keyboard driver");
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
/**
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
**/
unsigned int keyboard_decode(unsigned int scan_code) {
    static const char scancode_to_char[] = {
        0, 0, '1', '2', '3', '4', '5', '6', '7', '8',  /* 9 */
        '9', '0', '-', '=', 0, 0, 'q', 'w', 'e', 'r',  /* 19 */
        't', 'y', 'u', 'i', 'o', 'p', '[', ']', 0, 0,   /* 29 */
        'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', /* 39 */
        '\'', '`', 0, '\\', 'z', 'x', 'c', 'v', 'b', 'n', /* 49 */
        'm', ',', '.', '/', 0, '*', 0, ' '               /* 57 */
    };

    int key_pressed = !(scan_code & 0x80);
    unsigned int key_code = scan_code & 0x7F;

    // Handle special keys (Shift, Ctrl, Alt, etc.)
    switch (key_code) {
        case KEY_SHIFT:
            shift_pressed = key_pressed;
            return KEY_NULL;
        case KEY_CTRL:
            ctrl_pressed = key_pressed;
            return KEY_NULL;
        case KEY_ALT:
            alt_pressed = key_pressed;
            return KEY_NULL;
        case KEY_CAPS_LOCK:
            if (key_pressed){
                caps_lock_enabled = !caps_lock_enabled;
            }
            return KEY_NULL;
        case KEY_NUMLOCK:
            if (key_pressed){
                numlock_active = !numlock_active;
            }
            return KEY_NULL;
        default:
            // Check if the key code is within the range of our mapping
            if (key_code < sizeof(scancode_to_char)) {
                // Return the corresponding ASCII character
                return scancode_to_char[key_code];
            }
            return KEY_NULL;
    }
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

/**
=======
// keyboard.c
>>>>>>> 27e4a08ff47f916ab9b029323b6216e5a61be003
unsigned int keyboard_poll(void) {
    if (inportb(0x64) & 0x01) {  // Check if keyboard data is available
        unsigned int scan_code = keyboard_scan();
        unsigned int decoded_key = keyboard_decode(scan_code);
        printf("Scancode: %x, Decoded Key: %x\n", scan_code, decoded_key);  // Add debugging output
        return decoded_key;
    }
    return KEY_NULL;
}
**/
unsigned int keyboard_poll(void) {
    if (inportb(KBD_PORT_STAT) & 0x01) {  // Check if keyboard data is available
        unsigned int scan_code = keyboard_scan();
        unsigned int decoded_key = keyboard_decode(scan_code);

        // If decoded_key is a printable ASCII character, display it on the screen
        if (decoded_key >= 0x20 && decoded_key <= 0x7E) {
            vga_putc(decoded_key);  // Display the character using VGA driver
        }

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
