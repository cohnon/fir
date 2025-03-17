#ifndef FIR_TEST_UTIL_H
#define FIR_TEST_UTIL_H

#include <stdbool.h>
#include <stddef.h>

#define RST "\x1b[m"
#define RED "\x1b[31m"
#define BLUE "\x1b[34m"

void expect(const char *desc, bool got);
void expect_int(const char *desc, long expected, long got);
void expect_ptr(const char *desc, void *expected, void *got);
void expect_string(const char *desc, const char *expected, const char *got);

size_t get_error_count(void);

#endif
