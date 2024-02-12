#include "fir.h"

FirSym fir_sym_none(void) {
    return (FirSym){ 0 };
}

FirSym fir_sym_slc(char *c_str, size_t len) {
    return (FirSym){ .ptr = c_str, .len = len };
}
