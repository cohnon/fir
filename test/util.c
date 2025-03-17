#include "util.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

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

char *file_to_string(FILE *fp) {
    int res = fseek(fp, 0, SEEK_END);
    assert(res == 0);

    size_t len = ftell(fp);
    char *string = malloc(len + 1);
    assert(string != NULL);
    string[len] = '\0';

    rewind(fp);
    size_t read = fread(string, 1, len, fp);
    assert(read == len);

    return string;
}
