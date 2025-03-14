#include "fir.h"
#include "fir_util.h"

#include <assert.h>
#include <string.h>

fir_Module fir_module_init(const char *name) {
    assert(name != NULL && name[0] != 0);

    fir_Module module;

    strncpy(module.name, name, FIR_MODULE_NAME_MAX_LEN);

    module.arena = fir_arena_init(FIR_PAGE_4K);
    module.string_table = fir_array_init(char *, 64);
    module.funcs = fir_array_init(fir_Function, 32);

    return module;
}

void fir_module_deinit(fir_Module *module) {
    fir_array_deinit(&module->funcs);
    fir_array_deinit(&module->string_table);

    fir_arena_deinit(&module->arena);
}
