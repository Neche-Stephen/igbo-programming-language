#ifndef AST_H
#define AST_H

#include <stdlib.h>

typedef enum {
    NODE_PROGRAM,
    NODE_VAR_DECL,
    NODE_PRINT_STMT,
    NODE_BINARY_EXPR,
    NODE_IDENTIFIER,
    NODE_NUMBER,
    NODE_STRING
} NodeType;

typedef struct ASTNode {
    NodeType type;
    char *value;
    struct ASTNode *left;
    struct ASTNode *right;
} ASTNode;

ASTNode *create_ast_node(NodeType type, const char *value, ASTNode *left, ASTNode *right);
void free_ast_node(ASTNode *node);

#endif // AST_H
