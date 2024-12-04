#include "utils.h"
#include <ctype.h>
#include <string.h>

/* Function to remove whitespace from a string */
void remove_whitespace(char* dest, const char* src) {
    while (*src != '\0') {
        if (!isspace((unsigned char)*src)) {
            *dest++ = *src;
        }
        src++;
    }
    *dest = '\0';
}

double create_nan() {
    unsigned long nan_bits = 0x7FF8000000000000;
    double nan_value;
    memcpy(&nan_value, &nan_bits, sizeof(nan_value));
    return nan_value;
}

int is_nan(double x) {
    unsigned long bits;
    memcpy(&bits, &x, sizeof(bits));
    return (bits & 0x7FF0000000000000) == 0x7FF0000000000000 && (bits & 0x000FFFFFFFFFFFFF) != 0;
}