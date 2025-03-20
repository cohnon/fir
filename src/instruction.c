#include "fir.h"

#include <assert.h>
#include <inttypes.h>
#include <stdio.h>

bool fir_instr_is_terminator(fir_Instr *instr) {
    return FIR_INSTR_JMP <= instr->kind && instr->kind <= FIR_INSTR_RET;
}

static fir_Instr *create_instr(
    fir_Block *block, fir_InstrKind kind, fir_DataType type, size_t size
) {
    fir_Module *module = block->func->module;

    fir_Instr *instr = fir_arena_alloc(&module->arena, size);
    instr->kind = kind;
    instr->type = type;

    fir_array_append(fir_Instr *, &block->instrs, instr);

    return instr;
}

static fir_Instr *create_3addr_instr(
    fir_Block *block, fir_InstrKind kind, fir_Instr *arg0, fir_Instr *arg1
) {
    assert(block != NULL);
    assert(arg0 != NULL);
    assert(arg1 != NULL);
    assert(fir_type_eq(arg0->type, arg1->type));

    size_t instr_size = sizeof(fir_Instr) + (2 * sizeof(fir_Instr *));

    fir_Instr *instr = create_instr(block, kind, arg0->type, instr_size);

    fir_Instr **args = (fir_Instr **)instr->data;
    args[0] = arg0;
    args[1] = arg1;

    return instr;
}

static fir_Instr *create_lit_instr(fir_Block *block, fir_DataType type) {
    assert(block != NULL);

    size_t instr_size = sizeof(fir_Instr) + sizeof(fir_InstrLit);

    fir_Instr *instr = create_instr(block, FIR_INSTR_LIT, type, instr_size);

    return instr;
}

fir_Instr *fir_instr_lit_int(
    fir_Block *block,
    fir_DataType type,
    uint64_t val
) {
    assert(type.kind == FIR_TYPE_INT);

    fir_Instr *instr = create_lit_instr(block, type);

    fir_InstrLit *lit = (fir_InstrLit *)instr->data;
    lit->i = val;

    return instr;
}

fir_Instr *fir_instr_lit_float(
    fir_Block *block,
    fir_DataType type,
    double val
) {
    assert(type.kind == FIR_TYPE_FLOAT);

    fir_Instr *instr = create_lit_instr(block, type);

    fir_InstrLit *lit = (fir_InstrLit *)instr->data;
    lit->f = val;

    return instr;
}

fir_Instr *fir_instr_arg(fir_Block *block, size_t idx) {
    assert(block != NULL);
    assert(idx < block->input.len);

    fir_DataType type = *fir_array_get(fir_DataType, &block->input, idx);
    size_t instr_size = sizeof(fir_Instr) + sizeof(size_t);

    fir_Instr *instr = create_instr(block, FIR_INSTR_ARG, type, instr_size);
    
    *(size_t *)instr->data = idx;

    return instr;
}

fir_Instr *fir_instr_add(fir_Block *block, fir_Instr *lhs, fir_Instr *rhs) {
    return create_3addr_instr(block, FIR_INSTR_ADD, lhs, rhs);
}

fir_Instr *fir_instr_sub(fir_Block *block, fir_Instr *lhs, fir_Instr *rhs) {
    return create_3addr_instr(block, FIR_INSTR_SUB, lhs, rhs);
}

fir_Instr *fir_instr_mul(fir_Block *block, fir_Instr *lhs, fir_Instr *rhs) {
    return create_3addr_instr(block, FIR_INSTR_MUL, lhs, rhs);
}

fir_Instr *fir_instr_ret(fir_Block *block) {
    assert(block != NULL);

    size_t args_n = block->func->output.len;
    size_t instr_size =
        sizeof(fir_Instr) +
        sizeof(fir_InstrRet) +
        (args_n * sizeof(fir_Instr *));

    fir_Instr *instr =
        create_instr(block, FIR_INSTR_RET, fir_type_void(), instr_size);

    fir_InstrRet *ret = (fir_InstrRet *)instr->data;
    ret->args_n = args_n;

    fir_Instr **args = (fir_Instr **)ret->args;
    for (size_t i = 0; i < ret->args_n; i++) {
        args[i] = NULL;
    }

    return instr;
}

void fir_instr_ret_arg(fir_Instr *instr, fir_Instr *arg) {
    assert(instr != NULL);
    assert(arg != NULL);
    assert(instr->kind == FIR_INSTR_RET);

    fir_InstrRet *ret = (fir_InstrRet *)instr->data;
    fir_Instr **args = (fir_Instr **)ret->args;
    
    size_t i = 0;
    for (; i < ret->args_n; i++) {
        if (args[i] == NULL) {
            args[i] = arg;
            break;
        }
    }

    assert(i < ret->args_n && "passing in too many args to ret");
}

fir_Instr *fir_instr_call(fir_Block *block, fir_Function *func) {
    assert(block != NULL);
    assert(func != NULL);

    fir_DataType type;
    if (func->output.len == 0) {
        type = fir_type_void();
    } else if (func->output.len == 1) {
        type = *fir_array_get(fir_DataType, &func->output, 0);
    } else {
        type = fir_type_tuple(func->output.len);
    }

    size_t instr_size =
        sizeof(fir_Instr) +
        sizeof(fir_InstrCall) +
        (func->input.len * sizeof(fir_Instr *));

    fir_Instr *instr = create_instr(block, FIR_INSTR_CALL, type, instr_size);

    fir_InstrCall *call = (fir_InstrCall *)instr->data;
    call->func = func;

    fir_Instr **args = (fir_Instr **)call->args;

    for (size_t i = 0; i < func->input.len; i++) {
        args[i] = NULL;
    }

    return instr;
}

void fir_instr_call_arg(fir_Instr *instr, fir_Instr *arg) {
    assert(instr != NULL);
    assert(arg != NULL);
    assert(instr->kind == FIR_INSTR_CALL);

    fir_InstrCall *call = (fir_InstrCall *)instr->data;
    fir_Instr **args = (fir_Instr **)call->args;

    size_t i = 0;
    for (; i < call->func->input.len; i++) {
        if (args[i] == NULL) {
            args[i] = arg;
            break;
        }
    }

    assert(i < call->func->input.len && "passing in too many args to call");
}

fir_Instr *fir_instr_proj(fir_Block *block, fir_Instr *tuple, size_t idx) {
    assert(block != NULL);
    assert(tuple != NULL);
    assert(idx < tuple->type.data);
    assert(tuple->kind == FIR_INSTR_CALL);

    return NULL;
}

void fir_instr_dump(fir_Instr *instr, FILE *fp) {
    assert(instr != NULL);
    assert(fp != NULL);

    switch (instr->kind) {
    case FIR_INSTR_LIT: {
        fprintf(fp, "lit");

        fir_InstrLit *lit = (fir_InstrLit *)instr->data;
        switch (instr->type.kind) {
        case FIR_TYPE_INT:
            fprintf(fp, " %" PRId64, lit->i);
            break;
        case FIR_TYPE_FLOAT:
            fprintf(fp, " %g", lit->f);
            break;
        case FIR_TYPE_PTR:
            __builtin_unreachable();
        case FIR_TYPE_TUPLE:
            __builtin_unreachable();
        }
        break;
    }
    case FIR_INSTR_ARG: {
        size_t *arg_idx = (size_t *)instr->data;
        fprintf(fp, "arg");
        fprintf(fp, " %zu", *arg_idx);
        break;
    }
    case FIR_INSTR_ADD: {
        fir_Instr **args = (fir_Instr **)instr->data;

        fprintf(fp, "add");
        fprintf(fp, " R%d, R%d", args[0]->idx, args[1]->idx);
        break;
    }
    case FIR_INSTR_SUB: {
        fir_Instr **args = (fir_Instr **)instr->data;

        fprintf(fp, "sub");
        fprintf(fp, " R%d, R%d", args[0]->idx, args[1]->idx);
        break;
    }
    case FIR_INSTR_MUL: {
        fir_Instr **args = (fir_Instr **)instr->data;

        fprintf(fp, "mul");
        fprintf(fp, " R%d, R%d", args[0]->idx, args[1]->idx);
        break;
    }
    case FIR_INSTR_RET: {
        fprintf(fp, "ret");

        fir_InstrRet *ret = (fir_InstrRet *)instr->data;
        if (ret->args_n > 0) {
            fir_Instr **args = (fir_Instr **)ret->args;

            fprintf(fp, " ");
            for (size_t i = 0; i < ret->args_n; i++) {
                fprintf(fp, "R%d", args[i]->idx);
                if (i < ret->args_n - 1) {
                    fprintf(fp, ", ");
                }
            }
        }
        break;
    }
    case FIR_INSTR_CALL: {
        fir_InstrCall *call = (fir_InstrCall *)instr->data;
        char *name = fir_string_get(call->func->module, call->func->name);

        fprintf(fp, "call @%s", name);

        if (call->func->input.len > 0) {
            fprintf(fp, " ");
            fir_Instr **args = (fir_Instr **)call->args;
            for (size_t i = 0; i < call->func->input.len; i++) {
                fprintf(fp, "R%d", args[i]->idx);

                if (i < call->func->input.len - 1) {
                    fprintf(fp, ", ");
                }
            }
        }
        break;
    }
    default:
        fprintf(fp, "<unhandled instruction>");
        break;
    }
}
