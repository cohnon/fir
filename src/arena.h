#ifndef FIR_ARENA_H
#define FIR_ARENA_H

#include <stdalign.h>
#include <stddef.h>


#define FIR_ARENA_CHUNK_S (4 * 1024)

typedef struct FirArenaChunk FirArenaChunk;

typedef struct FirArena {
    size_t block_size;
    FirArenaChunk *cur;
} FirArena;

FirArena fir_arena_init(size_t block_size);
void fir_arena_deinit(FirArena *arena);

#define fir_arena_alloc_T(arena, T) fir_arena_alloc_aligned(arena, sizeof(T), alignof(T))
void *fir_arena_alloc_aligned(FirArena *arena, size_t size, size_t alignment);

#endif
