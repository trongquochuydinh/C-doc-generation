#ifndef PARSER_H
#define PARSER_H

/** 
 * @brief Enum to specify the type of each node in the expression tree.
 */
typedef enum { CONST, VAR, OPERATOR, FUNCTION } NodeType;

/** 
 * @brief Structure to represent a node in the expression tree.
 */
typedef struct Node {
    NodeType type;      /**< Type of the node (CONST, VAR, OPERATOR, FUNCTION) */
    double value;       /**< Constant value, if the node is of type CONST */
    char variable;      /**< Variable character (assumed to be 'x') */
    char operator;      /**< Operator character (+, -, *, /, ^) */
    char function[5];   /**< Function name (e.g., "sin", "cos") */
    struct Node *left;  /**< Pointer to the left child node */
    struct Node *right; /**< Pointer to the right child node */
} Node;

/* Function declarations */

/**
 * @brief Validates the mathematical expression for correctness.
 * 
 * @param[in] expr The input mathematical expression as a string.
 * @return int Returns TRUE if the expression is valid, otherwise FALSE.
 */
int validate_expression(const char *expr);

/**
 * @brief Checks whether a character is valid in the mathematical expression.
 * 
 * @param[in] c The character to validate.
 * @return int Returns TRUE if the character is valid, otherwise FALSE.
 */
int is_valid_character(char c);

/**
 * @brief Handles parsing and validation of functions like sin, cos, etc.
 * 
 * @param[in,out] expr Pointer to the current position in the expression string.
 * @param[in,out] paren_count Pointer to track the balance of parentheses.
 * @param[in,out] variable_found Pointer to track if the variable 'x' is found.
 * @return int Returns TRUE if the function is valid, otherwise FALSE.
 */
int handle_function(const char **expr, int *paren_count, int *variable_found);

/**
 * @brief Handles parentheses validation and balance tracking.
 * 
 * @param[in] c The current character ('(' or ')') being processed.
 * @param[in,out] paren_count Pointer to track the balance of parentheses.
 * @return int Returns TRUE if parentheses are balanced, otherwise FALSE.
 */
int handle_parentheses(char c, int *paren_count);

/**
 * @brief Reports an invalid character in the mathematical expression.
 * 
 * @param[in] c The invalid character found in the expression.
 */
void report_invalid_character(char c);

/**
 * @brief Reports unmatched opening parentheses in the expression.
 */
void report_unmatched_parenthesis(void);

/**
 * @brief Parses a mathematical expression and constructs its expression tree.
 * 
 * @param[in,out] expr Pointer to the current position in the expression string.
 * @return Node* Returns a pointer to the root of the expression tree.
 */
Node* parse_expression(const char** expr);

/**
 * @brief Parses a term from the expression, handling multiplication, division, and exponentiation.
 * 
 * @param[in,out] expr Pointer to the current position in the expression string.
 * @return Node* Returns a pointer to the parsed term node.
 */
Node* parse_term(const char** expr);

/**
 * @brief Parses a factor, which could be a number, variable, function, or subexpression.
 * 
 * @param[in,out] expr Pointer to the current position in the expression string.
 * @return Node* Returns a pointer to the parsed factor node.
 */
Node* parse_factor(const char** expr);

/**
 * @brief Creates a node representing a constant value.
 * 
 * @param[in] value The constant value to store in the node.
 * @return Node* Returns a pointer to the newly created constant node.
 */
Node* create_const_node(double value);

/**
 * @brief Creates a node representing the variable 'x'.
 * 
 * @return Node* Returns a pointer to the newly created variable node.
 */
Node* create_var_node(void);

/**
 * @brief Creates a node representing an operator.
 * 
 * @param[in] operator The operator character (+, -, *, /, ^).
 * @param[in] left Pointer to the left child node.
 * @param[in] right Pointer to the right child node.
 * @return Node* Returns a pointer to the newly created operator node.
 */
Node* create_operator_node(char operator, Node* left, Node* right);

/**
 * @brief Creates a node representing a mathematical function.
 * 
 * @param[in] function The function name (e.g., "sin", "cos").
 * @param[in] argument Pointer to the argument node for the function.
 * @return Node* Returns a pointer to the newly created function node.
 */
Node* create_function_node(const char* function, Node* argument);

/**
 * @brief Evaluates the expression tree for a given value of 'x'.
 * 
 * @param[in] root Pointer to the root of the expression tree.
 * @param[in] x The value of the variable 'x'.
 * @return double Returns the result of evaluating the expression tree.
 */
double evaluate(Node* root, double x);

/**
 * @brief Frees the memory allocated for the expression tree.
 * 
 * @param[in] root Pointer to the root of the expression tree.
 */
void free_tree(Node* root);

#endif /* PARSER_H */
