#include "fir_priv.h"
#include "symbol.h"
#include "arena.h"

#include <assert.h>


FirBuilder fir_builder_create(FirModule *module) {
    assert(module != NULL);

    FirBuilder builder;

    builder.module = module;
    builder.cur_func = NULL;
    builder.cur_blk = NULL;

    return builder;
}

void fir_builder_init_func(FirBuilder *firb, FirFunc *func) {
    assert(func != NULL);
    assert(func->blks.len == 0);

    FirBlock *entry = fir_blk_create_named(func, fir_sym_lit("entry"));

    fir_builder_set_insert_point(firb, entry);
}

void fir_builder_set_insert_point(FirBuilder *firb, FirBlock *blk) {
    assert(firb != NULL);
    assert(blk != NULL);

    firb->cur_func = fir_blk_get_parent(blk);
    firb->cur_blk = blk;
}

// instructions
static FirInstr *insert_instr(FirBuilder *firb, FirInstrKind kind, bool named) {
    assert(firb != NULL);

    FirInstr *instr = fir_arena_alloc_T(&firb->module->arena, FirInstr);
    instr->kind = kind;
    instr->name = fir_sym_none();
    if (named) {
        fir_sym_set_unique_instr_idx(&instr->name, firb->cur_func);
    }

    dynarr_push(&firb->cur_func->instrs, &instr);
    dynarr_push(&firb->cur_blk->instrs, &instr);


    return instr;
}

void fir_instr_mov(FirBuilder *firb, FirVal dst, FirVal src) {
    assert(firb != NULL);

    FirInstr *instr = insert_instr(firb, FirInstr_Mov, false);

    instr->args[0] = dst;
    instr->args[1] = src;
}

FirVal fir_instr_param(FirBuilder *firb, size_t i) {
    assert(firb != NULL);
    assert(firb->cur_func->param_types.len > 0);
    assert(i < firb->cur_func->param_types.len);

    FirInstr *instr = insert_instr(firb, FirInstr_Param, true);

    instr->type = dynarr_get(&firb->cur_func->param_types, i);
    instr->param = i;

    return fir_val_instr(instr);
}

FirVal fir_instr_add(FirBuilder *firb, FirType type, FirVal lhs, FirVal rhs) {
    assert(firb != NULL);
    assert(lhs.kind != FirVal_Invalid);
    assert(rhs.kind != FirVal_Invalid);

    FirInstr *instr = insert_instr(firb, FirInstr_Add, true);

    instr->type = type;

    instr->args[0] = lhs;
    instr->args[1] = rhs;

    return fir_val_instr(instr);
}


FirVal fir_instr_call(FirBuilder *firb, FirFunc *target, FirVal *args, size_t n_args) {
    assert(firb != NULL);
    assert(target != NULL);
    assert(target->param_types.len == n_args);

    FirInstr *instr = insert_instr(firb, FirInstr_Call, true);

    instr->call.name = target->name;
    dynarr_init(&instr->call.args, 32);
    instr->type = target->ret_type;
    for (size_t i = 0; i < n_args; i += 1) {
        FirCallArg arg = { dynarr_get(&target->param_types, i), args[i] };
        dynarr_push(&instr->call.args, &arg);
    }

    return fir_val_instr(instr);
}

FirVal fir_instr_icall(FirBuilder *firb, FirVal target, FirVal *args, size_t n_args) {
    (void)firb;
    (void)target;
    (void)args;
    (void)n_args;
    return fir_val_invalid();
}

// terminators
static FirTermi *insert_termi(FirBuilder *firb, FirTermiKind kind) {
    FirTermi *termi = fir_arena_alloc_T(&firb->module->arena, FirTermi);
    termi->kind = kind;

    firb->cur_blk->termi = termi;

    return termi;
}

void fir_instr_goto(FirBuilder *firb, FirBlock *to_blk) {
    assert(firb != NULL);
    assert(to_blk != NULL);
    assert(firb->cur_blk->termi == NULL);

    FirTermi *termi = insert_termi(firb, FirTermi_Goto);

    termi->_goto.blk = to_blk;
}

FirIfCtrl fir_instr_if(FirBuilder *firb, FirVal cond) {
    assert(firb != NULL);
    assert(firb->cur_blk->termi == NULL);

    FirTermi *termi = insert_termi(firb, FirTermi_If);

    FirFunc *func = firb->cur_func;
    FirBlock *blk = firb->cur_blk;

    termi->_if.cond = cond;
    termi->_if.then_blk = fir_blk_create_named(func, fir_sym_lit("if_then"));
    termi->_if.else_blk = fir_blk_create_named(func, fir_sym_lit("if_else"));
    FirBlock *join_blk  = fir_blk_create_named(func, fir_sym_lit("if_join"));

    // goto join
    fir_builder_set_insert_point(firb, termi->_if.then_blk);
    fir_instr_goto(firb, join_blk);

    fir_builder_set_insert_point(firb, termi->_if.else_blk);
    fir_instr_goto(firb, join_blk);

    fir_builder_set_insert_point(firb, blk);

    return (FirIfCtrl) {
        .then_blk = termi->_if.then_blk,
        .else_blk = termi->_if.else_blk,
        .join_blk = join_blk,
    };
}

void fir_instr_ret(FirBuilder *firb, FirVal ret_val) {
    assert(firb != NULL);
    assert(firb->cur_blk->termi == NULL);

    FirTermi *termi = insert_termi(firb, FirTermi_Ret);

    termi->ret.val = ret_val;
}

void fir_instr_ret_void(FirBuilder *firb) {
    fir_instr_ret(firb, fir_val_invalid());
}

