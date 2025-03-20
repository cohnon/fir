#ifndef FIR_H
#define FIR_H

#include "fir_util.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

typedef struct fir_Module fir_Module;
typedef struct fir_Block fir_Block;
typedef struct fir_Instr fir_Instr;

// String
typedef uint32_t fir_StringID;
#define FIR_STRING_INVALID (UINT32_MAX)
// TODO: back with hashmap
typedef fir_Array fir_StringTable;

fir_StringID fir_string_add(fir_Module *module, const char *c_str);
char *fir_string_get(fir_Module *module, fir_StringID string);

// Type
typedef enum {
    FIR_TYPE_INT,
    FIR_TYPE_FLOAT,
    FIR_TYPE_PTR,
    FIR_TYPE_TUPLE,
} fir_DataTypeKind;

typedef struct fir_DataType {
    fir_DataTypeKind kind : 4;
    uint16_t data : 12;
} fir_DataType;

bool fir_type_eq(fir_DataType a, fir_DataType b);

fir_DataType fir_type_void(void);
fir_DataType fir_type_int(size_t bits);
fir_DataType fir_type_float(size_t bits);
fir_DataType fir_type_tuple(size_t n);

bool fir_type_is_void(fir_DataType type);

void fir_type_dump(fir_DataType type, FILE *fp);


// Module
#define FIR_MODULE_NAME_MAX_LEN 127

struct fir_Module {
    char name[FIR_MODULE_NAME_MAX_LEN + 1];

    fir_Arena arena;
    fir_StringTable string_table;
    fir_Array funcs; // fir_Function *
};

fir_Module fir_module_init(const char *name);
void fir_module_deinit(fir_Module *module);

void fir_module_dump(fir_Module *module, FILE *fp);

// Function
typedef struct fir_Function {
    fir_Module *module;

    fir_StringID name;
    fir_Array blocks; // fir_Block *

    fir_Array input;  // fir_DataType
    fir_Array output; // fir_DataType
} fir_Function;

fir_Function *fir_func_create(fir_Module *module, const char *name);
void fir_func_destroy(fir_Function *func);

fir_Block *fir_func_get_entry(fir_Function *func);

void fir_func_add_input(fir_Function *func, fir_DataType type);
void fir_func_add_output(fir_Function *func, fir_DataType type);

void fir_func_dump(fir_Function *func, FILE *fp);

// Block
struct fir_Block {
    fir_Function *func;

    fir_StringID name;

    fir_Array input; // fir_DataType
    fir_Array instrs; // fir_Instr *
};

fir_Block *fir_block_create(fir_Function *func, const char *name);
void fir_block_destroy(fir_Block *block);

void fir_block_add_input(fir_Block *block, fir_DataType type);
fir_Instr *fir_block_get_terminator(fir_Block *block);

void fir_block_dump(fir_Function *func, fir_Block *block, FILE *fp);

// Instruction
typedef enum {
    FIR_INSTR_NOP,

    // values
    FIR_INSTR_LIT,
    FIR_INSTR_ARG,

    // arithmetic
    FIR_INSTR_ADD,
    FIR_INSTR_SUB,
    FIR_INSTR_MUL,
    FIR_INSTR_IDIV,
    FIR_INSTR_UDIV,
    FIR_INSTR_FDIV,

    FIR_INSTR_SHL,
    FIR_INSTR_SHR,


    FIR_INSTR_EQL,
    FIR_INSTR_NEQ,

    // terminators
    FIR_INSTR_JMP,
    FIR_INSTR_IF,
    FIR_INSTR_RET,

    // misc
    FIR_INSTR_CALL,
    FIR_INSTR_PROJ,
} fir_InstrKind;

struct fir_Instr {
    fir_InstrKind kind;
    fir_DataType type;

    size_t idx : 32;

    uint8_t data[];
};

typedef union fir_InstrLit {
    double f;
    uint64_t i;
} fir_InstrLit;

typedef struct fir_InstrRet {
    size_t args_n;
    fir_Instr *args[];
} fir_InstrRet;

typedef struct fir_InstrCall {
    fir_Function *func;
    fir_Instr *args[];
} fir_InstrCall;

bool fir_instr_is_terminator(fir_Instr *instr);

fir_Instr *fir_instr_lit_int(
    fir_Block *block,
    fir_DataType type,
    uint64_t val
);

fir_Instr *fir_instr_lit_float(
    fir_Block *block,
    fir_DataType type,
    double val
);

fir_Instr *fir_instr_arg(fir_Block *block, size_t idx);

fir_Instr *fir_instr_add(fir_Block *block, fir_Instr *lhs, fir_Instr *rhs);
fir_Instr *fir_instr_sub(fir_Block *block, fir_Instr *lhs, fir_Instr *rhs);
fir_Instr *fir_instr_mul(fir_Block *block, fir_Instr *lhs, fir_Instr *rhs);

fir_Instr *fir_instr_ret(fir_Block *block);
void fir_instr_ret_arg(fir_Instr *ret, fir_Instr *val);

fir_Instr *fir_instr_call(fir_Block *block, fir_Function *func);
void fir_instr_call_arg(fir_Instr *call, fir_Instr *arg);

fir_Instr *fir_instr_proj(fir_Block *block, fir_Instr *tuple, size_t idx);

void fir_instr_dump(fir_Instr *instr, FILE *fp);

#endif

