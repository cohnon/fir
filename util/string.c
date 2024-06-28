#include "fir.h"

#include <stdbool.h>
#include <string.h>


// TODO: generate an id for each string (separate to uniqe_id which is just
//       for debuggins) for fast comparisons
FirString fir_string_none(void) {
    return (FirString){ 0 };
}

FirString fir_string_slc(char *c_str, size_t len) {
    return (FirString){ .ptr = c_str, .len = len, .unique_id = 0 };
}

_Bool fir_string_eq(FirString a, FirString b) {
    if (a.len != b.len) { return false; }

    return strncmp(a.ptr, b.ptr, a.len) == 0;
}


