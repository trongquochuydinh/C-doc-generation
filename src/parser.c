#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "parser.h"
#include "utils.h"

#define MAX_VALUE 1e6
#define TRUE 1
#define FALSE 0

/* 
 * Validates the mathematical expression, ensuring proper syntax, balanced 
 * parentheses, and the presence of the variable 'x'.
 */
int validate_expression(const char *expr) {
    int variable_found = FALSE;
    int paren_count = 0;

    while (*expr) {
        if (is_valid_character(*expr)) {
            if (*expr == 'x') {
                variable_found = TRUE;
            }
        } else if (isalpha(*expr)) {
            if (!handle_function(&expr, &paren_count, &variable_found)) {
                return FALSE;
            }
        } else if (*expr == '(' || *expr == ')') {
            if (!handle_parentheses(*expr, &paren_count)) {
                return FALSE;
            }
        } else {
            report_invalid_character(*expr);
            return FALSE;
        }
        expr++;
    }

    if (paren_count != 0) {
        report_unmatched_parenthesis();
        return FALSE;
    }

    if (!variable_found) {
        fprintf(stderr, "Error: No variable 'x' found in the expression.\n");
        return FALSE;
    }

    return TRUE;
}

/* 
 * Checks whether the given character is valid in a mathematical expression.
 */
int is_valid_character(char c) {
    return isdigit(c) || c == '.' || c == ' ' || c == 'x' || 
           c == '+' || c == '-' || c == '*' || c == '/' || c == '^' || c == '|';
}

/* 
 * Parses and validates a mathematical function (e.g., sin, cos, log) in the expression.
 * Ensures the function is followed by parentheses and checks for nested arguments.
 */
int handle_function(const char **expr, int *paren_count, int *variable_found) {
    const char *start = *expr;
    const char *functions[] = {"sin", "cos", "tan", "ln", "log", "exp", 
                               "asin", "acos", "atan", "sinh", "cosh", "tanh", "abs"};
    int func_len = 0;

    /* Determine the length of the function name */
    while (isalpha(**expr)) {
        (*expr)++;
    }
    func_len = *expr - start;

    /* Validate function name */
    int is_valid = FALSE;
    for (size_t i = 0; i < sizeof(functions) / sizeof(functions[0]); i++) {
        if (strncmp(start, functions[i], func_len) == 0 && functions[i][func_len] == '\0') {
            is_valid = TRUE;
            break;
        }
    }

    if (!is_valid) {
        fprintf(stderr, "Error: Invalid function in expression: \"%.*s\".\n", func_len, start);
        return FALSE;
    }

    /* Ensure function is followed by '(' */
    if (**expr != '(') {
        fprintf(stderr, "Error: Function \"%.*s\" must be followed by '('.\n", func_len, start);
        return FALSE;
    }

    (*expr)++;
    (*paren_count)++;

    /* Parse function arguments */
    while (**expr && **expr != ')') {
        if (**expr == '(') {
            (*paren_count)++;
        } else if (**expr == ')') {
            (*paren_count)--;
        } else if (**expr == 'x') {
            *variable_found = TRUE;
        }
        (*expr)++;
    }

    if (**expr != ')') {
        fprintf(stderr, "Error: Unmatched opening parenthesis in function \"%.*s\".\n", func_len, start);
        return FALSE;
    }

    (*paren_count)--; 
    return TRUE;
}

/* 
 * Updates the parenthesis count and checks for unmatched parentheses.
 */
int handle_parentheses(char c, int *paren_count) {
    if (c == '(') {
        (*paren_count)++;
    } else if (c == ')') {
        (*paren_count)--;
        if (*paren_count < 0) {
            fprintf(stderr, "Error: Unmatched closing parenthesis.\n");
            return FALSE;
        }
    }
    return TRUE;
}

/* 
 * Reports an error for an invalid character encountered in the expression.
 */
void report_invalid_character(char c) {
    fprintf(stderr, "Error: Invalid character '%c' in expression.\n", c);
}

/* 
 * Reports an error for unmatched opening parentheses.
 */
void report_unmatched_parenthesis(void) {
    fprintf(stderr, "Error: Unmatched opening parenthesis.\n");
}

/* 
 * Skips whitespace characters in the expression string.
 */
void skip_whitespace(const char **expr) {
    while (isspace(**expr)) {
        (*expr)++;
    }
}

/* 
 * Parses a numeric value (hexadecimal, octal, or decimal) from the expression.
 */
Node* parse_number(const char **expr) {
    skip_whitespace(expr);

    if (**expr == '0' && ((*expr)[1] == 'x' || (*expr)[1] == 'X')) {
        char* end;
        long int value = strtol(*expr, &end, 16);
        *expr = end;
        return create_const_node((double)value);
    }

    if (**expr == '0' && isdigit((*expr)[1])) {
        char* end;
        long int value = strtol(*expr, &end, 8);
        *expr = end;
        return create_const_node((double)value);
    }

    char* end;
    double value = strtod(*expr, &end);
    *expr = end;
    return create_const_node(value);
}

/* 
 * Parses a mathematical function and its arguments from the expression.
 */
Node* parse_function(const char **expr) {
    skip_whitespace(expr);
    char func[5] = {0};
    int i = 0;

    while (isalpha(**expr) && i < 4) {
        func[i++] = *(*expr)++;
    }
    func[i] = '\0';

    if (**expr == '(') {
        (*expr)++;
        Node* argument = parse_expression(expr);
        if (**expr == ')') (*expr)++;
        return create_function_node(func, argument);
    }
    return NULL;
}

/* 
 * Parses a factor: a number, variable, function, or expression in parentheses.
 */
Node* parse_factor(const char **expr) {
    skip_whitespace(expr);

    if (**expr == '-') {
        (*expr)++;
        Node* node = parse_factor(expr);
        return create_operator_node('*', create_const_node(-1), node);
    }

    if (**expr == '|') {
        (*expr)++;
        Node* node = parse_expression(expr);
        if (**expr == '|') (*expr)++;
        return create_function_node("abs", node);
    }

    if (isdigit(**expr) || **expr == '.') {
        return parse_number(expr);
    } else if (**expr == 'x') {
        (*expr)++;
        return create_var_node();
    } else if (isalpha(**expr)) {
        return parse_function(expr);
    } else if (**expr == '(') {
        (*expr)++;
        Node* node = parse_expression(expr);
        if (**expr == ')') (*expr)++;
        return node;
    }
    return NULL;
}

/* 
 * Parses terms involving multiplication, division, or exponentiation.
 */
Node* parse_term(const char **expr) {
    skip_whitespace(expr);
    Node* node = parse_factor(expr);

    while (**expr == '^' || **expr == '*' || **expr == '/') {
        char operator = *(*expr)++;
        skip_whitespace(expr);
        Node* right = parse_factor(expr);
        node = create_operator_node(operator, node, right);
    }
    return node;
}

/* 
 * Parses the full expression, handling addition and subtraction.
 */
Node* parse_expression(const char **expr) {
    skip_whitespace(expr);
    Node* node = parse_term(expr);

    while (**expr == '+' || **expr == '-') {
        char operator = *(*expr)++;
        skip_whitespace(expr);
        Node* right = parse_term(expr);
        node = create_operator_node(operator, node, right);
    }
    return node;
}

/* Create a node for a constant value */
Node* create_const_node(double value) {
    Node* node = (Node*)malloc(sizeof(Node));
    node->type = CONST;
    node->value = value;
    node->left = node->right = NULL;
    return node;
}

/* Create a node for a variable */
Node* create_var_node() {
    Node* node = (Node*)malloc(sizeof(Node));
    node->type = VAR;
    node->variable = 'x';
    node->left = node->right = NULL;
    return node;
}

/* Create a node for an operator */
Node* create_operator_node(char operator, Node* left, Node* right) {
    Node* node = (Node*)malloc(sizeof(Node));
    node->type = OPERATOR;
    node->operator = operator;
    node->left = left;
    node->right = right;
    return node;
}

/* Create a node for a function */
Node* create_function_node(const char* function, Node* argument) {
    Node* node = (Node*)malloc(sizeof(Node));
    node->type = FUNCTION;
    strncpy(node->function, function, 4);
    node->function[4] = '\0';
    node->left = argument;
    node->right = NULL;
    return node;
}

double evaluate(Node* root, double x) {
    if (!root) return create_nan();

    switch (root->type) {
        case CONST:
            return root->value;
        case VAR:
            return x;
        case OPERATOR: {
            double left_val = evaluate(root->left, x);
            double right_val = evaluate(root->right, x);

            if (!isfinite(left_val) || !isfinite(right_val)) return create_nan();

            switch (root->operator) {
                case '+': return left_val + right_val;
                case '-': return left_val - right_val;
                case '*': return left_val * right_val;
                case '/': 
                    if (fabs(right_val) < 1e-10) return create_nan();
                    return left_val / right_val;
                case '^': 
                    if (left_val < 0 && floor(right_val) != right_val) return create_nan();
                    double result = pow(left_val, right_val);
                    return isfinite(result) && fabs(result) < MAX_VALUE ? result : create_nan();
            }
            break;
        }
        case FUNCTION: {
            double arg_val = evaluate(root->left, x);
            if (!isfinite(arg_val)) return create_nan();

            /* Check longer function names first */
            if (strcmp(root->function, "sinh") == 0) return sinh(arg_val);
            if (strcmp(root->function, "cosh") == 0) return cosh(arg_val);
            if (strcmp(root->function, "tanh") == 0) return tanh(arg_val);
            if (strcmp(root->function, "asin") == 0) return asin(arg_val);
            if (strcmp(root->function, "acos") == 0) return acos(arg_val);
            if (strcmp(root->function, "atan") == 0) return atan(arg_val);
            if (strcmp(root->function, "sin") == 0) return sin(arg_val);
            if (strcmp(root->function, "cos") == 0) return cos(arg_val);
            if (strcmp(root->function, "tan") == 0) return tan(arg_val);
            if (strcmp(root->function, "log") == 0) return arg_val > 0 ? log10(arg_val) : create_nan();
            if (strcmp(root->function, "exp") == 0) return exp(arg_val);
            if (strcmp(root->function, "ln") == 0) return arg_val > 0 ? log(arg_val) : create_nan();
            if (strcmp(root->function, "abs") == 0) return fabs(arg_val);
        }
    }
    return create_nan();
}

/* Free the nodes in the expression tree */
void free_tree(Node* root) {
    if (!root) return;
    free_tree(root->left);
    free_tree(root->right);
    free(root);
}
