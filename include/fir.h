#ifndef FIR_H
#define FIR_H

#include "fir_util.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

typedef struct fir_s_Module fir_Module;
typedef struct fir_s_Block fir_Block;

// String
typedef uint32_t fir_StringID;
#define FIR_STRING_INVALID (UINT32_MAX)
// TODO: back with hashmap
typedef fir_Array fir_StringTable;

fir_StringID fir_string_add(fir_Module *module, const char *c_str);
char *fir_string_get(fir_Module *module, fir_StringID string);

// Type
enum fir_Type {
    FIR_TYPE_INT,
    FIR_TYPE_FLOAT,
    FIR_TYPE_PTR,
};

typedef struct fir_s_Type {
    enum fir_Type kind : 4;
    uint16_t bits : 12;
} fir_Type;

fir_Type fir_type_int(size_t bits);

void fir_type_dump(fir_Type type, FILE *fp);


// Module
#define FIR_MODULE_NAME_MAX_LEN 127

struct fir_s_Module {
    char name[FIR_MODULE_NAME_MAX_LEN + 1];

    fir_Arena arena;
    fir_StringTable string_table;
    fir_Array funcs; // fir_Function *
};

fir_Module fir_module_init(const char *name);
void fir_module_deinit(fir_Module *module);

void fir_module_dump(fir_Module *module, FILE *fp);

// Function
typedef struct fir_s_Function {
    fir_Module *module;

    fir_StringID name;
    fir_Array blocks; // fir_Block *

    fir_Array inputs;  // fir_Type
    fir_Array outputs; // fir_Type
} fir_Function;

fir_Function *fir_func_create(fir_Module *module, const char *name);
void fir_func_destroy(fir_Function *func);

fir_Block *fir_func_get_entry(fir_Function *func);

void fir_func_add_input(fir_Function *func, fir_Type type);
void fir_func_add_output(fir_Function *func, fir_Type type);

void fir_func_dump(fir_Function *func, FILE *fp);

// Block
struct fir_s_Block {
    fir_Function *func;

    fir_StringID name;

    fir_Array inputs; // fir_Type
    fir_Array instrs; // fir_Instruction *
};

fir_Block *fir_block_create(fir_Function *func, const char *name);
void fir_block_destroy(fir_Block *block);

void fir_block_add_input(fir_Block *block, fir_Type type);

void fir_block_dump(fir_Function *func, fir_Block *block, FILE *fp);

// Instruction
enum fir_Instruction {
    FIR_INSTR_NOP,

    FIR_INSTR_LIT,
    FIR_INSTR_ARG,

    // arithmetic
    FIR_INSTR_ADD,
    FIR_INSTR_SUB,
    FIR_INSTR_MUL,
    FIR_INSTR_DIVI,
    FIR_INSTR_DIVU,

    FIR_INSTR_SHL,
    FIR_INSTR_SHR,

    FIR_INSTR_ADDF,
    FIR_INSTR_SUBF,
    FIR_INSTR_MULF,
    FIR_INSTR_DIVF,

    FIR_INSTR_EQL,
    FIR_INSTR_NEQ,

    // terminals
    FIR_INSTR_JMP,
    FIR_INSTR_IF,
    FIR_INSTR_RET,
    FIR_INSTR_CALL,
};

typedef struct fir_s_Instruction fir_Instruction;
struct fir_s_Instruction {
    enum fir_Instruction kind;
    fir_Type type;

    fir_StringID name;
    size_t idx : 32;

    union {
        fir_Instruction *args[2];
        union {
            double f;
            struct {
                uint64_t val;
                bool is_signed;
            } i;
        } lit;
    } _;
};

void fir_instr_lit_int(
    fir_Block *block,
    fir_Type type,
    uint64_t val,
    bool is_signed
);

void fir_instr_add(
    fir_Block *block,
    fir_Type type,
    fir_Instruction *lhs,
    fir_Instruction *rhs
);

void fir_instr_dump(fir_Block *block, fir_Instruction *instr, FILE *fp);

#endif
