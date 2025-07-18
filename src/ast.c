#include "ast.h"
#include "util.h"
#include <string.h>
#include <stdio.h>

ASTNode *create_ast_node(NodeType type, const char *value, ASTNode *left, ASTNode *right, ASTNode *third) {
    ASTNode *node = (ASTNode *)malloc(sizeof(ASTNode));
    if (!node) {
        report_error("Memory allocation failed for AST node", -1);
        return NULL;
    }
    node->type = type;
    node->value = value ? string_duplicate(value) : NULL;
    node->left = left;
    node->right = right;
    node->third = third;
    return node;
}

void free_ast_node(ASTNode *node) {
    if (!node) return;
    free_ast_node(node->left);
    free_ast_node(node->right);
    free_ast_node(node->third);
    free(node->value);
    free(node);
}

// Helper to print indentation
static void indent_spaces(int indent) {
    for (int i = 0; i < indent; ++i)
        putchar(' ');
}

// Recursively print the AST for debugging purposes
void print_ast(ASTNode *node, int indent) {
    if (!node) return;

    switch (node->type) {
        case NODE_PROGRAM:
            print_ast(node->left, indent);
            if (node->right)
                print_ast(node->right, indent);
            break;
        case NODE_VAR_DECL:
            indent_spaces(indent);
            printf("VarDecl %s\n", node->value);
            print_ast(node->left, indent + 2);
            break;
        case NODE_PRINT_STMT:
            indent_spaces(indent);
            printf("PrintStmt\n");
            print_ast(node->left, indent + 2);
            break;
        case NODE_IF_STMT:
            indent_spaces(indent);
            printf("IfStmt\n");
            indent_spaces(indent + 2);
            printf("Condition:\n");
            print_ast(node->left, indent + 4);
            indent_spaces(indent + 2);
            printf("Then:\n");
            print_ast(node->right, indent + 4);
            if (node->third) {
                indent_spaces(indent + 2);
                printf("Else:\n");
                print_ast(node->third, indent + 4);
            }
            break;
        case NODE_WHILE_STMT:
            indent_spaces(indent);
            printf("WhileStmt\n");
            indent_spaces(indent + 2);
            printf("Condition:\n");
            print_ast(node->left, indent + 4);
            indent_spaces(indent + 2);
            printf("Body:\n");
            print_ast(node->right, indent + 4);
            break;
        case NODE_BINARY_EXPR:
            indent_spaces(indent);
            printf("BinaryExpr '%s'\n", node->value);
            print_ast(node->left, indent + 2);
            print_ast(node->right, indent + 2);
            break;
        case NODE_IDENTIFIER:
            indent_spaces(indent);
            printf("Identifier %s\n", node->value);
            break;
        case NODE_NUMBER:
            indent_spaces(indent);
            printf("Number %s\n", node->value);
            break;
        case NODE_STRING:
            indent_spaces(indent);
            printf("String \"%s\"\n", node->value);
            break;
        case NODE_BOOL:
            indent_spaces(indent);
            printf("Bool %s\n", node->value);
            break;
        default:
            indent_spaces(indent);
            printf("<unknown node>\n");
    }
}
