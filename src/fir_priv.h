#ifndef FIR_PRIV_H
#define FIR_PRIV_H

#include "fir.h"
#include "instr.h"
#include "dynarr.h"
#include "arena.h"

#include <stdbool.h>
#include <stdint.h>


// == builder ================
typedef struct FirBuilder {
    FirModule *module;

    FirFunc   *cur_func;
    FirBlock  *cur_blk;
} FirBuilder;

FirBuilder firb_create(FirModule *module);

// == modules ===============
typedef struct FirModule {
    DynArr(FirFunc*) funcs;

    FirBuilder       builder;
    FirArena         arena;
} FirModule;

// == functions =============
typedef struct FirFunc {
    FirSym            name;
    FirModule         *parent;

    DynArr(FirType)   param_types;
    FirType           ret_type;

    DynArr(FirBlock*) blks;
    DynArr(FirInstr*) instrs;
} FirFunc;

// == blocks ================
typedef struct FirBlock {
    FirSym            name;
    FirFunc           *parent;

    DynArr(FirInstr*) instrs;
    FirTermi          *termi;

    DynArr(FirBlock*) preds;
    DynArr(FirBlock*) succs;
} FirBlock;

// == fir values ============
typedef enum FirImmKind {
    FirImm_Int,
} FirImmKind;
typedef struct FirImm {
    FirImmKind kind;
    union {
        struct {
            uint64_t n;
            bool     is_signed;
        } integer;
    };
} FirImm;

FirVal fir_val_instr(FirInstr *instr);

#endif
