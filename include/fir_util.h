#ifndef FIR_UTIL_H
#define FIR_UTIL_H

#include <stddef.h>
#include <stdint.h>

typedef struct {
    void *(*alloc)(size_t);
    void *(*realloc)(void *, size_t);
    void (*free)(void *);
} fir_Allocator;

typedef struct fir_Page fir_Page;
typedef struct {
    fir_Page *first_page;
    fir_Page *cur_page;

    size_t page_size;

    fir_Allocator *alc;
} fir_Arena;

fir_Arena fir_arena_init(size_t page_size, void *(*alloc)(size_t), void (*free)(void *));
void fir_arena_deinit(fir_Arena *arena);
void *fir_arena_alloc(fir_Arena *arena, size_t size);

typedef struct {
    void *ptr;
    size_t len : 32;
    size_t cap : 32;

    fir_Allocator *alc;
} fir_Array;

fir_Array fir_array_init(size_t elem_size, size_t init_cap);
void fir_array_deinit(fir_Array *arr);

void fir_array_append_untyped(fir_Array *arr, void *elem);
#define fir_array_append(T, arr, elem) fir_array_append_untyped(arr, (T*)&elem);

void *fir_array_get_untyped(fir_Array *arr, size_t idx);
#define fir_array_get(T, arr, idx) (T*)fir_array_get_untyped(arr, idx);

#endif
