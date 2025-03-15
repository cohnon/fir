#include "fir.h"
#include <stdio.h>

fir_Type fir_type_int(size_t bits) {
    fir_Type type;
    type.kind = FIR_TYPE_INT;
    type.bits = bits;

    return type;
}

void fir_type_dump(fir_Type type, FILE *fp) {
    switch (type.kind) {
    case FIR_TYPE_INT:
        fprintf(fp, "i%d", type.bits);
        break;
    default:
        fprintf(fp, "<unhandled type>");
        break;
    }
}
