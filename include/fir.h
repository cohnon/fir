#ifndef FIR_H
#define FIR_H

#include "fir_util.h"

#include <stddef.h>

typedef struct fir_Module fir_Module;

// String
typedef uint32_t fir_StringID;
#define FIR_STRING_INVALID (UINT32_MAX)
// TODO: back with hashmap
typedef fir_Array fir_StringTable;

fir_StringID fir_string_add(fir_Module *module, const char *c_str);
char *fir_string_get(fir_Module *module, fir_StringID string);

// Module
#define FIR_MODULE_NAME_MAX_LEN 128

struct fir_Module {
    char name[FIR_MODULE_NAME_MAX_LEN];

    fir_Arena arena;
    fir_StringTable string_table;

    fir_Array funcs;
};

fir_Module fir_module_init(const char *name);
void fir_module_deinit(fir_Module *module);

// Function
typedef struct fir_Function {
    fir_StringID name;
} fir_Function;

fir_Function *fir_func_create(fir_Module *module, const char *name);

#endif
