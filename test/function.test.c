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

    fir_func_add_input(func, fir_type_int(32));
    fir_func_add_input(func, fir_type_int(16));
    fir_func_add_output(func, fir_type_int(8));
    fir_func_add_output(func, fir_type_int(3));

    assert(func->input.len == 2);
    assert(func->output.len == 2);

    // dump
    char *expected =
        "@" NAME " : (i32, i16) -> (i8, i3)\n";

    char buffer[128] = {0};
    FILE *fp = fmemopen(buffer, sizeof(buffer), "w+");
    fir_func_dump(func, fp);

    assert(strcmp(buffer, expected) == 0);

    if (dump) {
        fir_module_dump(&module, stdout);
    }

    fir_module_deinit(&module);

    return true;
}
