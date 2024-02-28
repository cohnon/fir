#ifndef FIR_PRIV_H
#define FIR_PRIV_H

#include "fir.h"

#include "arena.h"
#include "dynarr.h"
#include "instr.h"
#include "string_builder.h"

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

// == target ================
typedef struct FirTarget {
    FirTargetKind kind;

    FirModule     *module;
    bool          built;

    StringBuilder output;
} FirTarget;

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
typedef struct FirImm {
    FirType type;

    union {
        struct {
            uint64_t n;
            bool     is_signed;
        } integer;
    };
} FirImm;

typedef enum FirGlobKind {
    FirGlob_Func,
} FirGlobKind;

typedef struct FirGlob {
    FirGlobKind kind;
    FirType     type;
    FirSym      name;
} FirGlob;

FirVal fir_val_instr(FirInstr *instr);

#endif
