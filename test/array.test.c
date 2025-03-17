#include "util.h"

#include <fir_util.h>

#include <stdbool.h>
#include <stdint.h>

void test_array(void) {
    fir_Array array = fir_array_init(uint64_t, 0);

    uint64_t a = 10;
    fir_array_append(uint64_t, &array, a);

    expect_int("array.length", array.len, 1);
    expect_int("array.cap", array.cap, 8);

    uint64_t b = 20;
    for (int i = 0; i < 8; i += 1) {
        fir_array_append(uint64_t, &array, b);
    }

    expect_int("array.len", array.len, 9);
    expect_int("array.cap", array.cap, 12);

    uint64_t a_back = *fir_array_get(uint64_t, &array, 0);
    expect_int("fir_array_get", a_back, a);

    uint64_t b_back = *fir_array_get(uint64_t, &array, 8);
    expect_int("fir_array_get", b_back, b);

    fir_array_deinit(&array);
}
