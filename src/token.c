#include "token.h"
#include "util.h"
#include <string.h>

Token *create_token(TokenType type, const char *value, int line_number) {
    Token *token = (Token *)malloc(sizeof(Token));
    if (!token) {
        report_error("Memory allocation failed for token", line_number);
        return NULL;
    }
    token->type = type;
    token->value = value ? string_duplicate(value) : NULL;
    token->line_number = line_number;
    return token;
}

void free_token(Token *token) {
    if (!token) return;
    free(token->value);
    free(token);
}
