#ifndef PARSER_H
#define PARSER_H

/* Enum to specify the type of each node */
typedef enum { CONST, VAR, OPERATOR, FUNCTION } NodeType;

typedef struct Node {
    NodeType type;  /* Type of the node */
    double value;   /* For constants */
    char variable;  /* For variable (assumed to be 'x') */
    char operator;  /* For operators like +, -, *, /, ^ */
    char function[5]; /* For functions like sin, cos, etc. */
    struct Node *left;
    struct Node *right;
} Node;

/* Function declarations */
int validate_expression(const char *expr);
Node* parse_expression(const char** expr);
Node* parse_term(const char** expr);
Node* parse_factor(const char** expr);
Node* create_const_node(double value);
Node* create_var_node();
Node* create_operator_node(char operator, Node* left, Node* right);
Node* create_function_node(const char* function, Node* argument);
double evaluate(Node* root, double x);
void free_tree(Node* root);

#endif /* PARSER_H */
