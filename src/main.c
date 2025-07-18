#include <stdio.h>
#include "token.h"
#include "lexer.h"
#include "ast.h"
#include "util.h"
#include "parser.h"
#include "interpreter.h"

static char *read_file(const char *path) {
    FILE *f = fopen(path, "r");
    if (!f) return NULL;
    fseek(f, 0, SEEK_END);
    long len = ftell(f);
    fseek(f, 0, SEEK_SET);
    char *buf = malloc(len + 1);
    if (!buf) { fclose(f); return NULL; }
    fread(buf, 1, len, f);
    buf[len] = '\0';
    fclose(f);
    return buf;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s program.igbo\n", argv[0]);
        return 1;
    }
    char *source = read_file(argv[1]);
    if (!source) {
        fprintf(stderr, "Could not read file: %s\n", argv[1]);
        return 1;
    }
    Token *tokens = tokenize(source);
    free(source);
    if (!tokens) return 1;
    ASTNode *ast = parse(tokens);
    interpret(ast);
    free_ast_node(ast);
    free_tokens(tokens);
    return 0;
}
