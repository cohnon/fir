#include <fir.h>

#include <assert.h>
#include <stdbool.h>
#include <string.h>

bool test_function(void) {
    fir_Module module = fir_module_init("function");

    char *name = "FUNCTION NAME";

    fir_Function *func = fir_func_create(&module, name);
    char *name_str = fir_string_get(&module, func->name);

    assert(strcmp(name_str, name) == 0);

    fir_module_deinit(&module);

    return true;
}
