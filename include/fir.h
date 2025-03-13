#ifndef FIR_H
#define FIR_H

#include "fir_util.h"
#include <stddef.h>

// Module
#define FIR_MODULE_NAME_MAX_LEN 128
typedef struct {
    char name[FIR_MODULE_NAME_MAX_LEN];

    fir_Allocator alc;
} fir_Module;

fir_Module fir_module_init(
    const char *name,
    void *(*alloc)(size_t size),
    void *(*realloc)(void *ptr, size_t size),
    void (*free)(void *ptr)
);
void fir_module_deinit(fir_Module *module);

#endif
