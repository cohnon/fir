#ifndef FIR_UTIL_H
#define FIR_UTIL_H

#include <stddef.h>
#include <stdint.h>

#define FIR_PAGE_1K (1 << 10)
#define FIR_PAGE_4K (FIR_PAGE_1K * 4)

typedef struct fir_Page fir_Page;
typedef struct {
    fir_Page *cur_page;

    size_t page_size;
} fir_Arena;

fir_Arena fir_arena_init(size_t page_size);
void fir_arena_deinit(fir_Arena *arena);
void *fir_arena_alloc(fir_Arena *arena, size_t size);

typedef struct {
    uint8_t *ptr;
    size_t len : 32;
    size_t cap : 32;
} fir_Array;

fir_Array fir_array_init_untyped(size_t elem_size, size_t init_cap);
#define fir_array_init(T, init_cap) fir_array_init_untyped(sizeof(T), init_cap)

void fir_array_deinit(fir_Array *array);

void fir_array_append_untyped(fir_Array *array, size_t elem_size, void *elem);
#define fir_array_append(T, array, elem) fir_array_append_untyped(array, sizeof(T), (T*)&elem)

void *fir_array_get_untyped(fir_Array *array, size_t elem_size, size_t idx);
#define fir_array_get(T, array, idx) ((T*)fir_array_get_untyped(array, sizeof(T), idx))

#endif
