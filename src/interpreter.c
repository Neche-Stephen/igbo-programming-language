#include "interpreter.h"
#include "util.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum { VAL_NUMBER, VAL_STRING, VAL_BOOL } ValueType;

typedef struct {
    ValueType type;
    union {
        double number;
        char *string;
        int boolean;
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
    if (value.type == VAL_STRING) {
        v->value.as.string = string_duplicate(value.as.string);
    } else if (value.type == VAL_BOOL) {
        v->value.as.boolean = value.as.boolean;
    } else {
        v->value.as.number = value.as.number;
    }
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
    } else if (v->value.type == VAL_BOOL) {
        return (Value){VAL_BOOL, {.boolean = v->value.as.boolean}};
    }
    return (Value){VAL_NUMBER, {.number = v->value.as.number}};
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
static void exec_stmt(ASTNode *node);

static void exec_block(ASTNode *block) {
    for (ASTNode *n = block; n != NULL; n = n->right) {
        exec_stmt(n->left);
    }
}

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
            } else if (val.type == VAL_NUMBER) {
                printf("%g\n", val.as.number);
            } else {
                printf(val.as.boolean ? "eziokwu\n" : "ụgha\n");
            }
            break;
        }
        case NODE_IF_STMT: {
            Value cond = eval(node->left);
            int truth = 0;
            if (cond.type == VAL_BOOL) truth = cond.as.boolean;
            else if (cond.type == VAL_NUMBER) truth = cond.as.number != 0;
            else if (cond.type == VAL_STRING) truth = cond.as.string[0] != '\0';
            if (cond.type == VAL_STRING) free(cond.as.string);
            if (truth)
                exec_block(node->right);
            else if (node->third)
                exec_block(node->third);
            break;
        }
        case NODE_WHILE_STMT: {
            const int MAX_ITERS = 10000;
            int iter = 0;
            while (1) {
                if (iter++ > MAX_ITERS) {
                    report_error("Possible infinite loop detected", -1);
                    break;
                }
                Value cond = eval(node->left);
                int truth = 0;
                if (cond.type == VAL_BOOL) truth = cond.as.boolean;
                else if (cond.type == VAL_NUMBER) truth = cond.as.number != 0;
                else if (cond.type == VAL_STRING) truth = cond.as.string[0] != '\0';
                if (cond.type == VAL_STRING) free(cond.as.string);
                if (!truth) break;
                exec_block(node->right);
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
    } else if (strcmp(op, "==") == 0) {
        if (left.type == VAL_STRING && right.type == VAL_STRING) {
            return (Value){VAL_BOOL, {.boolean = strcmp(left.as.string, right.as.string) == 0}};
        } else if (left.type == VAL_NUMBER && right.type == VAL_NUMBER) {
            return (Value){VAL_BOOL, {.boolean = left.as.number == right.as.number}};
        } else if (left.type == VAL_BOOL && right.type == VAL_BOOL) {
            return (Value){VAL_BOOL, {.boolean = left.as.boolean == right.as.boolean}};
        }
        report_error("Type mismatch for '=='", -1);
        return (Value){VAL_BOOL, {.boolean = 0}};
    } else if (strcmp(op, "!=") == 0) {
        if (left.type == VAL_STRING && right.type == VAL_STRING) {
            return (Value){VAL_BOOL, {.boolean = strcmp(left.as.string, right.as.string) != 0}};
        } else if (left.type == VAL_NUMBER && right.type == VAL_NUMBER) {
            return (Value){VAL_BOOL, {.boolean = left.as.number != right.as.number}};
        } else if (left.type == VAL_BOOL && right.type == VAL_BOOL) {
            return (Value){VAL_BOOL, {.boolean = left.as.boolean != right.as.boolean}};
        }
        report_error("Type mismatch for '!='", -1);
        return (Value){VAL_BOOL, {.boolean = 0}};
    } else if (strcmp(op, "<") == 0 || strcmp(op, ">") == 0 ||
               strcmp(op, "<=") == 0 || strcmp(op, ">=") == 0) {
        if (left.type != VAL_NUMBER || right.type != VAL_NUMBER) {
            report_error("Operands must be numbers for comparison", -1);
            return (Value){VAL_BOOL, {.boolean = 0}};
        }
        int result = 0;
        if (strcmp(op, "<") == 0) result = left.as.number < right.as.number;
        else if (strcmp(op, ">") == 0) result = left.as.number > right.as.number;
        else if (strcmp(op, "<=") == 0) result = left.as.number <= right.as.number;
        else if (strcmp(op, ">=") == 0) result = left.as.number >= right.as.number;
        return (Value){VAL_BOOL, {.boolean = result}};
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
        case NODE_BOOL:
            return (Value){VAL_BOOL, {.boolean = strcmp(node->value, "eziokwu") == 0}};
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
    exec_block(ast);
    free_vars();
}

