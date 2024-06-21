#include "fir.h"

#include <stdbool.h>
#include <string.h>


// TODO: generate an id for each string (separate to uniqe_id which is just
//       for debuggins) for fast comparisons
FirSym fir_sym_none(void) {
    return (FirSym){ 0 };
}

FirSym fir_sym_slc(char *c_str, size_t len) {
    return (FirSym){ .ptr = c_str, .len = len, .unique_id = 0 };
}

_Bool fir_sym_eq(FirSym a, FirSym b) {
    if (a.len != b.len) { return false; }

    return strncmp(a.ptr, b.ptr, a.len) == 0;
}


