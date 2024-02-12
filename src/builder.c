#include "fir_priv.h"
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

FirVal firb_add(FirBuilder *firb, FirType type, FirVal lhs, FirVal rhs) {
    assert(firb != NULL);

    FirFunc *func = firb->cur_func;
    FirBlock *blk = firb->cur_blk;

    FirInstr *instr = fir_arena_alloc_T(&func->parent->arena, FirInstr);
    dynarr_push(&blk->instrs, &instr);

    instr->kind = FirInstr_Add;
    instr->name = fir_sym_none();

    instr->type = type;

    instr->args[0] = lhs;
    instr->args[1] = rhs;

    return fir_val_instr(instr);
}
