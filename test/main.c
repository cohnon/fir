#include "util.h"

#include <fir.h>

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>

void test_arena(void);
void test_array(void);
void test_string(void);
void test_module(bool dump);

#define BREAK_ON_ERR if (get_error_count() > 0) {\
    fprintf(stderr, "[%ld test(s) failed]\n", get_error_count());\
    return 1;\
}

void print_header(const char *name) {
    printf(BLUE "[testing %s]" RST "\n", name);
}

int main(void) {
    print_header("arena");
    test_arena();

    BREAK_ON_ERR

    print_header("array");
    test_array();

    BREAK_ON_ERR

    print_header("string");
    test_string();

    BREAK_ON_ERR

    print_header("module");
    test_module(true);

    BREAK_ON_ERR

    printf("[all tests passed]\n");

    return 0;
}
