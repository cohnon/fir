#include "symbol.h"

#include "fir_priv.h"

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
