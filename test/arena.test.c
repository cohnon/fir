#include <fir_util.h>

#include <assert.h>
#include <stdbool.h>

bool test_arena(void) {
    const size_t page_size = 20;

    fir_Arena arena = fir_arena_init(page_size);

    fir_Page *first_page = arena.cur_page;

    uint64_t *a = fir_arena_alloc(&arena, sizeof(uint64_t));
    *a = 100;

    assert(arena.cur_page == first_page);

    uint64_t *b = fir_arena_alloc(&arena, sizeof(uint64_t));
    *b = 50;

    assert(arena.cur_page == first_page);

    uint64_t *c = fir_arena_alloc(&arena, sizeof(uint64_t));
    *c = 25;

    assert(arena.cur_page != first_page);

    assert(*a == 100);
    assert(*b == 50);
    assert(*c == 25);

    assert(arena.page_size == page_size);

    fir_arena_deinit(&arena);

    assert(arena.cur_page == NULL);
    assert(arena.page_size == 0);

    return true;
}
