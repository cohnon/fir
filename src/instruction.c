#include "fir.h"

#include <assert.h>
#include <inttypes.h>
#include <stdio.h>

static fir_Instruction *create_instr(
    fir_Block *block,
    enum fir_Instruction kind,
    fir_Type type
) {
    fir_Module *module = block->func->module;
    fir_Instruction *instr = fir_arena_alloc(&module->arena, sizeof(fir_Instruction));
    instr->kind = kind;
    instr->type = type;

    fir_array_append(fir_Instruction *, &block->instrs, instr);

    return instr;
}

void fir_instr_lit_int(
    fir_Block *block,
    fir_Type type,
    uint64_t val,
    bool is_signed
) {
    assert(block != NULL);

    fir_Instruction *instr = create_instr(block, FIR_INSTR_LIT, type);
    instr->_.lit.i.val = val;
    instr->_.lit.i.is_signed = is_signed;
}

void fir_instr_add(
    fir_Block *block,
    fir_Type type,
    fir_Instruction *lhs,
    fir_Instruction *rhs
) {
    assert(block != NULL);
    assert(lhs != NULL);
    assert(rhs != NULL);

    fir_Instruction *instr = create_instr(block, FIR_INSTR_ADD, type);
    instr->_.args[0] = lhs;
    instr->_.args[1] = rhs;
}

void fir_instr_dump(fir_Block *block, fir_Instruction *instr, FILE *fp) {
    assert(block != NULL);
    assert(instr != NULL);
    assert(fp != NULL);

    fir_Module *module = block->func->module;

    switch (instr->kind) {
    case FIR_INSTR_LIT:
        fprintf(fp, "lit.");
        fir_type_dump(instr->type, fp);
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

    case FIR_INSTR_ADD: {
        const char *lhs_name = fir_string_get(module, instr->_.args[0]->name);
        const char *rhs_name = fir_string_get(module, instr->_.args[1]->name);

        fprintf(fp, "add.");
        fir_type_dump(instr->type, fp);
        fprintf(fp, " %s, %s", lhs_name, rhs_name);
        break;
    }
    default:
        fprintf(fp, "<unhandled instruction>");
        break;
    }
}
