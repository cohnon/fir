#include "util.h"

#include <fir.h>

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>

void test_arena(void);
void test_array(void);
void test_string(void);
void test_function(bool dump);
void test_block(bool dump);

#define BREAK_ON_ERR if (get_error_count() > 0) {\
    fprintf(stderr, "[%ld test(s) failed]\n", get_error_count());\
    return 1;\
}

void print_header(const char *name) {
    printf(BLUE "[testing %s]\n" RST, name);
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

    print_header("function");
    test_function(true);

    BREAK_ON_ERR

    print_header("block");
    test_block(true);

    BREAK_ON_ERR

    printf("[all tests passed]\n");

    return 0;
}
