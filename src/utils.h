#ifndef UTILS_H
#define UTILS_H

/* Remove all spaces from a string */
void remove_whitespace(char* dest, const char* src);
/* Create NaN*/
double create_nan();
/* Check if the value is NaN */
int is_nan(double x);

#endif
