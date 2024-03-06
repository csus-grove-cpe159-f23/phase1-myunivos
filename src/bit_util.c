/**
 * CPE/CSC 159 Operating System Pragmatics
 * California State University, Sacramento
 *
 * Bit Utilities
 */
#include "bit_util.h"

/**
 * Counts the number of bits that are set
 * @param value - the integer value to count bits in
 * @return number of bits that are set
 */
int bit_count(int value) {
    int count = 0;
    while (value) {
        if (value & 1) {
            ++count;
        }
        value >>= 1;
    }
    return count;
}

/**
 * Checks if the given bit is set
 * @param value - the integer value to test
 * @param bit - which bit to check
 * @return 1 if set, 0 if not set
 */
int bit_test(int value, int bit) {
    int num = value >> bit;
    return (num & 1);
}

/**
 * Sets the specified bit in the given integer value
 * @param value - the integer value to modify
 * @param bit - which bit to set
 */
int bit_set(int value, int bit) {
    return ((1 << bit) | value);
}

/**
 * Clears the specified bit in the given integer value
 * @param value - the integer value to modify
 * @param bit - which bit to clear
 */
int bit_clear(int value, int bit) {
    return (value & ~(1 << bit));
}

/**
 * Toggles the specified bit in the given integer value
 * @param value - the integer value to modify
 * @param bit - which bit to toggle
 */
int bit_toggle(int value, int bit) {
    return (value ^ (1 << bit));
}
