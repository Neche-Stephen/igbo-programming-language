#include "util.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *string_duplicate(const char *src) {
    if (!src) return NULL;
    char *dup = (char *)malloc(strlen(src) + 1);
    if (!dup) return NULL;
    strcpy(dup, src);
    return dup;
}

void report_error(const char *message, int line_number) {
    if (line_number >= 0)
        fprintf(stderr, "Error (line %d): %s\n", line_number, message);
    else
        fprintf(stderr, "Error: %s\n", message);
}
