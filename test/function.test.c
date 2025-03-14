#include <fir.h>

#include <assert.h>
#include <stdbool.h>
#include <string.h>

bool test_function(bool dump) {
    fir_Module module = fir_module_init("func_test");

    #define NAME "function"

    fir_Function *func = fir_func_create(&module, NAME);
    char *name_str = fir_string_get(&module, func->name);

    assert(module.funcs.len == 1);
    assert(strcmp(name_str, NAME) == 0);

    // dump
    char *expected =
        "@" NAME " : () -> ()\n";

    char buffer[128] = {0};
    FILE *fp = fmemopen(buffer, sizeof(buffer), "w+");
    fir_func_dump(&module, func, fp);

    assert(strcmp(buffer, expected) == 0);

    if (dump) {
        fir_module_dump(&module, stdout);
    }

    fir_module_deinit(&module);

    return true;
}
