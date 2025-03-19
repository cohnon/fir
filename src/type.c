#include "fir.h"
#include <stdio.h>

bool fir_type_eq(fir_DataType a, fir_DataType b) {
    return (a.kind == b.kind) && (a.bits == b.bits);
}

fir_DataType fir_type_int(size_t bits) {
    fir_DataType type;
    type.kind = FIR_TYPE_INT;
    type.bits = bits;

    return type;
}

fir_DataType fir_type_float(size_t bits) {
    fir_DataType type;
    type.kind = FIR_TYPE_FLOAT;
    type.bits = bits;

    return type;
}

void fir_type_dump(fir_DataType type, FILE *fp) {
    switch (type.kind) {
    case FIR_TYPE_INT:
        fprintf(fp, "i%d", type.bits);
        break;
    case FIR_TYPE_FLOAT:
        fprintf(fp, "f%d", type.bits);
        break;
    default:
        fprintf(fp, "<unhandled type>");
        break;
    }
}
