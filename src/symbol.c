#include "symbol.h"

#include "fir_priv.h"
#include <string.h>

// TODO: generate an id for each string (separate to uniqe_id which is just
//       for debuggins) for fast comparisons
FirSym fir_sym_none(void) {
    return (FirSym){ 0 };
}

FirSym fir_sym_slc(char *c_str, size_t len) {
    return (FirSym){ .ptr = c_str, .len = len, .unique_id = 0 };
}

_Bool fir_sym_eq(FirSym a, FirSym b) {
    if (a.len != b.len) { return false; }

    return strncmp(a.ptr, b.ptr, a.len) == 0;
}

void fir_sym_set_unique_blk_idx(FirSym *sym, FirFunc *func) {
    int next_id = 0;
    dynarr_foreach(func->blks, i) {
        FirBlock *blk = dynarr_get(&func->blks, i);
        if (fir_sym_eq(*sym, blk->name) && blk->name.unique_id >= next_id) {
            next_id = blk->name.unique_id + 1;
        }
    }

    sym->unique_id = next_id;
}

void fir_sym_set_unique_instr_idx(FirSym *sym, FirFunc *func) {
    int next_id = 0;
    dynarr_foreach(func->instrs, i) {
        FirInstr *instr = dynarr_get(&func->instrs, i);
        if (fir_sym_eq(*sym, instr->name) && instr->name.unique_id >= next_id) {
            next_id = instr->name.unique_id + 1;
        }
    }

    sym->unique_id = next_id;
}
