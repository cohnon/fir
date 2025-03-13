#include "fir.h"

#include <assert.h>
#include <string.h>

fir_Module fir_module_init(
    const char *name,
    void *(*alloc)(size_t size),
    void *(*realloc)(void *ptr, size_t size),
    void (*free)(void *ptr)
) {
    assert(name != NULL && name[0] != 0);
    assert(alloc != NULL);
    assert(realloc != NULL);
    assert(free != NULL);

    fir_Module module;

    strncpy(module.name, name, FIR_MODULE_NAME_MAX_LEN);

    module.alc.alloc = alloc;
    module.alc.realloc = realloc;
    module.alc.free = free;

    return module;
}

void fir_module_deinit(fir_Module *module) {
    (void)module;
}
