#ifndef POST_SCRIPT_H
#define POST_SCRIPT_H

/* Forward declaration for PostScript generation */
void generate_postscript(const char *outfile, const char *func, double x_min, double x_max, double y_min, double y_max, int calc_x_range, int calc_y_range);

#endif