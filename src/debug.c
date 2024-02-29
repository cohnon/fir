#include "debug.h"

#include "fir_priv.h"

#include "stdio.h"

static void fird_print_type(FirType *type) {
    switch (type->kind) {
    case FirType_Int:
        if (type->data == 0) {
            printf("void");
        } else if (type->data == 1) {
            printf("bool");
        } else {
            printf("i%d", type->data);
        }

        break;

    case FirType_Ptr:
        printf("ptr");

        break;

    default: printf("<unhandled type>");
    }
}

static void fird_print_label(FirBlock *blk) {
    if (blk->name.len == 0) {
        printf("bb");
    } else {
        printf("%.*s%d", fir_sym_fmt(blk->name));
    }
}

static void fird_print_register(FirInstr *instr) {
    printf("%%%.*s%d", fir_sym_fmt(instr->name));
}

static void fird_print_val(FirVal *val) {
    switch (val->kind) {
    case FirVal_Reg:
        fird_print_register(val->reg);
        break;

    case FirVal_Imm:
        switch (val->imm->type.kind) {
        case FirType_Int:
            if (val->imm->integer.is_signed) {
                printf("-");
            }
            printf("%llu", val->imm->integer.n);

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
    printf("    ");

    if (fir_instr_creates_new_register(instr)) {
        fird_print_register(instr);
        printf(" = ");
    }

    switch (instr->kind) {
    case FirInstr_Mov:
        fird_print_val(&instr->args[0]);
        printf(" = ");
        fird_print_val(&instr->args[1]);

        break;
    
    case FirInstr_Param:
        printf("param %zu", instr->param);
        break;

    case FirInstr_Add:
        printf("add ");
        fird_print_type(&instr->type);
        printf(" ");
        fird_print_args(instr);
        
        break;

    case FirInstr_Call:
        printf("call ");
        fird_print_type(&instr->call.ret_type);
        printf(" @%.*s(", fir_sym_fmt_raw(instr->call.name));
        dynarr_foreach(instr->call.args, i) {
            FirCallArg *arg = dynarr_get_ref(&instr->call.args, i);
            fird_print_type(&arg->type);
            printf(" ");
            fird_print_val(&arg->val);

            if (i < instr->call.args.len - 1) {
                printf(", ");
            }
        }
        printf(")");

        break;

    default:
        printf("<unknown instruction>");
    }

    printf("\n");
}

static void fird_print_termi(FirTermi *termi) {
    printf("    ");

    // TODO: termi should never be NULL
    if (termi == NULL) {
        printf("<invalid terminator>\n");
        return;
    }

    switch (termi->kind) {
    case FirTermi_Goto:
        printf("goto ");
        fird_print_label(termi->_goto.blk);

        break;

    case FirTermi_If:
        printf("if ");
        fird_print_val(&termi->_if.cond);
        printf(" ");
        fird_print_label(termi->_if.then_blk);
        printf(" ");
        fird_print_label(termi->_if.else_blk);

        break;

    case FirTermi_Ret:
        printf("ret ");
        if (termi->ret.val.kind != FirVal_Invalid) {
            fird_print_val(&termi->ret.val);
        }

        break;

    default:
        printf("<unknown terminator>");
    }

    printf("\n");
}

void fird_print_blk(FirBlock *blk) {
    printf("  ");
    fird_print_label(blk);
    printf(":\n");

    dynarr_foreach(blk->instrs, i) {
        fird_print_instr(dynarr_get(&blk->instrs, i));
    }

    fird_print_termi(blk->termi);

    printf("\n");
}

void fird_print_func(FirFunc *func) {
    printf("func @%.*s(", fir_sym_fmt_raw(func->name));

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

void fird_print_module(FirModule *module) {
    dynarr_foreach(module->funcs, i) {
        fird_print_func(dynarr_get(&module->funcs, i));
    }
}

void fird_print_target(FirTarget *target) {
    assert(target->built && "cannot print unbuilt target");

    printf("%.*s", (int)target->output.len, target->output.buf);
}

