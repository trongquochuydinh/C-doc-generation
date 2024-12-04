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

int validate_expression(const char *expr) {
    int variable_found = FALSE;
    int paren_count = 0;

    while (*expr) {
        if (isdigit(*expr) || *expr == '.' || *expr == ' ' || *expr == 'x' || 
            *expr == '+' || *expr == '-' || *expr == '*' || *expr == '/' || *expr == '^' || *expr == '|') {
            if (*expr == 'x') {
                variable_found = TRUE;
            }
        }
        else if (isalpha(*expr)) {
            /* Recognize standard functions */
            if (strncmp(expr, "sin", 3) == 0 || strncmp(expr, "cos", 3) == 0 ||
                strncmp(expr, "tan", 3) == 0 || strncmp(expr, "ln", 2) == 0 ||
                strncmp(expr, "log", 3) == 0 || strncmp(expr, "exp", 3) == 0 ||
                strncmp(expr, "asin", 4) == 0 || strncmp(expr, "acos", 4) == 0 ||
                strncmp(expr, "atan", 4) == 0 || strncmp(expr, "sinh", 4) == 0 ||
                strncmp(expr, "cosh", 4) == 0 || strncmp(expr, "tanh", 4) == 0) {

                /* Move past the function name */
                int func_len = (strncmp(expr, "ln", 2) == 0) ? 2 : (strncmp(expr, "asin", 4) == 0 || 
                              strncmp(expr, "acos", 4) == 0 || strncmp(expr, "atan", 4) == 0 || 
                              strncmp(expr, "sinh", 4) == 0 || strncmp(expr, "cosh", 4) == 0 || 
                              strncmp(expr, "tanh", 4) == 0) ? 4 : 3;
                expr += func_len;

                /* Check for an opening parenthesis immediately after the function */
                if (*expr != '(') {
                    fprintf(stderr, "Error: Function \"%.*s\" must be followed by '('.\n", func_len, expr - func_len);
                    return FALSE;
                }

                /* Count the opening parenthesis for this function */
                paren_count++;
                expr++;

                /* Parse the function argument and check for 'x' */
                while (*expr && *expr != ')') {
                    if (*expr == '(') {
                        paren_count++;
                    } else if (*expr == ')') {
                        paren_count--;
                    } else if (*expr == 'x') {
                        variable_found = TRUE;
                    }
                    expr++;
                }

                /* If we reached the end without a closing parenthesis, report an error */
                if (*expr != ')') {
                    fprintf(stderr, "Error: Unmatched opening parenthesis in function \"%.*s\".\n", func_len, expr - func_len);
                    return FALSE;
                }

                paren_count--;
            } else {
                const char *start = expr;
                while (isalpha(*expr)) {
                    expr++;
                }
                fprintf(stderr, "Error: Invalid function in expression: \"%.*s\" is not recognized.\n", (int)(expr - start), start);
                return FALSE;
            }
        } else if (*expr == '(') {
            paren_count++;
        } else if (*expr == ')') {
            paren_count--;
            if (paren_count < 0) {
                fprintf(stderr, "Error: Unmatched closing parenthesis.\n");
                return FALSE;
            }
        } else {
            fprintf(stderr, "Error: Invalid character '%c' in expression.\n", *expr);
            return FALSE;
        }
        expr++;
    }

    if (paren_count != 0) {
        fprintf(stderr, "Error: Unmatched opening parenthesis.\n");
        return FALSE;
    }

    if (!variable_found) {
        fprintf(stderr, "Error: No variable 'x' found in the expression.\n");
        return FALSE;
    }

    return TRUE;
}

void skip_whitespace(const char** expr) {
    while (isspace(**expr)) {
        (*expr)++;
    }
}

Node* parse_number(const char** expr) {
    skip_whitespace(expr);

    /* Check if number is hexadecimal (starts with 0x or 0X) */
    if (**expr == '0' && ((*expr)[1] == 'x' || (*expr)[1] == 'X')) {
        char* end;
        long int value = strtol(*expr, &end, 16);
        *expr = end;
        return create_const_node((double)value);
    }
    
    /* Check if number is octal (starts with 0 followed by a digit, e.g., 010) */
    if (**expr == '0' && isdigit((*expr)[1])) {
        char* end;
        long int value = strtol(*expr, &end, 8);
        *expr = end;
        return create_const_node((double)value);
    }
    
    /* Parse as decimal integer or floating-point number (including scientific notation) */
    char* end;
    double value = strtod(*expr, &end);
    *expr = end;
    return create_const_node(value);
}


Node* parse_function(const char** expr) {
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

Node* parse_factor(const char** expr) {
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

Node* parse_term(const char** expr) {
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

Node* parse_expression(const char** expr) {
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
