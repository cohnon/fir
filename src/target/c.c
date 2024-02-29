#include "c.h"

#include "fir_priv.h"

#include "dynarr.h"
#include "instr.h"


typedef struct FirTargetCtx {
    FirTarget *target;
    StringBuilder *output;
    FirModule *module;
} FirTargetCtx;

static uint16_t next_power_2(uint16_t n) {
    // is already a power of 2
    if ((n & (n-1)) == 0) {
        return n;
    }

    // count the number of bits so we can get the next power of 2
    // i.e. 00101 -> 01000,
    //      01110 -> 10000
    int count = 0;
    while (n != 0) {
        n >>= 1;
        count += 1;
    }

    return 1 << count;
}

static void cg_type(FirTargetCtx *ctx, FirType type) {
    assert(ctx != NULL);

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
            // make a power of two
            sb_printf(ctx->output, "int%d_t", next_power_2(type.data));
            break;
        }
        break;

    case FirType_Ptr:
        sb_print_lit(ctx->output, "void*");
        break;
    }
}

static void cg_imm(FirTargetCtx *ctx, FirImm *imm) {
    assert(ctx != NULL);
    assert(imm != NULL);

    switch (imm->type.kind) {
    case FirType_Int:
        if (imm->integer.is_signed) {
            sb_print_lit(ctx->output, "-");
        }
        sb_printf(ctx->output, "%llu", imm->integer.n);
        break;
    }
}

static void cg_reg(FirTargetCtx *ctx, FirInstr *reg) {
    sb_printf(ctx->output, "reg_%.*s%d", fir_sym_fmt(reg->name));
}

static void cg_instr_assignment(FirTargetCtx *ctx, FirInstr *instr) {
    cg_type(ctx, instr->type);
    sb_print_lit(ctx->output, " ");
    cg_reg(ctx, instr);
    sb_print_lit(ctx->output, " = ");
}

static void cg_val(FirTargetCtx *ctx, FirVal *val) {
    assert(ctx != NULL);
    assert(val != NULL);

    switch (val->kind) {
    case FirVal_Imm:
        cg_imm(ctx, val->imm);
        break;

    case FirVal_Reg:
        cg_reg(ctx, val->reg);
        break;

    default:
        sb_print_lit(ctx->output, "<unhandled val>");
        break;
    }
}

static void cg_instr(FirTargetCtx *ctx, FirInstr *instr) {
    assert(ctx != NULL);
    assert(instr != NULL);

    sb_print_lit(ctx->output, "\t");

    switch (instr->kind) {
    case FirInstr_Mov:
        assert(instr->args[0].kind == FirVal_Reg);
        cg_reg(ctx, instr->args[0].reg);
        sb_print_lit(ctx->output, " = ");
        cg_val(ctx, &instr->args[1]);
        break;

    case FirInstr_Param:
        cg_instr_assignment(ctx, instr);
        sb_printf(ctx->output, "param_%zu", instr->param);
        
        break;

    case FirInstr_Add:
        cg_instr_assignment(ctx, instr);
        cg_val(ctx, &instr->args[0]);
        sb_print_lit(ctx->output, " + ");
        cg_val(ctx, &instr->args[1]);
        break;

    case FirInstr_Call:
        cg_instr_assignment(ctx, instr);
        sb_printf(ctx->output, "%.*s(", fir_sym_fmt_raw(instr->call.name));
        dynarr_foreach(instr->call.args, i) {
            FirCallArg arg = dynarr_get(&instr->call.args, i);
            cg_val(ctx, &arg.val);

            if (i < instr->call.args.len - 1) {
                sb_print_lit(ctx->output, ", ");
            }
        }
        sb_print_lit(ctx->output, ")");
        break;

    default:
        sb_print_lit(ctx->output, "<unhandled instr>");
        break;
    }

    sb_print_lit(ctx->output, ";\n");
}

static void cg_termi(FirTargetCtx *ctx, FirTermi *termi) {
    assert(ctx != NULL);
    assert(termi != NULL);

    sb_print_lit(ctx->output, "\t");

    switch (termi->kind) {
    case FirTermi_Goto:
        sb_printf(ctx->output, "goto blk_%.*s%d;", fir_sym_fmt(termi->_goto.blk->name));
        break;

    case FirTermi_If:
        sb_printf(ctx->output, "if (");
        cg_val(ctx, &termi->_if.cond);
        sb_printf(ctx->output,
            ") { goto blk_%.*s%d; } else { goto blk_%.*s%d; }",
            fir_sym_fmt(termi->_if.then_blk->name),
            fir_sym_fmt(termi->_if.else_blk->name)
        );
        break;

    case FirTermi_Ret:
        sb_print_lit(ctx->output, "return");
        if (termi->ret.val.kind != FirVal_Invalid) {
            sb_print_lit(ctx->output, " ");
            cg_val(ctx, &termi->ret.val);
        }
        sb_print_lit(ctx->output, ";");
        break;

    default:
        sb_print_lit(ctx->output, "<unhandled termi>");
        break;
    }

    sb_print_lit(ctx->output, "\n");
}

static void cg_blk(FirTargetCtx *ctx, FirBlock *blk) {
    assert(ctx != NULL);
    assert(blk != NULL);

    sb_printf(ctx->output, "\tblk_%.*s%d:;\n", fir_sym_fmt(blk->name));

    dynarr_foreach(blk->instrs, i) {
        FirInstr *instr = dynarr_get(&blk->instrs, i);
        cg_instr(ctx, instr);
    }

    cg_termi(ctx, blk->termi);

    sb_print_lit(ctx->output, "\n");
}

static void cg_func_sig(FirTargetCtx *ctx, FirFunc *func) {
    assert(ctx != NULL);
    assert(func != NULL);

    cg_type(ctx, func->ret_type);
    sb_printf(ctx->output, " %.*s(", fir_sym_fmt_raw(func->name));

    if (func->param_types.len == 0) {
        sb_printf(ctx->output, "void");
    }

    dynarr_foreach(func->param_types, i) {
        FirType param_type = dynarr_get(&func->param_types, i);
        cg_type(ctx, param_type);
        sb_printf(ctx->output, " param_%zu", i);

        if (i < func->param_types.len - 1) {
            sb_print_lit(ctx->output, ", ");
        }
    }

    sb_print_lit(ctx->output, ")");
}

static void cg_func(FirTargetCtx *ctx, FirFunc *func) {
    assert(ctx != NULL);
    assert(func != NULL);

    cg_func_sig(ctx, func);
    sb_print_lit(ctx->output, " {\n");

    dynarr_foreach(func->blks, i) {
        FirBlock *blk = dynarr_get(&func->blks, i);
        cg_blk(ctx, blk);
    }

    sb_print_lit(ctx->output, "}\n\n");
}

static void cg_module(FirTargetCtx *ctx, FirModule *module) {
    assert(ctx != NULL);
    assert(module != NULL);

    dynarr_foreach(ctx->module->funcs, i) {
        FirFunc *func = dynarr_get(&ctx->module->funcs, i);
        if (func->blks.len == 0) {
            continue;
        }

        cg_func(ctx, func);
    }
}

static void cg_forward_decls(FirTargetCtx *ctx, FirModule *module) {
    assert(ctx != NULL);
    assert(module != NULL);

    dynarr_foreach(module->funcs, i) {
        FirFunc *func = dynarr_get(&module->funcs, i);
        cg_func_sig(ctx, func);
        sb_print_lit(ctx->output, ";\n");
    }
}

static void cg_prelude(FirTargetCtx *ctx) {
    sb_print_lit(ctx->output, "#include <stddef.h>\n");
    sb_print_lit(ctx->output, "#include <stdint.h>\n");
}

void target_c(FirTarget *target) {
    assert(target != NULL);
    assert(target->module != NULL);
    assert(!target->built && "target is already built");

    FirTargetCtx ctx;
    ctx.target = target;
    ctx.output = &target->output;
    ctx.module = target->module;

    sb_print_lit(ctx.output, "// == prelude ==\n");
    cg_prelude(&ctx);

    sb_print_lit(ctx.output, "\n// == forward decls ====\n");
    cg_forward_decls(&ctx, target->module);

    sb_print_lit(ctx.output, "\n// == definitions ====\n");
    cg_module(&ctx, target->module);

    target->built = true;
}
