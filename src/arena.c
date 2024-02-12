#include "arena.h"

#include <assert.h>
#include <stdalign.h>
#include <stdbool.h>
#include <stdlib.h>


typedef struct FirArenaChunk {
    FirArenaChunk *prev;
    size_t len;
    alignas(max_align_t) unsigned char data[];
} FirArenaChunk;


static FirArenaChunk *create_chunk(FirArenaChunk *prev, size_t size) {
    FirArenaChunk *chunk = malloc(offsetof(FirArenaChunk, data) + size);

    chunk->prev = prev;
    chunk->len = 0;

    return chunk;
}

static bool is_power_of_2(size_t n) {
    return (n & (n-1)) == 0;
}

static size_t align_forward(size_t n, size_t alignment) {
    assert(is_power_of_2(alignment));

    // increases n to be over next aligned boundary
    size_t n_aligned = n + alignment - 1;

    // clears all lower bits
    n_aligned &= ~(alignment - 1);

    return n_aligned;
}

FirArena fir_arena_init(size_t block_size) {
    FirArena arena;

    arena.block_size = block_size;
    arena.cur = create_chunk(NULL, block_size);

    return arena;
}

void fir_arena_deinit(FirArena *arena) {
    while (arena->cur != NULL) {
        FirArenaChunk *cur = arena->cur;
        arena->cur = cur->prev;

        free(cur);
    }
}
#include <stdio.h>
void *fir_arena_alloc_aligned(FirArena *arena, size_t size, size_t alignment) {
    FirArenaChunk *chunk = arena->cur;
    size_t next_offset = align_forward(chunk->len, alignment);

    if (next_offset + size > arena->block_size) {
        next_offset = 0;

        chunk = create_chunk(chunk, arena->block_size);
        arena->cur = chunk;
    }

    chunk->len = next_offset + size;

    return (void*)(chunk->data + next_offset);
}





