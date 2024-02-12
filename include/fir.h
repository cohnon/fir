#ifndef FIR_H
#define FIR_H

#include <stddef.h>
#include <stdint.h>


typedef struct FirModule  FirModule;
typedef struct FirFunc    FirFunc;
typedef struct FirBlock   FirBlock;
typedef struct FirVal     FirVal;
typedef struct FirType    FirType;
typedef struct FirSym     FirSym;
typedef struct FirBuilder FirBuilder;

// == FirModule ==============
FirModule *fir_mod_create(void);

FirBuilder *fir_mod_get_builder(FirModule *module);

// == FirFunc ================
FirFunc *fir_func_create(FirModule *module, FirSym name);

void fir_func_set_signature(FirFunc *func, FirType ret_type, size_t n_params, FirType *param_types);

FirBlock *fir_func_get_entry_blk(FirFunc *func);

// == FirBlock ===============
FirBlock *fir_blk_create(FirFunc *func);
FirBlock *fir_blk_create_named(FirFunc *func, FirSym name);

FirFunc *fir_blk_get_parent(FirBlock *blk);

// == FirVal =================
typedef struct FirInstr FirInstr;
typedef struct FirImm   FirImm;

typedef enum FirValKind {
    FirVal_Invalid,
    FirVal_Reg,
    FirVal_Imm,
    FirVal_Glob,
} FirValKind;

typedef struct FirVal {
    FirValKind kind;
    
    union {
        FirInstr *reg;
        FirImm   *imm;
    };
} FirVal;

FirVal fir_val_invalid(void);
FirVal fir_imm_int(FirModule *module, unsigned long long n, _Bool is_signed);

// == FirType ================
typedef enum FirTypeKind {
    // integers with an arbitrary bit width represented by iN where N is the
    // number of bits from 1 to 64 (2^6)
    // NOTE: void is an i0 and bool is an i1
    // TODO: increase to i128 (maybe more?) using software arithmetic
    FirType_Int,

} FirTypeKind;

typedef struct FirType {
    FirTypeKind kind;
    union {
        uint8_t bits: 6;
    };
} FirType;

FirType fir_ty_int(size_t bits);
FirType fir_ty_void(void);
FirType fir_ty_bool(void);

// == FirSym =================
typedef struct FirSym {
    const char *ptr;
    uint16_t   len;
    uint16_t   unique_id;
} FirSym;

FirSym fir_sym_none(void);
#define fir_sym_lit(c_str) (FirSym){ .ptr = c_str, .len = sizeof(c_str) - 1 }
FirSym fir_sym_slc(char *c_str, size_t len);

_Bool fir_sym_eq(FirSym a, FirSym b);

#define fir_sym_fmt(sym) (int)(sym).len, (sym).ptr

// == FirBuilder =============
void firb_set_insert_point(FirBuilder *firb, FirBlock *blk);

// FirBuilder > instructions
// memory
void firb_mov(FirBuilder *firb, FirVal dst, FirVal src);
// arithmetic
FirVal firb_add(FirBuilder *firb, FirType type, FirVal lhs, FirVal rhs);
// terminators
typedef struct FirIfCtrl {
    FirBlock *then_blk;
    FirBlock *else_blk;
    FirBlock *join_blk;
} FirIfCtrl;
FirIfCtrl firb_if(FirBuilder *firb, FirVal cond);

#endif
