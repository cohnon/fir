#include "util.h"
#include <fir.h>

#include <stdbool.h>

void test_function(bool dump) {
    fir_Module module = fir_module_init("func_test");

    #define NAME "function"

    fir_Function *func = fir_func_create(&module, NAME);
    char *name_str = fir_string_get(&module, func->name);

    expect_int("func.len", 1, module.funcs.len);
    expect_string("name set", NAME, name_str);
    expect_int("entry block created", 1, func->blocks.len);

    fir_func_add_input(func, fir_type_int(32));
    fir_func_add_input(func, fir_type_int(16));
    fir_func_add_output(func, fir_type_int(8));
    fir_func_add_output(func, fir_type_int(4));

    expect_int("outputs set", 2, func->outputs.len);
    expect_int("inputs set", 2, func->inputs.len);

    // dump
    char *expected =
        "@" NAME " (i32, i16) -> (i8, i4)\n";

    char buffer[128] = {0};
    FILE *fp = fmemopen(buffer, sizeof(buffer), "w+");
    fir_func_dump(func, fp);

    expect_string("dump matches", expected, buffer);

    if (dump) {
        fir_module_dump(&module, stdout);
        printf("\n");
    }

    fir_module_deinit(&module);
}

