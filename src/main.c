#include <stdio.h>
#include "token.h"
#include "lexer.h"
#include "ast.h"
#include "util.h"

// Simple test function demonstrating the lexer
static void test_tokenize(void) {
    const char *code = "dee aha = \"Emeka\"";
    Token *tokens = tokenize(code);
    if (!tokens) return;

    printf("Tokenizing: %s\n", code);
    for (size_t i = 0; tokens[i].type != TOKEN_EOF; ++i) {
        const char *value = tokens[i].value ? tokens[i].value : "";
        printf("%s -> %s\n", token_type_string(tokens[i].type), value);
    }
    free_tokens(tokens);
}

int main(void) {
    printf("Igbo Programming Language Interpreter\n");
    test_tokenize();
    return 0;
}
