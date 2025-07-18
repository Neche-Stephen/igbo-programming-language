#ifndef TOKEN_H
#define TOKEN_H

#include <stdlib.h>

typedef enum {
    TOKEN_DEE,
    TOKEN_MA,
    TOKEN_MANA,
    TOKEN_MGBE,
    TOKEN_GOSI,
    TOKEN_EZIOKWU,
    TOKEN_UGHA,
    TOKEN_IDENTIFIER,
    TOKEN_NUMBER,
    TOKEN_STRING,
    TOKEN_ASSIGN,
    TOKEN_PLUS,
    TOKEN_MINUS,
    TOKEN_MULTIPLY,
    TOKEN_DIVIDE,
    TOKEN_EQUAL,
    TOKEN_NOT_EQUAL,
    TOKEN_LESS,
    TOKEN_GREATER,
    TOKEN_LESS_EQUAL,
    TOKEN_GREATER_EQUAL,
    TOKEN_LPAREN,
    TOKEN_RPAREN,
    TOKEN_LBRACE,
    TOKEN_RBRACE,
    TOKEN_EOF
} TokenType;

typedef struct Token {
    TokenType type;
    char *value;
    int line_number;
} Token;

Token *create_token(TokenType type, const char *value, int line_number);
void free_token(Token *token);

#endif // TOKEN_H
