#include "util.h"
#include <fir.h>

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

void test_block(bool dump) {
    fir_Module module = fir_module_init("block_test");

    fir_Function *func = fir_func_create(&module, "function");

    fir_func_add_input(func, fir_type_int(32));
    fir_func_add_input(func, fir_type_int(16));

    fir_Block *entry = fir_func_get_entry(func);

    expect_ptr("func ref set", func, entry->func);
    expect_int("inputs set", 2, entry->input.len);

    fir_instr_lit_int(entry, fir_type_int(32), 100);

    fir_Block *blk1 = fir_block_create(func, "block1");

    fir_instr_lit_int(blk1, fir_type_int(32), -50);

    expect_int("entry instruction count", 1, entry->instrs.len);
    expect_int("blk1 instruction count", 1, blk1->instrs.len);

    // dump
    char *expected =
        "@function (i32, i16) -> ()\n"
        "  i32 R0 = lit 100\n"
        "block1:\n"
        "  i32 R1 = lit -50\n"
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

