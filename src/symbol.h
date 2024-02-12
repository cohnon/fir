#ifndef FIR_SYMBOL_H
#define FIR_SYMBOL_H

#include "fir.h"

void fir_sym_set_unique_blk_idx(FirSym *sym, FirFunc *func);
void fir_sym_set_unique_instr_idx(FirSym *sym, FirFunc *func);

#endif
