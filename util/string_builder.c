#include "string_builder.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

StringBuilder sb_init(size_t init_cap) {
    StringBuilder sb;
    sb.len = 0;
    sb.cap = init_cap;
    sb.buf = malloc(sizeof(char) * sb.cap);

    return sb;
}

void sb_printn(StringBuilder *sb, const char *str, size_t len) {
    if (sb->len + len > sb->cap) {
        sb->cap *= 1.5;
        sb->buf = realloc(sb->buf, sizeof(char) * sb->cap);
    }

    strncpy(sb->buf + sb->len, str, len);

    sb->len += len;
}

__attribute__((format(printf, 2, 3)))
void sb_printf(StringBuilder *sb, const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);

    char buf[1024];
    int buf_len = vsnprintf(buf, 1024, fmt, args);

    va_end(args);

    sb_printn(sb, buf, buf_len);
}

FirString sb_to_string(StringBuilder *sb) {
    return (FirString) {
        .ptr = sb->buf,
        .len = sb->len,
    };
}
