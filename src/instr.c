#include "instr.h"


bool fir_instr_creates_new_register(FirInstr *instr) {
    return instr->kind != FirInstr_Mov;
}
