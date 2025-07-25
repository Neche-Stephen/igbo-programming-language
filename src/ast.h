#ifndef AST_H
#define AST_H

#include <stdlib.h>

typedef enum {
    NODE_PROGRAM,
    NODE_VAR_DECL,
    NODE_PRINT_STMT,
    NODE_IF_STMT,
    NODE_WHILE_STMT,
    NODE_BINARY_EXPR,
    NODE_IDENTIFIER,
    NODE_NUMBER,
    NODE_STRING,
    NODE_BOOL
} NodeType;

typedef struct ASTNode {
    NodeType type;
    char *value;
    struct ASTNode *left;
    struct ASTNode *right;
    struct ASTNode *third;
} ASTNode;

ASTNode *create_ast_node(NodeType type, const char *value, ASTNode *left, ASTNode *right, ASTNode *third);
void free_ast_node(ASTNode *node);

// Utility for debugging - print the AST structure in a readable form.
void print_ast(ASTNode *node, int indent);

#endif // AST_H
