#include "util.h"
#include <fir.h>

#include <stdbool.h>
#include <string.h>

void test_block(bool dump) {
    fir_Module module = fir_module_init("block_test");

    #define NAME "function"

    fir_Function *func = fir_func_create(&module, NAME);

    fir_func_add_input(func, fir_type_int(32));
    fir_func_add_input(func, fir_type_int(16));

    fir_Block *entry = fir_func_get_entry(func);

    expect_int("inputs set", 2, entry->inputs.len);

    fir_instr_add(func, entry, NULL, NULL);

    fir_Block *blk1 = fir_block_create(func, "block1");

    fir_instr_add(func, blk1, NULL, NULL);
    fir_instr_add(func, blk1, NULL, NULL);

    // dump
    char *expected =
        "@" NAME " (i32, i16) -> ()\n"
        "  <unhandled instruction>\n"
        ":block1\n"
        "  <unhandled instruction>\n"
        "  <unhandled instruction>\n"
    ;

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
