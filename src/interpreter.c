#include "interpreter.h"
#include "util.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum { VAL_NUMBER, VAL_STRING } ValueType;

typedef struct {
    ValueType type;
    union {
        double number;
        char *string;
    } as;
} Value;

typedef struct {
    char *name;
    Value value;
} Variable;

static Variable *vars = NULL;
static size_t var_count = 0;
static size_t var_capacity = 0;

static Variable *find_var(const char *name) {
    for (size_t i = 0; i < var_count; ++i) {
        if (strcmp(vars[i].name, name) == 0)
            return &vars[i];
    }
    return NULL;
}

static void set_var(const char *name, Value value) {
    Variable *v = find_var(name);
    if (!v) {
        if (var_count + 1 > var_capacity) {
            var_capacity = var_capacity ? var_capacity * 2 : 8;
            vars = realloc(vars, sizeof(Variable) * var_capacity);
            if (!vars) {
                report_error("Memory allocation failed for variables", -1);
                exit(1);
            }
        }
        v = &vars[var_count++];
        v->name = string_duplicate(name);
    } else {
        if (v->value.type == VAL_STRING)
            free(v->value.as.string);
    }
    v->value.type = value.type;
    if (value.type == VAL_STRING)
        v->value.as.string = string_duplicate(value.as.string);
    else
        v->value.as.number = value.as.number;
}

static Value get_var_value(const char *name) {
    Variable *v = find_var(name);
    if (!v) {
        char msg[128];
        snprintf(msg, sizeof(msg), "Undefined variable '%s'", name);
        report_error(msg, -1);
        Value err = {VAL_NUMBER, {.number = 0}};
        return err;
    }
    if (v->value.type == VAL_STRING) {
        Value copy = {VAL_STRING, {.string = string_duplicate(v->value.as.string)}};
        return copy;
    }
    return v->value;
}

static void free_vars(void) {
    for (size_t i = 0; i < var_count; ++i) {
        free(vars[i].name);
        if (vars[i].value.type == VAL_STRING)
            free(vars[i].value.as.string);
    }
    free(vars);
    vars = NULL;
    var_count = 0;
    var_capacity = 0;
}

static Value eval(ASTNode *node);

static void exec_stmt(ASTNode *node) {
    if (!node) return;
    switch (node->type) {
        case NODE_VAR_DECL: {
            Value val = eval(node->left);
            set_var(node->value, val);
            if (val.type == VAL_STRING)
                free(val.as.string);
            break;
        }
        case NODE_PRINT_STMT: {
            Value val = eval(node->left);
            if (val.type == VAL_STRING) {
                printf("%s\n", val.as.string);
                free(val.as.string);
            } else {
                printf("%g\n", val.as.number);
            }
            break;
        }
        default: {
            Value val = eval(node);
            if (val.type == VAL_STRING)
                free(val.as.string);
            break;
        }
    }
}

static Value eval_binary(Value left, Value right, const char *op) {
    if (strcmp(op, "+") == 0) {
        if (left.type == VAL_STRING || right.type == VAL_STRING) {
            const char *lstr = left.type == VAL_STRING ? left.as.string : NULL;
            const char *rstr = right.type == VAL_STRING ? right.as.string : NULL;
            char buf[64];
            if (!lstr) {
                snprintf(buf, sizeof(buf), "%g", left.as.number);
                lstr = buf;
            }
            char buf2[64];
            if (!rstr) {
                snprintf(buf2, sizeof(buf2), "%g", right.as.number);
                rstr = buf2;
            }
            char *res = malloc(strlen(lstr) + strlen(rstr) + 1);
            strcpy(res, lstr);
            strcat(res, rstr);
            Value v = {VAL_STRING, {.string = res}};
            return v;
        } else {
            Value v = {VAL_NUMBER, {.number = left.as.number + right.as.number}};
            return v;
        }
    } else if (strcmp(op, "-") == 0) {
        if (left.type != VAL_NUMBER || right.type != VAL_NUMBER) {
            report_error("Operands must be numbers for '-'", -1);
            return (Value){VAL_NUMBER, {.number = 0}};
        }
        return (Value){VAL_NUMBER, {.number = left.as.number - right.as.number}};
    } else if (strcmp(op, "*") == 0) {
        if (left.type != VAL_NUMBER || right.type != VAL_NUMBER) {
            report_error("Operands must be numbers for '*'", -1);
            return (Value){VAL_NUMBER, {.number = 0}};
        }
        return (Value){VAL_NUMBER, {.number = left.as.number * right.as.number}};
    } else if (strcmp(op, "/") == 0) {
        if (left.type != VAL_NUMBER || right.type != VAL_NUMBER) {
            report_error("Operands must be numbers for '/'", -1);
            return (Value){VAL_NUMBER, {.number = 0}};
        }
        return (Value){VAL_NUMBER, {.number = left.as.number / right.as.number}};
    }
    report_error("Unknown binary operator", -1);
    return (Value){VAL_NUMBER, {.number = 0}};
}

static Value eval(ASTNode *node) {
    switch (node->type) {
        case NODE_NUMBER:
            return (Value){VAL_NUMBER, {.number = atof(node->value)}};
        case NODE_STRING:
            return (Value){VAL_STRING, {.string = string_duplicate(node->value)}};
        case NODE_IDENTIFIER:
            return get_var_value(node->value);
        case NODE_BINARY_EXPR: {
            Value left = eval(node->left);
            Value right = eval(node->right);
            Value result = eval_binary(left, right, node->value);
            if (left.type == VAL_STRING) free(left.as.string);
            if (right.type == VAL_STRING) free(right.as.string);
            return result;
        }
        default:
            report_error("Invalid expression", -1);
            return (Value){VAL_NUMBER, {.number = 0}};
    }
}

void interpret(ASTNode *ast) {
    for (ASTNode *node = ast; node != NULL; node = node->right) {
        exec_stmt(node->left);
    }
    free_vars();
}

