#include <fir.h>

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>

bool test_arena(void);
bool test_array(void);
bool test_string(void);
bool test_function(void);

int main(void) {
    assert(test_arena());
    printf("arena tests passed...\n");

    assert(test_array());
    printf("array tests passed...\n");

    assert(test_string());
    printf("string tests passed...\n");

    assert(test_function());
    printf("function tests passed...\n");

    printf("== all tests passed ==\n");

    return 0;
}
