#include "utils.h"
#include <ctype.h>
#include <string.h>

/* 
 * Removes all whitespace characters from the source string and 
 * copies the result into the destination string.
 */
void remove_whitespace(char* dest, const char* src) {
    while (*src != '\0') {
        if (!isspace((unsigned char)*src)) {
            *dest++ = *src;
        }
        src++;
    }
    *dest = '\0';
}

/* 
 * Creates a Not-a-Number (NaN) value.
 * The function directly manipulates the binary representation of a double to create NaN.
 */
double create_nan() {
    unsigned long nan_bits = 0x7FF8000000000000; /* IEEE 754 representation for NaN */
    double nan_value;
    memcpy(&nan_value, &nan_bits, sizeof(nan_value));
    return nan_value;
}

/* 
 * Checks whether the provided double value is a NaN (Not-a-Number).
 * It inspects the binary representation to determine if the value is NaN.
 */
int is_nan(double x) {
    unsigned long bits;
    memcpy(&bits, &x, sizeof(bits));
    return (bits & 0x7FF0000000000000) == 0x7FF0000000000000 && (bits & 0x000FFFFFFFFFFFFF) != 0;
}
