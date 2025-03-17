#include "fir.h"
#include <stdio.h>


void fir_instr_add(
    fir_Function *function,
    fir_Block *block,
    fir_Instruction *lhs,
    fir_Instruction *rhs
) {
    (void)lhs;
    (void)rhs;
    fir_Instruction *instr = fir_arena_alloc(&function->module->arena, sizeof(fir_Instruction));
    fir_array_append(fir_Instruction *, &block->instrs, instr);

}

void fir_instr_dump(fir_Instruction *instr, FILE *fp) {
    (void)instr;
    fprintf(fp, "<unhandled instruction>");
}
