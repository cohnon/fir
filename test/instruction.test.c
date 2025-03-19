#include "util.h"
#include <fir.h>

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

void test_instr(bool dump) {
    fir_Module module = fir_module_init("instr_test");

    fir_Function *func = fir_func_create(&module, "function");

    fir_func_add_input(func, fir_type_int(32));
    fir_func_add_input(func, fir_type_float(64));

    fir_Block *entry = fir_func_get_entry(func);

    fir_Instr *arg0 = fir_instr_arg(entry, 0);
    fir_instr_arg(entry, 1);

    fir_Instr *lita = fir_instr_lit_int(entry, fir_type_int(32), 100, false);
    fir_instr_lit_float(entry, fir_type_float(32), 100.0f);

    fir_instr_add(entry, fir_type_int(32), arg0, lita);

    // dump
    char *expected =
        "@function (i32, f64) -> ()\n"
        "  i32 R0 = arg 0\n"
        "  f64 R1 = arg 1\n"
        "  i32 R2 = lit 100\n"
        "  f32 R3 = lit 100.000000\n"
        "  i32 R4 = add R0, R2\n"
    ;

    FILE *fp = tmpfile();
    fir_func_dump(func, fp);

    char *got = file_to_string(fp);
    expect_string("matching output", expected, got);
    free(got);

    if (get_error_count() == 0 && dump) {
        fir_module_dump(&module, stdout);
        printf("\n");
    }

    fir_module_deinit(&module);
}

