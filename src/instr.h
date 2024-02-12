#ifndef FIR_INSTR_H
#define FIR_INSTR_H

#include "fir.h"
#include "dynarr.h"
#include <stdbool.h>


//
// the stuff that go in basic blocks
//
// FirInstr: a 3-address instruction. these are all in ssa form with the
//           exception of 'mov' which overwrite an existing register.
//
// FirTermi: an terminating instruction that's inserted at the end of every basic block.
//


// == 3-addr instruction ====
typedef enum FirInstrKind {
    FirInstr_Invalid,

    // memory
    FirInstr_Mov,       // <arg0> = <arg1>
    FirInstr_Phi,       // %rx = phi( <blk0> <val0>, <blk1> <val1>, ... )

    // arithmetic
    FirInstr_Add,       // %rx = add <ty> <arg0> <arg1>
    
    // misc
    FirInstr_Call,      // %rx = call <ret ty> <name>( <ty0> <val0>, <ty1> <val1>, ... )

} FirInstrKind;

typedef struct FirCallArg {
    FirType type;
    FirVal  val;
} FirCallArg;

typedef struct FirCall {
    DynArr(FirCallArg) args;
} FirCall;

typedef struct FirPhiArg {
    FirVal   *val;
    FirBlock *block;
} FirPhiArg;

typedef struct FirPhi {
    DynArr(FirPhiArg) args;
} FirPhi;

typedef struct FirInstr FirInstr;
typedef struct FirInstr {
    FirInstrKind kind;
    FirSym       name;
    int          id;

    FirType  type;
    FirVal   args[2];

    // extended instructions
    union {
        FirCall call;
        FirPhi  phi;
    };
} FirInstr;

// == terminator ============
typedef enum FirTermiKind {
    FirTermi_Jmp,
    FirTermi_If,
} FirTermiKind;

typedef struct FirTermiIf {
    FirVal   cond;
    FirBlock *then_blk;
    FirBlock *else_blk;
} FirTermiIf;

typedef struct FirTermi {
    FirTermiKind kind;

    union {
        FirBlock  *jmp;
        FirTermiIf _if;
    };
} FirTermi;

bool fir_instr_creates_new_register(FirInstr *instr);

#endif
