#include "util.h"
#include <fir.h>

#include <stdbool.h>

void test_string(void) {
    fir_Module module = fir_module_init("string");

    const char *a = "a string";
    const char *b = "b string";

    fir_StringID a_id = fir_string_add(&module, a);
    fir_StringID a_dup_id = fir_string_add(&module, a);

    expect("same string to return same id", a_id == a_dup_id);

    fir_StringID b_id = fir_string_add(&module, b);

    expect("diff string to return diff id", a_id != b_id);

    char *a_got = fir_string_get(&module, a_id);
    char *b_got = fir_string_get(&module, b_id);

    expect_string("get 'a' string value back", a, a_got);
    expect_string("get 'b' string value back", b, b_got);

    fir_module_deinit(&module);
}
