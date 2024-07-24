#include "fir/dynarr.h"

#include <stdlib.h>
#include <string.h>


typedef DynArr(void) DynArrVoid;

static void reserve_one(void *anyarr, size_t size) {
    DynArrVoid *arr = (DynArrVoid*)anyarr;

    if (arr->len + 1 > arr->cap) {
        arr->cap = (size_t)((float)arr->cap * 1.5f) + 8;
        arr->ptr = realloc(arr->ptr, size * arr->cap);
    }
}

void dynarr_priv_init(void *anyarr, size_t size, size_t init_cap) {
    DynArrVoid *arr = (DynArrVoid*)anyarr;

    arr->ptr = malloc(size * init_cap);
    arr->cap = init_cap;
    arr->len = 0;
}

void dynarr_deinit(void *anyarr) {
    DynArrVoid *arr = (DynArrVoid*)anyarr;

    free(arr->ptr);
    arr->len = 0;
    arr->cap = 0;
}

void dynarr_priv_push(void *anyarr, size_t size, void *elem) {
    DynArrVoid *arr = (DynArrVoid*)anyarr;

    reserve_one(arr, size);

    memcpy((unsigned char*)arr->ptr + (size * arr->len), elem, size);
    arr->len += 1;
}

void *dynarr_priv_add(void *anyarr, size_t size) {
    DynArrVoid *arr = (DynArrVoid*)anyarr;

    reserve_one(arr, size);

    void *new = (unsigned char*)arr->ptr + (size * arr->len);
    arr->len += 1;

    return new;
}

void *dynarr_priv_get(void *anyarr, size_t size, size_t idx) {
    DynArrVoid *arr = (DynArrVoid*)anyarr;

    return (unsigned char*)arr->ptr + (size * idx);
}

