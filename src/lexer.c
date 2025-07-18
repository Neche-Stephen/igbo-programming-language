#include "lexer.h"
#include "util.h"
#include <ctype.h>
#include <stdio.h>
#include <string.h>

// Internal helper to grow the token array when needed
static void ensure_capacity(Token **tokens, size_t *capacity, size_t count) {
    if (count + 1 >= *capacity) {
        *capacity *= 2;
        Token *tmp = realloc(*tokens, sizeof(Token) * (*capacity));
        if (!tmp) {
            report_error("Memory allocation failed while tokenizing", -1);
            return;
        }
        *tokens = tmp;
    }
}

// Convert a token type to a human readable string
const char *token_type_string(TokenType type) {
    switch (type) {
        case TOKEN_DEE: return "DEE";
        case TOKEN_MA: return "MA";
        case TOKEN_MANA: return "MANA";
        case TOKEN_MGBE: return "MGBE";
        case TOKEN_GOSI: return "GOSI";
        case TOKEN_EZIOKWU: return "EZIOKWU";
        case TOKEN_UGHA: return "UGHA";
        case TOKEN_IDENTIFIER: return "IDENTIFIER";
        case TOKEN_NUMBER: return "NUMBER";
        case TOKEN_STRING: return "STRING";
        case TOKEN_ASSIGN: return "ASSIGN";
        case TOKEN_PLUS: return "PLUS";
        case TOKEN_MINUS: return "MINUS";
        case TOKEN_MULTIPLY: return "MULTIPLY";
        case TOKEN_DIVIDE: return "DIVIDE";
        case TOKEN_EQUAL: return "EQUAL";
        case TOKEN_NOT_EQUAL: return "NOT_EQUAL";
        case TOKEN_LESS: return "LESS";
        case TOKEN_GREATER: return "GREATER";
        case TOKEN_LESS_EQUAL: return "LESS_EQUAL";
        case TOKEN_GREATER_EQUAL: return "GREATER_EQUAL";
        case TOKEN_LPAREN: return "LPAREN";
        case TOKEN_RPAREN: return "RPAREN";
        case TOKEN_LBRACE: return "LBRACE";
        case TOKEN_RBRACE: return "RBRACE";
        case TOKEN_EOF: return "EOF";
        default: return "UNKNOWN";
    }
}

// Free an array of tokens produced by tokenize()
void free_tokens(Token *tokens) {
    if (!tokens) return;
    for (size_t i = 0; tokens[i].type != TOKEN_EOF; ++i) {
        free(tokens[i].value);
    }
    free(tokens);
}

// Main lexer implementation
Token *tokenize(const char *source) {
    size_t capacity = 64;
    size_t count = 0;
    Token *tokens = malloc(sizeof(Token) * capacity);
    if (!tokens) {
        report_error("Memory allocation failed for tokens", -1);
        return NULL;
    }

    size_t i = 0;
    int line = 1;

    while (source[i]) {
        char c = source[i];

        // Skip whitespace
        if (c == ' ' || c == '\t' || c == '\r') {
            i++;
            continue;
        }

        if (c == '\n') {
            line++;
            i++;
            continue;
        }

        // Skip comments
        if (c == '/' && source[i + 1] == '/') {
            i += 2;
            while (source[i] && source[i] != '\n')
                i++;
            continue;
        }

        // Identifiers and keywords
        if (isalpha((unsigned char)c) || c == '_' || (c & 0x80)) {
            size_t start = i;
            while (isalnum((unsigned char)source[i]) || source[i] == '_' || (source[i] & 0x80))
                i++;
            size_t len = i - start;
            char *text = malloc(len + 1);
            if (!text) {
                report_error("Memory allocation failed", line);
                break;
            }
            memcpy(text, &source[start], len);
            text[len] = '\0';

            TokenType type = TOKEN_IDENTIFIER;
            if (strcmp(text, "dee") == 0)
                type = TOKEN_DEE;
            else if (strcmp(text, "ma") == 0)
                type = TOKEN_MA;
            else if (strcmp(text, "mana") == 0)
                type = TOKEN_MANA;
            else if (strcmp(text, "mgbe") == 0)
                type = TOKEN_MGBE;
            else if (strcmp(text, "gosi") == 0)
                type = TOKEN_GOSI;
            else if (strcmp(text, "eziokwu") == 0)
                type = TOKEN_EZIOKWU;
            else if (strcmp(text, "ụgha") == 0)
                type = TOKEN_UGHA;

            ensure_capacity(&tokens, &capacity, count);
            tokens[count].type = type;
            tokens[count].value = text;
            tokens[count].line_number = line;
            count++;
            continue;
        }

        // Numbers
        if (isdigit((unsigned char)c)) {
            size_t start = i;
            while (isdigit((unsigned char)source[i]))
                i++;
            size_t len = i - start;
            char *num = malloc(len + 1);
            if (!num) {
                report_error("Memory allocation failed", line);
                break;
            }
            memcpy(num, &source[start], len);
            num[len] = '\0';

            ensure_capacity(&tokens, &capacity, count);
            tokens[count].type = TOKEN_NUMBER;
            tokens[count].value = num;
            tokens[count].line_number = line;
            count++;
            continue;
        }

        // Strings
        if (c == '"') {
            i++; // skip opening quote
            size_t start = i;
            while (source[i] && source[i] != '"') {
                if (source[i] == '\n')
                    line++;
                i++;
            }
            if (source[i] != '"') {
                report_error("Unterminated string", line);
                break;
            }
            size_t len = i - start;
            char *str = malloc(len + 1);
            if (!str) {
                report_error("Memory allocation failed", line);
                break;
            }
            memcpy(str, &source[start], len);
            str[len] = '\0';
            i++; // skip closing quote

            ensure_capacity(&tokens, &capacity, count);
            tokens[count].type = TOKEN_STRING;
            tokens[count].value = str;
            tokens[count].line_number = line;
            count++;
            continue;
        }

        // Operators and punctuation
        switch (c) {
            case '=':
                if (source[i + 1] == '=') {
                    ensure_capacity(&tokens, &capacity, count);
                    tokens[count].type = TOKEN_EQUAL;
                    tokens[count].value = string_duplicate("==");
                    tokens[count].line_number = line;
                    count++;
                    i += 2;
                } else {
                    ensure_capacity(&tokens, &capacity, count);
                    tokens[count].type = TOKEN_ASSIGN;
                    tokens[count].value = string_duplicate("=");
                    tokens[count].line_number = line;
                    count++;
                    i++;
                }
                continue;
            case '!':
                if (source[i + 1] == '=') {
                    ensure_capacity(&tokens, &capacity, count);
                    tokens[count].type = TOKEN_NOT_EQUAL;
                    tokens[count].value = string_duplicate("!=");
                    tokens[count].line_number = line;
                    count++;
                    i += 2;
                    continue;
                }
                break;
            case '<':
                if (source[i + 1] == '=') {
                    ensure_capacity(&tokens, &capacity, count);
                    tokens[count].type = TOKEN_LESS_EQUAL;
                    tokens[count].value = string_duplicate("<=");
                    tokens[count].line_number = line;
                    count++;
                    i += 2;
                } else {
                    ensure_capacity(&tokens, &capacity, count);
                    tokens[count].type = TOKEN_LESS;
                    tokens[count].value = string_duplicate("<");
                    tokens[count].line_number = line;
                    count++;
                    i++;
                }
                continue;
            case '>':
                if (source[i + 1] == '=') {
                    ensure_capacity(&tokens, &capacity, count);
                    tokens[count].type = TOKEN_GREATER_EQUAL;
                    tokens[count].value = string_duplicate(">=");
                    tokens[count].line_number = line;
                    count++;
                    i += 2;
                } else {
                    ensure_capacity(&tokens, &capacity, count);
                    tokens[count].type = TOKEN_GREATER;
                    tokens[count].value = string_duplicate(">");
                    tokens[count].line_number = line;
                    count++;
                    i++;
                }
                continue;
            case '+':
                ensure_capacity(&tokens, &capacity, count);
                tokens[count].type = TOKEN_PLUS;
                tokens[count].value = string_duplicate("+");
                tokens[count].line_number = line;
                count++;
                i++;
                continue;
            case '-':
                ensure_capacity(&tokens, &capacity, count);
                tokens[count].type = TOKEN_MINUS;
                tokens[count].value = string_duplicate("-");
                tokens[count].line_number = line;
                count++;
                i++;
                continue;
            case '*':
                ensure_capacity(&tokens, &capacity, count);
                tokens[count].type = TOKEN_MULTIPLY;
                tokens[count].value = string_duplicate("*");
                tokens[count].line_number = line;
                count++;
                i++;
                continue;
            case '/':
                ensure_capacity(&tokens, &capacity, count);
                tokens[count].type = TOKEN_DIVIDE;
                tokens[count].value = string_duplicate("/");
                tokens[count].line_number = line;
                count++;
                i++;
                continue;
            case '(':
                ensure_capacity(&tokens, &capacity, count);
                tokens[count].type = TOKEN_LPAREN;
                tokens[count].value = string_duplicate("(");
                tokens[count].line_number = line;
                count++;
                i++;
                continue;
            case ')':
                ensure_capacity(&tokens, &capacity, count);
                tokens[count].type = TOKEN_RPAREN;
                tokens[count].value = string_duplicate(")");
                tokens[count].line_number = line;
                count++;
                i++;
                continue;
            case '{':
                ensure_capacity(&tokens, &capacity, count);
                tokens[count].type = TOKEN_LBRACE;
                tokens[count].value = string_duplicate("{");
                tokens[count].line_number = line;
                count++;
                i++;
                continue;
            case '}':
                ensure_capacity(&tokens, &capacity, count);
                tokens[count].type = TOKEN_RBRACE;
                tokens[count].value = string_duplicate("}");
                tokens[count].line_number = line;
                count++;
                i++;
                continue;
        }

        // If we reach here, the character was unexpected
        char msg[64];
        snprintf(msg, sizeof(msg), "Unexpected character '%c'", c);
        report_error(msg, line);
        i++;
    }

    // Append EOF token
    ensure_capacity(&tokens, &capacity, count);
    tokens[count].type = TOKEN_EOF;
    tokens[count].value = NULL;
    tokens[count].line_number = line;
    count++;

    // Shrink array to exact size
    Token *result = realloc(tokens, sizeof(Token) * count);
    return result ? result : tokens;
}

