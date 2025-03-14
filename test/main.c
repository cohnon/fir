#include <fir.h>

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>

bool test_arena(void);
bool test_array(void);
bool test_string(void);
bool test_function(bool dump);

int main(void) {
    printf("[testing arena]\n");
    assert(test_arena());

    printf("[testing array]\n");
    assert(test_array());

    printf("[testing string]\n");
    assert(test_string());

    printf("[testing function]\n");
    assert(test_function(true));

    printf("[all tests passed]\n");

    return 0;
}
