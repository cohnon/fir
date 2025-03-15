#ifndef FIR_H
#define FIR_H

#include "fir_util.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

typedef struct fir_Module fir_Module;

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
};

typedef struct {
    enum fir_Type kind : 16;
    uint16_t bits;
} fir_Type;

fir_Type fir_type_int(size_t bits);

void fir_type_dump(fir_Type type, FILE *fp);


// Module
#define FIR_MODULE_NAME_MAX_LEN 127

struct fir_Module {
    char name[FIR_MODULE_NAME_MAX_LEN + 1];

    fir_Arena arena;
    fir_StringTable string_table;

    fir_Array funcs;
};

fir_Module fir_module_init(const char *name);
void fir_module_deinit(fir_Module *module);

void fir_module_dump(fir_Module *module, FILE *fp);

// Function
typedef struct {
    fir_Type type;
    uint32_t flags;
} fir_Arg;

typedef struct fir_Function {
    fir_Module *module;

    fir_StringID name;

    fir_Array input;
    fir_Array output;
} fir_Function;

fir_Function *fir_func_create(fir_Module *module, const char *name);

void fir_func_add_input(fir_Function *func, fir_Type type);
void fir_func_add_output(fir_Function *func, fir_Type type);

void fir_func_dump(fir_Function *func, FILE *fp);


#endif
