#ifndef POST_SCRIPT_H
#define POST_SCRIPT_H

#include "parser.h"

/**
 * @brief Generates a PostScript file for visualizing a mathematical function.
 * 
 * This function handles the entire process of initializing the PostScript file, 
 * calculating ranges, drawing grid lines, plotting the function, and adding axes.
 * 
 * @param[in] outfile The name of the output PostScript file.
 * @param[in] func The mathematical function as a string.
 * @param[in] x_min The minimum x-coordinate of the range.
 * @param[in] x_max The maximum x-coordinate of the range.
 * @param[in] y_min The minimum y-coordinate of the range.
 * @param[in] y_max The maximum y-coordinate of the range.
 * @param[in] calc_x_range Flag to calculate x range automatically if set to 1.
 * @param[in] calc_y_range Flag to calculate y range automatically if set to 1.
 */
void generate_postscript(const char *outfile, const char *func, double x_min, double x_max, double y_min, double y_max, int calc_x_range, int calc_y_range);

/**
 * @brief Initializes and opens a PostScript file for writing.
 * 
 * This function opens a file in write mode and writes the required PostScript headers.
 * 
 * @param[in] outfile The name of the output PostScript file.
 * @return FILE* Pointer to the opened file. Exits the program on failure.
 */
FILE* initialize_postscript(const char *outfile);

/**
 * @brief Calculates the x and y ranges for the graph if not provided by the user.
 * 
 * If the ranges are to be calculated, this function evaluates the expression over a step size
 * and determines the minimum and maximum y-values for the given x range.
 * 
 * @param[in] expression_tree Pointer to the root of the parsed expression tree.
 * @param[in,out] x_min Pointer to the minimum x-coordinate.
 * @param[in,out] x_max Pointer to the maximum x-coordinate.
 * @param[in,out] y_min Pointer to the minimum y-coordinate.
 * @param[in,out] y_max Pointer to the maximum y-coordinate.
 * @param[in] step The step size used to sample points for range calculation.
 * @param[in] calc_x_range Flag to calculate x range automatically if set to 1.
 * @param[in] calc_y_range Flag to calculate y range automatically if set to 1.
 */
void calculate_ranges(Node *expression_tree, double *x_min, double *x_max, double *y_min, double *y_max, double step, int calc_x_range, int calc_y_range);

/**
 * @brief Draws light gray grid lines on the PostScript canvas.
 * 
 * This function draws vertical and horizontal grid lines on a predefined bounding box.
 * 
 * @param[in,out] ps_file Pointer to the PostScript file being written.
 */
void draw_grid(FILE *ps_file);

/**
 * @brief Plots the mathematical function on the PostScript canvas.
 * 
 * This function evaluates the mathematical function at evenly spaced points and 
 * connects these points with lines to form the graph.
 * 
 * @param[in,out] ps_file Pointer to the PostScript file being written.
 * @param[in] expression_tree Pointer to the root of the parsed expression tree.
 * @param[in] x_min The minimum x-coordinate of the range.
 * @param[in] x_max The maximum x-coordinate of the range.
 * @param[in] y_min The minimum y-coordinate of the range.
 * @param[in] y_max The maximum y-coordinate of the range.
 * @param[in] step The step size for sampling x values.
 */
void plot_graph(FILE *ps_file, Node *expression_tree, double x_min, double x_max, double y_min, double y_max, double step);

/**
 * @brief Draws axes, bounding box, and axis labels on the PostScript canvas.
 * 
 * This function includes ticks and labels for both the x-axis and y-axis based
 * on the calculated or provided ranges.
 * 
 * @param[in,out] ps_file Pointer to the PostScript file being written.
 * @param[in] x_min The minimum x-coordinate of the range.
 * @param[in] x_max The maximum x-coordinate of the range.
 * @param[in] y_min The minimum y-coordinate of the range.
 * @param[in] y_max The maximum y-coordinate of the range.
 */
void draw_axes_and_labels(FILE *ps_file, double x_min, double x_max, double y_min, double y_max);

#endif /* POST_SCRIPT_H */
