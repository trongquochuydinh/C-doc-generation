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
    *dest = '\0';  // Null-terminate the destination string
}

