#ifndef STRING_BUILDER_H
#define STRING_BUILDER_H

#include <stddef.h>


typedef struct StringBuilder {
    char   *buf;
    size_t len;
    size_t cap;
} StringBuilder;

StringBuilder sb_init(size_t init_cap);

void sb_printn(StringBuilder *sb, const char *str, size_t len);

__attribute__((format(printf, 2, 3)))
void sb_printf(StringBuilder *sb, const char *fmt, ...);

#define sb_print_lit(sb, lit) sb_printn(sb, lit, sizeof(lit) - 1)

#endif
