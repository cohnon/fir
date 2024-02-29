#include "fir_priv.h"
#include "symbol.h"
#include "arena.h"

#include <assert.h>


FirBuilder firb_create(FirModule *module) {
    assert(module != NULL);

    FirBuilder builder;

    builder.module = module;
    builder.cur_func = NULL;
    builder.cur_blk = NULL;

    return builder;
}

void firb_init_func(FirBuilder *firb, FirFunc *func) {
    assert(func != NULL);
    assert(func->blks.len == 0);

    FirBlock *entry = fir_blk_create_named(func, fir_sym_lit("entry"));

    firb_set_insert_point(firb, entry);
}

void firb_set_insert_point(FirBuilder *firb, FirBlock *blk) {
    assert(firb != NULL);
    assert(blk != NULL);

    firb->cur_func = fir_blk_get_parent(blk);
    firb->cur_blk = blk;
}

void firb_mov(FirBuilder *firb, FirVal dst, FirVal src) {
    assert(firb != NULL);

    FirFunc *func = firb->cur_func;
    FirBlock *blk = firb->cur_blk;

    FirInstr *instr = fir_arena_alloc_T(&func->parent->arena, FirInstr);
    dynarr_push(&func->instrs, &instr);
    dynarr_push(&blk->instrs, &instr);

    instr->kind = FirInstr_Mov;
    instr->name = fir_sym_none();

    instr->args[0] = dst;
    instr->args[1] = src;
}

FirVal firb_add(FirBuilder *firb, FirType type, FirVal lhs, FirVal rhs) {
    assert(firb != NULL);

    FirFunc *func = firb->cur_func;
    FirBlock *blk = firb->cur_blk;

    FirSym name = fir_sym_none();
    fir_sym_set_unique_instr_idx(&name, func);

    FirInstr *instr = fir_arena_alloc_T(&func->parent->arena, FirInstr);
    dynarr_push(&func->instrs, &instr);
    dynarr_push(&blk->instrs, &instr);

    instr->kind = FirInstr_Add;
    instr->name = name;

    instr->type = type;

    instr->args[0] = lhs;
    instr->args[1] = rhs;

    return fir_val_instr(instr);
}


FirVal firb_call(FirBuilder *firb, FirFunc *target, FirVal *args, size_t n_args) {
    assert(firb != NULL);
    assert(target != NULL);
    assert(target->param_types.len == n_args);

    FirFunc *func = firb->cur_func;
    FirBlock *blk = firb->cur_blk;

    FirSym name = fir_sym_none();
    fir_sym_set_unique_instr_idx(&name, func);

    FirInstr *instr = fir_arena_alloc_T(&func->parent->arena, FirInstr);
    dynarr_push(&func->instrs, &instr);
    dynarr_push(&blk->instrs, &instr);

    instr->kind = FirInstr_Call;
    instr->name = name;

    instr->call.name = target->name;
    instr->call.ret_type = target->ret_type;
    for (size_t i = 0; i < n_args; i += 1) {
        FirCallArg arg = { dynarr_get(&target->param_types, i), args[i] };
        dynarr_push(&instr->call.args, &arg);
    }

    return fir_val_instr(instr);
}

FirVal firb_icall(FirBuilder *firb, FirVal target, FirVal *args, size_t n_args) {
    (void)firb;
    (void)target;
    (void)args;
    (void)n_args;
    return fir_val_invalid();
}

void firb_goto(FirBuilder *firb, FirBlock *to_blk) {
    assert(firb != NULL);
    assert(to_blk != NULL);
    assert(firb->cur_blk->termi == NULL);

    FirFunc *func = firb->cur_func;
    FirBlock *blk = firb->cur_blk;

    FirTermi *termi = fir_arena_alloc_T(&func->parent->arena, FirTermi);
    blk->termi = termi;

    termi->kind = FirTermi_Goto;
    termi->_goto.blk = to_blk;
}

FirIfCtrl firb_if(FirBuilder *firb, FirVal cond) {
    assert(firb != NULL);
    assert(firb->cur_blk->termi == NULL);

    FirFunc *func = firb->cur_func;
    FirBlock *blk = firb->cur_blk;
    
    FirTermi *termi = fir_arena_alloc_T(&func->parent->arena, FirTermi);
    blk->termi = termi;

    termi->kind = FirTermi_If;
    termi->_if.cond = cond;
    termi->_if.then_blk = fir_blk_create_named(func, fir_sym_lit("if_then"));
    termi->_if.else_blk = fir_blk_create_named(func, fir_sym_lit("if_else"));
    FirBlock *join_blk  = fir_blk_create_named(func, fir_sym_lit("if_join"));

    // goto join
    firb_set_insert_point(firb, termi->_if.then_blk);
    firb_goto(firb, join_blk);

    firb_set_insert_point(firb, termi->_if.else_blk);
    firb_goto(firb, join_blk);

    firb_set_insert_point(firb, blk);

    return (FirIfCtrl) {
        .then_blk = termi->_if.then_blk,
        .else_blk = termi->_if.else_blk,
        .join_blk = join_blk,
    };
}

void firb_ret(FirBuilder *firb, FirVal ret_val) {
    assert(firb != NULL);
    assert(firb->cur_blk->termi == NULL);

    FirFunc *func = firb->cur_func;
    FirBlock *blk = firb->cur_blk;

    FirTermi *termi = fir_arena_alloc_T(&func->parent->arena, FirTermi);
    blk->termi = termi;

    termi->kind = FirTermi_Ret;
    termi->ret.val = ret_val;
}

void firb_ret_void(FirBuilder *firb) {
    firb_ret(firb, fir_val_invalid());
}

