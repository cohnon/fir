#ifndef FIR_DEBUG_H
#define FIR_DEBUG_H

#include "fir.h"


void fir_print_module(FirModule *module);
void fir_print_func(FirFunc *func);
void fir_print_blk(FirBlock *blk);

void fir_print_target(FirTarget *target);

#endif
