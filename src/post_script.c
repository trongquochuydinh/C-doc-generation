#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "post_script.h"
#include "parser.h"
#include "utils.h"

#define PI 3.14159265358979323846
#define EPSILON 0.001
#define Y_THRESHOLD 10.0  /* Threshold to skip large y-values for asymptotes */
#define INFINITY HUGE_VALF

/* 
 * Generates the PostScript file by parsing the expression and plotting the graph.
 */
void generate_postscript(const char *outfile, const char *func, double x_min, double x_max, double y_min, double y_max, int calc_x_range, int calc_y_range) {
    FILE *ps_file = initialize_postscript(outfile);
    Node* expression_tree = parse_expression(&func);
    double step = 0.001;

    /* Calculate ranges if necessary */
    calculate_ranges(expression_tree, &x_min, &x_max, &y_min, &y_max, step, calc_x_range, calc_y_range);

    /* Draw grid, axes, and the graph */
    draw_grid(ps_file);
    plot_graph(ps_file, expression_tree, x_min, x_max, y_min, y_max, step);
    draw_axes_and_labels(ps_file, x_min, x_max, y_min, y_max);

    /* Cleanup */
    free_tree(expression_tree);
    fclose(ps_file);
}

/* 
 * Opens and initializes the PostScript file for writing.
 */
FILE* initialize_postscript(const char *outfile) {
    FILE *ps_file = fopen(outfile, "w");
    if (ps_file == NULL) {
        fprintf(stderr, "Error opening file for writing: %s\n", outfile);
        exit(1);
    }
    fprintf(ps_file, "%%!PS-Adobe-2.0\n");
    fprintf(ps_file, "%%%%BoundingBox: 0 0 500 500\n");
    return ps_file;
}

/* 
 * Calculates the x and y ranges for the graph if not provided by the user.
 */
void calculate_ranges(Node *expression_tree, double *x_min, double *x_max, double *y_min, double *y_max, double step, int calc_x_range, int calc_y_range) {
    if (calc_x_range) {
        *x_min = -10;
        *x_max = 10;
    }
    if (calc_y_range) {
        *y_min = -10;
        *y_max = 10;
    } else {
        *y_min = INFINITY;
        *y_max = -INFINITY;
        for (double x = *x_min; x <= *x_max; x += step) {
            double y = evaluate(expression_tree, x);
            if (!is_nan(y)) {
                if (y < *y_min) *y_min = y;
                if (y > *y_max) *y_max = y;
            }
        }
    }
}

/* 
 * Draws a light gray grid on the PostScript canvas.
 */
void draw_grid(FILE *ps_file) {
    fprintf(ps_file, "newpath\n");
    fprintf(ps_file, "0.8 0.8 0.8 setrgbcolor\n");  /* Light gray grid lines */
    for (int i = 100; i <= 400; i += 30) {
        /* Vertical grid lines */
        fprintf(ps_file, "%d 100 moveto\n", i);
        fprintf(ps_file, "%d 400 lineto\n", i);
        fprintf(ps_file, "stroke\n");

        /* Horizontal grid lines */
        fprintf(ps_file, "100 %d moveto\n", i);
        fprintf(ps_file, "400 %d lineto\n", i);
        fprintf(ps_file, "stroke\n");
    }
}

/* 
 * Plots the graph of the mathematical function.
 */
void plot_graph(FILE *ps_file, Node *expression_tree, double x_min, double x_max, double y_min, double y_max, double step) {
    fprintf(ps_file, "newpath\n");
    fprintf(ps_file, "1 0 0 setrgbcolor\n"); /* Red color for the graph */

    double x_scale = 300.0 / (x_max - x_min);
    double y_scale = 300.0 / (y_max - y_min);
    double x_offset = 250 - (x_max - x_min) * x_scale / 2;
    double y_offset = 250 - (y_max - y_min) * y_scale / 2;

    int start_new_line = 1;
    for (double x = x_min; x <= x_max; x += step) {
        double y = evaluate(expression_tree, x);
        if (is_nan(y)) {
            start_new_line = 1;
            continue;
        }

        double ps_x = x_offset + (x - x_min) * x_scale;
        double ps_y = y_offset + (y - y_min) * y_scale;

        if (ps_x < 100 || ps_x > 400 || ps_y < 100 || ps_y > 400) {
            start_new_line = 1;
            continue;
        }

        if (start_new_line) {
            fprintf(ps_file, "%lf %lf moveto\n", ps_x, ps_y);
            start_new_line = 0;
        } else {
            fprintf(ps_file, "%lf %lf lineto\n", ps_x, ps_y);
        }
    }
    fprintf(ps_file, "stroke\n");
}

/* 
 * Draws the bounding box, axes, and labels on the PostScript canvas.
 */
void draw_axes_and_labels(FILE *ps_file, double x_min, double x_max, double y_min, double y_max) {
    double x_range = x_max - x_min;
    double y_range = y_max - y_min;

    fprintf(ps_file, "newpath\n");
    fprintf(ps_file, "0 0 0 setrgbcolor\n");

    /* Bounding box */
    fprintf(ps_file, "100 100 moveto\n400 100 lineto\n400 400 lineto\n100 400 lineto\n100 100 lineto\nstroke\n");

    /* X and Y axis labels */
    fprintf(ps_file, "/Courier findfont 9 scalefont setfont\n");
    fprintf(ps_file, "250 60 moveto (x) show\n");
    fprintf(ps_file, "30 250 moveto\n90 rotate\n(f(x)) show\n-90 rotate\n");

    for (int i = 100; i <= 400; i += 30) {
        /* X-axis ticks and labels */
        fprintf(ps_file, "%d 90 moveto %d 110 lineto stroke\n", i, i);
        fprintf(ps_file, "%d 80 moveto (%0.1f) show\n", i - 10, x_min + (i - 100) * x_range / 300.0);

        /* Y-axis ticks and labels */
        fprintf(ps_file, "90 %d moveto 110 %d lineto stroke\n", i, i);
        fprintf(ps_file, "50 %d moveto (%0.1f) show\n", i - 5, y_min + (i - 100) * y_range / 300.0);
    }
}
