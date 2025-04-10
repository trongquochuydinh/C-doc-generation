#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils.h"
#include "parser.h"
#include "post_script.h"

#define MAX_EXPR_LENGTH 256

/**
 * @brief Parses the command line arguments and validates the input function.
 *
 * This function checks the provided command line parameters, validates the 
 * mathematical function, sets default or user-provided domain/range, and 
 * prepares the output file.
 *
 * @param[in] argc Number of arguments passed from the command line.
 * @param[in] argv Array of strings containing command-line arguments.
 * @param[out] func Pointer to a string holding the cleaned and validated 
 *                  mathematical function.
 * @param[out] outfile Pointer to a string holding the output file name.
 * @param[out] x_min Pointer to the lower bound of the x-axis domain.
 * @param[out] x_max Pointer to the upper bound of the x-axis domain.
 * @param[out] y_min Pointer to the lower bound of the y-axis range.
 * @param[out] y_max Pointer to the upper bound of the y-axis range.
 * @param[out] calc_x_range Flag indicating if the x range was set by the user.
 * @param[out] calc_y_range Flag indicating if the y range was set by the user.
 *
 * @return int Returns 0 on success, or an error code:
 * - 1: Insufficient arguments.
 * - 2: Invalid mathematical function.
 * - 3: Unable to create/write to the output file.
 * - 4: Invalid format for range specification.
 */
int parse_args(int argc, char *argv[], char **func, char **outfile,
               double *x_min, double *x_max, double *y_min, double *y_max,
               int *calc_x_range, int *calc_y_range) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <function> <output file> [x_min:x_max:y_min:y_max]\n", 
                argv[0]);
        return 1;
    }

    *func = NULL;

    /* Allocate memory for the cleaned function */
    char *cleaned_func = (char *)malloc(MAX_EXPR_LENGTH * sizeof(char));
    if (cleaned_func == NULL) {
        fprintf(stderr, "Error: Memory allocation failed.\n");
        return 1;
    }

    /* Remove whitespace and validate the function */
    remove_whitespace(cleaned_func, argv[1]);
    *func = cleaned_func;

    if (!validate_expression(*func)) {
        fprintf(stderr, "Error: Invalid function provided.\n");
        free(cleaned_func);
        *func = NULL;
        return 2;
    }

    *outfile = argv[2];
    FILE *test_file = fopen(*outfile, "w");
    if (test_file == NULL) {
        fprintf(stderr, "Error: Cannot create/write to file '%s'.\n", *outfile);
        free(cleaned_func);
        *func = NULL;
        return 3;
    }
    fclose(test_file);

    /* Set default ranges */
    *calc_x_range = 1;
    *calc_y_range = 1;
    *x_min = -10.0;
    *x_max = 10.0;
    *y_min = -10.0;
    *y_max = 10.0;

    /* Optional: Parse user-provided range */
    if (argc >= 4) {
        double temp_x_min, temp_x_max, temp_y_min, temp_y_max;
        int matched_values = sscanf(argv[3], "%lf:%lf:%lf:%lf", 
                                    &temp_x_min, &temp_x_max, &temp_y_min, &temp_y_max);
        if (matched_values == 4) {
            *x_min = temp_x_min;
            *x_max = temp_x_max;
            *y_min = temp_y_min;
            *y_max = temp_y_max;
            *calc_x_range = 0;
            *calc_y_range = 0;
        } else {
            fprintf(stderr, "Error: Invalid format for range. Expected x_min:x_max:y_min:y_max\n");
            free(cleaned_func);
            *func = NULL;
            return 4;
        }
    }

    return 0;
}

/**
 * @brief Entry point of the program.
 *
 * Parses arguments, validates the input function, and generates a PostScript
 * file representing the mathematical graph of the function.
 *
 * @param[in] argc Number of arguments passed from the command line.
 * @param[in] argv Array of strings containing command-line arguments.
 *
 * @return int Returns EXIT_SUCCESS (0) on success, or an error code on failure.
 */
int main(int argc, char *argv[]) {
    char *func = NULL, *outfile;
    double x_min, x_max, y_min, y_max;
    int calc_x_range, calc_y_range;
    int parse_args_status;

    /* Parse command-line arguments */
    parse_args_status = parse_args(argc, argv, &func, &outfile, 
                                   &x_min, &x_max, &y_min, &y_max, 
                                   &calc_x_range, &calc_y_range);
    if (parse_args_status != 0) {
        if (func) {
            free(func);
        }
        return parse_args_status;
    }

    /* Generate PostScript file for the mathematical function */
    generate_postscript(outfile, func, x_min, x_max, y_min, y_max, 
                        calc_x_range, calc_y_range);

    /* Free dynamically allocated memory */
    if (func) {
        free(func);
    }

    return EXIT_SUCCESS;
}
