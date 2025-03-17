#include "util.h"

#include <stdio.h>
#include <string.h>

size_t error_count = 0;

void expect(const char *desc, bool got) {
    if (!got) {
        error_count++;
        fprintf(stderr, RED "test failed: %s\n" RST, desc);
    }
}

void expect_int(const char *desc, long expected, long got) {
    if (expected != got) {
        error_count++;
        fprintf(
            stderr,
            RED "test failed: %s\n  expected %ld, got %ld\n" RST,
            desc, expected, got
        );
    }
}

void expect_ptr(const char *desc, void *expected, void *got) {
    if (expected != got) {
        error_count++;
        fprintf(
            stderr,
            RED "test failed: %s\n  expected %p, got %p\n" RST,
            desc, expected, got
        );
    }
}

void expect_string(const char *desc, const char *expected, const char *got) {
    int cmp = strcmp(expected, got);
    if (cmp != 0) {
        error_count++;
        fprintf(
            stderr,
            RED "test failed: %s\n  expected: %s\n  got: %s\n" RST,
            desc, expected, got
        );
    }
}

size_t get_error_count(void) {
    return error_count;
}
