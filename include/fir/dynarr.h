#ifndef FIR_DYNARR_H
#define FIR_DYNARR_H

#include <stddef.h>


#define DynArr(T) struct { \
    T *ptr;                \
    size_t len;            \
    size_t cap;            \
}

#define dynarr_init(arr, init_cap) dynarr_priv_init(arr, sizeof(__typeof__(*(arr)->ptr)), init_cap)
void dynarr_priv_init(void *arr, size_t size, size_t init_cap);
void dynarr_deinit(void *arr);

// assigns to tmp to warn if elem is of wrong type
#define dynarr_push(arr, elem) do { \
    __typeof__((arr)->ptr) tmp = elem; \
    dynarr_priv_push(arr, sizeof(__typeof__(*(arr)->ptr)), tmp); \
} while (0)
void dynarr_priv_push(void *arr, size_t size, void *elem);

#define dynarr_add(arr) (__typeof__( (arr)->ptr ))dynarr_priv_add(arr, sizeof(__typeof__(*(arr)->ptr)))
void *dynarr_priv_add(void *arr, size_t size);

#define dynarr_get(arr, idx) *dynarr_get_ref(arr, idx)
#define dynarr_get_ref(arr, idx) (__typeof__( (arr)->ptr ))dynarr_priv_get(arr, sizeof(__typeof__(*(arr)->ptr)), idx)
void *dynarr_priv_get(void *arr, size_t size, size_t idx);

#define dynarr_foreach(arr, i) for (size_t i = 0; i < (arr).len; i += 1)

#endif
