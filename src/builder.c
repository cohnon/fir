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

FirBuilder *firb_get(FirModule *module) {
    assert(module != NULL);

    FirBuilder *firb = fir_arena_alloc_T(&module->arena, FirBuilder);

    firb->module = module;

    return firb;
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

    return (FirIfCtrl) {
        .then_blk = termi->_if.then_blk,
        .else_blk = termi->_if.else_blk,
        .join_blk = join_blk,
    };
}
