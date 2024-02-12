#include "fir.h"
#include "fir_priv.h"
#include "dynarr.h"
#include "arena.h"

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>


FirModule *fir_mod_create(void) {
    FirModule *module = malloc(sizeof(FirModule));

    dynarr_init(&module->funcs, 16);
    module->builder = firb_create(module);
    module->arena = fir_arena_init(FIR_ARENA_CHUNK_S);

    return module;
}

FirBuilder *fir_mod_get_builder(FirModule *module) {
    assert(module != NULL);

    return &module->builder;
}

FirFunc *fir_func_create(FirModule *module, FirSym name) {
    assert(module != NULL);

    FirFunc *func = fir_arena_alloc_T(&module->arena, FirFunc);
    dynarr_push(&module->funcs, &func);

    func->name = name;
    func->parent = module;

    dynarr_init(&func->param_types, 16);
    dynarr_init(&func->blks,        32);

    fir_blk_create_named(func, fir_sym_lit("entry"));

    return func;
}

void fir_func_set_signature(FirFunc *func, FirType ret_type, FirType *param_types, size_t param_types_len) {
    assert(func != NULL);

    func->ret_type = ret_type;

    for (size_t i = 0; i < param_types_len; i += 1) {
        dynarr_push(&func->param_types, &param_types[i]);
    }
}

FirBlock *fir_func_get_entry_blk(FirFunc *func) {
    assert(func != NULL);

    return dynarr_get(&func->blks, 0);
}

FirBlock *fir_blk_create(FirFunc *func) {
    assert(func != NULL);

    return fir_blk_create_named(func, fir_sym_none());
}
#include <stdio.h>
FirBlock *fir_blk_create_named(FirFunc *func, FirSym name) {
    assert(func != NULL);

    FirBlock *blk = fir_arena_alloc_T(&func->parent->arena, FirBlock);
    dynarr_push(&func->blks, &blk);

    blk->name = name;
    blk->parent = func;

    dynarr_init(&blk->instrs, 32);
    blk->termi = NULL;

    dynarr_init(&blk->preds, 8);
    dynarr_init(&blk->succs, 4);

    return blk;
}

FirFunc *fir_blk_get_parent(FirBlock *blk) {
    assert(blk != NULL);

    return blk->parent;
}

FirVal fir_val_invalid(void) {
    return (FirVal) { .kind = FirVal_Invalid };
}

FirVal fir_val_instr(FirInstr *instr) {
    assert(instr != NULL);

    return (FirVal) {
        .kind = FirVal_Reg,
        .reg = instr,
    };
}

FirVal fir_imm_int(FirModule *module, unsigned long long n, bool is_signed) {
    assert(module != NULL);

    FirVal val;
    val.kind = FirVal_Imm;

    FirImm *imm = fir_arena_alloc_T(&module->arena, FirImm);
    imm->kind = FirImm_Int;
    imm->integer.n = n;
    imm->integer.is_signed = is_signed;

    val.imm = imm;

    return val;
}

