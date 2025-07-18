#include <stdio.h>
#include "token.h"
#include "lexer.h"
#include "ast.h"
#include "util.h"
#include "parser.h"
#include "interpreter.h"

static void test_interpreter(void) {
    const char *tests[] = {
        "dee aha = \"Emeka\" gosi(aha)",
        "dee omimi = 25 gosi(omimi)",
        "dee result = 10 + 5 gosi(result)"
    };

    for (size_t t = 0; t < 3; ++t) {
        printf("\nProgram: %s\n", tests[t]);
        Token *tokens = tokenize(tests[t]);
        if (!tokens) continue;
        ASTNode *ast = parse(tokens);
        interpret(ast);
        free_ast_node(ast);
        free_tokens(tokens);
    }
}

int main(void) {
    printf("Igbo Programming Language Interpreter\n");
    test_interpreter();
    return 0;
}
