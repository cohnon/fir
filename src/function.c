#include "fir.h"

fir_Function *fir_func_create(fir_Module *module, const char *name) {
    fir_Function *new = fir_arena_alloc(&module->arena, sizeof(fir_Module));

    new->name = fir_string_add(module, name);

    return new;
}
