#ifndef PARSER_H
#define PARSER_H

#include "token.h"
#include "ast.h"

// Parse the given array of tokens and return the root of the AST.
// The returned tree should be freed with free_ast_node().
ASTNode *parse(Token *tokens);

#endif // PARSER_H
