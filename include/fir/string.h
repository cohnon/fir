#ifndef FIR_STRING_H
#define FIR_STRING_H

#include <stdint.h>
#include <stddef.h>


typedef struct String {
    char *ptr;
    uint16_t   len;
    uint16_t   unique_id;
} String;

// constructors
String string_none(void);
String string_clone(String string);
#define string_lit(c_str) (String){ .ptr = c_str, .len = sizeof(c_str) - 1 }
String string_slc(char *c_str, size_t len);
String string_cstr(char *c_str);


String string_concat(String a, String b);
void string_append(String *a, String b);
_Bool string_eq(String a, String b);


#define string_fmt_id(sym) (int)(sym).len, (sym).ptr, (sym).unique_id
#define string_fmt(sym) (int)(sym).len, (sym).ptr

#endif
