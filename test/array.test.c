#include <fir_util.h>

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>

bool test_array(void) {
    fir_Array array = fir_array_init(uint64_t, 0);

    uint64_t a = 10;
    fir_array_append(uint64_t, &array, a);

    assert(array.len == 1);
    assert(array.cap == 8);

    uint64_t b = 20;
    for (int i = 0; i < 8; i += 1) {
        fir_array_append(uint64_t, &array, b);
    }

    assert(array.len == 9);
    assert(array.cap == 20);

    uint64_t a_back = *fir_array_get(uint64_t, &array, 0);
    assert(a_back == a);

    uint64_t b_back = *fir_array_get(uint64_t, &array, 8);
    assert(b_back == b);

    fir_array_deinit(&array);

    return 1;
}
