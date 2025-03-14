#include <fir.h>

#include <assert.h>
#include <stdbool.h>
#include <string.h>

bool test_string(void) {
    fir_Module module = fir_module_init("string");

    const char *a = "a string";
    const char *b = "b string";

    fir_StringID a_id = fir_string_add(&module, a);
    fir_StringID a_dup_id = fir_string_add(&module, a);

    assert(a_id == a_dup_id);

    fir_StringID b_id = fir_string_add(&module, b);

    assert(a_id != b_id);

    char *a_got = fir_string_get(&module, a_id);
    char *b_got = fir_string_get(&module, b_id);

    assert(strcmp(a_got, a) == 0);
    assert(strcmp(b_got, b) == 0);

    fir_module_deinit(&module);

    return true;
}
