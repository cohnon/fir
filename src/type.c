#include "fir.h"


FirType fir_ty_int(size_t bits) {
    return (FirType){ FirType_Int, .bits = bits };
}

FirType fir_ty_void(void) {
    return fir_ty_int(0);
}

FirType fir_ty_bool(void) {
    return fir_ty_int(1);
}
