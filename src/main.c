#include <stdio.h>
#include "token.h"
#include "lexer.h"
#include "ast.h"
#include "util.h"
#include "parser.h"

// Simple test function demonstrating the parser
static void test_parser(void) {
    const char *tests[] = {
        "dee aha = \"Emeka\"",
        "gosi(aha)",
        "dee omimi = 20 + 5"
    };

    for (size_t t = 0; t < 3; ++t) {
        printf("\nParsing: %s\n", tests[t]);
        Token *tokens = tokenize(tests[t]);
        if (!tokens) continue;
        ASTNode *ast = parse(tokens);
        print_ast(ast, 0);
        free_ast_node(ast);
        free_tokens(tokens);
    }
}

int main(void) {
    printf("Igbo Programming Language Interpreter\n");
    test_parser();
    return 0;
}
