#include "fir.h"
#include "fir_priv.h"

#include "arena.h"
#include "dynarr.h"
#include "symbol.h"
#include "target/c.h"

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


FirModule *fir_mod_create(void) {
    FirModule *module = malloc(sizeof(FirModule));
    assert(module != NULL);

    dynarr_init(&module->funcs, 16);
    module->builder = fir_builder_create(module);
    module->arena = fir_arena_init(FIR_ARENA_CHUNK_S);

    return module;
}

FirBuilder *fir_mod_get_builder(FirModule *module) {
    assert(module != NULL);

    return &module->builder;
}

FirTarget *fir_target_create(FirModule *module, FirTargetKind kind) {
    assert(module != NULL);

    FirTarget *target = malloc(sizeof(FirTarget));
    assert(target != NULL);

    target->kind = kind;

    target->module = module;
    target->built  = false;

    target->output = sb_init(128);

    return target;
}

void fir_target_build(FirTarget *target) {
    assert(target != NULL);
    assert(!target->built);

    switch (target->kind) {
    case FirTarget_C: target_c(target); break;
    }
}

void fir_target_to_file(FirTarget *target, const char *path) {
    assert(target != NULL);
    assert(path != NULL);

    FILE *out_file = fopen(path, "wb");
    assert(out_file != NULL);

    fwrite(target->output.buf, target->output.len, 1, out_file);

    fclose(out_file);
}

FirFunc *fir_func_create(FirModule *module, FirSym name, FirType ret_ty, FirType *param_types, size_t n_params) {
    assert(module != NULL);

    FirFunc *func = fir_arena_alloc_T(&module->arena, FirFunc);
    dynarr_push(&module->funcs, &func);

    func->name = name;
    func->parent = module;

    dynarr_init(&func->param_types, 16);
    dynarr_init(&func->blks,        32);
    dynarr_init(&func->instrs,      32);

    func->ret_type = ret_ty;
    for (size_t i = 0; i < n_params; i += 1) {
        assert(!fir_is_void(param_types[i]));

        dynarr_push(&func->param_types, &param_types[i]);
    }

    return func;
}

FirBlock *fir_func_get_entry_blk(FirFunc *func) {
    assert(func != NULL);

    return dynarr_get(&func->blks, 0);
}

FirBlock *fir_blk_create(FirFunc *func) {
    assert(func != NULL);

    return fir_blk_create_named(func, fir_sym_none());
}

FirBlock *fir_blk_create_named(FirFunc *func, FirSym name) {
    assert(func != NULL);

    fir_sym_set_unique_blk_idx(&name, func);

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

FirVal fir_imm_str(FirModule *module, FirSym str, bool zero_terminated) {
    assert(module != NULL);

    FirVal val;
    val.kind = FirVal_Imm;

    FirImm *imm = fir_arena_alloc_T(&module->arena, FirImm);
    imm->kind = FirImm_Str;
    imm->string.str = str;
    imm->string.zero_terminated = zero_terminated;

    val.imm = imm;

    return val;
}

static bool fir_verify_blk(FirBlock *blk) {
    if (blk->termi == NULL) {
        return false;
    }

    return true;
}

static bool fir_verify_func(FirFunc *func) {
    dynarr_foreach(func->blks, i) {
        FirBlock *blk = dynarr_get(&func->blks, i);
        if (!fir_verify_blk(blk)) {
            return false;
        }
    }

    return true;
}

bool fir_verify_module(FirModule *module) {
    dynarr_foreach(module->funcs, i) {
        FirFunc *func = dynarr_get(&module->funcs, i);
        if (!fir_verify_func(func)) {
            return false;
        }
    }

    return true;
}
