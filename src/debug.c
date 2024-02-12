#include "debug.h"

#include "fir_priv.h"

#include "stdio.h"

static void fird_print_type(FirType *type) {
    switch (type->kind) {
    case FirType_Int:
    if (type->bits == 0) {
        printf("void");
    } else if (type->bits == 1) {
        printf("bool");
    } else {
        printf("i%d", type->bits);
    }

    break;

    default: printf("fird_print_type: unhandled type (%d)\n", type->kind);
    }
}

static void fird_print_val(FirVal *val) {
    switch (val->kind) {
    case FirVal_Reg:
        printf("<reg>");
        break;

    case FirVal_Imm:
        switch (val->imm->kind) {
        case FirImm_Int:
            printf("i32(");
            if (val->imm->integer.is_signed) {
                printf("-");
            }
            printf("%ld)", val->imm->integer.n);

            break;
        }
        break;

    default:
        printf("<unknown value>");
        break;
    }
}

static void fird_print_args(FirInstr *instr) {
    fird_print_val(&instr->args[0]);
    printf(" ");
    fird_print_val(&instr->args[1]);
}

static void fird_print_instr(FirInstr *instr) {
    printf("  ");

    switch (instr->kind) {
    case FirInstr_Add:
        printf("add ");
        fird_print_args(instr);
        
        break;

    default:
        printf("<unknown instruction>");
    }

    printf("\n");
}

void fird_print_blk(FirBlock *blk) {
    printf(".%.*s\n", fir_sym_fmt(blk->name));

    dynarr_foreach(blk->instrs, i) {
        fird_print_instr(dynarr_get(&blk->instrs, i));
    }
}

void fird_print_func(FirFunc *func) {
    printf("func @%.*s(", fir_sym_fmt(func->name));

    if (func->param_types.len > 0) {
        printf(" ");
    }

    dynarr_foreach(func->param_types, i) {
        FirType *type = dynarr_get_ref(&func->param_types, i);

        fird_print_type(type);

        printf(" ");
    }

    printf(") ");

    fird_print_type(&func->ret_type);

    printf("\n");

    dynarr_foreach(func->blks, i) {
        fird_print_blk(dynarr_get(&func->blks, i));
    }
}
