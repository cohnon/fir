#include "fir.h"

#include <assert.h>
#include <inttypes.h>
#include <stdio.h>

static fir_Instr *create_instr(
    fir_Block *block,
    fir_InstrKind kind,
    fir_DataType type
) {
    fir_Module *module = block->func->module;
    fir_Instr *instr = fir_arena_alloc(&module->arena, sizeof(fir_Instr));
    instr->kind = kind;
    instr->type = type;

    fir_array_append(fir_Instr *, &block->instrs, instr);

    return instr;
}

fir_Instr *fir_instr_lit_int(
    fir_Block *block,
    fir_DataType type,
    uint64_t val,
    bool is_signed
) {
    assert(block != NULL);
    assert(type.kind == FIR_TYPE_INT);

    fir_Instr *instr = create_instr(block, FIR_INSTR_LIT, type);
    instr->_.lit.i.val = val;
    instr->_.lit.i.is_signed = is_signed;

    return instr;
}

fir_Instr *fir_instr_lit_float(
    fir_Block *block,
    fir_DataType type,
    double val
) {
    assert(block != NULL);
    assert(type.kind == FIR_TYPE_FLOAT);

    fir_Instr *instr = create_instr(block, FIR_INSTR_LIT, type);
    instr->_.lit.f = val;

    return instr;
}

fir_Instr *fir_instr_arg(fir_Block *block, size_t idx) {
    assert(block != NULL);
    assert(idx < block->inputs.len);

    fir_DataType type = *fir_array_get(fir_DataType, &block->inputs, idx);
    fir_Instr *instr = create_instr(block, FIR_INSTR_ARG, type);
    instr->_.arg = idx;

    return instr;
}

fir_Instr *fir_instr_add(
    fir_Block *block,
    fir_DataType type,
    fir_Instr *lhs,
    fir_Instr *rhs
) {
    assert(block != NULL);
    assert(lhs != NULL);
    assert(rhs != NULL);
    assert(fir_type_eq(lhs->type, rhs->type));

    fir_Instr *instr = create_instr(block, FIR_INSTR_ADD, type);
    instr->_.args[0] = lhs;
    instr->_.args[1] = rhs;

    return instr;
}

void fir_instr_dump(fir_Instr *instr, FILE *fp) {
    assert(instr != NULL);
    assert(fp != NULL);

    switch (instr->kind) {
    case FIR_INSTR_LIT:
        fprintf(fp, "lit");
        switch (instr->type.kind) {
        case FIR_TYPE_INT:
            fprintf(
                fp, " %s%" PRId64,
                instr->_.lit.i.is_signed ? "-" : "",
                instr->_.lit.i.val
            );
            break;
        case FIR_TYPE_FLOAT:
            fprintf(fp, " %f", instr->_.lit.f);
            break;
        case FIR_TYPE_PTR:
            __builtin_unreachable();
        }
        break;

    case FIR_INSTR_ARG:
        fprintf(fp, "arg");
        fprintf(fp, " %zu", instr->_.arg);
        break;

    case FIR_INSTR_ADD: {
        fir_Instr *lhs = instr->_.args[0];
        fir_Instr *rhs = instr->_.args[1];

        fprintf(fp, "add");
        fprintf(fp, " R%d, R%d", lhs->idx, rhs->idx);
        break;
    }
    default:
        fprintf(fp, "<unhandled instruction>");
        break;
    }
}
