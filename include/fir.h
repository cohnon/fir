#ifndef FIR_H
#define FIR_H

#include <stddef.h>
#include <stdint.h>
#include <assert.h>


typedef struct FirModule  FirModule;
typedef struct FirTarget  FirTarget;
typedef struct FirFunc    FirFunc;
typedef struct FirBlock   FirBlock;
typedef struct FirVal     FirVal;
typedef struct FirType    FirType;
typedef struct FirSym     FirSym;
typedef struct FirBuilder FirBuilder;

// == FirModule ==============
FirModule *fir_mod_create(void);

FirBuilder *fir_mod_get_builder(FirModule *module);

// == FirTarget ==============
typedef enum FirTargetKind {
    FirTarget_C,
} FirTargetKind;

FirTarget *fir_target_create(FirModule *module, FirTargetKind kind);
void fir_target_build(FirTarget *target);
char *fir_target_get_str_buf(FirTarget *target);
void fir_target_to_file(FirTarget *target, const char *path);

// == FirFunc ================
FirFunc *fir_func_create(FirModule *module, FirSym name, FirType ret_ty, FirType *param_types, size_t n_params);

FirBlock *fir_func_get_entry_blk(FirFunc *func);

// == FirBlock ===============
FirBlock *fir_blk_create(FirFunc *func);
FirBlock *fir_blk_create_named(FirFunc *func, FirSym name);

FirFunc *fir_blk_get_parent(FirBlock *blk);

// == FirVal =================
typedef struct FirInstr FirInstr;
typedef struct FirImm   FirImm;
typedef struct FirGlob  FirGlob;

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
        FirGlob  *glob;
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

    // a type that points to an address in memory (e.g. functions, globals)
    FirType_Ptr,

} FirTypeKind;

typedef struct FirType {
    uint16_t kind: 4;
    uint16_t data: 12;
} FirType;
static_assert(sizeof(FirType) == 2, "expected 'FirType' to be of size 2");

#define fir_ty_int(bits) (FirType){ FirType_Int, bits }
#define fir_ty_void()    (FirType){ FirType_Int, 0    }
#define fir_ty_bool()    (FirType){ FirType_Int, 1    }
#define fir_ty_ptr()     (FirType){ FirType_Ptr, 0    }

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

#define fir_sym_fmt(sym) (int)(sym).len, (sym).ptr, (sym).unique_id
#define fir_sym_fmt_raw(sym) (int)(sym).len, (sym).ptr

// == FirBuilder =============
void firb_init_func(FirBuilder *firb, FirFunc *func);
void firb_set_insert_point(FirBuilder *firb, FirBlock *blk);

// FirBuilder > instructions
// memory
void firb_mov(FirBuilder *firb, FirVal dst, FirVal src);
// arithmetic
FirVal firb_add(FirBuilder *firb, FirType type, FirVal lhs, FirVal rhs);
// misc
FirVal firb_call(FirBuilder *firb, FirFunc *target, FirVal *args, size_t n_args);
FirVal firb_icall(FirBuilder *firb, FirVal target, FirVal *args, size_t n_args);
// terminators
void firb_jmp(FirBuilder *firb, FirBlock *to_blk);
typedef struct FirIfCtrl {
    FirBlock *then_blk;
    FirBlock *else_blk;
    FirBlock *join_blk;
} FirIfCtrl;
FirIfCtrl firb_if(FirBuilder *firb, FirVal cond);
void firb_ret(FirBuilder *firb, FirVal ret_val);
void firb_ret_void(FirBuilder *firb);

// == passes =================
_Bool fir_verify_module(FirModule *module);

#endif
