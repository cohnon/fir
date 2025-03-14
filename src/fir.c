#include "fir.h"

#include <assert.h>
#include <string.h>

fir_Module fir_module_init(const char *name) {
    assert(name != NULL && name[0] != 0);

    fir_Module module;

    strncpy(module.name, name, FIR_MODULE_NAME_MAX_LEN);

    return module;
}

void fir_module_deinit(fir_Module *module) {
    (void)module;
}
