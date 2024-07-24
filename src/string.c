#include "fir/string.h"

#include <stdlib.h>
#include <string.h>


String string_none(void) {
    return (String){ .ptr = NULL, .len = 0, .unique_id = 0 };
}

String string_clone(String string) {
    char *ptr = malloc(string.len + 1);
    strncpy(ptr, string.ptr, string.len);
    ptr[string.len] = '\0';

    return string_slc(ptr, string.len);
}

String string_slc(char *ptr, size_t len) {
    return (String){ .ptr = ptr, .len = len, .unique_id = 0 };
}

String string_cstr(char *ptr) {
    return string_slc(ptr, strlen(ptr));
}

_Bool string_eq(String a, String b) {
    size_t max_len = a.len > b.len ? a.len : b.len;

    return strncmp(a.ptr, b.ptr, max_len) == 0;
}

String string_concat(String a, String b) {
    size_t len = a.len + b.len;
    char *ptr = malloc(len+1);

    strncpy(ptr, a.ptr, a.len);
    strncpy(ptr + a.len, b.ptr, b.len);
    ptr[len] = '\0';

    return (String){ .ptr = ptr, .len = len, .unique_id = 0 };
}

void string_append(String *a, String b) {
    size_t len = a->len + b.len;
    char *ptr = realloc(a->ptr, len+1);

    strncpy(ptr + a->len, b.ptr, b.len);
    ptr[len] = '\0';

    a->ptr = ptr;
    a->len = len;
}
