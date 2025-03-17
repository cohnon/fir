#include "util.h"
#include <fir_util.h>

#include <stdbool.h>

void test_arena(void) {
    const size_t page_size = 20;

    fir_Arena arena = fir_arena_init(page_size);

    fir_Page *first_page = arena.cur_page;

    uint64_t *a = fir_arena_alloc(&arena, sizeof(uint64_t));
    *a = 100;

    expect_ptr("use same page", arena.cur_page, first_page);

    uint64_t *b = fir_arena_alloc(&arena, sizeof(uint64_t));
    *b = 50;

    expect_ptr("use same page", arena.cur_page, first_page);

    uint64_t *c = fir_arena_alloc(&arena, sizeof(uint64_t));
    *c = 25;

    expect("create new page", arena.cur_page != first_page);

    expect_int("*a", *a, 100);
    expect_int("*b", *b, 50);
    expect_int("*c", *c, 25);

    fir_arena_deinit(&arena);

    expect_ptr("cleanup (1)", arena.cur_page, NULL);
    expect_int("cleanup (2)", arena.page_size, 0);
}
