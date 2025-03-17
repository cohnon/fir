#include "fir_util.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

static void ensure_cap(fir_Array *array, size_t elem_size, size_t cap) {
    if ((cap * elem_size) > array->cap) {
        array->ptr = realloc(array->ptr, cap * elem_size);
        assert(array->ptr != NULL);

        array->cap = cap;
    }
}

static void grow(fir_Array *array, size_t elem_size) {
    size_t new_cap = (size_t)((float)array->cap * 1.5);

    if (new_cap <= 4) {
        new_cap = 8;
    }
    ensure_cap(array, elem_size, new_cap);
}

static void *ptr_from_idx(fir_Array *array, size_t elem_size, size_t idx) {
    return array->ptr + (idx * elem_size);
}

fir_Array fir_array_init_untyped(size_t elem_size, size_t init_cap) {
    assert(elem_size > 0);

    fir_Array array;
    array.len = 0;
    array.cap = 0;
    array.ptr = NULL;

    ensure_cap(&array, elem_size, init_cap);

    return array;
}

void fir_array_deinit(fir_Array *array) {
    assert(array != NULL);

    free(array->ptr);
    array->ptr = NULL;
    array->len = 0;
    array->cap = 0;
}

void fir_array_append_untyped(fir_Array *array, size_t elem_size, void *elem) {
    assert(array != NULL);
    assert(elem_size > 0);
    assert(elem != NULL);

    if (array->len + 1 > array->cap) {
        grow(array, elem_size);
    }

    memcpy(ptr_from_idx(array, elem_size, array->len), elem, elem_size);

    array->len += 1;
}

void *fir_array_get_untyped(fir_Array *array, size_t elem_size, size_t idx) {
    assert(array != NULL);
    assert(elem_size > 0);
    assert(idx < array->len);

    return array->ptr + (idx * elem_size);
}
