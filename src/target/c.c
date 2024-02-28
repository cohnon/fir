#include "c.h"

#include "fir_priv.h"

#include "dynarr.h"


typedef struct FirTargetCtx {
    FirTarget *target;
    StringBuilder *output;
    FirModule *module;
} FirTargetCtx;

static void cg_type(FirTargetCtx *ctx, FirType type) {
    switch (type.kind) {
    case FirType_Int:
        switch (type.data) {
        case 0:
            sb_print_lit(ctx->output, "void");
            break;

        case 1:
            sb_print_lit(ctx->output, "bool");
            break;

        default:
            sb_printf(ctx->output, "uint%d_t", type.data);
            break;
        }
        break;

    case FirType_Ptr:
        sb_print_lit(ctx->output, "void*");
        break;
    }
}

static void cg_imm(FirTargetCtx *ctx, FirImm *imm) {
    switch (imm->type.kind) {
    case FirType_Int:
        sb_printf(ctx->output, "%llu", imm->integer.n);
        break;
    }
}

static void cg_val(FirTargetCtx *ctx, FirVal *val) {
    switch (val->kind) {
    case FirVal_Imm:
        cg_imm(ctx, val->imm);
        break;

    default:
        sb_print_lit(ctx->output, "<unhandled val>");
        break;
    }
}

static void cg_instr(FirTargetCtx *ctx, FirInstr *instr) {
    sb_print_lit(ctx->output, "\t");

    switch (instr->kind) {
    case FirInstr_Add:
        cg_type(ctx, instr->type);
        sb_printf(ctx->output, " reg_%.*s%d = ", fir_sym_fmt(instr->name), instr->name.unique_id);
        cg_val(ctx, &instr->args[0]);
        sb_print_lit(ctx->output, " + ");
        cg_val(ctx, &instr->args[1]);
        break;

    default:
        sb_print_lit(ctx->output, "<unhandled instr>");
        break;
    }

    sb_print_lit(ctx->output, ";\n");
}

static void cg_blk(FirTargetCtx *ctx, FirBlock *blk) {
    (void)blk;

    sb_printf(ctx->output, "\t%.*s:\n", fir_sym_fmt(blk->name));

    dynarr_foreach(blk->instrs, i) {
        FirInstr *instr = dynarr_get(&blk->instrs, i);
        cg_instr(ctx, instr);
    }

    sb_print_lit(ctx->output, "\n");
}

static void cg_func(FirTargetCtx *ctx, FirFunc *func) {
    cg_type(ctx, func->ret_type);
    sb_printf(ctx->output, " %.*s(", fir_sym_fmt(func->name));

    dynarr_foreach(func->param_types, i) {
        FirType param_type = dynarr_get(&func->param_types, i);
        cg_type(ctx, param_type);

        if (i < func->param_types.len - 1) {
            sb_print_lit(ctx->output, ", ");
        }
    }

    sb_print_lit(ctx->output, ") {\n");

    dynarr_foreach(func->blks, i) {
        FirBlock *blk = dynarr_get(&func->blks, i);
        cg_blk(ctx, blk);
    }

    sb_print_lit(ctx->output, "}\n\n");
}

void target_c(FirTarget *target) {
    assert(target != NULL);
    assert(target->module != NULL);
    assert(!target->built && "target is already built");

    FirTargetCtx ctx;
    ctx.target = target;
    ctx.output = &target->output;
    ctx.module = target->module;

    dynarr_foreach(ctx.module->funcs, i) {
        FirFunc *func = dynarr_get(&ctx.module->funcs, i);
        cg_func(&ctx, func);
    }

    target->built = true;
}
