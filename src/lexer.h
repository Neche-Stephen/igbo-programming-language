#ifndef LEXER_H
#define LEXER_H

#include "token.h"

// Tokenize the given source code and return a dynamically allocated
// array of tokens terminated by a TOKEN_EOF entry.
// The caller is responsible for freeing the returned array using
// free_tokens().
Token *tokenize(const char *source_code);

// Utility to free an array of tokens returned by tokenize().
void free_tokens(Token *tokens);

// String representation of a token type for debugging.
const char *token_type_string(TokenType type);

#endif // LEXER_H
