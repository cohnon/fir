#include "fir.h"

fir_Function *fir_func_create(fir_Module *module, const char *name) {
    fir_Function *new = fir_arena_alloc(&module->arena, sizeof(fir_Module));

    new->name = fir_string_add(module, name);

    fir_array_append(fir_Function *, &module->funcs, new);

    return new;
}

void fir_func_dump(fir_Module *module, fir_Function *func, FILE *fp) {
    char *name_str = fir_string_get(module, func->name);
    fprintf(fp, "@%s : () -> ()\n", name_str);
}
