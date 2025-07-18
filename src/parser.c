#include "parser.h"
#include "util.h"
#include <stdio.h>
#include <string.h>

// Simple recursive descent parser implementation following the grammar
// specified in the project instructions.

typedef struct {
    Token *tokens;   // array of tokens terminated by TOKEN_EOF
    size_t current;  // current token index
} Parser;

static Token *peek(Parser *p) { return &p->tokens[p->current]; }
static Token *previous(Parser *p) { return &p->tokens[p->current - 1]; }
static int is_at_end(Parser *p) { return peek(p)->type == TOKEN_EOF; }

static Token *advance(Parser *p) {
    if (!is_at_end(p)) p->current++;
    return previous(p);
}

static int check(Parser *p, TokenType type) {
    return peek(p)->type == type;
}

static int match(Parser *p, TokenType type) {
    if (check(p, type)) {
        advance(p);
        return 1;
    }
    return 0;
}

static void parser_error(Parser *p, const char *message) {
    report_error(message, peek(p)->line_number);
}

// Forward declarations
static ASTNode *statement(Parser *p);
static ASTNode *block(Parser *p);
static ASTNode *expression(Parser *p);
static ASTNode *equality(Parser *p);
static ASTNode *comparison(Parser *p);
static ASTNode *term(Parser *p);
static ASTNode *factor(Parser *p);
static ASTNode *unary(Parser *p);
static ASTNode *primary(Parser *p);

// program -> statement*
static ASTNode *program(Parser *p) {
    ASTNode *head = NULL;
    ASTNode *tail = NULL;
    while (!is_at_end(p)) {
        ASTNode *stmt = statement(p);
        if (!stmt) return head; // error already reported
        ASTNode *node = create_ast_node(NODE_PROGRAM, NULL, stmt, NULL, NULL);
        if (!head)
            head = node;
        else
            tail->right = node;
        tail = node;
    }
    return head;
}

// block -> "{" statement* "}"
static ASTNode *block(Parser *p) {
    if (!match(p, TOKEN_LBRACE)) {
        parser_error(p, "Expected '{' to start block");
        return NULL;
    }
    ASTNode *head = NULL;
    ASTNode *tail = NULL;
    while (!check(p, TOKEN_RBRACE) && !is_at_end(p)) {
        ASTNode *stmt = statement(p);
        if (!stmt) return head;
        ASTNode *node = create_ast_node(NODE_PROGRAM, NULL, stmt, NULL, NULL);
        if (!head)
            head = node;
        else
            tail->right = node;
        tail = node;
    }
    if (!match(p, TOKEN_RBRACE)) {
        parser_error(p, "Expected '}' after block");
    }
    return head;
}

// statement -> varDecl | printStmt | exprStmt
static ASTNode *statement(Parser *p) {
    if (match(p, TOKEN_DEE)) {
        // "dee" already consumed
        if (!check(p, TOKEN_IDENTIFIER)) {
            parser_error(p, "Expected identifier after 'dee'");
            return NULL;
        }
        Token *name = advance(p);
        if (!match(p, TOKEN_ASSIGN)) {
            parser_error(p, "Expected '=' after variable name");
            return NULL;
        }
        ASTNode *value = expression(p);
        if (!value) return NULL;
        return create_ast_node(NODE_VAR_DECL, name->value, value, NULL, NULL);
    }
    if (match(p, TOKEN_MA)) {
        ASTNode *cond = expression(p);
        ASTNode *thenBranch = block(p);
        ASTNode *elseBranch = NULL;
        if (match(p, TOKEN_MANA)) {
            elseBranch = block(p);
        }
        return create_ast_node(NODE_IF_STMT, NULL, cond, thenBranch, elseBranch);
    }
    if (match(p, TOKEN_MGBE)) {
        ASTNode *cond = expression(p);
        ASTNode *body = block(p);
        return create_ast_node(NODE_WHILE_STMT, NULL, cond, body, NULL);
    }
    if (match(p, TOKEN_GOSI)) {
        if (!match(p, TOKEN_LPAREN)) {
            parser_error(p, "Expected '(' after 'gosi'");
            return NULL;
        }
        ASTNode *expr = expression(p);
        if (!expr) return NULL;
        if (!match(p, TOKEN_RPAREN)) {
            parser_error(p, "Expected ')' after expression");
            return NULL;
        }
        return create_ast_node(NODE_PRINT_STMT, NULL, expr, NULL, NULL);
    }
    // exprStmt: just an expression on its own
    return expression(p);
}

// expression -> equality
static ASTNode *expression(Parser *p) {
    return equality(p);
}

// equality -> comparison ( ("==" | "!=") comparison )*
static ASTNode *equality(Parser *p) {
    ASTNode *node = comparison(p);
    while (match(p, TOKEN_EQUAL) || match(p, TOKEN_NOT_EQUAL)) {
        Token *op = previous(p);
        ASTNode *right = comparison(p);
        node = create_ast_node(NODE_BINARY_EXPR, op->value, node, right, NULL);
    }
    return node;
}

// comparison -> term ( (">" | "<" | ">=" | "<=") term )*
static ASTNode *comparison(Parser *p) {
    ASTNode *node = term(p);
    while (match(p, TOKEN_GREATER) || match(p, TOKEN_LESS) ||
           match(p, TOKEN_GREATER_EQUAL) || match(p, TOKEN_LESS_EQUAL)) {
        Token *op = previous(p);
        ASTNode *right = term(p);
        node = create_ast_node(NODE_BINARY_EXPR, op->value, node, right, NULL);
    }
    return node;
}

// term -> factor ( ("+" | "-") factor )*
static ASTNode *term(Parser *p) {
    ASTNode *node = factor(p);
    while (match(p, TOKEN_PLUS) || match(p, TOKEN_MINUS)) {
        Token *op = previous(p);
        ASTNode *right = factor(p);
        node = create_ast_node(NODE_BINARY_EXPR, op->value, node, right, NULL);
    }
    return node;
}

// factor -> unary ( ("*" | "/") unary )*
static ASTNode *factor(Parser *p) {
    ASTNode *node = unary(p);
    while (match(p, TOKEN_MULTIPLY) || match(p, TOKEN_DIVIDE)) {
        Token *op = previous(p);
        ASTNode *right = unary(p);
        node = create_ast_node(NODE_BINARY_EXPR, op->value, node, right, NULL);
    }
    return node;
}

// unary -> primary
static ASTNode *unary(Parser *p) {
    return primary(p);
}

// primary -> NUMBER | STRING | IDENTIFIER | "(" expression ")"
static ASTNode *primary(Parser *p) {
    if (match(p, TOKEN_NUMBER)) {
        Token *num = previous(p);
        return create_ast_node(NODE_NUMBER, num->value, NULL, NULL, NULL);
    }
    if (match(p, TOKEN_STRING)) {
        Token *str = previous(p);
        return create_ast_node(NODE_STRING, str->value, NULL, NULL, NULL);
    }
    if (match(p, TOKEN_IDENTIFIER)) {
        Token *id = previous(p);
        return create_ast_node(NODE_IDENTIFIER, id->value, NULL, NULL, NULL);
    }
    if (match(p, TOKEN_EZIOKWU)) {
        return create_ast_node(NODE_BOOL, "eziokwu", NULL, NULL, NULL);
    }
    if (match(p, TOKEN_UGHA)) {
        return create_ast_node(NODE_BOOL, "ụgha", NULL, NULL, NULL);
    }
    if (match(p, TOKEN_LPAREN)) {
        ASTNode *expr = expression(p);
        if (!expr) return NULL;
        if (!match(p, TOKEN_RPAREN)) {
            parser_error(p, "Expected ')' after expression");
            return NULL;
        }
        return expr;
    }
    parser_error(p, "Unexpected token");
    return NULL;
}

// Entry point exposed to other modules
ASTNode *parse(Token *tokens) {
    Parser p = { tokens, 0 };
    return program(&p);
}

