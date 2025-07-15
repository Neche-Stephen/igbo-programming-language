#include "ast.h"
#include "util.h"
#include <string.h>

ASTNode *create_ast_node(NodeType type, const char *value, ASTNode *left, ASTNode *right) {
    ASTNode *node = (ASTNode *)malloc(sizeof(ASTNode));
    if (!node) {
        report_error("Memory allocation failed for AST node", -1);
        return NULL;
    }
    node->type = type;
    node->value = value ? string_duplicate(value) : NULL;
    node->left = left;
    node->right = right;
    return node;
}

void free_ast_node(ASTNode *node) {
    if (!node) return;
    free_ast_node(node->left);
    free_ast_node(node->right);
    free(node->value);
    free(node);
}
