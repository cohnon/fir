#ifndef FIR_SYMBOL_H
#define FIR_SYMBOL_H

#include "fir.h"

void fir_string_set_unique_blk_idx(FirString *sym, FirFunc *func);
void fir_string_set_unique_instr_idx(FirString *sym, FirFunc *func);

#endif
