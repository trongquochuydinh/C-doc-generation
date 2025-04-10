#ifndef UTILS_H
#define UTILS_H

/**
 * @brief Removes all whitespace characters from a source string.
 * 
 * Copies the input string `src` into the output buffer `dest` while removing
 * all whitespace characters. The resulting string in `dest` is null-terminated.
 * 
 * @param[out] dest The destination buffer to store the string without whitespace.
 * @param[in] src The input string to process.
 */
void remove_whitespace(char* dest, const char* src);

/**
 * @brief Creates a Not-a-Number (NaN) value.
 * 
 * This function generates a NaN value by directly manipulating the IEEE 754 
 * binary representation of a double-precision floating-point number.
 * 
 * @return double Returns a NaN value.
 */
double create_nan();

/**
 * @brief Checks whether a double-precision value is NaN.
 * 
 * Inspects the binary representation of the double value to determine
 * if it corresponds to a NaN (Not-a-Number) state.
 * 
 * @param[in] x The double value to check.
 * @return int Returns 1 if the value is NaN, otherwise returns 0.
 */
int is_nan(double x);

#endif /* UTILS_H */
